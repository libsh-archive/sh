#include <iostream>
#include "ShSwizzle.hpp"

namespace SH {

ShSwizzle::ShSwizzle()
{
}

ShSwizzle::ShSwizzle(int i0)
{
  m_indices.push_back(i0);
}

ShSwizzle::ShSwizzle(int i0, int i1)
{
  m_indices.push_back(i0);
  m_indices.push_back(i1);
}

ShSwizzle::ShSwizzle(int i0, int i1, int i2)
{
  m_indices.push_back(i0);
  m_indices.push_back(i1);
  m_indices.push_back(i2);
}

ShSwizzle::ShSwizzle(int i0, int i1, int i2, int i3)
{
  m_indices.push_back(i0);
  m_indices.push_back(i1);
  m_indices.push_back(i2);
  m_indices.push_back(i3);
}

ShSwizzle::ShSwizzle(int size, int* indices)
{
  for (int i = 0; i < size; i++) {
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
  m_indices.clear();
  for (int i = 0; i < other.size(); i++) {
    m_indices.push_back(other[i]);
  }
  return *this;
}

ShSwizzle::ShSwizzle& ShSwizzle::operator*=(const ShSwizzle& other)
{
  if (!size()) {
    return *this = other;
  }
  std::vector<int> indices;

  for (int i = 0; i < other.size(); i++) {
    if (other[i] < 0 || other[i] >= size()) {
      throw ShSwizzleException();
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
  return m_indices[index];
}

ShSwizzleException::ShSwizzleException()
  : ShException("Swizzle error: out of range")
{
}

std::ostream& operator<<(std::ostream& out, const ShSwizzle& swizzle)
{
  if (!swizzle.size()) return out;
  out << "(";
  for (int i = 0; i < swizzle.size(); i++) {
    if (i > 0) out << ", ";
    out << swizzle[i];
  }
  out << ")";
  return out;
}

}
