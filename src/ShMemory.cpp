#include "ShMemory.hpp"
#include "ShDebug.hpp"
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

ShRefCount<ShStorage> ShMemory::findStorage(const std::string& id)
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

void ShMemory::updateTimestamp(int timestamp)
{
  SH_DEBUG_ASSERT(timestamp >= m_timestamp);
  m_timestamp = timestamp;
}

void ShMemory::addStorage(const ShRefCount<ShStorage>& storage)
{
  m_storages.push_back(storage);
}

void ShMemory::removeStorage(const ShRefCount<ShStorage>& storage)
{
  StorageList::iterator I = std::find(m_storages.begin(), m_storages.end(), storage);
  if (I == m_storages.end()) return;
  (*I)->orphan();
  m_storages.erase(I);

  // TODO: fix this, refcount lossage.
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
  SH_DEBUG_ASSERT(timestamp >= m_timestamp); // TODO: Assert this
                                             // assertion :).
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

void ShStorage::sync()
{
  SH_DEBUG_ASSERT(m_memory);
  
  if (m_memory->timestamp() == timestamp()) return; // We are already
                                                    // in sync

  // Out of sync. Find the cheapest other storage to sync from
  ShStorage* source = 0;
  int transfer_cost = -1;
  ShMemory::StorageList::const_iterator I;
  for (I = m_memory->m_storages.begin(); I != m_memory->m_storages.end(); ++I) {
    ShStorage* other = I->object();
    if (other == this) continue;
    if (other->timestamp() < m_memory->timestamp()) continue;
    int local_cost = cost(other, this);
    if (cost < 0) continue; // Can't transfer from that storage.
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
  transfer(source, this);
}

void ShStorage::dirty()
{
  // TODO: Maybe in the future check that the sync worked
  sync();

  // TODO: Don't do this if we are the only sync'd storage of the
  // memory.
  // That's an optimization though.
  m_timestamp++;
  m_memory->updateTimestamp(m_timestamp);
}

int ShStorage::cost(ShStorage* from, ShStorage* to)
{
  if (!from) return -1;
  if (!to) return -1;
  if (!m_transfers) return false;
  
  TransferMap::const_iterator I = m_transfers->find(std::make_pair(from->id(), to->id()));
  if (I == m_transfers->end()) return -1;

  return I->second->cost();
}

bool ShStorage::transfer(ShStorage* from, ShStorage* to)
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

ShStorage::ShStorage(ShMemory* memory)
  : m_memory(memory), m_timestamp(-1)
{
  m_memory->addStorage(this);
}

ShStorage::TransferMap* ShStorage::m_transfers = 0;

///////////////////////////
// --- ShHostStorage --- //
///////////////////////////

ShHostStorage::ShHostStorage(ShMemory* memory, int length)
  : ShStorage(memory),
    m_length(length),
    m_data(new char[length]),
    m_managed(true)
{
}

ShHostStorage::ShHostStorage(ShMemory* memory, int length, void* data)
  : ShStorage(memory),
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


int ShHostStorage::length() const
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

ShHostMemory::ShHostMemory(int length)
  : m_hostStorage(new ShHostStorage(this, length))
{
  m_hostStorage->setTimestamp(0);
}

ShHostMemory::ShHostMemory(int length, void* data)
  : m_hostStorage(new ShHostStorage(this, length, data))
{
  m_hostStorage->setTimestamp(0);
}

ShHostMemory::~ShHostMemory()
{
}

ShHostStoragePtr ShHostMemory::hostStorage()
{
  return m_hostStorage;
}

ShRefCount<const ShHostStorage> ShHostMemory::hostStorage() const
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

    // Check that sizes match
    if (host_from->length() != host_to->length()) return false;

    std::memcpy(host_to->data(), host_from->data(), host_to->length());
    
    return true;
  }

  int cost()
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