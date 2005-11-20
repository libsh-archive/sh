// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShMemory.hpp"
#include "ShDebug.hpp"
#include "ShVariant.hpp"
#include <cstring>
#include <algorithm>

namespace SH {

//////////////////////
// --- ShMemory --- //
//////////////////////

ShMemory::~ShMemory()
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    (*I)->orphan(); // We don't need these storages to tell us that they are
                    // going away, since we're going away ourselves.
  }
}

int ShMemory::timestamp() const
{
  return m_timestamp;
}

ShPointer<ShStorage> ShMemory::findStorage(const std::string& id)
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id) return *I;
  }
  return 0;
}

ShMemory::ShMemory()
  : m_timestamp(0)
{
}

int ShMemory::increment_timestamp()
{
  return ++m_timestamp;
}

void ShMemory::addStorage(const ShPointer<ShStorage>& storage)
{
  m_storages.push_back(storage);
}

void ShMemory::removeStorage(const ShPointer<ShStorage>& storage)
{
  StorageList::iterator I = std::find(m_storages.begin(), m_storages.end(), storage);
  if (I == m_storages.end()) return;
  (*I)->orphan();
  m_storages.erase(I);

  // TODO: fix this, refcount lossage.
}

void ShMemory::add_dep(ShMemoryDep* dep)
{
  dependencies.push_back(dep);
}

void ShMemory::flush()
{
  ShHostStoragePtr storage = shref_dynamic_cast<ShHostStorage>(findStorage("host"));
  storage->dirtyall();
  for(std::list<ShMemoryDep*>::iterator i = dependencies.begin(); i != dependencies.end(); i++)
    (*i)->memory_update();
}

////////////////////////
// --- ShTransfer --- //
////////////////////////
ShTransfer::ShTransfer(const std::string& from, const std::string& to)
{
  ShStorage::addTransfer(from, to, this);
}


///////////////////////
// --- ShStorage --- //
///////////////////////
ShStorage::ShStorage()
  : m_timestamp(-1)
{
}

ShStorage::~ShStorage()
{
  if (m_memory) {
    m_memory->removeStorage(this);
  }
}

int ShStorage::timestamp() const
{
  return m_timestamp;
}

void ShStorage::setTimestamp(int timestamp)
{
  SH_DEBUG_ASSERT(timestamp >= m_timestamp); // TODO: Assert this assertion :)
  m_timestamp = timestamp;
}

const ShMemory* ShStorage::memory() const
{
  return m_memory;
}

ShMemory* ShStorage::memory()
{
  return m_memory;
}

void ShStorage::orphan()
{
  m_memory = 0;
}

void ShStorage::sync() const
{
  SH_DEBUG_ASSERT(m_memory);
  
  if (m_memory->timestamp() == timestamp()) return; // We are already in sync

  // Out of sync. Find the cheapest other storage to sync from
  const ShStorage* source = 0;
  int transfer_cost = -1;
  ShMemory::StorageList::const_iterator I;
  for (I = m_memory->m_storages.begin(); I != m_memory->m_storages.end(); ++I) {
    const ShStorage* other = I->object();
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
  if (!transfer(source, const_cast<ShStorage*>(this))) {
    SH_DEBUG_WARN("Transfer from " << source << " to " << this << " failed!");
  }
}

void ShStorage::dirty()
{
  // TODO: Maybe in the future check that the sync worked
  sync();

  dirtyall();
}

void ShStorage::dirtyall()
{
  m_timestamp = m_memory->increment_timestamp();
}

int ShStorage::cost(const ShStorage* from, const ShStorage* to)
{
  if (!from) return -1;
  if (!to) return -1;
  if (!m_transfers) return false;
  
  TransferMap::const_iterator I = m_transfers->find(std::make_pair(from->id(), to->id()));
  if (I == m_transfers->end()) return -1;

  return I->second->cost(from, to);
}

bool ShStorage::transfer(const ShStorage* from, ShStorage* to)
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

void ShStorage::addTransfer(const std::string& from,
                            const std::string& to,
                            ShTransfer* transfer)
{
  if (!m_transfers) m_transfers = new TransferMap();
  (*m_transfers)[std::make_pair(from, to)] = transfer;
}

ShStorage::ShStorage(ShMemory* memory, ShValueType value_type)
  : m_value_type(value_type), 
    m_value_size(shTypeInfo(value_type)->datasize()),
    m_memory(memory), m_timestamp(-1)
{
  m_memory->addStorage(this);
}

void ShStorage::value_type(ShValueType value_type)
{ 
  m_value_type = value_type;
  m_value_size = shTypeInfo(value_type)->datasize();
}

ShStorage::TransferMap* ShStorage::m_transfers = 0;

///////////////////////////
// --- ShHostStorage --- //
///////////////////////////

ShHostStorage::ShHostStorage(ShMemory* memory, std::size_t length, ShValueType value_type)
  : ShStorage(memory, value_type),
    m_length(length),
    m_data(new char[length]),
    m_managed(true)
{
}

ShHostStorage::ShHostStorage(ShMemory* memory, std::size_t length, void* data, ShValueType value_type)
  : ShStorage(memory, value_type),
    m_length(length),
    m_data(data),
    m_managed(false)
{
}

ShHostStorage::~ShHostStorage()
{
  if (m_managed) {
    delete [] reinterpret_cast<char*>(m_data);
  }
}

std::string ShHostStorage::id() const
{
  return "host";
}


std::size_t ShHostStorage::length() const
{
  return m_length;
}

const void* ShHostStorage::data() const
{
  return m_data;
}

void* ShHostStorage::data()
{
  return m_data;
}

//////////////////////////
// --- ShHostMemory --- //
//////////////////////////

ShHostMemory::ShHostMemory(std::size_t length, ShValueType value_type)
  : m_hostStorage(0)
{
  // avoids base-from-member initialization problem
  m_hostStorage = new ShHostStorage(this, length, value_type);

  // Make the host storage represent the newest version of the memory
  m_hostStorage->dirtyall();
}

ShHostMemory::ShHostMemory(std::size_t length, void* data, ShValueType value_type)
  : m_hostStorage(0)
{
  // avoids base-from-member initialization problem
  m_hostStorage = new ShHostStorage(this, length, data, value_type);

  // Make the host storage represent the newest version of the memory
  m_hostStorage->dirtyall();
}

ShHostMemory::~ShHostMemory()
{
}

ShHostStoragePtr ShHostMemory::hostStorage()
{
  return m_hostStorage;
}

ShPointer<const ShHostStorage> ShHostMemory::hostStorage() const
{
  return m_hostStorage;
}

class ShHostHostTransfer : public ShTransfer {
public:
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    const ShHostStorage* host_from = dynamic_cast<const ShHostStorage*>(from);
    ShHostStorage* host_to = dynamic_cast<ShHostStorage*>(to);

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
      ShVariantPtr from_variant = shVariantFactory(host_from->value_type(), SH_MEM)->
	generate(nb_values, const_cast<void*>(host_from->data()), false);
      ShVariantPtr to_variant = shVariantFactory(host_to->value_type(), SH_MEM)->
	generate(nb_values, host_to->data(), false);
      to_variant->set(from_variant);
    }
    
    return true;
  }

  int cost(const ShStorage* from, const ShStorage* to)
  {
    return 10; // Maybe this should be 0, but you never know...
  }

private:
  ShHostHostTransfer()
    : ShTransfer("host", "host")
  {
  }
  
  static ShHostHostTransfer* instance;
};

ShHostHostTransfer* ShHostHostTransfer::instance = new ShHostHostTransfer();

}
