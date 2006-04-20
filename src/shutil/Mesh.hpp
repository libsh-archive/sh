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
#ifndef SHUTIL_SHMESH_HPP
#define SHUTIL_SHMESH_HPP

#include <list>
#include <map>
#include <set>

#include "sh/sh.hpp"

namespace ShUtil {

/** \file Mesh.hpp
 * A basic mesh structure based on half-edges.
 *
 * To use this class, define CopyConstructible 
 * vertex, face, and edge classes to hold your vertex/face/edge specific data
 * that are subclasses of MeshVertex, MeshFace, and MeshEdge
 * respectively.
 *
 * The half-edge Mesh class always keeps the following invariants: 
 * * Edge pointers: 
 *   For any edge e,
 *   a) if e.next, then e.next->prev = e
 *   b) if e.next, then e.next->start == e->end
 *   c) if e.prev, then e.prev->next = e
 *   d) if e.prev, then e.prev->end == e->start
 *   e) if e.sym != 0, then e.sym->sym = e
 *
 * * Vertex edge:
 *   For any vertex v, v.edge.start = v 
 *
 * * Face edge:
 *   For any face f, f.edge.face = f, and
 *   f.edge->next->next...  = f.edge after following enough next pointers.
 *
 * All the public Mesh class functions maintain these invariants.
 *
 * Null Pointers:
 * For any edge e, e.start and e.end are always != 0.
 * For any face f, f.edge is always != 0.
 * All other pointers can be 0. 
 */

template<typename VertexType, typename FaceType, typename EdgeType>
struct MeshType {
  typedef VertexType Vertex;
  typedef FaceType Face;
  typedef EdgeType Edge;
};

template<typename M>
struct MeshVertex {
  typedef typename M::Edge Edge;
  Edge *edge; //< Edge that starts at this vertex 

  /** \brief Constructor that sets edge to 0 */ 
  MeshVertex();

  /** \brief Constructor that sets edge to 0 */ 
  MeshVertex(const MeshVertex<M> &other);
};

template<typename M>
struct MeshFace {
  typedef typename M::Edge Edge;
  Edge *edge; //< Edge in this face 

  /** \brief Constructor that sets edge to 0 */ 
  MeshFace();

  /** \brief Constructor that sets edge to 0 */ 
  MeshFace(const MeshFace<M> &other);
};

// A half-edge going from start to end that is part of face.
template<typename M>
struct MeshEdge {
  typedef typename M::Vertex Vertex;
  typedef typename M::Face Face;
  typedef typename M::Edge Edge;
  Vertex *start; //< Start vertex 
  Vertex *end;  //< End vertex
  Face *face; //< Face  
  Edge *sym; //< Edge paired with this edge.
  Edge *next; //< Next edge in the face 
  Edge *prev; //< Previous edge in the face

  /** \brief Constructs a edge with all pointers = 0 */ 
  MeshEdge();

  /** \brief Constructor that sets all pointers = 0 */ 
  MeshEdge(const MeshEdge<M> &other);

  /** \brief Constructs a edge with pointers to the given objects 
   * (any may be 0 except start and end) */
  void setLinks(Vertex *s, Vertex *e, Face *f, 
      Edge *next, Edge *prev, Edge *sym);

  /** \brief Sets next, updating next->prev if next is non-null. */ 
  void setNext(Edge *n);

  /** \brief Sets prev, updating prev->next if prev is non-null */ 
  void setPrev(Edge *p);

  /** \brief Sets sym, updating sym->sym if sym is non-null */ 
  void setSym(Edge *s);
};


/** Mesh class stores a mesh using a half-edge data structure */
template<typename M>
class Mesh {
  public:
    typedef M MeshType; 
    typedef typename M::Vertex Vertex; 
    typedef typename M::Edge Edge; 
    typedef typename M::Face Face; 

    typedef std::set<Vertex*> VertexSet;
    typedef std::set<Edge*> EdgeSet;
    typedef std::set<Face*> FaceSet;
    typedef std::list<Vertex*> VertexList;

    /** \brief Empty mesh constructor */
    Mesh();

    /** \brief Copy constructor 
     * Makes copies of vertices, edges, and faces and builds a mesh isomorphic other */
    Mesh(const Mesh<M>& other);

    /** \brief Mesh destructor */
    ~Mesh();

    /** \brief Assignment Operator  */
    Mesh<M>& operator=(const Mesh<M>& other); 

    /** \brief removes all verts, edges, and faces in this mesh & deletes them*/
    void clear();

    /** \brief Adds a face to the mesh.
     * The face contains the given vertices in order (do not repeat first vertex).
     * Adds required edges and faces. 
     * The edge corresponding to vl(0) -> vl(1) is set to result->edge
     */
    Face* addFace(const VertexList &vl);

    /** \brief Removes a face from the mesh.
     * Deletes edges involed in the face, but not the vertices. 
     */
    void removeFace(Face *f);

    /** \brief Vertex merging function.
     * Merges any vertices that are "equal" according to the 
     * StrictWeakOrdering functor VertLess
     */
    template<typename VertLess>
    void mergeVertices();

    /** \brief Edge merging function.
     * Pairs up half-edges that match each other (i.e. e1.start = e2.end, e1.end = e2.start) 
     *
     * Note that if there are multiple edges between start->end 
     * that match up with an edge, e, from end->start, one of them will be  
     * set to e->sym.  Which one gets matched is undefined.
     */
    void mergeEdges();

    /** \brief Triangulates by ear. 
     * returnst true if any triangles removed */
    bool earTriangulate();

    VertexSet verts;
    EdgeSet edges;
    FaceSet faces;

  protected:
    typedef std::map<Vertex*, Vertex*> VertexMap;
    typedef std::map<Edge*, Edge*> EdgeMap;
    typedef std::map<Face*, Face*> FaceMap;

    // TODO this is a real hack...figure out how to do removeHalfEdge(e) in log or 
    // constant time without the incidence map for weird meshes 
    // (e.g. with articulation points).
    
    // On certain meshes, all edges incident to a vertex v can be found
    // by traversing v.edge->sym pointers, but this is not always
    // the case.
    typedef std::multimap<Vertex*, Edge*> IncidenceMap;
    typedef typename IncidenceMap::value_type Incidence;
    typedef typename IncidenceMap::iterator IncidenceIterator;
    typedef std::pair<typename IncidenceMap::iterator, 
              typename IncidenceMap::iterator> IncidenceRange;

    IncidenceMap m_incidences; // m_incidences[v] holds all edges e with e.start = v

    /** \brief Removes a half-edge from the mesh.
     * If e->start->edge == this, then e->start->edge is set
     * to a different element in the m_startMap;
     *
     * This is a private utility function that does not update 
     * e->face if e->face->edge == e. 
     */
    void removeHalfEdge(Edge *e);

    /** \brief Adds e to the edges set and m_incidenceEdges incidence map */
    void insertHalfEdge(Edge *e);
};


}

#include "MeshImpl.hpp"

#endif
