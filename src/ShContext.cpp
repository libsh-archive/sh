#include "ShContext.hpp"

namespace SH {

ShContext* ShContext::m_instance = 0;

ShContext* ShContext::current()
{
  if (!m_instance) {
    m_instance = new ShContext();
  }
  return m_instance;
}

ShContext::ShContext()
  : m_optimization(2),
    m_throw_errors(true)
{
}

int ShContext::optimization() const
{
  return m_optimization;
}

void ShContext::optimization(int level)
{
  m_optimization = level;
}

bool ShContext::throw_errors() const
{
  return m_throw_errors;
}

void ShContext::throw_errors(bool on)
{
  m_throw_errors = on;
}

}
