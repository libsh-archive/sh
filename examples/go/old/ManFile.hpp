#ifndef MAN_FILE_HPP
#define MAN_FILE_HPP
#include "Man.hpp"

/* Reads all paths in the SVG file and build a curve */
Curve2D m_svg_crv(const std::string& filename); 

/* Reads in the patch format of the original teapot/teaset */ 
Surface3D m_read_tea_surface(const char* filename);
#endif
