#include "ShException.hpp"

namespace SH {

ShException::ShException(const std::string& message)
  : m_message(message)
{
}

const std::string& ShException::message() const
{
  return m_message;
}

ShParseException::ShParseException(const std::string& message)
  : ShException("Parse Error: " + message)
{
}

ShImageException::ShImageException(const std::string& message)
  : ShException("Image Error: " + message)
{
}


}
