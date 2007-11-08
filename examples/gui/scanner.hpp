#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <map>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>

struct ShaderScanner: public SH::ShRefCountable {
  virtual ~ShaderScanner() {}
  virtual void scan(SH::ShAaScanErrMap& errMap, SH::ShProgram program) = 0;
};
typedef SH::ShPointer<ShaderScanner> ShaderScannerPtr;

struct LightnessScanner: public ShaderScanner { 
  void scan(SH::ShAaScanErrMap& errMap, SH::ShProgram program);
};


class AppWindow;
// A module that scans a program and provides things like the program's section
// tree
class Scanner { 
public:
  /* Initializes a scanner with the associated app window.
   * Scanner updates this when the original fsh changes.
   * This updates the simplified fsh in scanner when simplification occurs */ 
  Scanner(AppWindow& appwindow);

  // sets the shader to scan, reinitializes everything if fsh changes
  void setFsh(SH::ShProgram fsh);

  SH::ShProgram fsh();

  // sets the error computation mechanism to use, reinitializes 
  void setScanner(ShaderScannerPtr scanner);

  /** returns the current selection state of the given statement **/
  bool getState(const SH::ShStatement& stmt) const;

  /** returns the current selection state of the given statement **/
  void setState(const SH::ShStatement& stmt, bool state); 

  /** returns the "error" associated with the given statement for the currently
   * selected scanner */
  double getError(const SH::ShStatement& stmt) const;

  /* Returns a ShSectionTree for the current m_fsh */
  SH::ShSectionTree& getSectionTree();

  /* Produces a new simplified shader   
   * Should be called after changing any state */
  void simplify();
  
protected:
  AppWindow& m_appwindow;

  // called after any of the setFsh, setScanner methods to update the errors 
  // @todo should be a signal
  void update();

  // called by simplify
  // Checks if section needs to be removed.
  // If not, then checks individual statements and subsections.
  // Inserts assignments of result values to 0. 
  void real_simplify(SH::ShSectionNodePtr node);

  SH::ShProgram m_fsh;
  SH::ShSectionTree m_sectionTree;

  ShaderScannerPtr m_scanner;
  SH::ShAaScanErrMap m_errMap;

  /* maps stmt indices to state
   * The state = true for remove, false for keep (or partially keep at
   * least some children). */
  typedef std::map<SH::ShStmtIndex, bool> StateMap;
  StateMap m_stateMap;
};

#endif
