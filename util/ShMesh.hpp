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

#ifdef WIN32
#include <sh.hpp>
#else
#include <sh/sh.hpp>
#endif /* WIN32 */

namespace ShUtil {

/** \file ShMesh.hpp
 * A mesh structure based on half-edge data structure
 * for storing manifold meshes.
 *
 * This class is a clean implementation of the
 * basic interface from the 488 course notes. Additional
 * features should be added by making a subclass of
 * the mesh.
 *
 * All the pointers in a face are setup when faces are added
 * using addFace.  The edge syms are only set up after
 * calling merge. (And typically calling 
 *
 */

/** The following invariants must be maintained at all times:
 * In a ShMesh object x, x.verts, x.edges, and x.faces
 * must hold exactly the vertices, edges, and faces 
 * currently involved in the mesh.
 * (e.g. an edge can't point to a start/end vertex that is not in
 * x.verts) 
 *
 * But it is okay to have disconnected elements (i.e.
 * verts not in any edges, edges without faces, etc.)
 * depending on the nature of your particular mesh type.
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

  /** \brief Constructs a edge with the given start/end vertices */
  ShMeshEdge(ShMeshVertex<VT, ET, FT> *start, ShMeshVertex<VT, ET, FT> *end,
      ShMeshEdge<VT, ET, FT> *prev, ShMeshFace<VT, ET, FT> *face);

  /** \brief Copy constructor
   * This makes this->data a copy of other->data, but does not init any ptrs. 
   */
  ShMeshEdge(const ShMeshEdge<VT, ET, FT> &other);

  /** \brief Sets previous edge (and sets prev->next to this)
   */
  void setPrev(ShMeshEdge<VT, ET, FT> *p);

  /** \brief Sets sym edge and sets sym->sym to this
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
     * By default, this uses the pointer comparison
     */
    template<typename VertLess> 
    void mergeEdges();


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
