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
#include <map>
#include <iterator>
#include <algorithm>
#include <fstream>
#include "Context.hpp"
#include "CfgBlock.hpp"
#include "CtrlGraphWranglers.hpp"
#include "BitSet.hpp"
#include "Attrib.hpp"
#include "VariableNode.hpp"
#include "Record.hpp"
#include "Internals.hpp"
#include "Syntax.hpp"
#include "Optimizations.hpp"
#include "RangeBranchFixer.hpp"

namespace SH  {

typedef std::map<VariableNodePtr, BitSet> UsageMap;

// Sets a = a \union b, empties b, where union denotes the componentwise
// union of corresponding elements in the read/write maps.
void merge(UsageMap &a, UsageMap &b) 
{
  if(b.size() > a.size()) a.swap(b);

  UsageMap::iterator A; 
  UsageMap::iterator B = b.begin();

  // @todo range this can be done in O(m+n) time instead of O((m+n)log(m+n))
  for(; B != b.end(); ++B) {
    A = a.find(B->first);
    if(A == a.end()) a.insert(*B);
    else A->second |= B->second;
  }
  b.clear();
}

UsageMap intersect(UsageMap &a, UsageMap &b)
{
  UsageMap isct;
  std::insert_iterator<UsageMap> isct_inserter(isct, isct.begin());
  std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), 
      isct_inserter, isct.value_comp());

  // intersection copies elements from the first, so we need to 
  // & bit masks with b and throw out empties. 
  //
  // @todo range this can also be done in O(b) time instead of O(rlog(b))
  // if we write the set intersection code ourselves.
  UsageMap::iterator I;
  UsageMap result;
  for(I = isct.begin(); I != isct.end(); ++I) {
    I->second |= b[I->first];
    if(!I->second.empty()) result.insert(result.end(), *I); // const time insertion
  }
  
  return result;
   
}

std::ostream& operator<<(std::ostream &out, const UsageMap &a) 
{
  out << "UsageMap {" << std::endl;
  for(UsageMap::const_iterator I = a.begin(); I != a.end(); ++I) {
    out << "  " << I->first->name() << I->second << std::endl; 
  }
  out << "}" << std::endl;
  return out;
}

struct UsageInfo {
  UsageMap write; // holds variable nodes (masked) used as dest in this structural node

  // adds usage info from the cfg node to this
  void add(CtrlGraphNode* node)
  {
    if(!node || !node->block) return;
    BasicBlockPtr block = node->block;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      addToMap(write, I->dest);
  //    addToMap(use, I->dest);
   //   for(int i = 0; i < opInfo[I->op].arity; ++i) addToMap(use, I->src[i]); 
    }
  }

  private:
    void addToMap(UsageMap &umap, Variable &var) 
    {
      if(var.null()) return;
      UsageMap::iterator I = umap.find(var.node());
      if(I == umap.end()) {
        I = umap.insert(UsageMap::value_type(var.node(), BitSet(var.node()->size()))).first;
      }
      const Swizzle &swiz = var.swizzle();
      BitSet &mask = I->second;
      for(int i = 0; i < swiz.size(); ++i) {
        mask[swiz[i]] = true;
      }
    }
};

void merge(UsageInfo &a, UsageInfo &b)
{
  //merge(a.use, b.use);
  merge(a.write, b.write);
}

std::ostream& operator<<(std::ostream &out, const UsageInfo &a) {
  out << "UsageInfo {" << std::endl;
  //out << "Use " << a.use;
  out << "Write " << a.write;
  out << "}" << std::endl;
  return out;
}

// Checks if its necessary to fix branches
struct RangeBranchChecker {
  bool m_needfix;
  RangeBranchChecker(): m_needfix(false) {}

  void operator()(CtrlGraphNode* node) 
  {
    if (!node || m_needfix) return;
    for(CtrlGraphNode::SuccessorConstIt I = node->successors_begin();
        I != node->successors_end(); ++I) {
      if(isInterval(I->cond.valueType())) {
        m_needfix = true;
        SH_DEBUG_PRINT("Found conditional branch variable " << I->cond.node()->nameOfType() << " " << I->cond.name());
      }
    }
  }

