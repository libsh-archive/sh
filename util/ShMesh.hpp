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
#ifndef SHUTIL_SHMESH_HPP
#define SHUTIL_SHMESH_HPP

#include <list>
#include <set>
#include <map>
#include "sh.hpp"

namespace ShUtil {

/** \file ShMesh.hpp
 * A mesh structure based on half-edge data structure
 * for storing meshes.
 *
 * This class is a clean implementation of the
 * basic interface from the CS488 course notes. Additional
 * features should be added by making a subclass of
 * the mesh.
 *
 * The majority of the links are kept in the edges,
 * and each vertex points to an edge which starts on that vertex,
 * and each face points to an edge in the face.
 */

/** The following invariants must be maintained at all times:
 *
 * I) In a ShMesh object x, x.verts, x.edges and x.faces sets hold
 * the vertices, edges, and faces of the mesh.
 * If p is in one of these sets and has a pointer to vert/edge/face q,
 * then q is also in the appropriate set.
 *
 * II) If an edge e = f.edge for any face f, then
 * following e->next pointers must form a cycle that returns to e.
 * There must be at least two edges in each face.
 *
 * III) Some edge pointer invariants:
 *   a) if e->next, then e->next->prev = e
 *   b) if e->next, e->next->start == e->end
 *   c) if e->prev, then e->prev->next = e
 *   d) if e->prev, e->prev->end == e->start
 *   e) if e->sym != 0, then e->sym->sym = e
 *
 * Otherwise anything goes.  It is okay to have disconnected
 * vertices, edges that don't cycle (as long as they are not 
 * in faces), duplicate edges, etc. 
 *
 * All the functions defined directly in ShMesh should maintain
 * these invariants.
 */

template<typename VT, typename ET, typename FT>
struct ShMeshEdge;

template<typename VT, typename ET, typename FT>
struct ShMeshFace;

template<typename VT, typename ET, typename FT>
struct ShMeshVertex {
  ShMeshEdge<VT, ET, FT> *edge;
  VT data;

  /** \brief Constructor fo ShMeshVertex
   * Constructs a vertex with edge = 0, default data
   */
  ShMeshVertex();

  /** \brief Constructor fo ShMeshVertex
   * Constructs a vertex with given data 
   */
  ShMeshVertex(const VT &data);

  /** \brief Copy constructor
   * This makes this->data a copy of other->data, but does not init the edge.
   */
  ShMeshVertex(const ShMeshVertex<VT, ET, FT> &other);
};


// A half-edge going from start to end that is part of face.
template<typename VT, typename ET, typename FT>
struct ShMeshEdge {
  ShMeshVertex<VT, ET, FT> *start, *end;
  ShMeshFace<VT, ET, FT> *face;
  ShMeshEdge<VT, ET, FT> *sym, *next, *prev;
  ET data;

  /** \brief Constructor for ShMeshEdge 
   * Constructs a edge with all pointers = 0, default data
   */
  ShMeshEdge();

  /** \brief Constructs a edge with the given pointers vertices */
  ShMeshEdge(ShMeshVertex<VT, ET, FT> *start, ShMeshVertex<VT, ET, FT> *end,
      ShMeshFace<VT, ET, FT> *face,
      ShMeshEdge<VT, ET, FT> *next, ShMeshEdge<VT, ET, FT> *prev,
      ShMeshEdge<VT, ET, FT> *sym);

  /** \brief Constructs a edge with the given pointers vertices & data */
  ShMeshEdge(ShMeshVertex<VT, ET, FT> *start, ShMeshVertex<VT, ET, FT> *end,
      ShMeshFace<VT, ET, FT> *face,
      ShMeshEdge<VT, ET, FT> *next, ShMeshEdge<VT, ET, FT> *prev,
      ShMeshEdge<VT, ET, FT> *sym, 
      const ET &data);

  /** \brief Copy constructor
   * This makes this->data a copy of other->data, but does not init any ptrs. 
   */
  ShMeshEdge(const ShMeshEdge<VT, ET, FT> &other);

