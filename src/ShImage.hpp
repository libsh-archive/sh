#ifndef SHIMAGE_HPP
#define SHIMAGE_HPP

#include <string>
#include "ShRefCount.hpp"

namespace SH {

class ShImage : public ShRefCountable {
public:
  ShImage();
  ShImage(int width, int height, int depth);
  ShImage(const ShImage& other);

  ~ShImage();

  ShImage& operator=(const ShImage& other);

  int width() const;
  int height() const;
  int depth() const;

  float operator()(int x, int y, int i) const;
  float& operator()(int x, int y, int i);

  void loadPng(const std::string& filename);

  const float* data() const;
  
private:
  int m_width, m_height;
  int m_depth;
  float* m_data;
};

}

#endif
