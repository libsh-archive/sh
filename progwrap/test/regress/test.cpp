#include "test.hpp"

Test::Test(int argc, char** argv)
{
  m_backend = "cpu";
  if (argc >= 2) m_backend = argv[1];
  SH::shSetBackend(m_backend);
}
