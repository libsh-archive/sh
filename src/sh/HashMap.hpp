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
#ifndef SHHASHMAP_HPP
#define SHHASHMAP_HPP

#ifdef _WIN32
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#include <cstddef>
#include <iosfwd>


namespace SH {

/** @file HashMap.hpp
 * A wrapper around hash_map that behaves properly under both GNU libstdc++ 
 * and Microsoft's VS .NET libraries. The interface is the common subset
 * of functionality available in both implementations.
 * (This means that some of the "unusual" methods in the VC++ implementation
 * that really shouldn't be methods like lower_bound, upper_bound, etc.
 * are not available)
 *
 * The Less functor is only used in VS .NET, and the Equal functor is only
 * used under libstdc++.
 */
#if defined(_MSC_VER)
#define STD_HASH(T) stdext::hash_compare<T>
/** Implementation of hash_compare interface */
template<class Key, class Hash, class Less>
class HashCompare {
  public:
    static const size_t bucket_size = 4;
    static const size_t min_buckets = 8;
    size_t operator( )(const Key& key) const { return m_hash(key); }
    bool operator( )(const Key& key1, const Key& key2) { return m_less(key1, key2); }

  private:
    Hash m_hash;
    Less m_less;
};

#else
#define STD_HASH(T) HashFunc<T> 
template<typename T>
struct HashFunc {
  size_t operator()(const T& data) const
  { return size_t(data); }
};
#endif

template<class Key, class Data, class Hash=STD_HASH(Key),
  class Less=std::less<Key>, class Equal=std::equal_to<Key> >
class HashMap {
#if defined(_MSC_VER)
    typedef stdext::hash_map<Key, Data, HashCompare<Key, Hash, Less> > map_type;
#elif defined(__SGI_STL_PORT)
    typedef std::hash_map<Key, Data, Hash, Equal> map_type;
#else
    typedef __gnu_cxx::hash_map<Key, Data, Hash, Equal> map_type;
#endif
  public:

    typedef Key key_type;
    typedef Data data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef int size_type;

    typedef typename map_type::iterator iterator;
    typedef typename map_type::const_iterator const_iterator;

    /** Default constructor/copy constructor should work */

    /** Wrapped member functions */
   
    /** Iterators */
    iterator begin() 
    { return m_map.begin(); }

    const_iterator begin() const 
    { return m_map.begin(); }

    iterator end() 
    { return m_map.end(); }

    const_iterator end() const 
    { return m_map.end(); }

    
    /** Map size */
    size_type size() const 
    { return m_map.size(); }

    size_type max_size() const 
    { return m_map.max_size(); }

    bool empty() const 
    { return m_map.empty(); }

    /** Insert/Delete elements */
    void clear()
    { m_map.clear(); }

    std::pair<iterator, bool> insert(const value_type &value)
    { return m_map.insert(value); }

    iterator insert(iterator hint, const value_type& value)
    { return m_map.insert(hint, value); }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last)
    { m_map.insert(first, last); }

    void erase(iterator pos)
    { m_map.erase(pos); }

    void erase(iterator first, iterator last)
    { m_map.erase(first, last); }

    size_type erase(const value_type& value)
    { return m_map.erase(value); }

    /** Search */
    iterator find(const key_type &key) 
    { return m_map.find(key); }

    const_iterator find(const key_type &key) const
    { return m_map.find(key); }

    size_type count(const key_type &key) const
    { return m_map.count(key); }

    std::pair<iterator, iterator> equal_range(const key_type &key)
    { return m_map.equal_range(key); }

    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const
    { return m_map.equal_range(key); }

    /** Copy/assign */
    void swap(HashMap &other) 
    { m_map.swap(other.m_map); }

    data_type& operator[](const key_type &key) 
    { return m_map[key]; }

  private:
    map_type m_map;
};

/** Some useful stuff */
template<class Key1, class Key2, class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2)>
struct PairHash {
  typedef std::pair<Key1, Key2> key_type;
  Hash1 m_hash1;
  Hash2 m_hash2;
  size_t operator()(const key_type &key) const 
  { return m_hash1(key.first) | ~m_hash2(key.second); } 
};

