#include "test.hpp"

void init_tests(int argc, char** argv)
{
  std::string backend = "cpu";
  if (argc >= 2) backend = argv[1];
  SH::shSetBackend(backend);
}
