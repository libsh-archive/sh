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
#ifndef SHCASTMANAGER_HPP
#define SHCASTMANAGER_HPP

#include <map>
#include "ShRefCount.hpp"
#include "ShGraph.hpp"

namespace SH {

class ShCloak;
class ShCloakCast;
typedef ShPointer<ShCloakCast> ShCloakCastPtr;

/** 
 * The ShCastManager class holds information about automatically 
 * performed type promotions and available non-automatic
 * type casts that can be done explicitly.
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

struct ShCastMgrEdge: public ShGraphEdge<ShCastMgrGraphType> 
{
  // Creates an edges describing a relationship between two types.
  // automatic = true iff the cast is automatic
  // precedence = true iff this is an edge in the precedenced DAG 
  ShCastMgrEdge(ShCloakCastPtr caster, bool automatic, bool precedence);
  ShCastMgrEdge(const ShCastMgrEdge &other);

  std::ostream& graphvizDump(std::ostream& out) const;

  ShCloakCastPtr m_caster;
  bool m_automatic;
  bool m_precedence;
};

struct ShCastMgrVertex: public ShGraphVertex<ShCastMgrGraphType>
{
  ShCastMgrVertex(int typeIndex);
  ShCastMgrVertex(const ShCastMgrVertex &other);

  std::ostream& graphvizDump(std::ostream& out) const;

  int m_typeIndex;
};

class ShCastMgrGraph: public ShGraph<ShCastMgrGraphType>
{
  public:
    // functions to use instead of default addVertex, addEdge
    void addVertex(int typeIndex);

    // Use this function instead of the default addVertex/addEdge
    // automatically adds src/dest indices and sets start/end on the edge 
    void addEdge(int srcIndex, int destIndex, ShCastMgrEdge* edge); 

  protected:
    typedef std::vector<ShCastMgrVertex*> VertexVec;
    VertexVec m_vert;
    
};

class ShCastManager {
  public:
    void addCast(int destIndex, int srcIndex, ShCloakCastPtr caster, 
        bool automatic, bool precedence); 

    // initializes caches, checks cast graph for errors 
    // (duplicate edges, cycles) 
    void init();

    ShPointer<ShCloak> doCast(int destIndex, ShPointer<ShCloak> srcValue, 
        bool autoOnly = true);
    ShPointer<const ShCloak> doCast(int destIndex, 
        ShPointer<const ShCloak> srcValue, 
        bool autoOnly = true);

    // returns distance of a cast in the precedence DAG 
    // -1 if the cast is impossible
    int castDist(int destIndex, int srcIndex);

    std::ostream& graphvizDump(std::ostream& out) const;

    static ShCastManager* instance();

  protected:
    // graph of available ShCloakCast objects (explicit and automatic)
    ShCastMgrGraph m_casts;

    // TODO the two graphs should realy be one with a different 
    // distance functor 

    // add cached versions of cast order for different casts between indices
    // FirstCastMap[dest][src] holds the first caster to use for getting from
    // src to dest (or 0 if no cast path exists)
    typedef std::vector<std::vector<ShCloakCastPtr> > FirstCastMap; 
    typedef std::vector<std::vector<int> > CastDistMap; 

    // shortest paths using any kind of cast
    FirstCastMap m_castStep;

    // shortest paths using only automatic casts
    FirstCastMap m_autoStep;

    // shortest distance only in precedence DAG 
    CastDistMap m_precDist;

    static ShCastManager* m_instance;
};

}

#endif