  bool needfix() { return m_needfix; }
};

// Fixes branch/loop code 
// During fixing, the structural graph may no longer be valid for certain fields
// that we don't use (like children, preds)
//
// @todo range - the fixer makes certain assumptions about what structures
// will turn up (i.e. single entry points into certain structures, single exits 
// for other structures).
//
// Will have to make sure this all works with planned control flow features.
//
// @todo range - this should work just as well for affine types later
// once we implement LO, HI, and UNION ops on those.
// (The translation here for loops will work in theory, but further down the line,
// we don't know how to handle them yet when inserting error symbols)
struct RangeBranchFixer {
  CtrlGraph* owner;
  Structural& st; 

  RangeBranchFixer(CtrlGraph* owner, Structural& st): owner(owner), st(st) {}
  // Makes two empty ctrl graph nodes and enters them into the currently
  // parsing program.  
  //
  // This is useful for defining graph transformations.  The two nodes
  // act as a point of insertion for an existing ctrl flow subgraph in 
  // a structural node.
  void makeCfgHolder(CtrlGraphNode* &entry, CtrlGraphNode* &exit) {
    entry = new CtrlGraphNode(owner);
    exit = new CtrlGraphNode(owner);
    Context::current()->parsing()->tokenizer.blockList()->addBlock(
        new CfgBlock(entry, false));
    Context::current()->parsing()->tokenizer.blockList()->addBlock(
        new CfgBlock(exit, false));
  }

  // Fixes any IF (and later LOOP maybe) structural nodes under node (including
  // node itself) and adds usageInfo for the node to usageInfo.
  //
  // (the passed in usageInfo may not be empty)
  //
  // @todo range.  This assumes that the only unreduced nodes are the leaf cfg
  // nodes (i.e. a traversal down structnodes lists will hit the whole
  // structural graph).
  void fix(StructuralNode* node, UsageInfo &usageInfo)
  {
    SH_DEBUG_PRINT("Entering fix " << node); 
    SH_DEBUG_PRINT("Node type = " << node->type);
    switch(node->type) {
      case StructuralNode::UNREDUCED:
        if(node->cfg_node) {
          usageInfo.add(node->cfg_node);
          SH_DEBUG_PRINT("fix UNREDUCED");
          std::cout << usageInfo;
          std::cout << std::endl;
        }
        break;
      case StructuralNode::BLOCK:
        for(StructuralNode::StructNodeList::iterator S = node->structnodes.begin();
            S != node->structnodes.end(); ++S) {
          SH_DEBUG_PRINT("fix BLOCK");
          fix(*S, usageInfo);
        }
        break;
      //case StructuralNode::IF: 
      case StructuralNode::IFELSE:
        fixIf(node, usageInfo);
        break;

      case StructuralNode::SELFLOOP:
        SH_DEBUG_PRINT("Self loop");
        fixSelfloop(node, usageInfo);
        break;

      case StructuralNode::WHILELOOP:
        SH_DEBUG_PRINT("While loop");
        fixWhileloop(node, usageInfo);
        break;
      default:
        SH_DEBUG_ERROR("Unknonw Structural Node type");

    }
  }

  void makeUsageRecord(Record &a, UsageMap &usage) {
    for(UsageMap::iterator U = usage.begin(); U != usage.end(); ++U) {
      SH_DEBUG_PRINT("  saving " << U->first->name());
      a.append(Variable(U->first));
    }
  }

