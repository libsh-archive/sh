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
#ifndef SHSECTION_HPP
#define SHSECTION_HPP

#include <map>
#include <list>
#include <iosfwd>
#include "DllExport.hpp"
#include "Info.hpp"
#include "RefCount.hpp"
#include "CtrlGraph.hpp"
#include "Structural.hpp"

/** @file SectionNode.hpp
 * ities for generating information from nested SECTION blocks in a
 * structural analysis of a control flow graph.
 */

// @todo range
// This is poorly written...clean up if this ever needs to be merged into main

namespace SH {

/* Really just a bastard child of StructuralNode.
 * Maintains SectionNode information only in a much cleaner form
 * (Might just want to merge this into StructuralNode later...)
 */
class SectionNode: public RefCountable {
  public:
    /** Constructs an empty section */
    SectionNode();

    typedef std::set< Pointer<SectionNode> > SectionSet;
    typedef SectionSet::iterator iterator;
    typedef SectionSet::const_iterator const_iterator;

    SectionSet children; ///< Sections nested within this section in the hierarchy 
    SectionNode* parent; ///< parent in tree, except for root where this = 0.

    iterator begin() { return children.begin(); }
    iterator end() { return children.end(); }
    const_iterator begin() const { return children.begin(); }
    const_iterator end() const { return children.end(); }
    void addChild(Pointer<SectionNode> child); 

    typedef std::list<CtrlGraphNode*> CfgNodeList; 
    typedef CfgNodeList::iterator cfg_iterator;
    typedef CfgNodeList::const_iterator const_cfg_iterator;

    /** Cfg nodes immediately under this section 
     * cfgNodes.front() must start with the STARTSEC for this section
     * and cfgNodes.back() must end with the  ENDSEC.
     *
     * Otherwise, there's no particular ordering
     */
    CfgNodeList cfgNodes;  

    cfg_iterator cfgBegin() { return cfgNodes.begin(); }
    cfg_iterator cfgEnd() { return cfgNodes.end(); }
    const_cfg_iterator cfgBegin() const { return cfgNodes.begin(); }
    const_cfg_iterator cfgEnd() const { return cfgNodes.end(); }
    void addCfg(CtrlGraphNode* cfgNode) { cfgNodes.push_back(cfgNode); }

    /* Returns whether this is the root */
    bool isRoot() const { return !parent; }

    /** Retrieves the STARTSEC and ENDSEC statements for this section */ 
    Statement* getStart();
    Statement* getEnd();

    /** Retrieves the section name from the STARTSEC */
    std::string name();

    /* Some useful queries about nodes in the tree */ 
    int depth();
};
typedef Pointer<SectionNode> SectionNodePtr;

class SectionTree {
  public:
    SectionNodePtr root;

    SectionTree(Structural &structural);

    /* DFS through the section tree
     * This is just a copy of the Cfg code...should really refactor */
    template<class F>
    void dfs(F& functor);
     
    /* Retrieves the section containing cfgNode */
    SectionNodePtr operator[](CtrlGraphNode* cfgNode);

    /* Returns whether a section node in this tree contains a particular section
     * node. */ 
    bool contains(const SectionNodePtr& section, CtrlGraphNode* cfgNode);

    /* Graphviz dump */
    std::ostream& dump(std::ostream& out); 

    /* Graphviz dump with a special functor with two functions:
     *    void operator()(std::ostream&, CtrlGraphNodePtr) that outputs a HTML-style label 
     *    for a cfg node
     *
     *    void operator()(std::ostream&, SectionNodePtr) that outputs any section specific
     *    attributes (as part of the stmts in a a section's subgraph)
     */
    template<class F>
    std::ostream& dump(std::ostream& out, F& functor);

  private:
    // adds structural subtree rooted at structNode to the section 
    static void makeSection(const StructuralNodePtr& structNode, SectionNodePtr section);

    typedef std::map<CtrlGraphNode*, SectionNodePtr> CfgSectionMap;
    CfgSectionMap cfgSection; 
    void gatherCfgSection(const SectionNodePtr& section); 

    /* DFS through the section tree */
    template<class F>
    static void realDfs(const SectionNodePtr& node, F& functor);

    /* Dump a node and subtree using the given functor (functor described above in dump()) 
     *
     * @param out Actual output stream to use
     * @param cfgout Output stream to use for edges that cross section
     *   boundaries (These must be specified after all involved sections are,
     *   otherwise DOT does stupid things with the cfg nodes) 
     * @param node The root of the section subtree to process
     * @param F functor as described above in dump*/
    template<class F>
    void realDump(std::ostream& out, std::ostream& cfgout, const SectionNodePtr& node, F& functor);
};

}

#include "SectionImpl.hpp"

#endif
