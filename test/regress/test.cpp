// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "test.hpp"

using namespace std;

Test::Test(int argc, char** argv)
{
  m_backend = "";
  char *env_var = getenv("SH_REGRESS_BACKEND");
  if (env_var != NULL) m_backend = env_var;
  if (argc >= 2) m_backend = argv[1];

  if ("" == m_backend) {
    cerr << "You must specify the backend to test through the SH_REGRESS_BACKEND"
         << " environment variable or by passing an argument to a unit test "
         << "program. For example:" << endl << endl
         << "  SH_REGRESS_BACKEND=arb make check" << endl << endl
         << "will run all tests on the arb backend, and:" << endl << endl
         << "  test/regress/add glsl" << endl << endl
         << "will run the addition unit test on the glsl backend." << endl;
    exit(1);
  }
  if (m_backend != "host") SH::setBackend(m_backend); 
}

void Test::print_values(const char* varname, const std::string& values)
{
  std::cout << varname << " = [ ";
  std::cout << values;
  std::cout << " ]" << std::endl;;
}

void Test::print_fail(std::string name)
{
    std::cout << SH_COLOR_YELLOW << "Test: " << SH_COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name 
              << SH_COLOR_RED
              << " FAILED"
              << SH_COLOR_NORMAL
              << " [" << m_backend << "]"
              << std::endl;
  
}

void Test::print_pass(std::string name)
{
    std::cout << SH_COLOR_YELLOW << "Test: " << SH_COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name 
              << SH_COLOR_GREEN
              << " PASSED"
              << SH_COLOR_NORMAL
              << " [" << m_backend << "]"
              << SH_COLOR_NORMAL << std::endl;
}
