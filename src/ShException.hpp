#ifndef SHEXCEPTION_HPP
#define SHEXCEPTION_HPP

#include <string>

namespace SH {

/** A generic exception.
 * You should derive from this class to make more specific exceptions.
 */
class ShException {
public:
  /// Construct a general exception with the given message.
  ShException(const std::string& message);

  /// Return an informative message describing the exception.
  const std::string& message() const;
  
private:
  std::string m_message;
};

/** An exception representing a parse error.
 */
class ShParseException : public ShException {
public:
  ShParseException(const std::string& message);
};

}

#endif
