#include <iostream>
#include <sstream>
#include "ShSwizzle.hpp"

namespace SH {

ShSwizzle::ShSwizzle(int srcSize)
{
  for (int i = 0; i < srcSize; i++) m_indices.push_back(i);
}

ShSwizzle::ShSwizzle(int srcSize, int i0)
{
  if (i0 < 0 || i0 >= srcSize) throw ShSwizzleException(*this, i0, srcSize);
  m_indices.push_back(i0);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1)
{
  if (i0 < 0 || i0 >= srcSize) throw ShSwizzleException(*this, i0, srcSize);
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) throw ShSwizzleException(*this, i1, srcSize);
  m_indices.push_back(i1);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2)
{
  if (i0 < 0 || i0 >= srcSize) throw ShSwizzleException(*this, i0, srcSize);
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) throw ShSwizzleException(*this, i1, srcSize);
  m_indices.push_back(i1);
  if (i2 < 0 || i2 >= srcSize) throw ShSwizzleException(*this, i2, srcSize);
  m_indices.push_back(i2);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2, int i3)
{
  if (i0 < 0 || i0 >= srcSize) throw ShSwizzleException(*this, i0, srcSize);
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) throw ShSwizzleException(*this, i1, srcSize);
  m_indices.push_back(i1);
  if (i2 < 0 || i2 >= srcSize) throw ShSwizzleException(*this, i2, srcSize);
  m_indices.push_back(i2);
  if (i3 < 0 || i3 >= srcSize) throw ShSwizzleException(*this, i3, srcSize);
  m_indices.push_back(i3);
}

ShSwizzle::ShSwizzle(int srcSize, int size, int* indices)
{
  for (int i = 0; i < size; i++) {
    if (indices[i] < 0 || indices[i] >= srcSize) throw ShSwizzleException(*this, indices[i], srcSize);
    m_indices.push_back(indices[i]);
  }
}

ShSwizzle::ShSwizzle(const ShSwizzle& other)
{
  *this = other;
}

ShSwizzle::~ShSwizzle()
{
}

ShSwizzle& ShSwizzle::operator=(const ShSwizzle& other)
{
  m_indices = other.m_indices;
  //  m_indices.clear();
  //  for (int i = 0; i < other.size(); i++) {
  //    m_indices.push_back(other[i]);
  //  }
  return *this;
}

ShSwizzle::ShSwizzle& ShSwizzle::operator*=(const ShSwizzle& other)
{
  std::vector<int> indices;

  for (int i = 0; i < other.size(); i++) {
    if (other[i] < 0 || other[i] >= size()) {
      throw ShSwizzleException(*this, other[i], size());
    }
    indices.push_back(m_indices[other[i]]);
  }
  m_indices = indices;
  return *this;
}

ShSwizzle ShSwizzle::operator*(const ShSwizzle& other) const
{
  ShSwizzle swizzle(*this);
  swizzle *= other;
  return swizzle;
}

int ShSwizzle::size() const
{
  return m_indices.size();
}

int ShSwizzle::operator[](int index) const
{
  if (index >= size() || index < 0) throw ShSwizzleException(*this, index, size());
  return m_indices[index];
}

ShSwizzleException::ShSwizzleException(const ShSwizzle& s, int index, int size)
  : ShException("")
{
  std::ostringstream out;
  out << "Swizzle error: " << index << " out of range [0, " << size << ") in " << s;

  m_message = out.str();
}

std::ostream& operator<<(std::ostream& out, const ShSwizzle& swizzle)
{
  out << "(";
  for (int i = 0; i < swizzle.size(); i++) {
    if (i > 0) out << ", ";
    out << swizzle[i];
  }
  out << ")";
  return out;
}

}
