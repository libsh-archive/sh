#ifndef SHPARSER_HPP
#define SHPARSER_HPP

#include "ShCtrlGraph.hpp"

namespace SH {

class ShParser {
public:
  static void parse(ShCtrlGraphNodePtr& head,
                    ShCtrlGraphNodePtr& tail,
                    ShBlockListPtr blocks);
  
private:
  static void parseStmts(ShCtrlGraphNodePtr& head,
                         ShCtrlGraphNodePtr& tail,
                         ShBlockListPtr blocks);
  static void parseIf(ShCtrlGraphNodePtr& head,
                      ShCtrlGraphNodePtr& tail,
                      ShBlockListPtr blocks);
  static void parseFor(ShCtrlGraphNodePtr& head,
                       ShCtrlGraphNodePtr& tail,
                       ShBlockListPtr blocks);
  static void parseWhile(ShCtrlGraphNodePtr& head,
                         ShCtrlGraphNodePtr& tail,
                         ShBlockListPtr blocks);
  static void parseDo(ShCtrlGraphNodePtr& head,
                      ShCtrlGraphNodePtr& tail,
                      ShBlockListPtr blocks);

  // NOT IMPLEMENTED
  ShParser();
  ShParser(const ShParser&);
  ~ShParser();
};
 
}

#endif
