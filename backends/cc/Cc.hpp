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
#ifndef SHCC_HPP
#define SHCC_HPP

#include <map>
#include <string>
#include <sstream>

#include "ShBackend.hpp"

typedef void (*CcFunc)(float** inputs,
			float** params,
			float** streams,
			void** textures,
			float** outputs);

namespace ShCc {
  
  class CcVariable
    {
    public:
      CcVariable(void);
      CcVariable(int num, const std::string& name);
      CcVariable(int num, const std::string& name, int size);

    public:
      int m_num;
      std::string m_name;
      int m_size;
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
      
      void updateUniform(const SH::ShVariableNodePtr& uniform);
      
      std::ostream& print(std::ostream& out);
      
      std::ostream& printInputOutputFormat(std::ostream& out);

    protected:
      friend class CcBackend;
      bool generate(void);
      bool execute(SH::ShStream& dest);

    private:
      void allocate_consts(void);
      void allocate_inputs(void);
      void allocate_outputs(void);
      void allocate_streams(void);
      void allocate_textures(void);
      void allocate_uniforms(void);

      std::string resolve(const SH::ShVariable& v);
      std::string resolve(const SH::ShVariable& v, int idx);

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
      void emit(SH::ShBasicBlockPtr block);
      void emit(SH::ShCtrlGraphNodePtr node);
      
    private:
      const SH::ShProgramNodeCPtr& m_program;

      std::map<SH::ShCtrlGraphNodePtr, int> m_label_map;
      std::map<SH::ShVariableNodePtr, CcVariable> m_varmap;

      std::stringstream m_code;

#ifdef WIN32
      HMODULE m_hmodule;
#else
      void* m_handle;
#endif /* WIN32 */

      CcFunc m_func;

      int m_cur_temp;

      float** m_params;
      std::vector<SH::ShChannelNodePtr> m_streams;
      std::vector<CcVariable> m_temps;
      std::vector<SH::ShTextureNodePtr> m_textures;
    };
  
  class CcBackend: public SH::ShBackend
    {
    public:
      CcBackend(void);
      ~CcBackend(void);

      std::string name(void) const;

      SH::ShBackendCodePtr generateCode(const std::string& target,
					const SH::ShProgramNodeCPtr& program);
      
      void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);
    };


  typedef SH::ShPointer<CcBackendCode> CcBackendCodePtr;
  typedef SH::ShPointer<CcBackend> CcBackendPtr;
}

#endif
