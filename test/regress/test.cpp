#include "test.hpp"

Test::Test(int argc, char** argv)
{
  m_backend = "gcc";
  if (argc >= 2) m_backend = argv[1];
  if (m_backend != "host") SH::shSetBackend(m_backend);
}

void Test::print_fail(std::string name)
{
    std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name 
              << COLOR_RED
              << " FAILED"
              << COLOR_NORMAL
              << " [" << m_backend << "]"
              << std::endl;
  
}

void Test::print_pass(std::string name)
{
    std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name 
              << COLOR_GREEN
              << " PASSED"
              << COLOR_NORMAL
              << " [" << m_backend << "]"
              << COLOR_NORMAL << std::endl;
}
