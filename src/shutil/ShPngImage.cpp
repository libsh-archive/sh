// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "ShPngImage.hpp"
#include <png.h>
#include <sstream>
#include "sh/ShException.hpp"
#include "sh/ShError.hpp"

namespace ShUtil {

using namespace std;

float* ShPngImage::read_PNG(const string& filename, int& width, int& height, int& elements)
{
  // check that the file is a png file
  png_byte buf[8];

  FILE* in = fopen(filename.c_str(), "rb");

  if (!in) shError( ShImageException("Unable to open " + filename) );
  
  for (int i = 0; i < 8; i++) {
    if (!(buf[i] = fgetc(in))) shError( ShImageException("Not a PNG file") );
  }
  if (png_sig_cmp(buf, 0, 8)) shError( ShImageException("Not a PNG file") );

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     0, 0, 0); // FIXME: use error handlers
  if (!png_ptr) shError( ShImageException("Error initialising libpng (png_ptr)") );

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    shError( ShImageException("Error initialising libpng (info_ptr)") );
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    shError( ShImageException("Error initialising libpng (setjmp/lngjmp)") );    
  }
  
  //  png_set_read_fn(png_ptr, reinterpret_cast<void*>(&in), my_istream_read_data);
  png_init_io(png_ptr, in);
  
  png_set_sig_bytes(png_ptr, 8);

  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0);

  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  if (bit_depth % 8) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    ostringstream os;
    os << "Invalid bit elements " << bit_depth;
    shError( ShImageException(os.str()) );
  }

  int colour_type = png_get_color_type(png_ptr, info_ptr);

  if (colour_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png_ptr);
    colour_type = PNG_COLOR_TYPE_RGB;
  }

  if (colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_gray_1_2_4_to_8(png_ptr);
  }

  if (colour_type != PNG_COLOR_TYPE_RGB &&
      colour_type != PNG_COLOR_TYPE_GRAY &&
      colour_type != PNG_COLOR_TYPE_RGBA) {
    png_destroy_read_struct(&png_ptr, 0, 0);
    shError( ShImageException("Invalid colour type") );
  }

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  switch (colour_type) {
  case PNG_COLOR_TYPE_RGB:
    elements = 3;
    break;
  case PNG_COLOR_TYPE_RGBA:
    elements = 4;
    break;
  default:
    elements = 1;
    break;
  }

  float* data = new float[width * height * elements];

  png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      for (int i=0; i < elements; i++) {
	png_byte *row = row_pointers[y];
	int index = elements * (y * width + x) + i;
	
        long element = 0;
        for (int j = bit_depth/8 - 1; j >= 0; j--) {
          element <<= 8;
          element += row[(x * elements + i) * bit_depth/8 + j];
        }
        
	data[index] = element / static_cast<float>((1 << bit_depth) - 1);
      }
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
  return data;
}

void ShPngImage::write_PNG(const string& filename, const float* data, int inverse_alpha, 
                                  int width, int height, int elements)
{
  FILE* fout = fopen(filename.c_str(), "wb");
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
  switch (elements) {
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
               width, height,
               8, 
               color_type,
               PNG_INTERLACE_NONE, 
               PNG_COMPRESSION_TYPE_DEFAULT, 
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr); 
    
  // Actual writing
  png_byte* temp_line = (png_byte*)malloc(width * sizeof(png_byte) * elements);

  for (int i=0; i < height; i += 1){
    for (int j=0; j < width; j += 1){
      for (int k=0; k < elements; k += 1) {
        int storage_index = elements * (width * i + j) + k;
        temp_line[elements*j+k] = static_cast<png_byte>(data[storage_index]*255.0); 
      }

      // inverse alpha
      if (inverse_alpha && (4 == elements)) {
        temp_line[elements * j+3] = 255 - temp_line[elements * j+3];
      }
    }
    png_write_row(png_ptr, temp_line);
  }

  // closing and freeing the structs
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(temp_line);
  fclose(fout);
}

void ShPngImage::write_PNG16(const string& filename, const float* data, int inverse_alpha,
                                    int width, int height, int elements)
{
  FILE* fout = fopen(filename.c_str(), "w");
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
  switch (elements) {
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
               width, height,
               16, 
               color_type,
               PNG_INTERLACE_NONE, 
               PNG_COMPRESSION_TYPE_DEFAULT, 
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr); 
    
  // Actual writing
  png_uint_16* temp_line = (png_uint_16*)malloc(width * sizeof(png_uint_16) * elements);

  for (int i=0; i < height; i += 1) {
    for (int j=0; j < width; j += 1) {
      for (int k = 0; k < elements; k += 1) {
        int storage_index = elements * (width * i + j) + k;
        temp_line[elements*j+k] = static_cast<png_uint_16>(data[storage_index]*65535.0);
      }

      // inverse alpha
      if(inverse_alpha && (4 == elements)) {
        temp_line[elements * j+3] = 65535 - temp_line[elements * j+3];
      }
    }
    png_write_row(png_ptr, (png_byte*)temp_line);
  }

  // closing and freeing the structs
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(temp_line);
  fclose(fout);
}

} // namespace
