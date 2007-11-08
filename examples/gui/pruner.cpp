#include <iostream>

#include <sh/ShInclusion.hpp>
#include "shaderTree.hpp"

using namespace SH;
using namespace ShUtil;

void LightnessScanner::scan(ShAaScanErrMap& errMap, ShProgram program)
{
  errMap = shAaScanLightnessErr(program);
}

Scanner::Scanner()
  : m_scanner(new LightnessScanner())
{
}

void Scanner::setScanner(ShaderScannerPtr scanner)
{
  m_scanner = scanner;
  update();
}

void Scanner::setFsh(ShProgram fsh)
{
  if(m_fsh != fsh) {
    m_fsh = fsh;
    update();
  }
}

Scanner::StmtState Scanner::getState(const ShStatement& stmt) const
{
  const ShStmtIndex* index = stmt.get_info<ShStmtIndex>();
  StateMap::const_iterator I = m_stateMap.find(*index);
  if(I == m_stateMap.end()) {
    return KEEP;
  }
  return I->second; 
}

void Scanner::setState(const SH::ShStatement& stmt, StmtState state) 
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
  ShStmtIndex* index = stmt.get_info<ShStmtIndex>();
  if(!index) {
    std::cerr << "Null statement index for " << stmt << std::endl;
    return 0;
  }
  ShAaScanErrMap::const_iterator I = m_errMap.find(*index);
  if(I == m_errMap.end()) {
    std::cer << "Unable to find error for " << stmt << std::endl;
    return 0;
  }
  return I->second;
}

void Scanner::update()
{
  if(!m_fsh.node() || !m_scanner) {
    std::cerr << "Cannot update with a null fsh or scanner!" << std::endl;
    return;
  }

  // do error scanning 
  errMap.clear();

  m_scanner->scan(errMap, m_fsh);

  std::cout << "Done scanning for errors" << std::endl;
}

