// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHMEMORY_HPP
#define SHMEMORY_HPP

#include <list>
#include <map>
#include <utility>
#include <string>
#include "ShRefCount.hpp"

namespace SH {

class ShStorage;

/** A memory object.
 * A memory object represents a chunk of data.   Note, however, that
 * copies of this data may be stored in more than one place, i.e. on
 * both the CPU and the GPU.   This class keeps track of the "current"
 * version and makes transfers as necessary.
 * @see ShStorage
 */
class ShMemory : public ShRefCountable {
public:
  virtual ~ShMemory();
  
  int timestamp() const;

  ShRefCount<ShStorage> findStorage(const std::string& id);

  // Functor is a functor of ShStoragePtr -> bool
  template<typename Functor>
  ShRefCount<ShStorage> findStorage(const std::string& id, const Functor& f);
  
  void removeStorage(const ShRefCount<ShStorage>& storage);

protected:
  ShMemory();
  
private:
  void updateTimestamp(int timestamp);

  void addStorage(const ShRefCount<ShStorage>& storage);

  typedef std::list< ShRefCount<ShStorage> > StorageList;
  StorageList m_storages;
  int m_timestamp;

  // TODO: Maybe length field (in bytes)?
  // I think that would be useful.
  
  friend class ShStorage;

  // NOT IMPLEMENTED
  ShMemory& operator=(const ShMemory& other);
  ShMemory(const ShMemory& other);
};

typedef ShRefCount<ShMemory> ShMemoryPtr;
typedef ShRefCount<const ShMemory> ShMemoryCPtr;

// A transfer specifies the cost and manner of transferring data from
// one storage to another.
// Static instances of these should be created for any possible
// storage transfers.
// Use the ShStorage::addTransfer() function to add a transfer to the
// list.
class ShTransfer {
public:
  // Returns true if the transfer succeeded.
  virtual bool transfer(const ShStorage* from, ShStorage* to) = 0;

  // Returns -1 if transfer impossible, otherwise the cost of
  // transfer.
  // TODO: Perhaps this needs more information, e.g. size of the storages.
  virtual int cost() = 0;

protected:
  ShTransfer(const std::string& from, const std::string& to);

private:
  
  // NOT IMPLEMENTED
  ShTransfer(const ShTransfer& other);
  ShTransfer& operator=(const ShTransfer& other);
};

class ShStorage : public ShRefCountable {
public:
  ShStorage();
  virtual ~ShStorage();
  
  int timestamp() const;
  void setTimestamp(int timestamp);
  const ShMemory* memory() const;
  ShMemory* memory();

  // Make sure this storage is in sync with the latest version of the
  // memory.
  // TODO: Maybe make this const... after all, we need to do this on a
  // read!
  void sync();

  // Mark an upcoming write to this storage
  // This will sync, if necessary.
  void dirty();
  
  // Return an id uniquely identifying the _type_ of this storage
  // This is used for looking up transfer functions.
  // e.g.: host, opengl:texture, sm:texture
  virtual std::string id() const = 0;
  
  // Return the cost of transferring from one storage to another
  // Returns -1 if there is no possible transfer.
  static int cost(ShStorage* from, ShStorage* to);

  // Transfer data from one storage to another
  // Returns true if the transfer succeeded.
  static bool transfer(ShStorage* from, ShStorage* to);

  // Use this to register new transfer functions when they are instantiated.
  static void addTransfer(const std::string& from,
                          const std::string& to,
                          ShTransfer* transfer);

  // todo sync()

  void orphan();
  
protected:
  ShStorage(ShMemory* memory);

private:
  ShMemory* m_memory;
  int m_timestamp;

  typedef std::map<std::pair<std::string, std::string>, ShTransfer*> TransferMap;
  static TransferMap* m_transfers;

  // TODO: Orphaning
  
  // NOT IMPLEMENTED
  ShStorage(const ShStorage& other);
  ShStorage& operator=(const ShStorage& other);
};

typedef ShRefCount<ShStorage> ShStoragePtr;
typedef ShRefCount<const ShStorage> ShStorageCPtr;

class ShHostStorage : public ShStorage {
public:
  ShHostStorage(ShMemory* memory, int length); // Internally managed storage
  ShHostStorage(ShMemory* memory, int length, void* data); // Externally managed storage

  ~ShHostStorage();

  std::string id() const;

  int length() const;
  const void* data() const;
  void* data();
  
private:
  int m_length; // number of bytes stored
  void* m_data; // the actual data, stored on the host

  bool m_managed; // Did we create the data? If so, this is true

  // NOT IMPLEMENTED
  ShHostStorage& operator=(const ShHostStorage& other);
  ShHostStorage(const ShHostStorage& other);
};

typedef ShRefCount<ShHostStorage> ShHostStoragePtr;
typedef ShRefCount<const ShHostStorage> ShHostStorageCPtr;

class ShHostMemory : public ShMemory {
public:
  ShHostMemory(int length);
  ShHostMemory(int length, void* data);

  ~ShHostMemory();

  ShHostStoragePtr hostStorage();
  ShRefCount<const ShHostStorage> hostStorage() const;

private:
  ShHostStoragePtr m_hostStorage;
  // NOT IMPLEMENTED
  ShHostMemory& operator=(const ShHostMemory& other);
  ShHostMemory(const ShHostMemory& other);
};

typedef ShRefCount<ShHostMemory> ShHostMemoryPtr;
typedef ShRefCount<const ShHostMemory> ShHostMemoryCPtr;

template<typename Functor>
ShRefCount<ShStorage> ShMemory::findStorage(const std::string& id, const Functor& f)
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id && f(*I)) return *I;
  }
  return 0;
}

}

#endif
