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

//-----------------------------------------------------------------------------
// A very simple scheduler implementation.
//-----------------------------------------------------------------------------

#ifndef SHSCHEDULER_HPP
#define SHSCHEDULER_HPP

#include <list>
#include "ShDllExport.hpp"
#include "ShMeta.hpp"

namespace SH {

// TODO: Make sure I didn't miss anything in here 
class
SH_DLLEXPORT ShScheduler : public virtual ShMeta {
public:
	ShScheduler();        // must initialize a schedule
	~ShScheduler();       // whatever cleanups we might need
private:
  struct Pass{
    ShProgramNode &prog;
	std::ist<int> inputs;
	std::list<int> outputs;
  } pass;

  struct Output{
    bool isStream;
	ShChannelNode out;
  } out;

  struct Input{
    bool isStream;
	ShChannelNode in;
  } in;

  struct Schedule{
    Pass::const_iterator begin() const;
	Pass::const_iterator end() const;
	push_back(Pass p);
  private:
	std::list<Pass> passes;
  } sched;

};

} // end namespace SH

#endif // SHSCHEDULER_HPP


d
