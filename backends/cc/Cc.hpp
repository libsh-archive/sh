// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#include "ShVariant.hpp"
#include "ShBackend.hpp"
#include "ShTransformer.hpp"

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

namespace ShCc {

struct CcVariable
{
  CcVariable(void);
  CcVariable(int num, const std::string& name, int size, SH::ShValueType valueType);

  int m_num;
  std::string m_name;
  int m_size;
  SH::ShValueType m_valueType;
};

class CcBackendCode: public SH::ShBackendCode
{
public:
  CcBackendCode(const SH::ShProgramNodeCPtr& program);
  ~CcBackendCode(void);

  bool allocateRegister(const SH::ShVariableNodePtr& var);
  void freeRegister(const SH::ShVariableNodePtr& var);
    
  void upload(void);
  void bind(void);
  void unbind(void);
  void update(void);
    
  void updateUniform(const SH::ShVariableNodePtr& uniform);
    
  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  std::ostream& describe_bindings(std::ostream& out);

protected:
  friend class CcBackend;
  bool generate(void);
  bool execute(SH::ShStream& dest);

private:

  /// Starting from num = 0, adds CcVariables to the m_varmap with variables
  /// named varPrefix + num = arrayName[num]. 
  /// 
  /// T must be of type ShVariableNodePtr 
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
  void allocate_channels(void);
  void allocate_textures(void);
  void allocate_uniforms(void);
  void allocate_temps(void);
  // @}

  std::string resolve(const SH::ShVariable& v);
  std::string resolve(const SH::ShVariable& v, int idx);
  const char* ctype(SH::ShValueType valueType);

  class LabelFunctor
  {
  public:
    LabelFunctor(std::map<SH::ShCtrlGraphNodePtr, int>& label_map);
        
    void operator()(SH::ShCtrlGraphNode* node);
        
  public:
    int m_cur_label;
    std::map<SH::ShCtrlGraphNodePtr, int>& m_label_map;
  };
        
  class EmitFunctor
  {
  public:
    EmitFunctor(CcBackendCode* bec);

    void operator()(SH::ShCtrlGraphNode* node);
        
  public:
    CcBackendCode* m_bec;
  };
        
  void emit(const SH::ShStatement& stmt);
  void emitTexLookup(const SH::ShStatement &stmt, const char* texfunc);
  void emit(const SH::ShBasicBlockPtr& block);
  void emit(const SH::ShCtrlGraphNodePtr& node);
      
private:
  const SH::ShProgramNodeCPtr& m_original_program;
  SH::ShProgramNodePtr m_program;

  std::map<SH::ShCtrlGraphNodePtr, int> m_label_map;
  std::map<SH::ShVariableNodePtr, CcVariable> m_varmap;

  /// The conversions done to change types not handled in hardware into
  // floating point types
  //
  // @todo may want more intelligent conversion if hardware 
  SH::ShTransformer::ValueTypeMap m_convertMap;

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
  std::vector<SH::ShVariantPtr> m_paramVariants;

  //std::vector<SH::ShChannelNodePtr> m_channels;
  //std::vector<CcVariable> m_temps;
  //std::vector<SH::ShTextureNodePtr> m_textures;
};
  
class CcBackend: public SH::ShBackend
{
public:
  CcBackend(void);
  ~CcBackend(void);
  
  std::string name(void) const { return "cc"; }
  std::string version(void) const { return "1.0"; }
  
  SH::ShBackendCodePtr generate_code(const std::string& target,
				     const SH::ShProgramNodeCPtr& program);
  
  void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);
};


typedef SH::ShPointer<CcBackendCode> CcBackendCodePtr;
typedef SH::ShPointer<CcBackend> CcBackendPtr;

}

#endif
