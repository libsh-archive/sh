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
#include "ShDebug.hpp"

namespace SH {

ShImage::ShImage()
  : m_width(0), m_height(0), m_elements(0), m_memory(0)
{
}

ShImage::ShImage(int width, int height, int elements)
  : m_width(width), m_height(height), m_elements(elements),
    m_memory(new ShHostMemory(sizeof(float) * m_width * m_height * m_elements))
{
}

ShImage::ShImage(const ShImage& other)
  : m_width(other.m_width), m_height(other.m_height), m_elements(other.m_elements),
    m_memory(other.m_memory ? new ShHostMemory(sizeof(float) * m_width * m_height * m_elements) : 0)
{
  if (m_memory) {
    std::memcpy(m_memory->hostStorage()->data(),
                other.m_memory->hostStorage()->data(),
                m_width * m_height * m_elements * sizeof(float));
  }
}

ShImage::~ShImage()
{
}

ShImage& ShImage::operator=(const ShImage& other)
{
  m_width = other.m_width;
  m_height = other.m_height;
  m_elements = other.m_elements;
  m_memory = (other.m_memory ? new ShHostMemory(sizeof(float) * m_width * m_height * m_elements) : 0);
  std::memcpy(m_memory->hostStorage()->data(),
              other.m_memory->hostStorage()->data(),
              m_width * m_height * m_elements * sizeof(float));
  
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

int ShImage::elements() const
{
  return m_elements;
}

float ShImage::operator()(int x, int y, int i) const
{
  SH_DEBUG_ASSERT(m_memory);
  return data()[m_elements * (m_width * y + x) + i];
}

float& ShImage::operator()(int x, int y, int i)
{
  return data()[m_elements * (m_width * y + x) + i];
}

void ShImage::savePng(const std::string& filename, int inverse_alpha)
{
  FILE* fout = std::fopen(filename.c_str(), "w");
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  setjmp(png_ptr->jmpbuf);

  /* Setup PNG I/O */
  png_init_io(png_ptr, fout);
	 
  /* Optionally setup a callback to indicate when a row has been
   * written. */  

  /* Setup filtering. Use Paeth filtering */
  png_set_filter(png_ptr, 0, PNG_FILTER_PAETH);

  /* Setup compression level. */
  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

  /* Setup PNG header information and write it to the file */

  int color_type;
  switch (m_elements) {
  case 1:
    color_type = PNG_COLOR_TYPE_GRAY;
    break;
  case 2:
    color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
    break;
  case 3:
    color_type = PNG_COLOR_TYPE_RGB;
    break;
  case 4:
    color_type = PNG_COLOR_TYPE_RGBA;
    break;
  default:
    throw ShImageException("Invalid element size");
  }
   
  png_set_IHDR(png_ptr, info_ptr,
               m_width, m_height,
               8, 
               color_type,
               PNG_INTERLACE_NONE, 
               PNG_COMPRESSION_TYPE_DEFAULT, 
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr); 
    
  // Actual writing
  png_byte* tempLine = (png_byte*)malloc(m_width * sizeof(png_byte) * m_elements);

  for(int i=0;i<m_height;i+=1){
    for(int j=0;j<m_width;j+=1){
      for(int k = 0;k<m_elements;k+=1)
        tempLine[m_elements*j+k] = static_cast<png_byte>((*this)(j, i, k)*255.0); 
       
      // inverse alpha
      if(inverse_alpha && m_elements == 4)
        tempLine[m_elements*j+3] = 255 - tempLine[m_elements*j+3];
    }
    png_write_row(png_ptr, tempLine);
  }

  // closing and freeing the structs
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(tempLine);
  fclose(fout);
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
    os << "Invalid bit elements " << bit_depth;
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

  m_memory = 0;

  m_width = png_get_image_width(png_ptr, info_ptr);
  m_height = png_get_image_height(png_ptr, info_ptr);
  switch (colour_type) {
  case PNG_COLOR_TYPE_RGB:
    m_elements = 3;
    break;
  case PNG_COLOR_TYPE_RGBA:
    m_elements = 4;
    break;
  default:
    m_elements = 1;
    break;
  }
  
  png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

  m_memory = new ShHostMemory(sizeof(float) * m_width * m_height * m_elements);

  for (int y = 0; y < m_height; y++) {
    for (int x = 0; x < m_width; x++) {
      for (int i = 0; i < m_elements; i++) {
	png_byte *row = row_pointers[y];
	int index = m_elements * (y * m_width + x) + i;
	long element = 0;
	for (int j = 0; j < bit_depth/8; j++) {
	  element <<= 8;
	  element += row[(x * m_elements + i) * bit_depth/8 + j];
	}
	data()[index] = element / static_cast<float>((1 << bit_depth) - 1);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
}

const float* ShImage::data() const
{
  if (!m_memory) return 0;
  return reinterpret_cast<const float*>(m_memory->hostStorage()->data());
}

float* ShImage::data()
{
  if (!m_memory) return 0;
  return reinterpret_cast<float*>(m_memory->hostStorage()->data());
}

ShMemoryPtr ShImage::memory()
{
  return m_memory;
}

ShRefCount<const ShMemory> ShImage::memory() const
{
  return m_memory;
}

}
