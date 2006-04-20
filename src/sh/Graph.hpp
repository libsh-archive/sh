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
#ifndef SHGRAPH_HPP 
#define SHGRAPH_HPP

#include <list>
#include <set>
#include <map>
#include <iostream>

namespace SH {

/* Framework for building directed graph classes and
 * algorithms on directed graph classes
 *
 * Built along the same lines as Mesh.hpp, except
 * instead of a half-edge data structure this 
 * uses an ordered edge adjacency list.
 *
 */

// This might be too weird, but
// could make graph operations return other graphs
// as an alternate operation mode
//
// e.g. shortest path would replace the edges with 
// edges that hold an original edge for the first step in the
// shortest path + a weight for total distance of the shortest
// path...
//
// These new graphs would have predefined graphvizdump ops
// so it would be easy to output the shortest-path
// result through graphviz.
// 


// We do not impose any index scheme on vertices/edges 
// (may want integer indexs on verts later to run faster)
// so the user can choose an appropriate indexing scheme
// or build a subclass that uses a specific indexing scheme

template<typename VertexType, typename EdgeType>
struct GraphType {
  typedef VertexType Vertex;
  typedef EdgeType Edge;

};

template<typename G>
struct GraphVertex {
  typedef typename G::Edge Edge;
  typedef std::list<Edge*> EdgeList;

  /** \brief Builds an unmarked graph vertex with no children. */
  GraphVertex();

  /** \brief Clones the vertex.  Default version only copies marked field */
  GraphVertex(const GraphVertex<G> &other);

  std::ostream& graphvizDump(std::ostream& out) const;

  EdgeList edges;
  bool marked;
};

template<typename G>
struct GraphEdge {
  typedef typename G::Vertex Vertex;

  /** \brief Builds an edge with null endpoints 
   */
  GraphEdge(); 
  // TODO: should not have this?

  /** \brief Builds an edge with the given endpoints */
  GraphEdge(Vertex *start, Vertex *end);

  /** \brief Clones the edge, but leaves start, end empty */
  GraphEdge(const GraphEdge<G> &other);

  std::ostream& graphvizDump(std::ostream& out) const;

  Vertex *start;
  Vertex *end;
};

// TODO may want to not bother with memory management - use smart pointers for
// verts/edges
template<typename G>
class Graph {
  public:
    // TODO maybe these typedefs should go somewhere else?
    typedef typename G::Vertex Vertex;
    typedef typename G::Edge Edge;

    typedef std::set<Vertex*> VertexSet;
    typedef std::set<Edge*> EdgeSet;

    typedef std::list<Vertex*> VertexList;
    typedef std::list<Edge*> EdgeList;

    typedef std::pair<Vertex*, Vertex*> VertexPair;

    template<typename T>
    struct VertexMap: public std::map<Vertex*, T> {};

    template<typename T>
    struct VertexPairMap: public std::map<VertexPair, T> {
      T& operator()(Vertex* u, Vertex *v) { return (*this)[VertexPair(u, v)]; }
      const T& operator()(Vertex* u, Vertex *v) const { return (*this)[VertexPair(u, v)]; }
    };

    Graph();

    /** \brief Makes a copy of the graph using operator= */ 
    Graph(const Graph<G> &other);

    /** Destroys Graph and deletes any added vertices/edges */
    ~Graph();

    /** \brief Adds a vertex to the graph */
    void addVertex(Vertex *v);

    /** \brief Adds an edge to the graph.
     * Appends e to the end of e->start's edge list */
    void addEdge(Edge *e);

    /** \brief Removes a vertex from the graph and all incident edges */
    void removeVertex(Vertex *v);

    /** \brief Removes an edge from the graph */
    void removeEdge(Edge *e);

    /** \brief erases all vertices/edges from the graph and deletes them */
    void clear();

    /** \brief clears marks on all vertices */
    void clearMarked();

    /** \brief Clones the vertices and edges in this graph */ 
    Graph<G>& operator=(const Graph<G> &other);

    /** Some useful graph algorithms */
    // TODO: may want to separate these out

    /** \brief Performs a DFS of the graph starting from the given vertex,
     * applying the given functor to each node as it is traversed
     */
    template<typename F>
    void dfs(Vertex *start, F &functor);

