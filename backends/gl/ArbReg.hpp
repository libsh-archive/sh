#ifndef ARBREG_HPP
#define ARBREG_HPP

#include <string>
#include <iosfwd>

#include "ShRefCount.hpp"

namespace shgl {

/** Possible register types in the ARB spec.
 */
enum ArbRegType {
  SH_ARB_REG_ATTRIB,
  SH_ARB_REG_PARAM,
  SH_ARB_REG_TEMP,
  SH_ARB_REG_ADDRESS,
  SH_ARB_REG_OUTPUT,
  SH_ARB_REG_CONST,
  SH_ARB_REG_TEXTURE
};

/** Possible bindings for a register (see ARB spec).
 */
enum ArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PARAMLOC,
  SH_ARB_REG_PARAMENV,
  // Output
  SH_ARB_REG_RESULTCOL,

  // VERTEX
  // Input
  SH_ARB_REG_VERTEXPOS,
  SH_ARB_REG_VERTEXWGT,
  SH_ARB_REG_VERTEXNRM,
  SH_ARB_REG_VERTEXCOL,
  SH_ARB_REG_VERTEXFOG,
  SH_ARB_REG_VERTEXTEX,
  SH_ARB_REG_VERTEXMAT,
  SH_ARB_REG_VERTEXATR,
  // Output
  SH_ARB_REG_RESULTPOS,
  SH_ARB_REG_RESULTFOG,
  SH_ARB_REG_RESULTPTS, ///< Result point size
  SH_ARB_REG_RESULTTEX,

  // FRAGMENT
  // Input
  SH_ARB_REG_FRAGMENTCOL,
  SH_ARB_REG_FRAGMENTTEX,
  SH_ARB_REG_FRAGMENTFOG,
  SH_ARB_REG_FRAGMENTPOS,
  // Output
  SH_ARB_REG_RESULTDPT,

  SH_ARB_REG_NONE
};

/** An ARB register.
 */
struct ArbReg : public SH::ShRefCountable {
  ArbReg();
  ArbReg(ArbRegType type, int index, std::string name = "");

  ArbRegType type;
  int index;
  std::string name; //< variable name (if any) associated with the register

  union {
    struct {
      ArbRegBinding binding;
      int bindingIndex;
    };
    float values[4];
  };

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);
  
  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const;
};


}

#endif
