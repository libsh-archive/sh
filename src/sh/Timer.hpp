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


#ifndef SHTIMER_HPP
#define SHTIMER_HPP

#include <string>
#include <sys/time.h>
#include <time.h>
#include "Context.hpp"

namespace SH {

class Timer {
public:
  /* Construct a timer set to right now */
  Timer() { start(); }

  /* Set timer start to right now */
  void start() { gettimeofday(&m_startval, 0); }

  /* Elapsed time in milliseconds */ 
  double diff() {
    timeval endval;
    gettimeofday(&endval, 0);
    return (endval.tv_sec - m_startval.tv_sec) * 1000 + (endval.tv_usec - m_startval.tv_usec) / 1000.0;
  }

private:
  timeval m_startval;
};

/* A Timer that automatically registers a stat value in the current Context on diff calls */ 
class StatTimer: public Timer {
public:
  StatTimer(const std::string& name): m_name(name) {}

  double diff() {
    double result = Timer::diff();
    Context::current()->set_stat(m_name.c_str(), result);
    return result;
  }

private:
  std::string m_name;
};

}

#endif
