// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "test.hpp"

Test::Test(int argc, char** argv)
{
  m_backend = "cc";
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