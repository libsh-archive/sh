#ifndef SHCONTEXT_HPP
#define SHCONTEXT_HPP

namespace SH {
class ShContext {
public:
  static ShContext* current();

  /// 0 means no optimizations. The default level is 2.
  int optimization() const;
  void optimization(int level);

  /// Whether exceptions are being thrown instead of error messages
  /// printed to stdout. The default is to throw exceptions.
  bool throw_errors() const;
  void throw_errors(bool on);

private:
  ShContext();

  int m_optimization;
  bool m_throw_errors;
  
  static ShContext* m_instance;

  // NOT IMPLEMENTED
  ShContext(const ShContext& other);
};

}

#endif
