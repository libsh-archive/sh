#include <iostream>

#include <sh/ShInclusion.hpp>
#include <sh/ShCtrlGraphWranglers.hpp>
#include "appwindow.hpp"
#include "shaderTree.hpp"
#include "scanner.hpp"

using namespace SH;
using namespace ShUtil;

void LightnessScanner::scan(ShAaScanErrMap& errMap, ShProgram program)
{
  // @todo range switch to cc backend for now - gpu stream backend 
  // may not be able to handle programs...
  ShBackendPtr oldBackend = ShEnvironment::backend; 
  shSetBackend("cc");

  // change program type to a stream program 
  // @todo range this should be done somewhere else
  program.target() = "cc:stream";
  errMap = shAaScanLightnessErr(program);

  SH::ShEnvironment::backend = oldBackend; 
}

Scanner::Scanner(AppWindow& appwindow)
  : m_appwindow(appwindow),
    m_scanner(new LightnessScanner())
{
}

void Scanner::setScanner(ShaderScannerPtr scanner)
{
  m_scanner = scanner;
  update();
}

void Scanner::setFsh(ShProgram fsh)
{
  if(m_fsh.node() != fsh.node()) {
    m_fsh = fsh;

    ShStructural sstruct(m_fsh.node()->ctrlGraph);
    m_sectionTree = ShSectionTree(sstruct);

    update();
  }
}

ShProgram Scanner::fsh() {
  return m_fsh;
}

bool Scanner::getState(const ShStatement& stmt) const
{
  const ShStmtIndex* index = stmt.get_info<ShStmtIndex>();
  StateMap::const_iterator I = m_stateMap.find(*index);
  if(I == m_stateMap.end()) {
    return false;
  }
  return I->second; 
}

void Scanner::setState(const ShStatement& stmt, bool state) 
{
  const ShStmtIndex* index = stmt.get_info<ShStmtIndex>();
  if(!index) {
    std::cerr << "Null statement index for " << stmt << std::endl;
    return;
  }
  m_stateMap[*index] = state;
}

double Scanner::getError(const ShStatement& stmt) const
{
  const ShStmtIndex* index = stmt.get_info<ShStmtIndex>();
  if(!index) {
    std::cerr << "Null statement index for " << stmt << std::endl;
    return 0;
  }
  ShAaScanErrMap::const_iterator I = m_errMap.find(*index);
  if(I == m_errMap.end()) {
    std::cerr << "Unable to find error for " << stmt << std::endl;
    return 0;
  }
  return I->second.err;
}

ShSectionTree& Scanner::getSectionTree()
{
  if(m_fsh.node()) {
    std::cerr << "Null fsh" << std::endl;
  }
  return m_sectionTree;
}

void Scanner::simplify()
{
  std::cerr << "Simplifying" << std::endl;

  SH::ShProgram m_simple(m_fsh.node()->clone());
  ShStructural sstruct(m_simple.node()->ctrlGraph);
  ShSectionTree stree = ShSectionTree(sstruct);

  real_simplify(stree.root);

  m_appwindow.update_simplefsh(m_simple);
}

void Scanner::real_simplify(ShSectionNodePtr node)
{
  // check if section removed

  bool removed = false;
  ShStatement* start = node->getStart();
  if(start) {
    removed = getState(*start);
  }
  if(removed) {
    std::cerr << "Removing section " << node->name() << std::endl;
    structRemove(node->structnode);
  } else {
    std::cerr << "Keeping section " << node->name() << std::endl;
    // if not, check children
    for(ShSectionNode::iterator I = node->begin(); I != node->end(); ++I) {
      real_simplify(*I);
    }
    for(ShSectionNode::cfg_iterator C = node->cfgBegin(); C != node->cfgEnd(); ++C) {
      if(!(*C)->block) {
        continue;
      }
      ShBasicBlockPtr block = (*C)->block;
      for(ShBasicBlock::ShStmtList::iterator S = block->begin(); 
          S != block->end();) {
        if(getState(*S)) {
          S = block->erase(S);
        } else {
          ++S;
        }
      }
    }
  }
}

void Scanner::update()
{
  if(!m_fsh.node() || !m_scanner) {
    std::cerr << "Cannot update with a null fsh or scanner!" << std::endl;
    return;
  }

  // do error scanning 
  m_errMap.clear();

  std::cout << "Starting error scan" << std::endl;

  try {
    // @todo debug
  //  m_scanner->scan(m_errMap, m_fsh);
    add_stmt_indices(m_fsh); // @todo debug
  } catch (const SH::ShException &e) {
    std::cerr << "  Sh Exception " << e.message(); 
  }

  std::cout << "Done scanning for errors" << std::endl;
}

