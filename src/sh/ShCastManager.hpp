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
#include "ShHashMap.hpp"
#include "ShRefCount.hpp"
#include "ShGraph.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

class ShVariant;
class ShVariantCast;

/** 
 * The ShCastManager class holds information about automatically 
 * performed type promotions and type conversions 
 *
 * It also holds the precedence DAG for types. 
 *
 * The graph formed by automatic type casts should remain a DAG
 * // TODO maybe it's okay otherewise if we define "least common ancestor"
 * succinctly, but probably would be too confusing to end users if 
 * automatic promotions could end up in cycles...
 *
 * and the ShCastManager class can answer queries like 
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
 * The ShCastManager does an online preprocessing step as automatic/explicit
 * casts are registered so that lookups are O(1) (or O(k) where k is the length
 * of the cast sequence for ops that return these).
 *
 */
struct ShCastMgrEdge;
struct ShCastMgrVertex;
typedef ShGraphType<ShCastMgrVertex, ShCastMgrEdge> ShCastMgrGraphType;

struct 
SH_DLLEXPORT
ShCastMgrEdge: public ShGraphEdge<ShCastMgrGraphType> 
{
  // Creates an edges describing a relationship between two types.
  // automatic = true iff the cast is automatic
  // precedence = true iff this is an edge in the precedenced DAG 
  ShCastMgrEdge(const ShVariantCast *caster, bool automatic);
  ShCastMgrEdge(const ShCastMgrEdge &other);

  std::ostream& graphviz_dump(std::ostream& out) const;

  const ShVariantCast *m_caster;
  bool m_auto; ///< indicates whether this is an automatic promotion 
};

struct 
SH_DLLEXPORT
ShCastMgrVertex: public ShGraphVertex<ShCastMgrGraphType>
{
  ShCastMgrVertex(ShValueType valueType, ShDataType dataType);
  ShCastMgrVertex(const ShCastMgrVertex &other);

  std::ostream& graphviz_dump(std::ostream& out) const;

  ShValueType m_valueType;
  ShDataType m_dataType;
};

class 
SH_DLLEXPORT
ShCastMgrGraph: public ShGraph<ShCastMgrGraphType>
{
  public:
    ShCastMgrGraph();

    // functions to use instead of default addVertex, addEdge
    ShCastMgrVertex *addVertex(ShValueType valueType, ShDataType dataType);

    // Use this function instead of the default addVertex/addEdge
    // automatically adds src/dest indices and sets start/end on the edge 
    void addEdge(ShCastMgrEdge* edge); 

  protected:
    typedef ShPairHashMap<ShValueType, ShDataType, ShCastMgrVertex*> VertexArray;
    VertexArray m_vert;
};

class 
SH_DLLEXPORT
ShCastManager {
  public:
    void addCast(const ShVariantCast *caster, bool automatic);

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
    void doCast(ShVariant *dest, const ShVariant *src);
    // @}

    /** Casts src to the requested type and puts the result in dest
     * or if no casting is necessary, or src = 0, simply makes dest = src 
     *
     * @returns true iff a new variant was allocated (caller
     * is responsible for deallocation with delete)
     * @{
     */
    bool doAllocCast(ShVariant *& dest, ShVariant *src,
        ShValueType valueType, ShDataType dataType);
    bool doAllocCast(const ShVariant *& dest, const ShVariant *src,
        ShValueType valueType, ShDataType dataType);
    // @}

    // returns distance of a cast using automatic promotions 
    // -1 if the cast is impossible
    int castDist(ShValueType destValueType, ShValueType srcValueType);

    std::ostream& graphviz_dump(std::ostream& out) const;

    static ShCastManager* instance();

  protected:
    // graph of available ShVariantCast objects (explicit and automatic)
    ShCastMgrGraph m_casts;

    // TODO the two graphs should realy be one with a different 
    // distance functor 

    // add cached versions of cast order for different casts between indices
    // FirstCastMap[dest][destdt][src][srcdt] holds the first caster to use for 
    // getting from src to dest (or 0 if no cast path exists)
    typedef ShPairPairHashMap<ShValueType, ShDataType, ShValueType, ShDataType, 
              const ShVariantCast*> FirstCastMap;

    typedef ShPairHashMap<ShValueType, ShValueType, int> CastDistMap;

    // shortest paths using any kind of cast
    FirstCastMap m_castStep;

    // shortest distance only in precedence DAG 
    CastDistMap m_autoDist;

    static ShCastManager* m_instance;
};

}

#endif
