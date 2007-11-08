#ifndef UISH_STYLE_HPP
#define UISH_STYLE_HPP

#include <string>
#include <sh/sh.hpp>
#include "uishBorder.hpp"

/* Style structures that determine basic look and feel.
 * Very basic stuff right now
 */
struct uishStyle: public SH::ShRefCountable {
  virtual ~uishStyle() {} 

  // color scheme (colours may be semi-transparent)
  /** color of emptiness */
  virtual SH::ShColor4f colorClear() = 0;

  /** color of widget backgrounds */
  virtual SH::ShColor4f colorBackground() = 0; 

  /** text color */
  virtual SH::ShColor4f colorText() = 0;

  // border style 
  virtual SH::ShPointer<uishBorder> border() = 0; 

  // default spacing 
  virtual double spacePad() = 0;

  // font information
  virtual std::string fontFamily() = 0;
  virtual int fontSize() = 0;
};
typedef SH::ShPointer<uishStyle> uishStylePtr;

struct uishDefaultStyle: public uishStyle {
  virtual ~uishDefaultStyle() {} 

  virtual SH::ShColor4f colorClear(); 
  virtual SH::ShColor4f colorBackground(); 
  virtual SH::ShColor4f colorText(); 

  virtual SH::ShPointer<uishBorder> border(); 

  virtual double spacePad();

  // fonts
  virtual std::string fontFamily();
  virtual int fontSize(); 

  private:
    SH::ShPointer<uishBorder> m_border;
};

/* returns random values for spotless layout.
struct uishRansomNoteStyle: public uishStyle {
};
*/

#endif
