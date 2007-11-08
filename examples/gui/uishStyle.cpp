#include "uishConstants.hpp"
#include "uishStyle.hpp"

SH::ShColor4f uishDefaultStyle::colorClear() {
  return uishColor::White;
}

SH::ShColor4f uishDefaultStyle::colorBackground() {
  return uishColor::White;
}

SH::ShColor4f uishDefaultStyle::colorText() {
  return uishColor::Black;
}

SH::ShPointer<uishBorder> uishDefaultStyle::border() {
  if(!m_border) {
    m_border = new uishBasicBorder(uishColor::Black, uishColor::Black,
        uishColor::Black, uishColor::Black);
  }
  return m_border; 
}

double uishDefaultStyle::spacePad() {
  return 1;
}

std::string uishDefaultStyle::fontFamily() {
  return "Arial";
}

int uishDefaultStyle::fontSize() {
  return 14;
}


