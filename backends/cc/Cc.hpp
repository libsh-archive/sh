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
#ifndef SHCC_HPP
#define SHCC_HPP

#ifdef _WIN32
#include <windows.h>
#endif

#include <map>
#include <string>
#include <sstream>

#include "Variant.hpp"
#include "Backend.hpp"
#include "Transformer.hpp"

// #define SH_CC_DEBUG 1

// function representing a single kernel
// Each of the parameters is an array of single storage-typed arrays
// (The size and type are known during code emission)
// @todo type - inputs should be allowed to change?
extern "C" typedef void (*CcShaderFunc)(void** inputs, 
			                                  void** params,
			                                  void** channels,
			                                  void** textures,
			                                  void** outputs);

namespace Cc {

struct CcVariable
{
  CcVariable(void);
  CcVariable(int num, const std::string& name, int size, SH::ValueType valueType);

  int m_num;
  std::string m_name;
  int m_size;
  SH::ValueType m_valueType;
};

class CcBackendCode: public SH::BackendCode
{
public:
  CcBackendCode(const SH::ProgramNodeCPtr& program);
  ~CcBackendCode(void);

  bool allocateRegister(const SH::VariableNodePtr& var);
  void freeRegister(const SH::VariableNodePtr& var);
    
  void upload(void);
  void bind(void);
  void unbind(void);
  void update(void);
    
  void updateUniform(const SH::VariableNodePtr& uniform);
    
  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  std::ostream& describe_bindings(std::ostream& out);

protected:
  friend class CcBackend;
  bool generate(void);
  bool execute(const SH::Program& prg, SH::Stream& dest);

private:

  /// Starting from num = 0, adds CcVariables to the m_varmap with variables
  /// named varPrefix + num = arrayName[num]. 
  /// 
  /// T must be of type VariableNodePtr 
  template<typename T>
  void allocate_varlist(const std::list<T> &varList, const char* varPrefix, const char* arrayName, const char* typePrefix=""); 

  // Allocates a variable names for different kinds of data 
  // and initializes the variable to index into the appropriate
  // element of an array passed into the CcShaderFunc.
  //
  // Spits out these variable declarations & assignments to m_code
  //
  // void* type of the array gets cast to the variable type
  // @{
  void allocate_consts(void);
  void allocate_inputs(void);
  void allocate_outputs(void);
  void allocate_textures(void);
  void allocate_uniforms(void);
  void allocate_temps(void);
  // @}

  std::string resolve(const SH::Variable& v);
  std::string resolve(const SH::Variable& v, int idx);
  const char* ctype(SH::ValueType valueType);

  class LabelFunctor
  {
  public:
    LabelFunctor(std::map<SH::CtrlGraphNode*, int>& label_map);
        
    void operator()(SH::CtrlGraphNode* node);
        
  public:
    int m_cur_label;
    std::map<SH::CtrlGraphNode*, int>& m_label_map;
  };
        
  class EmitFunctor
  {
  public:
    EmitFunctor(CcBackendCode* bec);

    void operator()(SH::CtrlGraphNode* node);
        
  public:
    CcBackendCode* m_bec;
  };
        
  void emit(const SH::Statement& stmt);
  void emitTexLookup(const SH::Statement &stmt, const char* texfunc);
  void emit(const SH::BasicBlockPtr& block);
  void emit(SH::CtrlGraphNode* node);
      
private:
  SH::ProgramNodeCPtr m_original_program;
  SH::ProgramNodePtr m_program;

  std::map<SH::CtrlGraphNode*, int> m_label_map;
  std::map<SH::VariableNodePtr, CcVariable> m_varmap;

  /// The conversions done to change types not handled in hardware into
  // floating point types
  //
  // @todo may want more intelligent conversion if hardware 
  SH::Transformer::ValueTypeMap m_convertMap;

  std::stringstream m_code;

#ifdef _WIN32
  HMODULE m_hmodule;
#else
  void* m_handle;
#endif /* _WIN32 */

  CcShaderFunc m_shader_func;

  std::string m_code_filename;
  std::string m_lib_filename;

  void delete_temporary_files();
  bool load_shader_func(const std::stringstream& prologue, const std::stringstream& epilogue);

  int m_cur_temp;

  void** m_params;
  std::vector<SH::VariantPtr> m_paramVariants;

  //std::vector<SH::ChannelNodePtr> m_channels;
  //std::vector<CcVariable> m_temps;
  //std::vector<SH::TextureNodePtr> m_textures;
};
  
class CcBackend: public SH::Backend
{
public:
  CcBackend(void);
  ~CcBackend(void);
  
  SH::BackendCodePtr generate_code(const std::string& target,
				     const SH::ProgramNodeCPtr& program);
  
  void execute(const SH::Program& program, SH::Stream& dest);
};


typedef SH::Pointer<CcBackendCode> CcBackendCodePtr;
typedef SH::Pointer<CcBackend> CcBackendPtr;

}

#endif
