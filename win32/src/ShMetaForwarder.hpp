#ifndef SHMETAFORWARDER_HPP
#define SHMETAFORWARDER_HPP

#include <string>

namespace SH {

class ShMeta;

class
SH_DLLEXPORT ShMetaForwarder {
public:
  ShMetaForwarder(ShMeta* meta);
  
  std::string name() const;
  void name(const std::string& n);
  bool has_name() const;
  
  bool internal() const;
  void internal(bool);

  const std::string& title() const;
  void title(const std::string& t);

  const std::string& description() const;
  void description(const std::string& d);

  std::string meta(std::string key) const;
  void meta(std::string key, std::string value);

protected:
  ShMeta* real_meta();
  void real_meta(ShMeta*);
  
private:
  ShMeta* m_meta;
};

}

#endif
