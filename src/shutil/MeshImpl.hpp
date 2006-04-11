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
#ifndef SHUTIL_SHMESHIMPL_HPP
#define SHUTIL_SHMESHIMPL_HPP

#include <iostream>
#include <string>
#include <map>
#include "sh/Error.hpp"
#include "sh/Exception.hpp"
#include "sh/Debug.hpp"
#include "Mesh.hpp"

namespace ShUtil {

/** MeshVertex method definitions */
template<typename M>
MeshVertex<M>::MeshVertex()
  : edge(0) {}

template<typename M>
MeshVertex<M>::MeshVertex(const MeshVertex<M> &other)
  : edge(0) {}

template<typename M>
MeshFace<M>::MeshFace()
  : edge(0) {}

template<typename M>
MeshFace<M>::MeshFace(const MeshFace<M> &other)
  : edge(0) {}

/** MeshEdge method definitions */
template<typename M>
MeshEdge<M>::MeshEdge()
  : start(0), end(0), face(0), sym(0), next(0), prev(0) {}

template<typename M>
MeshEdge<M>::MeshEdge(const MeshEdge<M> &other)
  : start(0), end(0), face(0), sym(0), next(0), prev(0) {} 

template<typename M>
void MeshEdge<M>::setLinks(Vertex *s, Vertex *e, Face *f,
    Edge *next, Edge *prev, Edge *sym) {
  // TODO Figure out what to do here instead of shellacking
  // the user with a dumb error message.
  if(start || end) {
    DEBUG_WARN("Changing start/end vertex of an edge.  "
      << "This is probably not a good idea.");
  }

  start = s;
  end = e;
  face = f;
  setNext(next);
  setPrev(prev);
  setSym(sym);
} 

template<typename M>
void MeshEdge<M>::setNext(Edge *n) {
  if( next ) next->prev = 0;
  next = n;
  if( next ) next->prev = reinterpret_cast<Edge*>(this);
}

template<typename M>
void MeshEdge<M>::setPrev(Edge *p) {
  if( prev ) prev->next = 0;
  prev = p;
  if( prev ) prev->next = reinterpret_cast<Edge*>(this);
}

template<typename M>
void MeshEdge<M>::setSym(Edge *e) {
  if( sym ) sym->sym = 0;
  sym = e;
  if( sym ) sym->sym = reinterpret_cast<Edge*>(this);
}

/* Mesh method definitions */
template<typename M>
Mesh<M>::Mesh() {}

template<typename M>
Mesh<M>::Mesh(const Mesh<M> &other) {
  *this = other;
}

template<typename M>
Mesh<M>::~Mesh() {
  clear();
}

template<typename M>
Mesh<M>& Mesh<M>::operator=(const Mesh<M> &other) {
  // TODO switch to hash_maps for O(V + E + F) runtime instead of
  // O(VlogV + ElogE + FlogF) run-time.
  VertexMap vmap;
  EdgeMap emap;
  FaceMap fmap;

  DEBUG_WARN("Assignment");
  clear();

  // mappings for null pointers
  vmap[0] = 0;
  emap[0] = 0;
  fmap[0] = 0;

  // make copies
  for(typename EdgeSet::const_iterator J = other.edges.begin(); J != other.edges.end(); ++J) {
    Edge* newedge = new Edge(**J); 
    edges.insert(newedge);
    emap[*J] = newedge; 
  }

  for(typename VertexSet::const_iterator I = other.verts.begin(); I != other.verts.end(); ++I) {
    Vertex* newvert = new Vertex(**I); 
    verts.insert(newvert);
    vmap[*I] = newvert;
    newvert->edge = emap[(*I)->edge];
  }

  for(typename FaceSet::const_iterator K = other.faces.begin(); K != other.faces.end(); ++K) {
    Face* newface = new Face(**K); 
    faces.insert(newface);
    fmap[*K] = newface; 
    newface->edge = emap[(*K)->edge]; 
  }

  for(typename EdgeSet::const_iterator J = other.edges.begin(); J != other.edges.end(); ++J) {
    Edge *e = emap[*J];
    e->start = vmap[(*J)->start]; 
    e->end = vmap[(*J)->end]; 
    e->face = fmap[(*J)->face]; 
    e->sym = emap[(*J)->sym]; 
    e->prev = emap[(*J)->prev]; 
    e->next  = emap[(*J)->next]; 
    m_incidences.insert(Incidence(e->start, e));
  }

  return *this;
}

template<typename M>
void Mesh<M>::clear() {
  for(typename VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    delete (*I);
  }
  verts.clear();

  for(typename FaceSet::iterator K = faces.begin(); K != faces.end(); ++K) {
    delete (*K);
  }
  faces.clear();

  for(typename EdgeSet::iterator J = edges.begin(); J != edges.end(); ++J) {
    delete (*J);
  }
  edges.clear();

  m_incidences.clear();
}

template<typename M>
typename Mesh<M>::Face* 
Mesh<M>::addFace(const typename Mesh<M>::VertexList &vl) {
  verts.insert(vl.begin(), vl.end());  

  if( vl.size() < 1 ) {
    SH::error(SH::Exception("Mesh::addFace can only handle faces with >= 1 vertices")); 
  }
  Face *newf = new Face();
  faces.insert(newf);

  Edge *newe = 0, *olde = 0;
  Vertex *first = vl.front();
  for(typename VertexList::const_iterator I = vl.begin(); I != vl.end();) {
    Vertex *start = *(I++);
    Vertex *end = (I == vl.end() ? first : *I); 

    DEBUG_ASSERT(start);

    newe = new Edge();
    newe->setLinks(start, end, newf, 0, olde, 0);
    olde = newe;

    if( !newf->edge ) newf->edge = newe; // assign first edge to newf->edge
    insertHalfEdge(newe);
  }
  newf->edge->setPrev(newe); // close the loop
  return newf;
}


template<typename M>
void Mesh<M>::removeFace(Face *f) {
  Edge *olde, *e;
  e = f->edge;
  //TODO this may fail if != is redefined to 
  //access pointer contents 
  //In that case, might need to store a list of edges to be deleted
  //instead of deleting as we traverse the next pointers...
  do {
    olde = e;
    e = e->next;
    removeHalfEdge(e);
  } while( e != f->edge );
  faces.erase(f);
  delete f;
}

template<typename M>
template<typename VertLess>
void Mesh<M>::mergeVertices() {
  typedef std::map<Vertex*, Vertex*, VertLess> MergedVertMap;
  MergedVertMap mvmap;

  // keep only the first occurrence of a similar vertex
  for(typename VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    if( mvmap.count(*I) == 0 ) mvmap[*I] = *I;
  }

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
      Vertex *deadVert = *deadI;
      verts.erase(deadI);

      // fix incidence map
      IncidenceRange ir = m_incidences.equal_range(deadVert);
      for(IncidenceIterator K = ir.first; K != ir.second; ++K) {
        m_incidences.insert(Incidence(mvmap[K->first], K->second));
      }
      m_incidences.erase(ir.first, ir.second);

      delete deadVert;
    } else {
      ++I;;
    }
  }
}

