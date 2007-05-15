#ifndef PLOT_HPP
#define PLOT_HPP
#include <sh/sh.hpp>

/* Returns a fragment shader which plots a graph of the range inclusion evaluation
 * on any square with texcoords in [0,1] x [0,1].
 */
SH::Color3f plot(SH::Program func, const SH::Attrib2f& texcoord, const SH::Attrib1i_f &input_range, 
  const SH::Attrib1f& scale, const SH::Attrib2f& offset); 
#endif
