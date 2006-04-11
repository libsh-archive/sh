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
#ifndef SHMEMORY_HPP
#define SHMEMORY_HPP

#include <list>
#include <map>
#include <utility>
#include <string>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "MemoryDep.hpp"
#include "StorageType.hpp"

namespace SH {

/** @defgroup memory Buffer Management
 * @{
 */

class Storage;

/** A memory object.
 * A memory object represents a chunk of data.   Note, however, that
 * copies of this data may be stored in more than one place, i.e. on
 * both the CPU and the GPU.   This class keeps track of the "current"
 * version and makes transfers as necessary.
 * @todo Maybe keep a length field (in bytes)?
 * @see Storage
 */
class
DLLEXPORT Memory : public RefCountable {
public:
  virtual ~Memory();

  /// Return the timestamp of the most up-to-date storage of this memory
  int timestamp() const;

  /// Find a storage of a given id.
  Pointer<Storage> findStorage(const std::string& id);

  Pointer<const Storage> findStorage(const std::string& id) const;

  /// Find a storage of a given id selected by an external functor.
  /// @arg functor A functor of StoragePtr -> bool.
  template<typename Functor>
  Pointer<Storage> findStorage(const std::string& id, const Functor& f);
  template<typename Functor>
  Pointer<Storage> findStorage(const std::string& id, Functor& f);

  /// Discard this storage from this memory
  void removeStorage(const Pointer<Storage>& storage);

  /// link the memory to a specific dependency
  void add_dep(MemoryDep* dep);

  /// modify the memory data by using the links to the dependencies
  void flush();
  
  /// (un)freeze the current timestamp
  void freeze(bool state);

protected:
  Memory();

private:
  int increment_timestamp();

  void addStorage(const Pointer<Storage>& storage);

  typedef std::list< Pointer<Storage> > StorageList;
  StorageList m_storages;
  int m_timestamp;

  /// frozen memory's timestamp is updated only once it is thawed
  bool m_frozen;
  /// timestamp at the point when the memory was frozen
  int m_frozenTimestamp;

  /// the list of all dependencies, for update calls
  std::list<MemoryDep*> dependencies;

  friend class Storage;

  /// NOT IMPLEMENTED
  Memory& operator=(const Memory& other);
  /// NOT IMPLEMENTED
  Memory(const Memory& other);
};

typedef Pointer<Memory> MemoryPtr;
typedef Pointer<const Memory> MemoryCPtr;

/** A Storage Transfer function.
 * A transfer specifies the cost and manner of transferring data from
 * one storage to another.
 * Static instances of these should be created for any possible
 * storage transfers.
 * Use the Storage::addTransfer() function to add a transfer to the
 * list.
 *
 * @see Memory
 * @see Storage
 */
class
DLLEXPORT Transfer {
public:
  virtual ~Transfer() {}

  /// Returns true if the transfer succeeded.
  virtual bool transfer(const Storage* from, Storage* to) = 0;

  /// Returns -1 if transfer impossible, otherwise the cost of
  /// transfer.
  /// @todo Perhaps this needs more information, e.g. size of the storages.
  virtual int cost(const Storage* from, const Storage* to) = 0;

protected:
  Transfer(const std::string& from, const std::string& to);

private:
  
  /// NOT IMPLEMENTED
  Transfer(const Transfer& other);
  /// NOT IMPLEMENTED
  Transfer& operator=(const Transfer& other);
};

/** A Storage object
 * Storage represents actual, physical storage of some memory's
 * contents. This might be on the CPU, the GPU, a filesystem or even
 * across the network.
 *
 * @see Memory
 * @see Transfer
 */
class
DLLEXPORT Storage : public RefCountable {
public:
  Storage();
  virtual ~Storage();

  /// Return the version of the data currently stored by this storage
  int timestamp() const;
  
  /// Return the memory this storage represents
  const Memory* memory() const;
  /// Return the memory this storage represents
  Memory* memory();

  /// Make sure this storage is in sync with the latest version of the
  /// memory.
  void sync() const;

  /// Mark an upcoming write to this storage.
  /// This will sync, if necessary.
  void dirty();
  
  /// Mark an upcoming write to this storage.
  /// Don't call sync, all storages are replaced.
  void dirtyall();
  
  /// Return an id uniquely identifying the _type_ of this storage
  /// This is used for looking up transfer functions.
  /// e.g.: host, opengl:texture, sm:texture
  virtual std::string id() const = 0;
  
  /// Return the cost of transferring from one storage to another.
  /// Returns -1 if there is no possible transfer.
  static int cost(const Storage* from, const Storage* to);

  /// Transfer data from one storage to another.
  /// Returns true if the transfer succeeded.
  static bool transfer(const Storage* from, Storage* to);

  /// Use this to register new transfer functions when they are instantiated.
  static void addTransfer(const std::string& from,
                          const std::string& to,
                          Transfer* transfer);

  /// Called by Memory when it destructs. Necessary for refcounting purposes.
  void orphan();
  
  /// Return the type that is stored inside this object
  ValueType value_type() const { return m_value_type; }

  /// Change the internal type (does NOT convert the values)
  void value_type(ValueType value_type);

  /// Return the size in bytes of one value in the storage array
  int value_size() const { return m_value_size; }

protected:
  Storage(Memory* memory, ValueType value_type);
  
  ValueType m_value_type; // type index expected of data on host
  int m_value_size; // size of one element

private:
  Memory* m_memory;
  int m_timestamp;

  /// Force-set the version of the data currently stored by this storage
  void setTimestamp(int timestamp);
  
  typedef std::map<std::pair<std::string, std::string>, Transfer*> TransferMap;
  static TransferMap* m_transfers;

  /// NOT IMPLEMENTED
  Storage(const Storage& other);
  /// NOT IMPLEMENTED
  Storage& operator=(const Storage& other);
};

typedef Pointer<Storage> StoragePtr;
typedef Pointer<const Storage> StorageCPtr;

/** An Storage representing data stored on the CPU host memory.
 * You probably want to use HostMemory to construct this.
 * @see Memory
 * @see Storage
 * @see HostMemory
 */
class
DLLEXPORT HostStorage : public Storage {
public:
  HostStorage(Memory* memory, std::size_t length, ValueType storage_type, std::size_t align = 1); ///< Internally managed storage
  HostStorage(Memory* memory, std::size_t length, void* data, ValueType storage_type); ///< Externally managed storage

  /// Destruct this storage.
  /// If the memory was allocated internally, it will be freed automatically.
  ~HostStorage();

  std::string id() const;

  /// Return the length (in bytes) of data represented by this storage.
  std::size_t length() const;

  /// Return the location of the storage's data on the host
  const void* data() const;
  /// Return the location of the storage's data on the host
  void* data();
  
private:
  std::size_t m_length; ///< number of bytes stored
  void* m_data;         ///< the actual data, stored on the host, aligned
  void* m_data_unaligned; ///< the actual data, stored on the host

  bool m_managed; ///< Did we create the data? If so, this is true

  // NOT IMPLEMENTED
  HostStorage& operator=(const HostStorage& other);
  HostStorage(const HostStorage& other);
};

typedef Pointer<HostStorage> HostStoragePtr;
typedef Pointer<const HostStorage> HostStorageCPtr;

/** An Memory initially originating in CPU host memory.
 * @see Memory
 * @see HostStorage
 */
class
DLLEXPORT HostMemory : public Memory {
public:
  HostMemory(std::size_t length, ValueType value_type, std::size_t align = 1);
  HostMemory(std::size_t length, void* data, ValueType value_type);

  ~HostMemory();

  HostStoragePtr hostStorage();
  Pointer<const HostStorage> hostStorage() const;

private:
  HostStoragePtr m_hostStorage;
  // NOT IMPLEMENTED
  HostMemory& operator=(const HostMemory& other);
  HostMemory(const HostMemory& other);
};

typedef Pointer<HostMemory> HostMemoryPtr;
typedef Pointer<const HostMemory> HostMemoryCPtr;

template<typename Functor>
Pointer<Storage> Memory::findStorage(const std::string& id, Functor& f)
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id && f(*I)) return *I;
  }
  return 0;
}

template<typename Functor>
Pointer<Storage> Memory::findStorage(const std::string& id, const Functor& f)
{
  for (StorageList::iterator I = m_storages.begin(); I != m_storages.end(); ++I) {
    if ((*I)->id() == id && f(*I)) return *I;
  }
  return 0;
}

/*@}*/

}

#endif
