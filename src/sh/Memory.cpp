// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "Memory.hpp"
#include "Debug.hpp"
#include "Variant.hpp"
#include <cstring>
#include <algorithm>

namespace SH {

//////////////////////
// --- Memory --- //
//////////////////////

Memory::~Memory()
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    (*I)->orphan(); // We don't need these storages to tell us that they are
                    // going away, since we're going away ourselves.
  }
}

int Memory::timestamp() const
{
  if (m_frozen)
    return m_frozenTimestamp;
  else
    return m_timestamp;
}

Pointer<Storage> Memory::findStorage(const std::string& id)
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id) {
      return *I;
    }
  }
  return 0;
}

Pointer<const Storage> Memory::findStorage(const std::string& id) const
{
  for (StorageList::const_iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id) {
      return *I;
    }
  }
  return 0;
}

Memory::Memory()
  : m_timestamp(0), m_frozen(false), m_frozenTimestamp(0)
{
}

int Memory::increment_timestamp()
{
  return ++m_timestamp;
}

void Memory::addStorage(const Pointer<Storage>& storage)
{
  m_storages.push_back(storage);
}

void Memory::removeStorage(const Pointer<Storage>& storage)
{
  StorageList::iterator I = std::find(m_storages.begin(), m_storages.end(), storage);
  if (I == m_storages.end()) {
    return;
  }
  (*I)->orphan();
  m_storages.erase(I);

  // TODO: fix this, refcount lossage.
}

void Memory::add_dep(MemoryDep* dep)
{
  dependencies.push_back(dep);
}

void Memory::flush()
{
  HostStoragePtr storage = shref_dynamic_cast<HostStorage>(findStorage("host"));
  storage->dirtyall();
  for(std::list<MemoryDep*>::iterator i = dependencies.begin(); i != dependencies.end(); i++)
    (*i)->memory_update();
}

void Memory::freeze(bool state)
{
  m_frozen = state;
  m_frozenTimestamp = m_timestamp;
}

////////////////////////
// --- Transfer --- //
////////////////////////
Transfer::Transfer(const std::string& from, const std::string& to)
{
  Storage::addTransfer(from, to, this);
}


///////////////////////
// --- Storage --- //
///////////////////////
Storage::Storage()
  : m_timestamp(-1)
{
}

Storage::~Storage()
{
  if (m_memory) {
    m_memory->removeStorage(this);
  }
}

int Storage::timestamp() const
{
  return m_timestamp;
}

void Storage::setTimestamp(int timestamp)
{
  SH_DEBUG_ASSERT(timestamp >= m_timestamp); // TODO: Assert this assertion :)
  m_timestamp = timestamp;
}

const Memory* Storage::memory() const
{
  return m_memory;
}

Memory* Storage::memory()
{
  return m_memory;
}

void Storage::orphan()
{
  m_memory = 0;
}

void Storage::sync() const
{
  SH_DEBUG_ASSERT(m_memory);
  
  if (m_memory->timestamp() == timestamp()) return; // We are already in sync

  // Out of sync. Find the cheapest other storage to sync from
  const Storage* source = 0;
  int transfer_cost = -1;
  Memory::StorageList::const_iterator I;
  for (I = m_memory->m_storages.begin(); I != m_memory->m_storages.end(); ++I) {
    const Storage* other = I->object();
    if (other == this) continue;
    if (other->timestamp() < m_memory->timestamp()) continue;
    int local_cost = cost(other, this);
    if (local_cost < 0) continue; // Can't transfer from that storage.
    if (!source || local_cost < transfer_cost) {
      source = other;
      transfer_cost = local_cost;
    }
  }
  
  // For now:
  SH_DEBUG_ASSERT(source);
  // TODO: In the future, traverse the graph of transfers to find a
  // cheap, working one.

  // Do the actual transfer
  // Need to cast away the constness since we actually want to write TO this,
  // although all we're doing is "updating" it to the latest version.
  if (!transfer(source, const_cast<Storage*>(this))) {
    SH_DEBUG_WARN("Transfer from " << source << " to " << this << " failed!");
    SH_DEBUG_ASSERT(0);
  }
}

void Storage::dirty()
{
  // TODO: Maybe in the future check that the sync worked
  sync();

  dirtyall();
}

void Storage::dirtyall()
{
  m_timestamp = m_memory->increment_timestamp();
}

int Storage::cost(const Storage* from, const Storage* to)
{
  if (!from) return -1;
  if (!to) return -1;
  if (!m_transfers) return false;
  
  TransferMap::const_iterator I = m_transfers->find(std::make_pair(from->id(), to->id()));
  if (I == m_transfers->end()) return -1;

  return I->second->cost(from, to);
}