  /** \brief ShMeshEdge destructor.
   * If next, prev or sym are set, then next->prev, prev->next, sym->sym 
   * are all updated to reflect the demise of this edge.
   */
  ~ShMeshEdge();

  /** \brief Sets next edge 
   * Sets p->prev to this and clear any old next->prev to 0. 
   */
  void setNext(ShMeshEdge<VT, ET, FT> *n);

  /** \brief Sets previous edge 
   * Sets p->next to this and clear any old prev->next to 0. 
   */
  void setPrev(ShMeshEdge<VT, ET, FT> *p);

  /** \brief Sets sym edge and sets s->sym to this
   * while clearing any old sym->sym to 0.
   */
  void setSym(ShMeshEdge<VT, ET, FT> *s);

};


template<typename VT, typename ET, typename FT> 
struct ShMeshFace {
  ShMeshEdge<VT, ET, FT> *edge;
  FT data;

  /** \brief Constructor for ShMeshFace 
   * Constructs a face with all pointers = 0, default data
   */
  ShMeshFace();

  /** \brief Copy constructor
   * This makes this->data a copy of other->data, but does not init any ptrs. 
   */
  ShMeshFace(const ShMeshFace<VT, ET, FT> &other);
};

template<typename VT, typename ET, typename FT>
class ShMesh {
  public:
    typedef ShMeshVertex<VT, ET, FT> VertexType;
    typedef ShMeshEdge<VT, ET, FT> EdgeType;
    typedef ShMeshFace<VT, ET, FT> FaceType;

    typedef std::set<VertexType*> VertexSet;
    typedef std::set<EdgeType*> EdgeSet;
    typedef std::set<FaceType*> FaceSet;
    typedef std::list<VertexType*> VertexList;

    /** \brief Empty mesh constructor */
    ShMesh();

    /** \brief Copies other mesh to this mesh */
    ShMesh(const ShMesh<VT, ET, FT>& other);

    /** \brief ShMesh Destructor */
    ~ShMesh();

    /** \brief Assignment Operator */
    ShMesh<VT, ET, FT>& operator=(const ShMesh<VT, ET, FT>& other); 

    /** \brief removes all verts, edges, and faces in this mesh & deletes them*/
    void clear();

    /** \brief Adds a face to the mesh.
     * Adds required edges and faces with uninitialized data.
     * The edge corresponding to vl(0) -> vl(1) is set to result->edge
     * Any edges that make a pair with an existing edge will have edge->sym 
     * set correctly.
     */
    FaceType* addFace(const VertexList &vl);

    /** \brief Removes a face from the mesh.
     * Deletes any edges from this face, but does not delete any vertices.
     */
    void removeFace(FaceType *f);

    /** \brief Vertex merging function.
     * Merges any vertices that are "equal" according to the 
     * STL Strict Weak Ordering functor VertLess
     */
    template<typename VertLess>
    void mergeVertices();

    /** \brief Edge merging function.
     * Pairs up half-edges that match each other (i.e. e1.start = e2.end, e1.end = e2.start) 
     * using the given STL Strict Weak Ordering functor VertLess 
     *
     * This is best used after mergeVertices, then VertLess can be the default std::less
     * and it will obey the ordering established by mergeVertices.
     *
     * Note that if there are multiple edges between start->end 
     * that match up with an edge, e, from end->start, one of them will be  
     * set to e->sym.  Which one gets matched is undefined.
     */
    template<typename VertLess> 
    void mergeEdges();

    /** \brief Triangulates by ear. 
     * Removes any two-sided faces from the mesh
     * returnst true if any triangles removed */
    bool earTriangulate();

    VertexSet verts;
    EdgeSet edges;
    FaceSet faces;

  protected:
    typedef std::map<VertexType*, VertexType*> VertMap;
    typedef std::map<EdgeType*, EdgeType*> EdgeMap;
    typedef std::map<FaceType*, FaceType*> FaceMap;
};


}

#include "ShMeshImpl.hpp"

#endif
