#include "test.hpp"

Test::Test(int argc, char** argv)
{
  m_backend = "gcc";
  if (argc >= 2) m_backend = argv[1];
  SH::shSetBackend(m_backend);
}
