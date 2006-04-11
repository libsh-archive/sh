import common, semantic

common.header()

common.guard("SHATTRIB_HPP")
common.inprint('#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL')
common.inprint('#  define SH_DO_NOT_INCLUDE_GENERIC_IMPL')
common.inprint('#  include "Generic.hpp"')
common.inprint('#  undef SH_DO_NOT_INCLUDE_GENERIC_IMPL')
common.inprint('#else');
common.inprint('#  include "Generic.hpp"')
common.inprint('#endif');
common.namespace()

decl = semantic.Class("Attrib", "attribute", "SH_ATTRIB",
                      """A generic attribute (or parameter) holding N values.

The reason we have the Swizzle template argument is so that
swizzled variables, which need to be copied (i.e. have an ASN
statement generated) when another variable is initialized through
them, may otherwise be simply placed into new variables (through
copy constructor eliding, as per paragraph 12.8.15 of the C++
standard), causing variables to share VariableNodes when they
shouldn't. By making swizzled variables separate types we enforce
calling a conversion constructor instead, which cannot be elided.
If this paragraph confuses you, and you're not modifying Sh
internals, you may safely ignore it.""",
                      "Generic", "<N, T>")

decl.declare_all()

common.endnamespace()

common.inprint('#include "GenericImpl.hpp"')
common.inprint('#include "AttribImpl.hpp"')
common.endguard("SHATTRIB_HPP")
