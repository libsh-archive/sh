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
#ifndef SHUTIL_SHMESHIMPL_HPP
#define SHUTIL_SHMESHIMPL_HPP

#include <iostream>
#include <string>
#include <map>
#include "ShError.hpp"
#include "ShException.hpp"
#include "ShMesh.hpp"

namespace ShUtil {

template<typename VT, typename ET, typename FT>
ShMeshVertex<VT, ET, FT>::ShMeshVertex()
  : edge(0) {}

template<typename VT, typename ET, typename FT>
ShMeshVertex<VT, ET, FT>::ShMeshVertex(const VT &data)
  : edge(0), data(data) {}

template<typename VT, typename ET, typename FT>
ShMeshVertex<VT, ET, FT>::ShMeshVertex(const ShMeshVertex<VT, ET, FT> &other)
  : edge(0), data(other.data) {}

template<typename VT, typename ET, typename FT>
ShMeshEdge<VT, ET, FT>::ShMeshEdge()
  : start(0), end(0), face(0), next(0), prev(0), sym(0) {}

template<typename VT, typename ET, typename FT>
ShMeshEdge<VT, ET, FT>::ShMeshEdge(ShMeshVertex<VT, ET, FT> *start,
    ShMeshVertex<VT, ET, FT> *end, ShMeshFace<VT, ET, FT> *face,
    ShMeshEdge<VT, ET, FT> *next, ShMeshEdge<VT, ET, FT> *prev,
    ShMeshEdge<VT, ET, FT> *sym) 
  : start(start), end(end), face(face), next(0), prev(0), sym(0) { 
  setNext(next);
  setPrev(prev);
  setSym(sym);
} 

template<typename VT, typename ET, typename FT>
ShMeshEdge<VT, ET, FT>::ShMeshEdge(ShMeshVertex<VT, ET, FT> *start,
    ShMeshVertex<VT, ET, FT> *end, ShMeshFace<VT, ET, FT> *face,
    ShMeshEdge<VT, ET, FT> *next, ShMeshEdge<VT, ET, FT> *prev,
    ShMeshEdge<VT, ET, FT> *sym, const ET &data)
  : start(start), end(end), face(face), next(0), prev(0), sym(0), data(data) { 
  setNext(next);
  setPrev(prev);
  setSym(sym);
} 

template<typename VT, typename ET, typename FT>
ShMeshEdge<VT, ET, FT>::ShMeshEdge(const ShMeshEdge<VT, ET, FT> &other)
  : start(0), end(0), face(0), next(0), prev(0), sym(0), data(other.data) {}

template<typename VT, typename ET, typename FT>
ShMeshEdge<VT, ET, FT>::~ShMeshEdge() {
  setNext(0);
  setPrev(0);
  setSym(0);
  if( start->edge == this ) {
    // TODO figure out what to do
  }
}

template<typename VT, typename ET, typename FT>
void ShMeshEdge<VT, ET, FT>::setNext(ShMeshEdge<VT, ET, FT> *n) {
  if( next ) next->prev = 0;
  next = n;
  if( next ) next->prev = this;
}

template<typename VT, typename ET, typename FT>
void ShMeshEdge<VT, ET, FT>::setPrev(ShMeshEdge<VT, ET, FT> *p) {
  if( prev ) prev->next = 0;
  prev = p;
  if( prev ) prev->next = this;
}

template<typename VT, typename ET, typename FT>
void ShMeshEdge<VT, ET, FT>::setSym(ShMeshEdge<VT, ET, FT> *e) {
  if( sym ) sym->sym = 0;
  sym = e;
  if( sym ) sym->sym = this;
}

template<typename VT, typename ET, typename FT>
ShMeshFace<VT, ET, FT>::ShMeshFace()
  : edge(0) {}

template<typename VT, typename ET, typename FT>
ShMeshFace<VT, ET, FT>::ShMeshFace(const ShMeshFace<VT, ET, FT> &other)
  : edge(0), data(other.data) {}

template<typename VT, typename ET, typename FT>
ShMesh<VT, ET, FT>::ShMesh() {}

template<typename VT, typename ET, typename FT>
ShMesh<VT, ET, FT>::ShMesh(const ShMesh<VT, ET, FT> &other) {
  *this = other;
}

template<typename VT, typename ET, typename FT>
ShMesh<VT, ET, FT>::~ShMesh() {
  clear();
}

template<typename VT, typename ET, typename FT>
ShMesh<VT, ET, FT>& ShMesh<VT, ET, FT>::operator=(const ShMesh<VT, ET, FT> &other) {
  // TODO switch to hash_maps for O(V + E + F) runtime instead of
  // O(VlogV + ElogE + FlogF) run-time.
  VertMap vmap;
  EdgeMap emap;
  FaceMap fmap;

  SH_DEBUG_WARN("Assignment");
  clear();

  // mappings for null pointers
  vmap[0] = 0;
  emap[0] = 0;
  fmap[0] = 0;

  // make copies
  for(typename EdgeSet::iterator J = other.edges.begin(); J != other.edges.end(); ++J) {
    EdgeType* newedge = new EdgeType(**J); 
    edges.insert(newedge);
    emap[*J] = newedge; 
  }

  for(typename VertexSet::iterator I = other.verts.begin(); I != other.verts.end(); ++I) {
    VertexType* newvert = new VertexType(**I); 
    verts.insert(newvert);
    vmap[*I] = newvert;
    newvert->edge = emap[(*I)->edge];
  }

  for(typename FaceSet::iterator K = other.faces.begin(); K != other.faces.end(); ++K) {
    FaceType* newface = new FaceType(**K); 
    faces.insert(newface);
    fmap[*K] = newface; 
    newface->edge = emap[(*K)->edge]; 
  }

  for(typename EdgeSet::iterator J = other.edges.begin(); J != other.edges.end(); ++J) {
    emap[*J]->start = vmap[(*J)->start]; 
    emap[*J]->end = vmap[(*J)->end]; 
    emap[*J]->face = fmap[(*J)->face]; 
    emap[*J]->sym = emap[(*J)->sym]; 
    emap[*J]->prev = emap[(*J)->prev]; 
    emap[*J]->next  = emap[(*J)->next]; 
  }

  return *this;
}

template<typename VT, typename ET, typename FT>
void ShMesh<VT, ET, FT>::clear() {
  for(typename VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    delete (*I);
  }
  verts.clear();

  for(typename EdgeSet::iterator J = edges.begin(); J != edges.end(); ++J) {
    delete (*J);
  }
  edges.clear();

  for(typename FaceSet::iterator K = faces.begin(); K != faces.end(); ++K) {
    delete (*K);
  }
  faces.clear();
}

template<typename VT, typename ET, typename FT>
typename ShMesh<VT, ET, FT>::FaceType* 
ShMesh<VT, ET, FT>::addFace(const ShMesh<VT, ET, FT>::VertexList &vl) {
  verts.insert(vl.begin(), vl.end());  

  if( vl.size() < 2 ) {
    SH::ShError(SH::ShException("ShMesh::addFace can only handle faces with >= 2 vertices")); 
  }
  FaceType *newf = new FaceType();
  faces.insert(newf);

  EdgeType *newe = 0;
  for(typename VertexList::const_iterator I = vl.begin(); I != vl.end();) {
    VertexType *start = *(I++);
    VertexType *end = (I == vl.end() ? newf->edge->start : *I); 

    newe = new EdgeType(start, end, newf, 0, newe, 0);
    if( !newf->edge ) newf->edge = newe; // assign first edge to newf->edge
    edges.insert(newe);
  }
  newf->edge->setPrev(newe); // close the loop
  return newf;
}

template<typename VT, typename ET, typename FT>
void ShMesh<VT, ET, FT>::removeFace(FaceType *f) {
  EdgeType *olde;
  EdgeType *e = f->edge;
  do {
    edges.erase(e);
    olde = e;
    e = e->next;
    delete olde;
  } while( e != f->edge );
  faces.erase(f);
  delete f;
}

template<typename VT, typename ET, typename FT>
template<typename VertLess>
void ShMesh<VT, ET, FT>::mergeVertices() {
  typedef std::map<VertexType*, VertexType*, VertLess> MergedVertMap;
  MergedVertMap mvmap;

  // keep only the first occurrence of a similar vertex
  for(typename VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    if( mvmap.count(*I) == 0 ) mvmap[*I] = *I;
  }
  SH_DEBUG_PRINT("uh-huh");

  for(typename EdgeSet::iterator J = edges.begin(); J != edges.end(); ++J) {
    (*J)->start = mvmap[(*J)->start];
    (*J)->end = mvmap[(*J)->end];
  }

  /* Go through and erase dead vertices 
   * (keep the first occurence which is the mvmap key,
   * later occurences can be safely deleted since they do not occur
   * as a key or value in the mvmap)*/ 
  for(typename VertexSet::iterator I = verts.begin(); I != verts.end();) {
    if( mvmap[*I] != *I ) {
      typename VertexSet::iterator deadI = I; 
      ++I;
      VertexType *deadVert = *deadI;
      verts.erase(deadI);
      delete deadVert;
    } else {
      ++I;;
    }
  }
  SH_DEBUG_PRINT("duh duh duh");
}

template<typename VT, typename ET, typename FT>
template<typename VertLess> 
void ShMesh<VT, ET, FT>::mergeEdges() {
  typedef std::map<VertexType*, std::map<VertexType*, EdgeType*, VertLess>, VertLess> EdgeMatchMap;

  EdgeMatchMap edgeMatch;

  for(typename EdgeSet::iterator J = edges.begin(); J != edges.end(); ++J) {
    EdgeType *e = (*J);
    EdgeType *match = edgeMatch[e->end][e->start]; 

    if( match != 0 ) match->setSym(e);

    if( edgeMatch[e->start][e->end] != 0 ) {
      SH_DEBUG_WARN("Duplicate edge found in mesh");
    }
    edgeMatch[e->start][e->end] = e;
  }
}

template<typename VT, typename ET, typename FT>
bool ShMesh<VT, ET, FT>::earTriangulate() {
  bool changed = false;
  for(typename FaceSet::iterator I = faces.begin(); I != faces.end(); ++I) {
    EdgeType *e = (*I)->edge;

    if( e->next->next->next == e ) continue;  // ignore 3-sided faces

    changed = true;
    if( e->next->next == e ) { // remove 2-sided faces
      removeFace(*(I++));
      continue;
    }

    // triangulate face
    FaceType *lastface = *I;
    EdgeType *e0 = e; // first edge in face
    EdgeType *en = e0->prev; // last edge in face 
    for(e = e->next->next; e != en; e = e->next) {
      FaceType *newf = new FaceType();
      newf->edge = e;

      // make edges from e-> start to e0 and e0-> start to e->start
      EdgeType *ee0 = new EdgeType(e->start, e0->start, lastface, 
          e->prev->prev, e->prev, 0, e->data);
      EdgeType *e0e = new EdgeType(e0->start, e->start, newf,
          e, en, 0, e0->data);
      ee0->setSym(e0e);

      edges.insert(ee0);
      edges.insert(e0e);
      faces.insert(newf);

      lastface = newf;
    }
    e->face = lastface;
  }
  return changed;
}

}

#endif