template<class Key1, class Key2, class Data, class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2)>
class PairHashMap: public HashMap<std::pair<Key1, Key2>, Data, PairHash<Key1, Key2, Hash1, Hash2> > {
  typedef std::pair<Key1, Key2> key_type; 
  public:
    Data& operator()(const Key1& key1, const Key2& key2) 
    {
      return operator[](key_type(key1, key2));
    }
};

template<class T1, class T2, class T3>
struct Triple
{
  T1 v1;
  T2 v2;
  T3 v3;

  Triple(const T1 &v1, const T2 &v2, const T3 &v3)
    : v1(v1), v2(v2), v3(v3) {}

  bool operator==(const Triple &b) const
  { return v1 == b.v1 && v2 == b.v2 && v3 == b.v3; } 

  bool operator<(const Triple &b) const
  { 
    return v1 < b.v1 || 
           (v1 == b.v1 && 
           (v2 < b.v2 ||
           (v2 == b.v2 && 
           (v3 < b.v3))));  
  } 
};

template<class Key1, class Key2, class Key3, 
  class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2), 
  class Hash3=STD_HASH(Key3)> 
struct TripleHash {
  typedef Triple<Key1, Key2, Key3> key_type;
  Hash1 hash1;
  Hash2 hash2;
  Hash3 hash3;
  size_t operator()(const key_type &key) const
  { return hash1(key.v1) + hash2(key.v2) + hash3(key.v3); } 
};

template<class Key1, class Key2, class Key3, class Data, 
  class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2), 
  class Hash3=STD_HASH(Key3)> 
class TripleHashMap: public HashMap<Triple<Key1, Key2, Key3>, Data,
    TripleHash<Key1, Key2, Key3, Hash1, Hash2, Hash3> > {
  public:
    typedef Triple<Key1, Key2, Key3> key_type;
    Data& operator()(const Key1 &key1, const Key2 &key2, const Key3 &key3)
    {
      return operator[](key_type(key1, key2, key3));
    }
};

template<class T1, class T2, class T3, class T4>
struct PairPair
{
  T1 v1;
  T2 v2;
  T3 v3;
  T4 v4;

  PairPair(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4)
    : v1(v1), v2(v2), v3(v3), v4(v4) {}

  bool operator==(const PairPair &b) const
  { return v1 == b.v1 && v2 == b.v2 && v3 == b.v3 && v4 == b.v4; }

  bool operator<(const PairPair &b) const
  { 
    return v1 < b.v1 || 
           (v1 == b.v1 && 
           (v2 < b.v2 ||
           (v2 == b.v2 && 
           (v3 < b.v3 ||
           (v3 == b.v3 && 
           (v4 < b.v4))))));  
  } 
};

template<class Key1, class Key2, class Key3, class Key4, 
  class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2), 
  class Hash3=STD_HASH(Key3), class Hash4=STD_HASH(Key4)>
struct PairPairHash {
  typedef PairPair<Key1, Key2, Key3, Key4> key_type;
  Hash1 hash1;
  Hash2 hash2;
  Hash3 hash3;
  Hash4 hash4;
  size_t operator()(const key_type &key) const
  { return hash1(key.v1) + hash2(key.v2) + hash3(key.v3) + hash4(key.v4); } 
};

template<class Key1, class Key2, class Key3, class Key4, class Data, 
  class Hash1=STD_HASH(Key1), class Hash2=STD_HASH(Key2), 
  class Hash3=STD_HASH(Key3), class Hash4=STD_HASH(Key4)>
class PairPairHashMap: public HashMap<PairPair<Key1, Key2, Key3, Key4>, Data,
    PairPairHash<Key1, Key2, Key3, Key4, Hash1, Hash2, Hash3, Hash4> > {
  public:
    typedef PairPair<Key1, Key2, Key3, Key4> key_type;
    Data& operator()(const Key1 &key1, const Key2 &key2, const Key3 &key3, const Key4 &key4) 
    {
      return operator[](key_type(key1, key2, key3, key4));
    }
};

#undef STD_HASH

}

#endif