  void makeUsageBackup(Record &acopy, std::string suffix, UsageMap &usage) {
    for(UsageMap::iterator U = usage.begin(); U != usage.end(); ++U) {
      VariableNodePtr newNode = U->first->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, false, false); 
      newNode->name(newNode->name() + suffix); 
      acopy.append(Variable(newNode));
    }
  }

  void recUnion(Record &a, Record &b) {
    Record::iterator A, B; 
    for(A = a.begin(), B = b.begin(); A != a.end(); ++A, ++B) {
      SH_DEBUG_PRINT("Adding a union for " << A->name());
      shUNION(*A, *A, *B);
    }
  }

  void fixIf(StructuralNode* node, UsageInfo &usageInfo) 
  {
    SH_DEBUG_PRINT("fix IFELSE {");
    // Pick out the three struct nodes in the region
    StructuralNode::StructNodeList::iterator R = node->structnodes.begin();
    StructuralNode *condNode, *thenNode, *elseNode; 
    condNode = *R; ++R; 
    thenNode = *R; ++R; 
    elseNode = *R;
    UsageInfo thenUsage, elseUsage;

    // fix branches under each node 
    SH_DEBUG_PRINT("  fixing cond");
    fix(condNode, usageInfo);

    SH_DEBUG_PRINT("  fixing then");
    fix(thenNode, thenUsage);

    SH_DEBUG_PRINT("  fixing else");
    fix(elseNode, elseUsage);

    Variable &cond = condNode->succs.front().first; 
    if(cond.size() > 1) {
      error(Exception("Conditional in SH_IF must be scalar"));
    }
    SH_DEBUG_PRINT("cond variable is " << cond.name());
    if(!isInterval(cond.valueType())) {
      SH_DEBUG_PRINT("} no fix - cond variable is " << cond.name());
      return;
    } 

    // Figure out set of variables saved we need to save if we run the then
    // branch first and set of variables W that both write to and may need
    // merging later.
    //
    // Original structure 
    // IF-ELSE {
    //    condNode
    //    if then branch true, do thenNode
    //    else do elseNode
    // }
    //
    //
    // Transform the graph to look like
    // BLOCK {
    //   condNode 
    //   
    //   condProgram {
    //     trueVar = cond may be true 
    //     falseVar = cond may be false
    //   }
    //
    //   IF-ELSE { // thenStruct
    //      copy S to S' 
    //      if trueVar, do thenNode (with S replaced by S')
    //      else null cfg node 
    //   }
    //   IF-ELSE { // elseStruct
    //      if falseVar, do elseNode 
    //      else null cfg node 
    //   }
    //   merge S' and S into S' 
    //   S = cond(trueVar, S, S')
    // } 
    UsageMap save;
    merge(save, thenUsage.write);
    merge(save, elseUsage.write); 
    SH_DEBUG_PRINT("Intersected usage maps:");
    std::cout << save << std::endl;

    CtrlGraphNode* thenEntry, *thenExit; 
    CtrlGraphNode* elseEntry, *elseExit; 

    // use Program to specify the graph transformation
    int oldOpt = Context::current()->optimization();
    Context::current()->optimization(0);
    Program newGraph = SH_BEGIN_PROGRAM() {
      // generate code to assign to trueVar, falseVar 
      Attrib1f SH_NAMEDECL(trueVar, cond.name() + "_maybetrue");
      Attrib1f SH_NAMEDECL(falseVar, cond.name() + "_maybefalse");

      shLO(trueVar, cond);
      shHI(falseVar, cond);
      falseVar = falseVar <= 0.0f;

      // generate records representing the variables that need to be saved 
      Record saved, savedCopy, temp;
      makeUsageRecord(saved, save);
      makeUsageBackup(savedCopy, "_copy", save);
      makeUsageBackup(temp, "_temp", save);

      // if condition may be true, save variables and execute then branch 
      savedCopy = saved; // assigns all vars in record
      SH_IF(trueVar) {
        makeCfgHolder(thenEntry, thenExit);
      } SH_ENDIF;

      // if condition may be false, execute else branch
      SH_IF(falseVar) {
        // revert saved
        temp = saved;
        saved = savedCopy;
        savedCopy = temp;

        makeCfgHolder(elseEntry, elseExit);
      } SH_ENDIF;

      // now if only one branch executed, the results are in saved, and
      // we're okay.  if both branches exeuted, then we need to merge
      SH_IF(trueVar && falseVar) {
        recUnion(saved, savedCopy);
      } SH_ENDIF;
    } SH_END;
    Context::current()->optimization(oldOpt);

    // do structural analysis on newGraph
    Structural newStruct(newGraph.node()->ctrlGraph);
    st.addStruct(newStruct);
    StructuralNode* newHead = newStruct.head();
    CtrlGraphNode* newEntry = newGraph.node()->ctrlGraph->entry();
    CtrlGraphNode* newExit = newGraph.node()->ctrlGraph->exit();

    // Do some more graph mangling to get things in the right places
    structSplit(condNode, thenNode, newEntry, thenEntry);
    structReplaceExits(thenNode, thenExit, newExit);
    structSplit(condNode, elseNode, newEntry, elseEntry);
    structReplaceExits(elseNode, elseExit, newExit);

    // add newHead to the region
    // (Note, the structnodes no longer matches the expected format for an
    // IF_ELSE, but we assume that since we're doing a bottom up fixing in
    // the structural tree that this node will never need to be fixed
    // again.
    node->structnodes.push_back(newHead);
    node->children.push_back(newHead);
    newHead->parent = node; 

    merge(usageInfo, thenUsage);
    merge(usageInfo, elseUsage);

    SH_DEBUG_PRINT("Usage Info");
    std::cout << usageInfo;
    std::cout << std::endl;
    SH_DEBUG_PRINT("} IFELSE");
  }

  void fixSelfloop(StructuralNode* node, UsageInfo &usageInfo) 
  {
    SH_DEBUG_PRINT("fix SELFLOOP {");
    StructuralNode* selfNode = node->structnodes.front(); 
    UsageInfo selfUsage; 

    // fix branches under each node 
    SH_DEBUG_PRINT("  fixing self");
    fix(selfNode, selfUsage);

    Variable cond; 
    // @todo range - assuming that the self loop is for when the conditions fail since Sh only has do untilloops
    SH_DEBUG_ASSERT(selfNode->succs.size() == 2);
    for(StructuralNode::SuccessorList::iterator S = selfNode->succs.begin();
        S != selfNode->succs.end(); ++S) {
      if(S->second != selfNode) {
        cond = S->first;
        if(cond.null()) {
          SH_DEBUG_PRINT("self loop with null condition...");
          continue;
        }
        break;
      }
    }
    SH_DEBUG_ASSERT(!cond.null());

    if(cond.size() > 1) {
      error(Exception("Conditional in SELFLOOP must be scalar"));
    }

    SH_DEBUG_PRINT("  cond variable is " << cond.name());
    if(!isInterval(cond.valueType())) {
      SH_DEBUG_PRINT("} no fix - cond variable is " << cond.name());
      return;
    } 

    // Figure out set of variables saved we need to save if we run the then
    // branch first and set of variables W that both write to and may need
    // merging later.
    //
    // Original structure 
    // SELFLOOP do { 
    //  stuff
    // } while cond is true
    //
    //
    // Transform the graph to look like
    // Let S be the set of variables written to in the loop
    // Use S' to hold all possible results after the loop.  
    // SELFLOOP do {
    //  stuff
    //  if cond may be false
    //    merge S with S' 
    // } while cond may be true 
    SH_DEBUG_PRINT("Self usage maps:");
    std::cout << selfUsage << std::endl;

    CtrlGraphNode* selfEntry, selfExit; 

    // use Program to specify the graph transformation
    int oldOpt = Context::current()->optimization();
    Context::current()->optimization(0);
    Program newGraph = SH_BEGIN_PROGRAM() {
      // generate code to assign to trueVar, falseVar 
      Attrib1f SH_NAMEDECL(maybeVar, cond.name() + "_maybe"); // may be true
      Attrib1f SH_NAMEDECL(trueVar, cond.name() + "_true"); // must be true 
      Attrib1f SH_DECL(first) = 1.0f;

      // generate records representing the variables that need to be saved 
      Record saved, result; 
      makeUsageRecord(saved, selfUsage.write);
      makeUsageBackup(result, "_result", selfUsage.write);

      SH_DO {
        makeCfgHolder(selfEntry, selfExit);

        shHI(maybeVar, cond);
        shLO(trueVar, cond);
        SH_IF(maybeVar) { // if could exit loop, then need to merge output with results
          // could unroll instead of doing this
          SH_IF(first) {
            result = saved;
            first = 0.0f;
          } SH_ELSE {
            recUnion(result, saved);
          } SH_ENDIF;
        } SH_ENDIF;
      } SH_UNTIL(trueVar) // must be true
      saved = result;
    } SH_END;
    Context::current()->optimization(oldOpt);

    std::ostringstream sout; 
    newGraph.node()->ctrlGraph->graphviz_dump(sout);
    dotGen(sout.str(), "selfloop");

    // do structural analysis on newGraph
    Structural newStruct(newGraph.node()->ctrlGraph);
    st.addstruct(newStruct);
    StructuralNode* newHead = newStruct.head();
    CtrlGraphNode* newEntry = newGraph.node()->ctrlGraph->entry();
    CtrlGraphNode* newExit = newGraph.node()->ctrlGraph->exit();

    // Do some more graph mangling to get things in the right places
    structReplaceExits(selfNode, selfExit, newExit); 
    structReplaceEntries(selfNode, newEntry, selfEntry); 

    std::ostringstream sout2; 
    newGraph.node()->ctrlGraph->graphviz_dump(sout2);
    dotGen(sout2.str(), "selfloop-after");

    // add newHead to the region
    // (Note, the structnodes no longer matches the expected format for an
    // SELFLOOP, but we assume that since we're doing a bottom up fixing in
    // the structural tree that this node will never need to be fixed
    // again.
    node->structnodes.push_back(newHead);

    merge(usageInfo, selfUsage);

    SH_DEBUG_PRINT("Usage Info");
    std::cout << usageInfo;
    std::cout << std::endl;
    SH_DEBUG_PRINT("} SELFLOOP");
  }
  
  void fixWhileloop(StructuralNode* node, UsageInfo &usageInfo) 
  {
    SH_DEBUG_PRINT("fix WHILELOOP {");
    StructuralNode* condNode = node->structnodes.front(); 
    StructuralNode* bodyNode = node->structnodes.back(); 
    UsageInfo condUsage, bodyUsage; 

    // fix branches under each node 
    SH_DEBUG_PRINT("  fixing cond");
    fix(condNode, condUsage);

    SH_DEBUG_PRINT("  fixing body");
    fix(bodyNode, bodyUsage);

    Variable cond; 
    // @todo range - assuming that the cond is in the branch from condNode to
    // bodyNode 
    SH_DEBUG_ASSERT(bodyNode->preds.size() == 1);
    cond = bodyNode->preds.front().first;
    SH_DEBUG_ASSERT(!cond.null());

    if(cond.size() > 1) {
      error(Exception("Conditional in WHILELOOP must be scalar"));
    }

    SH_DEBUG_PRINT("  cond variable is " << cond.name());
    if(!isInterval(cond.valueType())) {
      SH_DEBUG_PRINT("} no fix - cond variable is " << cond.name());
      return;
    } 

    // Figure out set of variables saved we need to save if we run the then
    // branch first and set of variables W that both write to and may need
    // merging later.
    //
    // Original structure 
    // WHILELOOP { 
    //  condNode
    //  break if !cond
    //  bodyNode
    // } 
    //
    // Transform the graph to look like
    // Let S be the set of variables written to in the loop
    // Use S' to hold all possible results after the loop.  
    // WHILELOOP {
    //  condNode
    //  if cond may be false, S' = union(S, S')
    //  break if cond must be false
    //  bodyNode
    // } 
    SH_DEBUG_PRINT("usage maps:");
    std::cout << condUsage << std::endl;
    std::cout << bodyUsage << std::endl;

    UsageInfo usage;
    merge(usage, condUsage);
    merge(usage, bodyUsage);

    CtrlGraphNode* condEntry, condExit; 
    CtrlGraphNode* bodyEntry, bodyExit; 

    // use Program to specify the graph transformation
    int oldOpt = Context::current()->optimization();
    Context::current()->optimization(0);
    Program newGraph = SH_BEGIN_PROGRAM() {
      // generate code to assign to trueVar, falseVar 
      Attrib1f SH_NAMEDECL(maybeVar, cond.name() + "_maybe"); // may be false 
      Attrib1f SH_NAMEDECL(falseVar, cond.name() + "_true"); // must be false 

      // generate records representing the variables that need to be saved 
      Record saved, result; 
      makeUsageRecord(saved, usage.write);
      makeUsageBackup(result, "_result", usage.write);

      // @todo range - once BREAK is ready, shouldn't really reach into the
      // internals like this
      Attrib1f SH_DECL(first) = 1.0f;
      bool arg = SH_PUSH_ARG_QUEUE && SH_PUSH_ARG; // beginning of while
        makeCfgHolder(condEntry, condExit);
        shLO(maybeVar, cond);
        maybeVar = maybeVar <= 0.0;
        shHI(falseVar, cond);
        falseVar = falseVar <= 0.0;

        SH_IF(maybeVar) {
          SH_IF(first) {
            result = saved;
            first = 0.0;
          } SH_ELSE {
            recUnion(result, saved);
          } SH_ENDIF;
        } SH_ENDIF;

        bool internal_cond; // dummy var since we're parsing
        arg = arg && SH_PROCESS_ARG(falseVar, &internal_cond);

      ::SH::while(arg); {
        makeCfgHolder(bodyEntry, bodyExit);
      } ::SH::endWhile();
      saved = result;
    } SH_END;
    Context::current()->optimization(oldOpt);

    std::ostringstream sout; 
    newGraph.node()->ctrlGraph->graphviz_dump(sout);
    dotGen(sout.str(), "whileloop");

    // do structural analysis on newGraph
    Structural newStruct(newGraph.node()->ctrlGraph);
    st.addStruct(newStruct);
    StructuralNode* newHead = newStruct.head();
    CtrlGraphNode* newEntry = newGraph.node()->ctrlGraph->entry();
    CtrlGraphNode* newExit = newGraph.node()->ctrlGraph->exit();

    // Do some more graph mangling to get things in the right places
    structReplaceExits(condNode, condExit, newExit); 
    structReplaceEntries(condNode, newEntry, condEntry); 
    structSplit(condNode, bodyNode, condExit, bodyEntry);
    structSplit(bodyNode, condNode, bodyExit, condEntry);

    std::ostringstream sout2; 
    newGraph.node()->ctrlGraph->graphviz_dump(sout2);
    dotGen(sout2.str(), "whileloop-after");

    // add newHead to the region
    // (Note, the structnodes no longer matches the expected format for an
    // WHILELOOP, but we assume that since we're doing a bottom up fixing in
    // the structural tree that this node will never need to be fixed
    // again.
    node->structnodes.push_back(newHead);

    merge(usageInfo, usage);

    SH_DEBUG_PRINT("Usage Info");
    std::cout << usageInfo;
    std::cout << std::endl;
    SH_DEBUG_PRINT("} WHILELOOP");
  }
};

bool fixRangeBranches(ProgramNodePtr p) {
  RangeBranchChecker rbc;
  p->ctrlGraph->dfs(rbc);
  if(!rbc.needfix()) return false;

  Structural structural(p->ctrlGraph);
  UsageInfo ui;
  RangeBranchFixer rbf(p->ctrlGraph, structural);
  rbf.fix(structural.head(), ui);
  return true;
}

bool fixRangeBranches(Program &p) {
  return fixRangeBranches(p.node());
}

}
