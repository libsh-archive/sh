// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "ShImage.hpp"
#include <string>
#include <cstring>
#include <cstdio>
#include <png.h>
#include <sstream>
#include "ShException.hpp"
#include "ShError.hpp"

namespace SH {

ShImage::ShImage()
  : m_width(0), m_height(0), m_depth(0), m_data(0)
{
}

ShImage::ShImage(int width, int height, int depth)
  : m_width(width), m_height(height), m_depth(depth),
    m_data(new float[m_width * m_height * m_depth])
{
}

ShImage::ShImage(const ShImage& other)
  : m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth),
    m_data(new float[m_width * m_height * m_depth])
{
  std::memcpy(m_data, other.m_data, m_width * m_height * m_depth * sizeof(float));
}

ShImage::~ShImage()
{
  delete [] m_data;
}

ShImage& ShImage::operator=(const ShImage& other)
{
  delete [] m_data;
  m_width = other.m_width;
  m_height = other.m_height;
  m_depth = other.m_depth;
  m_data = new float[m_width * m_height * m_depth];
  std::memcpy(m_data, other.m_data, m_width * m_height * m_depth * sizeof(float));
  return *this;
}

int ShImage::width() const
{
  return m_width;
}

int ShImage::height() const
{
  return m_height;
}

int ShImage::depth() const
{
  return m_depth;
}

float ShImage::operator()(int x, int y, int i) const
{
  return m_data[m_depth * (m_width * y + x) + i];
}

float& ShImage::operator()(int x, int y, int i)
{
  return m_data[m_depth * (m_width * y + x) + i];
}

void ShImage::loadPng(const std::string& filename)
{
  // check that the file is a png file
  png_byte buf[8];

  FILE* in = std::fopen(filename.c_str(), "rb");

  if (!in) ShError( ShImageException("Unable to open " + filename) );
  
  for (int i = 0; i < 8; i++) {
    if (!(buf[i] = fgetc(in))) ShError( ShImageException("Not a PNG file") );
  }
  if (png_sig_cmp(buf, 0, 8)) ShError( ShImageException("Not a PNG file") );

  png_structp png_ptr =
    png_create_read_struct(PNG_LIBPNG_VER_STRING,
			   0, 0, 0); // FIXME: use error handlers
  if (!png_ptr) ShError( ShImageException("Error initialising libpng (png_ptr)") );

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    ShError( ShImageException("Error initialising libpng (info_ptr)") );
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    ShError( ShImageException("Error initialising libpng (setjmp/lngjmp)") );    
  }
  
  //  png_set_read_fn(png_ptr, reinterpret_cast<void*>(&in), my_istream_read_data);
  png_init_io(png_ptr, in);
  
  png_set_sig_bytes(png_ptr, 8);

  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);

  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  if (bit_depth % 8) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    std::ostringstream os;
    os << "Invalid bit depth " << bit_depth;
    ShError( ShImageException(os.str()) );
  }

  int colour_type = png_get_color_type(png_ptr, info_ptr);

  if (colour_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    colour_type = PNG_COLOR_TYPE_RGB;
  }

  if (colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_gray_1_2_4_to_8(png_ptr);
  }

  if (colour_type != PNG_COLOR_TYPE_RGB
      && colour_type != PNG_COLOR_TYPE_GRAY
      && colour_type != PNG_COLOR_TYPE_RGBA) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    ShError( ShImageException("Invalid colour type") );
  }

  delete [] m_data;

  m_width = png_get_image_width(png_ptr, info_ptr);
  m_height = png_get_image_height(png_ptr, info_ptr);
  switch (colour_type) {
  case PNG_COLOR_TYPE_RGB:
    m_depth = 3;
    break;
  case PNG_COLOR_TYPE_RGBA:
    m_depth = 4;
    break;
  default:
    m_depth = 1;
    break;
  }
  
  png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

  m_data = new float[m_width * m_height * m_depth];

  for (int y = 0; y < m_height; y++) {
    for (int x = 0; x < m_width; x++) {
      for (int i = 0; i < m_depth; i++) {
	png_byte *row = row_pointers[y];
	int index = m_depth * (y * m_width + x) + i;
	long data = 0;
	for (int j = 0; j < bit_depth/8; j++) {
	  data <<= 8;
	  data += row[(x * m_depth + i) * bit_depth/8 + j];
	}
	m_data[index] = data / static_cast<float>((1 << bit_depth) - 1);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
}

const float* ShImage::data() const
{
  return m_data;
}

}
