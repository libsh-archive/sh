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
#ifndef SHSECTION_HPP
#define SHSECTION_HPP

#include <map>
#include <list>
#include <iosfwd>
#include "ShDllExport.hpp"
#include "ShInfo.hpp"
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"
#include "ShStructural.hpp"

/** @file ShSectionNode.hpp
 * ities for generating information from nested SECTION blocks in a
 * structural analysis of a control flow graph.
 */

// @todo range
// This is poorly written...clean up if this ever needs to be merged into main

namespace SH {

/* Really just a bastard child of ShStructuralNode.
 * Maintains ShSectionNode information only in a much cleaner form
 * (Might just want to merge this into ShStructuralNode later...)
 */
class ShSectionNode: public ShRefCountable {
  public:
    /** Constructs an empty section */
    ShSectionNode();

    typedef std::set<ShPointer<ShSectionNode> > SectionSet;
    typedef SectionSet::iterator iterator;
    typedef SectionSet::const_iterator const_iterator;

    SectionSet children; ///< Sections nested within this section in the hierarchy 
    ShSectionNode* parent; ///< parent in tree, except for root where this = 0.

    iterator begin() { return children.begin(); }
    iterator end() { return children.end(); }
    const_iterator begin() const { return children.begin(); }
    const_iterator end() const { return children.end(); }
    void addChild(ShPointer<ShSectionNode> child); 

    typedef std::list<ShCtrlGraphNodePtr> CfgNodeList; 
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
    void addCfg(ShCtrlGraphNodePtr cfgNode) { cfgNodes.push_back(cfgNode); }

    /* Returns whether this is the root */
    bool isRoot() const { return !parent; }

    /** Retrieves the STARTSEC and ENDSEC statements for this section */ 
    ShStatement* getStart();
    ShStatement* getEnd();

    /** Retrieves the section name from the STARTSEC */
    std::string name();

    /* Some useful queries about nodes in the tree */ 
    int depth();
};
typedef ShPointer<ShSectionNode> ShSectionNodePtr;

class ShSectionTree {
  public:
    ShSectionNodePtr root;

    ShSectionTree(ShStructural &structural);

    /* DFS through the section tree
     * This is just a copy of the Cfg code...should really refactor */
    template<class F>
    void dfs(F& functor);
     
    /* Retrieves the section containing cfgNode */
    ShSectionNodePtr operator[](ShCtrlGraphNodePtr cfgNode);

    /* Returns whether a section node in this tree contains a particular section
     * node. */ 
    bool contains(ShSectionNodePtr section, ShCtrlGraphNodePtr cfgNode);

    /* Graphviz dump */
    std::ostream& dump(std::ostream& out); 

    /* Graphviz dump with a special functor with two functions:
     *    void operator()(std::ostream&, ShCtrlGraphNodePtr) that outputs a HTML-style label 
     *    for a cfg node
     *
     *    void operator()(std::ostream&, ShSectionNodePtr) that outputs any section specific
     *    attributes (as part of the stmts in a a section's subgraph)
     */
    template<class F>
    std::ostream& dump(std::ostream& out, F& functor);

  private:
    // adds structural subtree rooted at structNode to the section 
    static void makeSection(ShStructuralNodePtr structNode, ShSectionNodePtr section);

    typedef std::map<ShCtrlGraphNodePtr, ShSectionNodePtr> CfgSectionMap;
    CfgSectionMap cfgSection; 
    void gatherCfgSection(ShSectionNodePtr section); 

    /* DFS through the section tree */
    template<class F>
    static void realDfs(ShSectionNodePtr node, F& functor);

    /* Dump a node and subtree using the given functor (functor described above in dump()) 
     *
     * @param out Actual output stream to use
     * @param cfgout Output stream to use for edges that cross section
     *   boundaries (These must be specified after all involved sections are,
     *   otherwise DOT does stupid things with the cfg nodes) 
     * @param node The root of the section subtree to process
     * @param F functor as described above in dump*/
    template<class F>
    void realDump(std::ostream& out, std::ostream& cfgout, ShSectionNodePtr node, F& functor);
};

}

#include "ShSectionImpl.hpp"

#endif
