import common, semantic

common.header()

common.guard("SHPOSITION_HPP")
common.inprint('#include "Point.hpp"')
common.namespace()

decl = semantic.Class("Position", "position", "POSITION",
                      """A vertex or fragment position.

This is semantically a point, but is bound preferentially to the position
input and output of the rasterizer or to the vertex position when
used in vertex and fragment shaders.
""", "Point")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHPOSITION_HPP")
