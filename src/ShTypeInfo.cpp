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
#include "ShTypeInfo.hpp"
#include "ShContext.hpp"
#include "ShInterval.hpp"
#include "ShEval.hpp"
#include "ShCastManager.hpp"
#include "ShCloakCast.hpp"

namespace {
using namespace SH;

template<typename DEST, typename SRC>
void addCast(bool automatic, bool precedence)
{
  ShCastManager::instance()->addCast(shTypeIndex<DEST>(), shTypeIndex<SRC>(),  
      new ShDataCloakCast<DEST, SRC>(), automatic, precedence);
}


// This adds the available automatic and explicit casts 
// and should only be run after type indices are available from the context.
void addCasts()
{
  // precedence DAG edges 
  
  // @todo addCast<ShInterval<double>, ShInterval<float> >(true, true);
  addCast<ShInterval<float>, float>(true, true);
  addCast<ShInterval<double>, double>(true, true);

  // float types
  addCast<double, float>(true, true);

  // int types
  addCast<int, short>(true, true);
  addCast<short, char>(true, true);

  // between int and float types
  addCast<double, int>(true, true);
  addCast<float, short>(true, true); // @todo change this to half
  addCast<float, char>(true, true); // @todo change this to half


  // automatic casts (but not precedence DAG edges)
  // @todo not sure what to do here.
  //
  // choice 1 - always use standard C++ cast, in which case
  // any cast going up the precedence DAG should be in one step
  addCast<float, int>(true, false);

  // may want to make these explicit...
  addCast<float, double>(true, false);
  addCast<int, double>(true, false);
  addCast<int, float>(true, false);

  // explicit casts 
  // decide what should be here...
  // not sure...may want to separate classes some more so
  // some automatic casts print out warnings
  // @todo addCast<ShInterval<float>, ShInterval<double> >(false, false);

  ShCastManager::instance()->init();

  /* DEBUG */ //ShCastManager::instance()->graphvizDump(std::cout);
}

}

namespace SH {

template<> const char* ShConcreteTypeInfo<double>::m_name = "d";
template<> const char* ShConcreteTypeInfo<float>::m_name = "f";

template<> const char* ShConcreteTypeInfo<int>::m_name = "i";
template<> const char* ShConcreteTypeInfo<short>::m_name = "s";
template<> const char* ShConcreteTypeInfo<char>::m_name = "b";

// @todo type
#if 0
template<> const char* ShConcreteTypeInfo<unsigned int>::m_name = "ui";
template<> const char* ShConcreteTypeInfo<unsigned short>::m_name = "us";
template<> const char* ShConcreteTypeInfo<unsigned char>::m_name = "ub";
#endif

template<> const char* ShConcreteTypeInfo<ShInterval<double> >::m_name = "i_d";
template<> const char* ShConcreteTypeInfo<ShInterval<float> >::m_name = "i_f";

void shTypeInfoInit()
{
  ShContext* context = ShContext::current();

  context->addTypeInfo(new ShConcreteTypeInfo<double>());
  context->addTypeInfo(new ShConcreteTypeInfo<float>());

  context->addTypeInfo(new ShConcreteTypeInfo<int>());
  context->addTypeInfo(new ShConcreteTypeInfo<short>());
  context->addTypeInfo(new ShConcreteTypeInfo<char>());

  // @todo type
#if 0
  context->addTypeInfo(new ShConcreteTypeInfo<unsigned int>());
  context->addTypeInfo(new ShConcreteTypeInfo<unsigned short>());
  context->addTypeInfo(new ShConcreteTypeInfo<unsigned char>());
#endif

  context->addTypeInfo(new ShConcreteTypeInfo<ShInterval<double> >());
  context->addTypeInfo(new ShConcreteTypeInfo<ShInterval<float> >());

  // instantiate the regular eval ops 
  _shInitFloatOps<double>();
  _shInitFloatOps<float>();

  _shInitIntOps<int>();
  _shInitIntOps<short>();
  _shInitIntOps<char>();

  // @todo type
#if 0
  _shInitIntOps<unsigned int>();
  _shInitIntOps<unsigned short>();
  _shInitIntOps<unsigned char>();
#endif

  _shInitFloatOps<ShInterval<double> >();
  _shInitFloatOps<ShInterval<float> >();

  // instantiate range eval ops 
  _shInitIntervalOps<double>();
  _shInitIntervalOps<float>();

  SH_DEBUG_PRINT("ShEval ops: \n" << ShEval::instance()->availableOps());

  addCasts();
}

}
