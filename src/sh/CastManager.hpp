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
#ifndef SHCASTMANAGER_HPP
#define SHCASTMANAGER_HPP

#include <map>
#include "HashMap.hpp"
#include "RefCount.hpp"
#include "Graph.hpp"
#include "TypeInfo.hpp"

namespace SH {

class Variant;
class VariantCast;

/** 
 * The CastManager class holds information about automatically 
 * performed type promotions and type conversions 
 *
 * It also holds the precedence DAG for types. 
 *
 * The graph formed by automatic type casts should remain a DAG
 * (maybe it's okay otherewise if we define "least common ancestor")
 * succinctly, but probably would be too confusing to end users if 
 * automatic promotions could end up in cycles...
 *
 * and the CastManager class can answer queries like 
 * -finding a "least common ancestor" of two types using only 
 *  automatic promotions.
 *  (useful to decide what intermediate type to use during computation)
 *
 *  -for DAGS...will have to deal with ambiguity problems
 *
 * -find the shortest path from one type to another over any
 *  registered casts
 *  (useful for explicit casts where that particular cast is not registered
 *  but a sequence of casts that produces the same type conversion exists)
 *
 * The CastManager does an online preprocessing step as automatic/explicit
 * casts are registered so that lookups are O(1) (or O(k) where k is the length
 * of the cast sequence for ops that return these).
 *
 */
struct CastMgrEdge;
struct CastMgrVertex;
typedef GraphType<CastMgrVertex, CastMgrEdge> CastMgrGraphType;

struct 
SH_DLLEXPORT
CastMgrEdge: public GraphEdge<CastMgrGraphType> 
{
  // Creates an edges describing a relationship between two types.
  // automatic = true iff the cast is automatic
  // precedence = true iff this is an edge in the precedenced DAG 
  CastMgrEdge(const VariantCast *caster, bool automatic);
  CastMgrEdge(const CastMgrEdge &other);

  std::ostream& graphviz_dump(std::ostream& out) const;

  const VariantCast *m_caster;
  bool m_auto; ///< indicates whether this is an automatic promotion 
};

struct 
SH_DLLEXPORT
CastMgrVertex: public GraphVertex<CastMgrGraphType>
{
  CastMgrVertex(ValueType valueType, DataType dataType);
  CastMgrVertex(const CastMgrVertex &other);

  std::ostream& graphviz_dump(std::ostream& out) const;

  ValueType m_valueType;
  DataType m_dataType;
};

class 
SH_DLLEXPORT
CastMgrGraph: public Graph<CastMgrGraphType>
{
  public:
    CastMgrGraph();

    // functions to use instead of default addVertex, addEdge
    CastMgrVertex *addVertex(ValueType valueType, DataType dataType);

    // Use this function instead of the default addVertex/addEdge
    // automatically adds src/dest indices and sets start/end on the edge 
    void addEdge(CastMgrEdge* edge); 

  protected:
    typedef PairHashMap<ValueType, DataType, CastMgrVertex*> VertexArray;
    VertexArray m_vert;
};

class 
SH_DLLEXPORT
CastManager {
  public:
    void addCast(const VariantCast *caster, bool automatic);

    // initializes caches, checks cast graph for errors 
    // (duplicate edges, cycles) 
    void init();

    /** Casts the contents of the src variant to dest variant
     * Both must be previously allocated and the same size,
     * and caller should check dest != src.
     *
     * When dest, src have same type, this just becomes a data copy.
     * @{
     */
    void doCast(Variant *dest, const Variant *src);
    // @}

    /** Casts src to the requested type and puts the result in dest
     * or if no casting is necessary, or src = 0, simply makes dest = src 
     *
     * @returns true iff a new variant was allocated (caller
     * is responsible for deallocation with delete)
     * @{
     */
    bool doAllocCast(Variant *& dest, Variant *src,
        ValueType valueType, DataType dataType);
    bool doAllocCast(const Variant *& dest, const Variant *src,
        ValueType valueType, DataType dataType);
    // @}

    // returns distance of a cast using automatic promotions 
    // -1 if the cast is impossible
    int castDist(ValueType destValueType, ValueType srcValueType);

    std::ostream& graphviz_dump(std::ostream& out) const;

    static CastManager* instance();

  protected:
    // graph of available VariantCast objects (explicit and automatic)
    CastMgrGraph m_casts;

    // TODO the two graphs should realy be one with a different 
    // distance functor 

    // add cached versions of cast order for different casts between indices
    // FirstCastMap[dest][destdt][src][srcdt] holds the first caster to use for 
    // getting from src to dest (or 0 if no cast path exists)
    typedef PairPairHashMap<ValueType, DataType, ValueType, DataType, 
              const VariantCast*> FirstCastMap;

    typedef PairHashMap<ValueType, ValueType, int> CastDistMap;

    // shortest paths using any kind of cast
    FirstCastMap m_castStep;

    // shortest distance only in precedence DAG 
    CastDistMap m_autoDist;

    static CastManager* m_instance;
};

}

#endif
