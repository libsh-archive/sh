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
#include "ShVariantCast.hpp"

namespace {
std::vector<const SH::ShTypeInfo*> storageTypes;
}

namespace {
using namespace SH;

template<typename DEST, typename SRC>
void addCast(bool automatic, bool precedence)
{
  ShCastManager::instance()->addCast(shTypeIndex<DEST>(), shTypeIndex<SRC>(),  
      new ShDataVariantCast<DEST, SRC>(), automatic, precedence);
}


// This adds the available automatic and explicit casts 
// and should only be run after type indices are available from the context.
void addCasts()
{
  // Write a python script to generate these...or come up with a better way.
  
  // precedence DAG edges 
  
  // @todo addCast<ShInterval<double>, ShInterval<float> >(true, true);
  addCast<ShInterval<float>, float>(true, true);
  addCast<ShInterval<double>, double>(true, true);

  // float types
  addCast<double, float>(true, true);

  // int types
  addCast<int, short>(true, true);
  addCast<short, char>(true, true);

  // unsigned int types
  addCast<unsigned int, unsigned short>(true, true);
  addCast<unsigned short, unsigned char>(true, true);

  // between int -> float types
  addCast<float, int>(true, true);
  addCast<float, short>(true, true); // @todo change this to half
  addCast<float, char>(true, true); // @todo change this to half

  // between unsigned int -> float types
  addCast<float, unsigned int>(true, true);
  addCast<float, unsigned short>(true, true); // @todo change this to half
  addCast<float, unsigned char>(true, true); // @todo change this to half

  // between unsigned int -> int types
  addCast<int, unsigned short>(true, true); // @todo change this to half
  addCast<short, unsigned char>(true, true); // @todo change this to half

  // automatic casts (but not precedence DAG edges)
  // @todo not sure what to do here.
  //
  // choice 1 - always use standard C++ cast, in which case
  // any cast going up the precedence DAG should be in one step

  // may want to make these explicit...
  addCast<float, double>(true, false);
  addCast<int, double>(true, false);
  addCast<short, double>(true, false);
  addCast<char, double>(true, false);
  addCast<unsigned int, double>(true, false);
  addCast<unsigned short, double>(true, false);
  addCast<unsigned char, double>(true, false);

  addCast<int, float>(true, false);
  addCast<short, float>(true, false);
  addCast<char, float>(true, false);
  addCast<unsigned int, float>(true, false);
  addCast<unsigned short, float>(true, false);
  addCast<unsigned char, float>(true, false);

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

// names
template<> const char* ShConcreteTypeInfo<double>::m_name = "d";
template<> const char* ShConcreteTypeInfo<float>::m_name = "f";

template<> const char* ShConcreteTypeInfo<int>::m_name = "i";
template<> const char* ShConcreteTypeInfo<short>::m_name = "s";
template<> const char* ShConcreteTypeInfo<char>::m_name = "b";

template<> const char* ShConcreteTypeInfo<unsigned int>::m_name = "ui";
template<> const char* ShConcreteTypeInfo<unsigned short>::m_name = "us";
template<> const char* ShConcreteTypeInfo<unsigned char>::m_name = "ub";

template<> const char* ShConcreteTypeInfo<ShInterval<double> >::m_name = "i_d";
template<> const char* ShConcreteTypeInfo<ShInterval<float> >::m_name = "i_f";


void shTypeInfoInit()
{
  // set up index->ShTypeInfo mappings
  if(storageTypes.empty()) {
    storageTypes.push_back(0);
    storageTypes.push_back(ShConcreteTypeInfo<ShInterval<double> >::instance());
    storageTypes.push_back(ShConcreteTypeInfo<ShInterval<float> >::instance());
    storageTypes.push_back(ShConcreteTypeInfo<double>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<float>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<int>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<short>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<char>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<unsigned int>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<unsigned short>::instance());
    storageTypes.push_back(ShConcreteTypeInfo<unsigned char>::instance());
  }

  // instantiate the regular eval ops 
  _shInitFloatOps<double>();
  _shInitFloatOps<float>();

  _shInitIntOps<int>();
  _shInitIntOps<short>();
  _shInitIntOps<char>();

  // @todo type
  _shInitIntOps<unsigned int>();
  _shInitIntOps<unsigned short>();
  _shInitIntOps<unsigned char>();

  _shInitFloatOps<ShInterval<double> >();
  _shInitFloatOps<ShInterval<float> >();

  // instantiate range eval ops 
  _shInitIntervalOps<double>();
  _shInitIntervalOps<float>();

  // @todo type
  //SH_DEBUG_PRINT("ShEval ops: \n" << ShEval::instance()->availableOps());

  addCasts();
}

int shNumTypes()
{
  return storageTypes.size() - 1;
}

const ShTypeInfo* shTypeInfo(int typeIndex)
{
  // @todo type remove this assert
  SH_DEBUG_ASSERT(typeIndex >= 0 && (unsigned int) typeIndex < storageTypes.size());
  return storageTypes[typeIndex];
}

}