bool Storage::transfer(const Storage* from, Storage* to)
{
  if (!from) return false;
  if (!to) return false;
  if (!m_transfers) return false;

  TransferMap::const_iterator I = m_transfers->find(std::make_pair(from->id(), to->id()));
  if (I == m_transfers->end()) return false; // No transfer function?

  // TODO: Should we only allow transfers between storages
  // corresponding to the same memory?
  // Probably.
  if (I->second->transfer(from, to)) {
    to->setTimestamp(from->timestamp());
    return true;
  } else {
    return false;
  }
}

void Storage::addTransfer(const std::string& from,
                            const std::string& to,
                            Transfer* transfer)
{
  if (!m_transfers) m_transfers = new TransferMap();
  (*m_transfers)[std::make_pair(from, to)] = transfer;
}

Storage::Storage(Memory* memory, ValueType value_type)
  : m_value_type(value_type), 
    m_value_size(typeInfo(value_type, MEM)->datasize()),
    m_memory(memory), m_timestamp(-1)
{
  m_memory->addStorage(this);
}

void Storage::value_type(ValueType value_type)
{ 
  m_value_type = value_type;
  m_value_size = typeInfo(value_type, MEM)->datasize();
}

Storage::TransferMap* Storage::m_transfers = 0;

///////////////////////////
// --- HostStorage --- //
///////////////////////////

HostStorage::HostStorage(Memory* memory, std::size_t length, ValueType value_type, std::size_t align)
  : Storage(memory, value_type),
    m_length(length),
    m_data_unaligned(new char[length + align - 1]),
    m_managed(true)
{
  // Assumes align is a power of two
  m_data = reinterpret_cast<void *>((reinterpret_cast<ptrdiff_t>(m_data_unaligned) + align - 1) & ~(align - 1));
}

HostStorage::HostStorage(Memory* memory, std::size_t length, void* data, ValueType value_type)
  : Storage(memory, value_type),
    m_length(length),
    m_data(data),
    m_data_unaligned(data),
    m_managed(false)
{
}

HostStorage::~HostStorage()
{
  if (m_managed) {
    delete [] reinterpret_cast<char*>(m_data_unaligned);
  }
}

std::string HostStorage::id() const
{
  return "host";
}


std::size_t HostStorage::length() const
{
  return m_length;
}

const void* HostStorage::data() const
{
  return m_data;
}

void* HostStorage::data()
{
  return m_data;
}

//////////////////////////
// --- HostMemory --- //
//////////////////////////

HostMemory::HostMemory(std::size_t length, ValueType value_type, std::size_t align)
  : m_hostStorage(0)
{
  // avoids base-from-member initialization problem
  m_hostStorage = new HostStorage(this, length, value_type, align);

  // Make the host storage represent the newest version of the memory
  m_hostStorage->dirtyall();
}

HostMemory::HostMemory(std::size_t length, void* data, ValueType value_type)
  : m_hostStorage(0)
{
  // avoids base-from-member initialization problem
  m_hostStorage = new HostStorage(this, length, data, value_type);

  // Make the host storage represent the newest version of the memory
  m_hostStorage->dirtyall();
}

HostMemory::~HostMemory()
{
}

HostStoragePtr HostMemory::hostStorage()
{
  return m_hostStorage;
}

Pointer<const HostStorage> HostMemory::hostStorage() const
{
  return m_hostStorage;
}

class HostHostTransfer : public Transfer {
public:
  bool transfer(const Storage* from, Storage* to)
  {
    const HostStorage* host_from = dynamic_cast<const HostStorage*>(from);
    HostStorage* host_to = dynamic_cast<HostStorage*>(to);

    // Check that casts succeeded
    if (!host_from) return false;
    if (!host_to) return false;

    if (host_from->value_type() == host_to->value_type()) {
      // Check that sizes match
      if (host_from->length() != host_to->length()) {
        std::cerr << "From length = " << host_from->length() << ", To length = " << host_to->length() << std::endl;
        return false;
      }
      std::memcpy(host_to->data(), host_from->data(), host_to->length());
    } else {
      // Do the type conversion
      const std::size_t nb_values = host_from->length() / host_from->value_size();
      VariantPtr from_variant = variantFactory(host_from->value_type(), MEM)->
	generate(nb_values, const_cast<void*>(host_from->data()), false);
      VariantPtr to_variant = variantFactory(host_to->value_type(), MEM)->
	generate(nb_values, host_to->data(), false);
      to_variant->set(from_variant);
    }
    
    return true;
  }

  int cost(const Storage* from, const Storage* to)
  {
    return 10; // Maybe this should be 0, but you never know...
  }

private:
  HostHostTransfer()
    : Transfer("host", "host")
  {
  }
  
  static HostHostTransfer* instance;
};

HostHostTransfer* HostHostTransfer::instance = new HostHostTransfer();

}
