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
#include <iostream>
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

void ShImage::loadRawData(float* data){
  std::memcpy(m_data, data, m_width* m_height*m_depth*sizeof(float));
}

void ShImage::loadRawData(int width, int height, int depth, float* data){
  m_width = width;
  m_height = height;
  m_depth = depth;
  if(m_data)
    delete [] m_data;
  
  m_data = new float[m_width * m_height * m_depth];
 
  loadRawData(data);
}


void ShImage::compose(const ShImage& other){
 
  int d = m_depth;

  /// d should be 3 or 4
  if(d!=3 && d!=4){
    std::cout<<"Error in compose: Invalid depth! d = "<<d<<" and should be 3 or 4"<<std::endl;
  }

  for(int i=0;i<m_width * m_height;i+=1){
    
    /// transparent pixel
    if(m_data[d*i+2])
      continue;

    /// get the coordinates
    int x = (int) floorf(m_data[d*i] * 512.0f);
    int y = (int) floorf(m_data[d*i+1] * 512.0f);

    /// get the new value
    float fx = other(x, y, 0);
    float fy = other(x, y, 1);
    float verify = other(x, y, 1);

    /// verification
    if(verify==1){
      std::cout<<"Error in compose: good pixel maps to a bad pixel!"<<std::endl;
    }

    /// write new data
    m_data[d*i] = fx;
    m_data[d*i+1] = fy;
    m_data[d*i+2] = verify;

    if(d>3)
      m_data[d*i+3] = 0;

  }// for i
}

void ShImage::merge(const ShImage& other, float* clear_col){
   
/// TODO: now it assumes that the images have the same size. wrong assumtion, is it?

  int d = m_depth;

  bool temp_col = 0;
  if(clear_col==0){
    temp_col = 1;
    clear_col = new float[other.depth()];
    for(int i=0;i<other.depth();i+=1)
      clear_col[i] = 0;
  }

  for(int i=0;i<other.width() * other.height();i+=1){
    bool bclear = 1;
    for(int j=0;j<d;j+=1)
      if(m_data[d*i+j]!=clear_col[j]){
	bclear = 0;
	break;
      }

    /// if the pixel is written, go to the next pixel
    if(!bclear)
      continue;

    bclear = 1;
    
    for(int j=0;j<d;j+=1)
      if(other.m_data[d*i+j]!=clear_col[j]){
	bclear = 0;
	break;
      }

    /// update the pixel
    if(!bclear)
      for(int j=0;j<d;j+=1)
	m_data[d*i+j] = other.m_data[d*i+j];

  }// for i



  /// clean-up
  if(temp_col)
    delete [] clear_col;

}


void ShImage::streamify(const ShImage& other,  ShImage* inv, float* clear_col){
  
  bool temp_col = 0;

  /// store the inverse image
  ShImage inverse = other;

  if(clear_col==0){
    temp_col = 1;
    clear_col = new float[other.depth()];
    for(int i=0;i<other.depth();i+=1)
      clear_col[i] = 0;
  }

  // initialization step
  if(m_data)
    delete [] m_data;

  m_height = 1;
  m_depth = other.m_depth;

  std::cout<<"Step 1"<<std::endl;

  /// allocate more than I need
  m_data = new float[other.depth()*other.width()*other.height()];

  const float* data = other.data();

  std::cout<<"data: "<<data<<std::endl;
  
  int d = other.depth();

  std::cout<<"Step 2 "<<d<<" "<<other.depth()<<std::endl;

  m_width = 0;
  // count the width
  for(int i=0;i<other.width() * other.height();i+=1){

    bool bAdd = 0;
    for(int j=0;j<d;j+=1){
      // std::cout<<"j = "<<j<<std::endl;
      if(data[d*i+j]!=clear_col[j]){
	bAdd = 1;
	break;
      }// if
    }// for j

    if(bAdd){
      for(int j=0;j<d;j+=1){
	m_data[m_width*d+j] = data[i*d+j];
      }
	
      inverse.m_data[i*d+0] = (float) (m_width % 512) / 512.0f;
      inverse.m_data[i*d+1] = floorf((float) (m_width / 512)) / 512.0f;
      inverse.m_data[i*d+2] = inverse.m_data[i*d+3] = 0;

      m_width +=1;
    } else {
      inverse.m_data[i*d+0] = 1;
      inverse.m_data[i*d+1] = 1;
      inverse.m_data[i*d+2] = inverse.m_data[i*d+3] = 1;
    }
  }

  // hack
  for(int j=0;j<d;j+=1){
    m_data[512*512*d - d +j] = clear_col[j];
  }

  std::cout<<"Step 3"<<std::endl;

  if(temp_col)
    delete [] clear_col;

  if(inv)
    *inv = inverse;

  std::cout<<"Step 4"<<std::endl;

  // hack for now
  m_width = 512;
  m_height = 512;

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
   /* assumes RGBA */
   if(m_depth!=4){
     std::cout<<"Problem!!!!!!!!!! depth is not 4; it is "<<m_depth<<std::endl;
   }

   png_set_IHDR(png_ptr, info_ptr,
		m_width, m_height,
		8, 
		PNG_COLOR_TYPE_RGBA, 
		PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT, 
		PNG_FILTER_TYPE_DEFAULT);
   png_write_info(png_ptr, info_ptr); 
    


   // Actual writing
   png_byte* tempLine = (png_byte*)malloc(m_width * sizeof(png_byte) * 4);

   for(int i=m_height-1;i>=0;i-=1){
     for(int j=0;j<m_width;j+=1){
       for(int k = 0;k<4;k+=1)
	 tempLine[4*j+k] = static_cast<png_byte>((*this)(j, i, k)*255.0); 
       
       // inverse alpha
       if(inverse_alpha)
	 tempLine[4*j+3] = 255 - tempLine[4*j+3];
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
