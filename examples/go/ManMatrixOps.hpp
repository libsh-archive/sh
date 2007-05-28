#ifndef MANMATRIX_OPS_HPP
#define MANMATRIX_OPS_HPP

#include <vector>
#include <sh/sh.hpp>
#include "man.hpp"
#include "ManMatrix.hpp"

/* These are the analog of LibMatrix.hpp in the sh library */

ManMatrix m_scale(const Man& s); 
ManMatrix m_translate(const Man& t); 
ManMatrix m_rotate(const Man& axis, const Man& angle); 
ManMatrix mul(const ManMatrix& a, const ManMatrix& b);



#endif
