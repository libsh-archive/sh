#include "ShMetaForwarder.hpp"
#include "ShMeta.hpp"

namespace SH {

ShMetaForwarder::ShMetaForwarder(ShMeta* meta)
  : m_meta(meta)
{
}

std::string ShMetaForwarder::name() const
{
  return m_meta->name();
}

void ShMetaForwarder::name(const std::string& n)
{
  m_meta->name(n);
}

bool ShMetaForwarder::has_name() const
{
  return m_meta->has_name();
}

bool ShMetaForwarder::internal() const
{
  return m_meta->internal();
}

void ShMetaForwarder::internal(bool i)
{
  m_meta->internal(i);
}

const std::string& ShMetaForwarder::title() const
{
  return m_meta->title();
}

void ShMetaForwarder::title(const std::string& t)
{
  m_meta->title(t);
}

const std::string& ShMetaForwarder::description() const
{
  return m_meta->description();
}

void ShMetaForwarder::description(const std::string& d)
{
  m_meta->description(d);
}

std::string ShMetaForwarder::meta(std::string key) const
{
  return m_meta->meta(key);
}

void ShMetaForwarder::meta(std::string key, std::string value)
{
  m_meta->meta(key, value);
}

ShMeta* ShMetaForwarder::real_meta()
{
  return m_meta;
}

void ShMetaForwarder::real_meta(ShMeta* m)
{
  m_meta = m;
}

}