    /** \brief Shortest path between two nodes using the given weight functor
     * on edges.
     *
     * W::WeightType must define the type used for weight values. 
     * W::LARGE must be a large constant (larger than any expected shortest path
     * length)
     * BUT for now, LARGE must also be MAX_INT / 2 + longest edge weight
     *
     * W::ZERO is the initalizing for distance of a vert from itself.
     *
     * The second version of the function returns a single shortest path
     * as a sequence of edges
     *
     * Returns W::LARGE if end is not connected to start
     * @{
     */
    /*
    template<typename W>
    W::WeightType dijkstra(Vertex *start, Vertex *end, W &weigher); 

    template<typename W>
    W::WeightType dijkstra(Vertex *start, Vertex *end, W &weigher, EdgeList &path); 
    */
    // @}

    /** \brief Single-Source shortest path using Bellman-Ford (CLRS 24.1)
     * @{
     * */
    template<typename W>
    typename W::WeightType bellmanFord(Vertex *start, Vertex *end, W &weigher, EdgeList *path = 0); 
    // TODO: might want this to return all shortest distances/paths (from the given start)
    // @}

    /** \brief All-pairs shortest path using Floyd-Warshall (CLRS 25.2)
     * Returns result in dist, first edge in a shortest path in path
     *
     * Note, if you use bool for a weighttype, this produces a 
     * reasonably efficient transitive closure implementation...
     *
     * @{
     */

    /* Describes a relationship between two vertices by the first step.
     * Mostly used in algorithms for backtracking one step at a time */
    typedef VertexPairMap<Edge*> FirstStepMap;

    template<typename W>
    void floydWarshall(W &weigher, VertexPairMap<typename W::WeightType> &dist, FirstStepMap *path = 0); 
    // @}

    // implemented like floyd warshall as described in CLRS 25.2
    typedef VertexPairMap<bool> TransitiveClosureMap;
    void transitiveClosure(TransitiveClosureMap &tcm);

    // functions on DAGs
   
    // Returns the set of root vertices (those with no incoming edges) 
    void rootSet(VertexSet &roots);
    
    // Finds the height of each node 
    // where height is the longest path from any root
    typedef VertexMap<int> HeightMap;
    void vertexHeight(const VertexSet &roots, HeightMap &heights);


    // Least Common Ancestor of a DAG 
    //
    // TODO implement DAG checking code - just a dfs cycle checker
    //
    // A least common ancestor of two verts x, y 
    // is a common ancestor of x and y, and not the ancestor
    // of any other common ancestor. (paraphrase of definition from 
    // Bender '01).
    //
    // Bender '01 has the fastest known asymptotic behaviour, but
    // it's easier to use the slower transitive closure...so 
    // that's what we use here.
    typedef VertexPairMap<Vertex *> LCAMap;
    void leastCommonAncestor(LCAMap &ancestor);


    VertexSet verts;
    EdgeSet edges;

};

// TODO
// * extract a subgraph from a graph based on a bool functor on edges/verts

/** Utility functions on graphs */

/** \brief does a graphviz dump of the graph
 *
 * D is a functor that has two operators:
 * ostream& operator()(ostream& out, const G::Vertex & v);
 * ostream& operator()(ostream& out, const G::Edge& e);
 * that outputs information per vertex and per edge.
 *
 * The default dump functor uses the built in graphvizDump functions in 
 * GraphVertex and GraphEdge.
 *
 * You can either override the function sin the vertex/edge or here
 * to change the dump behaviour (changing the vertex/edge is useful
 * for changing default dump behvaiour, making a special dumper is 
 * useful for example to visualize the results of running 
 * a specific algorithm on the graph)
 */
template<typename G>
struct GraphDefaultDumper {
  std::ostream& operator()(std::ostream& out, const typename G::Vertex *v);
  std::ostream& operator()(std::ostream& out, const typename G::Edge *e);
};

template<typename G>
std::ostream& graphvizDump(std::ostream &out, const Graph<G> &g); 

template<typename G, typename D>
std::ostream& graphvizDump(std::ostream &out, const Graph<G> &g, D &dumpFunctor); 

}

#include "GraphImpl.hpp"

#endif