template<typename M>
void Mesh<M>::mergeEdges() {
  typedef std::map<Vertex*, std::map<Vertex*, Edge*> > EdgeMatchMap;

  EdgeMatchMap edgeMatch;

  for(typename EdgeSet::iterator J = edges.begin(); J != edges.end(); ++J) {
    Edge *e = (*J);
    Edge *match = edgeMatch[e->end][e->start]; 

    if(match) match->setSym(e);

    if( edgeMatch[e->start][e->end] != 0 ) {
      DEBUG_WARN("Duplicate edge found in mesh");
    }
    edgeMatch[e->start][e->end] = e;
  }
}

template<typename M>
bool Mesh<M>::earTriangulate() {
  bool changed = false;
  for(typename FaceSet::iterator I = faces.begin(); I != faces.end(); ++I) {
    Edge *e = (*I)->edge;

    if( e->next->next->next == e ) continue;  // ignore 3-sided faces

    changed = true;
    if( e->next == e || e->next->next == e ) { // remove 1-sided and 2-sided faces
      removeFace(*(I++));
      continue;
    }

    // triangulate face
    Face *lastface = *I;
    Edge *e0 = e; // first edge in face
    Edge *en = e0->prev; // last edge in face 
    for(e = e->next->next; e != en; e = e->next) {
      Face *newf = new Face();
      newf->edge = e;

      // make edges from e-> start to e0 and e0-> start to e->start
      Edge *ee0 = new Edge(*e);
      ee0->setLinks(e->start, e0->start, lastface, e->prev->prev, e->prev, 0); 

      Edge *e0e = new Edge(*e0);
      e0e->setLinks(e0->start, e->start, newf, e, en, 0); 

      ee0->setSym(e0e);

      insertHalfEdge(ee0);
      insertHalfEdge(e0e);
      faces.insert(newf);

      lastface = newf;
    }
    e->face = lastface;
  }
  return changed;
}

template<typename M>
void Mesh<M>::removeHalfEdge(Edge *e) {
  edges.erase(e);
  e->setNext(0);
  e->setPrev(0);
  e->setSym(0);

  // Update incidence map and e->start
  IncidenceRange ir = m_incidences.equal_range(e->start);
  for(IncidenceIterator I = ir.first; I != ir.second; ++I) {
    if(I->second == e) {
      m_incidences.erase(I);
      break;
    }
  }
  if(e->start->edge == e)  {
    if(m_incidences.count(e->start) > 0) {
      e->start->edge = m_incidences.find(e->start)->second;
    } else {
      e->start->edge = 0; 
    }
  }
  delete e;
}

template<typename M>
void Mesh<M>::insertHalfEdge(Edge *e) {
  edges.insert(e);
  DEBUG_ASSERT(e->start);
  m_incidences.insert(Incidence(e->start, e));
}

}

#endif
