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
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>

#include "CPU.hpp" 
#include "ShDebug.hpp" 
#include "ShStream.hpp" 

void __cpu_lookup1D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("1D/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)1, s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = s - floor(s);
	  break;
	}

      int u = (int)floor(tex->width()*s);
      u = std::max(0, std::min(tex->width()-1, u));

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*u + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*u + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("1D/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookup2D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("2D/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)1, s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = s - floor(s);
	  break;
	}

      float t = src[1];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  t = std::max((float)0, std::min((float)1, t));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  t = t - floor(t);
	  break;
	}

      int u = (int)floor(tex->width()*s);
      u = std::max(0, std::min(tex->width()-1, u));

      int v = (int)floor(tex->width()*t);
      v = std::max(0, std::min(tex->height()-1, v));

      int offset = v*tex->width()+u;

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*offset + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*offset + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("2D/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookupRECT(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("RECT/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)1, s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = s - floor(s);
	  break;
	}

      float t = src[1];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  t = std::max((float)0, std::min((float)1, t));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  t = t - floor(t);
	  break;
	}

      int u = (int)floor(tex->width()*s);
      u = std::max(0, std::min(tex->width()-1, u));

      int v = (int)floor(tex->width()*t);
      v = std::max(0, std::min(tex->height()-1, v));

      int offset = v*tex->width()+u;

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*offset + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*offset + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("RECT/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookup3D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  SH_DEBUG_PRINT("3D textures unsupported...");
  }

void __cpu_lookupCUBE(SH::ShTextureNode* tex, float* src, float* dst)
  {
  SH_DEBUG_PRINT("CUBE textures unsupported...");
  }

extern "C" void __cpu_lookup(void *t, float* src, float* dst)
  {
  // [0, 1) float based texture lookup
  SH::ShTextureNode* tex = (SH::ShTextureNode*)t;

  switch(tex->dims())
    {
    case SH::SH_TEXTURE_1D:
      __cpu_lookup1D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_2D:
      __cpu_lookup2D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_RECT:
      __cpu_lookupRECT(tex, src, dst);
      break;
    case SH::SH_TEXTURE_3D:
      __cpu_lookup3D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_CUBE:
      __cpu_lookupCUBE(tex, src, dst);
      break;
    default:
      SH_DEBUG_PRINT("unknown texture dimension (" << tex->dims() << ")");
      break;
    }
  }

void __cpu_lookupi1D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("1D/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)tex->width(), s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = tex->width()*(s/tex->width() - floor(s/tex->width()));
	  break;
	}

      int u = (int)floor(s);
      u = std::max(0, std::min(tex->width()-1, u));

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*u + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*u + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("1D/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookupi2D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("2D/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)tex->width(), s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = tex->width()*(s/tex->width() - floor(s/tex->width()));
	  break;
	}

      float t = src[1];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  t = std::max((float)0, std::min((float)tex->height(), t));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  t = tex->height()*(t/tex->height() - floor(t/tex->height()));
	  break;
	}

      int u = (int)floor(s);
      u = std::max(0, std::min(tex->width()-1, u));

      int v = (int)floor(t);
      v = std::max(0, std::min(tex->height()-1, v));

      int offset = v*tex->width()+u;

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*offset + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*offset + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("2D/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookupiRECT(SH::ShTextureNode* tex, float* src, float* dst)
  {
  if (tex->traits().filtering() == SH::ShTextureTraits::SH_FILTER_MIPMAP)
    {
    SH_DEBUG_PRINT("RECT/SH_FILTER_MIPMAP textures unsupported...");
    return;
    }

  SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(tex->memory()->findStorage("host"));
  if (!storage)
    {
    SH_DEBUG_PRINT("failed to find host storage...");
    }
  float* data = (float*)storage->data();
  
  switch(tex->traits().interpolation())
    {
    case 0:
      {
      // nearest neighbour
      float s = src[0];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  s = std::max((float)0, std::min((float)tex->width(), s));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  s = tex->width()*(s/tex->width() - floor(s/tex->width()));
	  break;
	}

      float t = src[1];
      switch(tex->traits().wrapping())
	{
	case SH::ShTextureTraits::SH_WRAP_CLAMP:
	case SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE:
	  t = std::max((float)0, std::min((float)tex->height(), t));
	  break;
	case SH::ShTextureTraits::SH_WRAP_REPEAT:
	  t = tex->height()*(t/tex->height() - floor(t/tex->height()));
	  break;
	}

      int u = (int)floor(s);
      u = std::max(0, std::min(tex->width()-1, u));

      int v = (int)floor(t);
      v = std::max(0, std::min(tex->height()-1, v));

      int offset = v*tex->width()+u;

      switch(tex->traits().clamping())
	{
	case SH::ShTextureTraits::SH_CLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = std::max((float)0, std::min((float)1, data[tex->size()*offset + i]));
	    }
	  break;
	case SH::ShTextureTraits::SH_UNCLAMPED:
	  for(int i = 0; i < tex->size(); i++)
	    {
	    dst[i] = data[tex->size()*offset + i];
	    }
	  break;
	}
      break;
      }
    case 1:
      // linear interpolation
      SH_DEBUG_PRINT("RECT/LINEAR textures unsupported...");
      break;
    }
  }

void __cpu_lookupi3D(SH::ShTextureNode* tex, float* src, float* dst)
  {
  SH_DEBUG_PRINT("3D textures unsupported...");
  }

void __cpu_lookupiCUBE(SH::ShTextureNode* tex, float* src, float* dst)
  {
  SH_DEBUG_PRINT("CUBE textures unsupported...");
  }

extern "C" void __cpu_lookupi(void* t, float* src, float* dst)
  {
  // [0, size] integer based texture lookup
  SH::ShTextureNode* tex = (SH::ShTextureNode*)t;

  switch(tex->dims())
    {
    case SH::SH_TEXTURE_1D:
      __cpu_lookupi1D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_2D:
      __cpu_lookupi2D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_RECT:
      __cpu_lookupiRECT(tex, src, dst);
      break;
    case SH::SH_TEXTURE_3D:
      __cpu_lookupi3D(tex, src, dst);
      break;
    case SH::SH_TEXTURE_CUBE:
      __cpu_lookupiCUBE(tex, src, dst);
      break;
    default:
      SH_DEBUG_PRINT("unknown texture dimension (" << tex->dims() << ")");
      break;
    }
  }

std::string encode(const SH::ShVariable& v)
  {
  std::stringstream ret;
  if (v.neg()) ret << "-";
  ret << v.name();
  ret << v.swizzle();
  return ret.str();
  }

namespace ShCPU {

  CPUVariable::CPUVariable(void) :
    m_num(-1),
    m_size(-1)
    {
    }
  
  CPUVariable::CPUVariable(int num, const std::string& name) :
    m_num(num),
    m_name(name),
    m_size(-1)
    {
    }

  CPUVariable::CPUVariable(int num, const std::string& name, int size) :
    m_num(num),
    m_name(name),
    m_size(size)
    {
    }

  CPUBackendCode::LabelFunctor::LabelFunctor(std::map<SH::ShCtrlGraphNodePtr, int>& label_map) :
    m_cur_label(0),
    m_label_map(label_map)
    {
    }

  void CPUBackendCode::LabelFunctor::operator()(SH::ShCtrlGraphNode* node)
    {
    m_label_map[node] = m_cur_label++;
    }
  
  CPUBackendCode::EmitFunctor::EmitFunctor(CPUBackendCode* bec) :
    m_bec(bec)
    {
    }
  
  void CPUBackendCode::EmitFunctor::operator()(SH::ShCtrlGraphNode* node)
    {
    m_bec->emit(node);
    }
  
  CPUBackendCode::CPUBackendCode(const SH::ShProgram& program) :
    m_program(program),
    m_handle(NULL),
    m_func(NULL),
    m_cur_temp(0),
    m_params(NULL)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  CPUBackendCode::~CPUBackendCode(void)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  bool CPUBackendCode::allocateRegister(const SH::ShVariableNodePtr& var)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    return false;
    }

  void CPUBackendCode::freeRegister(const SH::ShVariableNodePtr& var)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  void CPUBackendCode::upload(void)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  void CPUBackendCode::bind(void)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  void CPUBackendCode::updateUniform(const SH::ShVariableNodePtr& uniform)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  std::ostream& CPUBackendCode::print(std::ostream& out)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    return out;
    }

  std::ostream& CPUBackendCode::printInputOutputFormat(std::ostream& out)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    return out;
    }
  
  void CPUBackendCode::allocate_consts(void)
    {
    int cur_const = 0;
    for (SH::ShProgramNode::VarList::const_iterator itr = m_program->constants.begin();
	 itr != m_program->constants.end();
	 itr++)
      {
      SH::ShVariableNodePtr node = (*itr);

      // allocate internal variable as constant
      char name[128];
      sprintf(name, "var_c_%d", cur_const);
      m_varmap[node] = CPUVariable(cur_const++, name);

      // output constant definition
      m_code << "float " << name << "[" << node->size() << "] = {";
      for (int i = 0; i < node->size(); i++)
	{
	if (i == 0)
	  m_code << node->getValue(i);
	else
	  m_code << ", " << node->getValue(i);
	}
      m_code << "};" << std::endl;
      }

    SH_DEBUG_PRINT("Found " << cur_const << " consts...");
    }

  void CPUBackendCode::allocate_inputs(void)
    {
    int cur_input = 0;
    for (SH::ShProgramNode::VarList::const_iterator itr = m_program->inputs.begin();
	 itr != m_program->inputs.end();
	 itr++)
      {
      SH::ShVariableNodePtr node = (*itr);
    
      // allocate internal variable as attribute
      char name[128];
      sprintf(name, "inputs[%d]", cur_input);
      m_varmap[node] = CPUVariable(cur_input++, name);
      }

    SH_DEBUG_PRINT("Found " << cur_input << " inputs...");
    }

  void CPUBackendCode::allocate_outputs(void)
    {
    int cur_output = 0;
    for (SH::ShProgramNode::VarList::const_iterator itr = m_program->outputs.begin();
	 itr != m_program->outputs.end();
	 itr++)
      {
      SH::ShVariableNodePtr node = (*itr);
    
      // allocate internal variable as attribute
      char name[128];
      sprintf(name, "outputs[%d]", cur_output);
      m_varmap[node] = CPUVariable(cur_output++, name);
      }

    SH_DEBUG_PRINT("Found " << cur_output << " outputs...");
    }

  void CPUBackendCode::allocate_streams(void)
    {
    int cur_stream = 0;
    for (SH::ShProgramNode::ChannelList::const_iterator itr = m_program->channels.begin();
	 itr != m_program->channels.end();
	 itr++)
      {
      SH::ShChannelNodePtr node = (*itr);

      // allocate internal variable as stream
      char name[128];
      sprintf(name, "streams[%d]", cur_stream);
      m_varmap[node] = CPUVariable(cur_stream++, name);
      m_streams.push_back(node);
      }

    SH_DEBUG_PRINT("Found " << cur_stream << " streams...");
    }

  void CPUBackendCode::allocate_textures(void)
    {
    int cur_texture = 0;
    for (SH::ShProgramNode::TexList::const_iterator itr = m_program->textures.begin();
	 itr != m_program->textures.end();
	 itr++)
      {
      SH::ShTextureNodePtr node = (*itr);

      // allocate internal variable as texture
      char name[128];
      sprintf(name, "textures[%d]", cur_texture);
      m_varmap[node] = CPUVariable(cur_texture++, name);
      m_textures.push_back(node);
      }

    SH_DEBUG_PRINT("Found " << cur_texture << " textures...");
    }

  void CPUBackendCode::allocate_uniforms(void)
    {
    int cur_uniform = 0;
    std::vector<SH::ShVariableNodePtr> uniforms;

    for (SH::ShProgramNode::VarList::const_iterator itr = m_program->uniforms.begin();
	 itr != m_program->uniforms.end();
	 itr++)
      {
      SH::ShVariableNodePtr node = (*itr);

      // allocate internal variable as parameter
      char name[128];
      sprintf(name, "params[%d]", cur_uniform);
      m_varmap[node] = CPUVariable(cur_uniform++, name);
      uniforms.push_back(node);
      }

    m_params = new float*[uniforms.size()];
    for(unsigned int i = 0; i < uniforms.size(); i++)
      {
      SH::ShVariableNodePtr node = uniforms[i];
      m_params[i] = new float[node->size()];

      if (node->hasValues())
	{
	for(int j = 0; j < node->size(); j++)
	  {
	  m_params[i][j] = node->getValue(j);
	  }
	}
      else
	{
	for(int j = 0; j < node->size(); j++)
	  {
	  m_params[i][j] = 0;
	  }
	}
      }

    SH_DEBUG_PRINT("Found " << cur_uniform << " uniforms...");
    }

  std::string CPUBackendCode::resolve(const SH::ShVariable& v)
    {
    CPUVariable v2;
    std::map<SH::ShVariableNodePtr, CPUVariable>::iterator itr = m_varmap.find(v.node());
    
    if (itr == m_varmap.end())
      {
      // not defined yet, it had better be a temp...
      SH_DEBUG_ASSERT(v.node()->kind() == SH::SH_TEMP);

      // allocate internal variable as temp
      char name[128];
      sprintf(name, "var_t_%d", m_cur_temp);
      v2 = CPUVariable(m_cur_temp++, name, v.node()->size());
      m_varmap[v.node()] = v2;
      m_temps.push_back(v2);
      }
    else
      {
      v2 = (*itr).second;
      }

    std::stringstream buf;
    if (v.neg()) buf << "-";
    buf << v2.m_name;
    return buf.str();
    }

  std::string CPUBackendCode::resolve(const SH::ShVariable& v, int idx)
    {
    CPUVariable v2;
    std::map<SH::ShVariableNodePtr, CPUVariable>::iterator itr = m_varmap.find(v.node());
    
    if (itr == m_varmap.end())
      {
      // not defined yet, it had better be a temp...
      SH_DEBUG_ASSERT(v.node()->kind() == SH::SH_TEMP);

      // allocate internal variable as temp
      char name[128];
      sprintf(name, "var_t_%d", m_cur_temp);
      v2 = CPUVariable(m_cur_temp++, name, v.node()->size());
      m_varmap[v.node()] = v2;
      m_temps.push_back(v2);
      }
    else
      {
      v2 = (*itr).second;
      }

    std::stringstream buf;
    if (v.neg()) buf << "-";
    buf << v2.m_name;
    buf << "[" << v.swizzle()[idx] << "]";
    return buf.str();
    }

  void CPUBackendCode::emit(const SH::ShStatement& stmt)
    {
    // output SH intermediate m_code for reference
    switch(SH::opInfo[stmt.op].arity)
      {
      case 0:
	m_code << "  // "
	       << SH::opInfo[stmt.op].name << " "
	       << encode(stmt.dest)
	       << std::endl;
	break;
      case 1:
	m_code << "  // "
	       << SH::opInfo[stmt.op].name << " "
	       << encode(stmt.dest) << ", "
	       << encode(stmt.src[0])
	       << std::endl;
	break;
      case 2:
	m_code << "  // "
	       << SH::opInfo[stmt.op].name << " "
	       << encode(stmt.dest) << ", "
	       << encode(stmt.src[0]) << ", "
	       << encode(stmt.src[1])
	       << std::endl;
	break;
      case 3:
	m_code << "  // "
	       << SH::opInfo[stmt.op].name << " "
	       << encode(stmt.dest) << ", "
	       << encode(stmt.src[0]) << ", "
	       << encode(stmt.src[1]) << ", "
	       << encode(stmt.src[2])
	       << std::endl;
	break;
      default:
	m_code << "  // "
	       << SH::opInfo[stmt.op].name << " "
	       << "<unknown format>"
	       << std::endl;
	break;
      }
  
    // generate C m_code from statement
    switch(stmt.op)
      {
      case SH::SH_OP_ASN:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = "
		 << resolve(stmt.src[0], i)
		 << ";" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_NEG:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = -"
		 << resolve(stmt.src[0], i)
		 << ";" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_ADD:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = " 
		 << resolve(stmt.src[0], i)
		 << " + " 
		 << resolve(stmt.src[1], i)
		 << ";" << std::endl;
	  }
	break;
	}
      case SH::SH_OP_MUL:
	{
	if (stmt.src[0].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  ";
	    m_code << resolve(stmt.dest, i);
	    m_code << " = " ;
	    m_code << resolve(stmt.src[0], 0);
	    m_code << " * " ;
	    m_code << resolve(stmt.src[1], i);
	    m_code << ";";
	    m_code << std::endl;
	    }
	  }
	else if (stmt.src[1].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  ";
	    m_code << resolve(stmt.dest, i);
	    m_code << " = " ;
	    m_code << resolve(stmt.src[0], i);
	    m_code << " * " ;
	    m_code << resolve(stmt.src[1], 0);
	    m_code << ";";
	    m_code << std::endl;
	    }
	  }
	else
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  ";
	    m_code << resolve(stmt.dest, i);
	    m_code << " = " ;
	    m_code << resolve(stmt.src[0], i);
	    m_code << " * " ;
	    m_code << resolve(stmt.src[1], i);
	    m_code << ";";
	    m_code << std::endl;
	    }
	  }

	break;
	}
      case SH::SH_OP_DIV:
	{
	if (stmt.src[1].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = " 
		   << resolve(stmt.src[0], i)
		   << " / " 
		   << resolve(stmt.src[1], 0)
		   << ";"
		   << std::endl;
	    }
	  }
	else
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = " 
		   << resolve(stmt.src[0], i)
		   << " / " 
		   << resolve(stmt.src[1], i)
		   << ";"
		   << std::endl;
	    }
	  }

	break;
	}
      case SH::SH_OP_SLT:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " < "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_SLE:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " <= "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_SGT:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " > "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_SGE:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " >= "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_SEQ:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " == "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_SNE:
	{
	bool scalar0 = (stmt.src[0].size() == 1);
	bool scalar1 = (stmt.src[1].size() == 1);
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = (" 
		 << resolve(stmt.src[0], (scalar0 ? 0 : i))
		 << " != "
		 << resolve(stmt.src[1], (scalar1 ? 0 : i))
		 << " ? 1.0f : 0.0f)"
		 << ";"
		 << std::endl;
	  }
	break;
	}
      case SH::SH_OP_ABS:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = fabs("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_ACOS:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = acos("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_ASIN:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = asin("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_ATAN:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = atan("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_ATAN2:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = atan2("
		 << resolve(stmt.src[0], i)
		 << ", "
		 << resolve(stmt.src[1], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_CEIL:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = ceil("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_COS:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = cos("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_DOT:
	{
	SH_DEBUG_ASSERT(stmt.dest.size() == 1);

	if (stmt.src[0].swizzle().size() == 1)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, 0)
		 << " = ";
	  for(int i = 0; i < stmt.src[1].size(); i++)
	    {
	    if (i != 0) 
	      m_code << " + ";
	    
	    m_code << "("
		   << resolve(stmt.src[0], 0)
		   << "*"
		   << resolve(stmt.src[1], i)
		   << ")";
	    }
	  m_code << ";" << std::endl;
	  }
	else if (stmt.src[1].swizzle().size() == 1)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, 0)
		 << " = ";
	  for(int i = 0; i < stmt.src[0].size(); i++)
	    {
	    if (i != 0) 
	      m_code << " + ";
	    
	    m_code << "("
		   << resolve(stmt.src[0], i)
		   << "*"
		   << resolve(stmt.src[1], 0)
		   << ")";
	    }
	  m_code << ";" << std::endl;
	  }
	else
	  {
	  m_code << "  "
		 << resolve(stmt.dest, 0)
		 << " = ";
	  for(int i = 0; i < stmt.src[0].size(); i++)
	    {
	    if (i != 0) 
	      m_code << " + ";
	    
	    m_code << "("
		   << resolve(stmt.src[0], i)
		   << "*"
		   << resolve(stmt.src[1], i)
		   << ")";
	    }
	  m_code << ";" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_EXP:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = exp("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_EXP2:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = exp2("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_EXP10:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = exp10("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_FLR:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = floor("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_FRAC:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = "
		 << resolve(stmt.src[0], i)
		 << " - floor("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_LRP:
	{
	if (stmt.src[0].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = "
		   << resolve(stmt.src[0], 0)
		   << " * (" 
		   << resolve(stmt.src[1], i)
		   << " - " 
		   << resolve(stmt.src[2], i)
		   << ") + " 
		   << resolve(stmt.src[2], i)
		   << ");" << std::endl;
	    }
	  }
	else
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = "
		   << resolve(stmt.src[0], i)
		   << " * ("
		   << resolve(stmt.src[1], i)
		   << " - " 
		   << resolve(stmt.src[2], i)
		   << ") + " 
		   << resolve(stmt.src[2], i)
		   << ");" << std::endl;
	    }
	  }
	
	break;
	}
      case SH::SH_OP_MAD:
	{
	if (stmt.src[0].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = "
		   << resolve(stmt.src[0], 0)
		   << " * " 
		   << resolve(stmt.src[1], i)
		   << " + " 
		   << resolve(stmt.src[2], i)
		   << ";" << std::endl;
	    }
	  }
	else if (stmt.src[1].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = "
		   << resolve(stmt.src[0], i)
		   << " * "
		   << resolve(stmt.src[1], 0)
		   << " + "
		   << resolve(stmt.src[2], i)
		   << ";" << std::endl;
	    }
	  }
	else
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = "
		   << resolve(stmt.src[0], i)
		   << " * "
		   << resolve(stmt.src[1], i)
		   << " + "
		   << resolve(stmt.src[2], i)
		   << ";" << std::endl;
	    }
	  }
	
	break;
	}
      case SH::SH_OP_MAX:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = ("
		 << resolve(stmt.src[0], i)
		 << " > "
		 << resolve(stmt.src[1], i)
		 << " ? "
		 << resolve(stmt.src[0], i)
		 << " : "
		 << resolve(stmt.src[1], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_MIN:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = ("
		 << resolve(stmt.src[0], i)
		 << " < "
		 << resolve(stmt.src[1], i)
		 << " ? "
		 << resolve(stmt.src[0], i)
		 << " : "
		 << resolve(stmt.src[1], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_MOD:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = fmod("
		 << resolve(stmt.src[0], i)
		 << ", "
		 << resolve(stmt.src[1], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_POW:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = pow("
		 << resolve(stmt.src[0], i)
		 << ", "
		 << resolve(stmt.src[1], (stmt.src[1].size() > 1 ? i : 0))
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_SIN:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = sin("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_SQRT:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = sqrt("
		 << resolve(stmt.src[0], i)
		 << ");" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_NORM:
	{
	m_code << "    {" << std::endl;
	m_code << "    float len = 1.0/sqrt(";
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  if (i != 0) m_code << " + ";
	  m_code << resolve(stmt.src[0], i)
		 << " * "
		 << resolve(stmt.src[0], i);
	  }
	m_code << ");" << std::endl;
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "    "
		 << resolve(stmt.dest, i)
		 << " = len*"
		 << resolve(stmt.src[0], i)
		 << ";" << std::endl;
	  }
	m_code << "    }" << std::endl;

	break;
	}
      case SH::SH_OP_XPD:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  int i0 = (i+1)%3;
	  int i1 = (i+2)%3;
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = "
		 << resolve(stmt.src[0], i0)
		 << " * "
		 << resolve(stmt.src[1], i1)
		 << " - "
		 << resolve(stmt.src[1], i0)
		 << " * "
		 << resolve(stmt.src[0], i1)
		 << ";" << std::endl;
	  }

	break;
	}
      case SH::SH_OP_TEX:
	{
	// TODO: negation flags?
	if (stmt.dest.swizzle().identity())
	  {
	  if (stmt.src[1].swizzle().identity())
	    {
	    m_code << "  __cpu_lookup("
		   << resolve(stmt.src[0])
		   << ", "
		   << resolve(stmt.src[1])
		   << ", "
		   << resolve(stmt.dest)
		   << ");" << std::endl;
	    }
	  else
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float input[" << stmt.src[1].size() << "];" << std::endl;
	    for(int i = 0; i < stmt.src[1].size(); i++)
	      {
	      m_code << "    "
		     << "input["
		     << i
		     << "] = " 
		     << resolve(stmt.src[1], i)
		     << ";" << std::endl;
	      }
	    m_code << "    __cpu_lookup("
		   << resolve(stmt.src[0])
		   << ", "
		   << "input"
		   << ", "
		   << resolve(stmt.dest)
		   << ");" << std::endl;
	    m_code << "    }" << std::endl;
	    }
	  }
	else
	  {
	  if (stmt.src[1].swizzle().identity())
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float result[" << stmt.dest.size() << "];" << std::endl;
	    m_code << "    __cpu_lookup("
		   << resolve(stmt.src[0])
		   << ", "
		   << resolve(stmt.src[1])
		   << ", "
		   << "result"
		   << ");" << std::endl;
	    for(int i = 0; i < stmt.dest.size(); i++)
	      {
	      m_code << "    "
		     << resolve(stmt.dest, i)
		     << " = result["
		     << i
		     << "];" << std::endl;
	      }
	    m_code << "    }" << std::endl;
	    }
	  else
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float result[" << stmt.dest.size() << "];" << std::endl;
	    m_code << "    float input[" << stmt.src[1].size() << "];" << std::endl;
	    for(int i = 0; i < stmt.src[1].size(); i++)
	      {
	      m_code << "    "
		     << "input[" 
		     << i
		     << "] = " 
		     << resolve(stmt.src[1], i)
		     << ";" << std::endl;
	      }
	    m_code << "    __cpu_lookup("
		   << resolve(stmt.src[0])
		   << ", "
		   << "input"
		   << ", "
		   << "result"
		   << ");" << std::endl;
	    for(int i = 0; i < stmt.dest.size(); i++)
	      {
	      m_code << "    "
		     << resolve(stmt.dest, i)
		     << " = result["
		     << i
		     << "];" << std::endl;
	      }
	    m_code << "    }" << std::endl;
	    }
	  }
	break;
	}
      case SH::SH_OP_TEXI:
	{
	// TODO: negation flags?
	if (stmt.dest.swizzle().identity())
	  {
	  if (stmt.src[1].swizzle().identity())
	    {
	    m_code << "  __cpu_lookupi("
		   << resolve(stmt.src[0])
		   << ", "
		   << resolve(stmt.src[1])
		   << ", "
		   << resolve(stmt.dest)
		   << ");" << std::endl;
	    }
	  else
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float input[" << stmt.src[1].size() << "];" << std::endl;
	    for(int i = 0; i < stmt.src[1].size(); i++)
	      {
	      m_code << "    "
		     << "input["
		     << i
		     << "] = " 
		     << resolve(stmt.src[1], i)
		     << ";" << std::endl;
	      }
	    m_code << "    __cpu_lookupi("
		   << resolve(stmt.src[0])
		   << ", "
		   << "input"
		   << ", "
		   << resolve(stmt.dest)
		   << ");" << std::endl;
	    m_code << "    }" << std::endl;
	    }
	  }
	else
	  {
	  if (stmt.src[1].swizzle().identity())
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float result[" << stmt.dest.size() << "];" << std::endl;
	    m_code << "    __cpu_lookupi("
		   << resolve(stmt.src[0])
		   << ", "
		   << resolve(stmt.src[1])
		   << ", "
		   << "result"
		   << ");" << std::endl;
	    for(int i = 0; i < stmt.dest.size(); i++)
	      {
	      m_code << "    "
		     << resolve(stmt.dest, i)
		     << " = result["
		     << i
		     << "];" << std::endl;
	      }
	    m_code << "    }" << std::endl;
	    }
	  else
	    {
	    m_code << "    {" << std::endl;
	    m_code << "    float result[" << stmt.dest.size() << "];" << std::endl;
	    m_code << "    float input[" << stmt.src[1].size() << "];" << std::endl;
	    for(int i = 0; i < stmt.src[1].size(); i++)
	      {
	      m_code << "    "
		     << "input[" 
		     << i
		     << "] = " 
		     << resolve(stmt.src[1], i)
		     << ";" << std::endl;
	      }
	    m_code << "    __cpu_lookupi("
		   << resolve(stmt.src[0])
		   << ", "
		   << "input"
		   << ", "
		   << "result"
		   << ");" << std::endl;
	    for(int i = 0; i < stmt.dest.size(); i++)
	      {
	      m_code << "    "
		     << resolve(stmt.dest, i)
		     << " = result["
		     << i
		     << "];" << std::endl;
	      }
	    m_code << "    }" << std::endl;
	    }
	  }
	break;
	}
      case SH::SH_OP_COND:
	{
	if (stmt.src[0].swizzle().size() == 1)
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = ("
		   << resolve(stmt.src[0], 0)
		   << " > 0 ? "
		   << resolve(stmt.src[1], i)
		   << " : "
		   << resolve(stmt.src[2], i)
		   << ");" << std::endl;
	    }
	  }
	else
	  {
	  for(int i = 0; i < stmt.dest.size(); i++)
	    {
	    m_code << "  "
		   << resolve(stmt.dest, i)
		   << " = ("
		   << resolve(stmt.src[0], i)
		   << " > 0 ? "
		   << resolve(stmt.src[1], i)
		   << " : "
		   << resolve(stmt.src[2], i)
		   << ");" << std::endl;
	    }
	  }
	break;
	}
      case SH::SH_OP_KIL:
	{
	// TODO: maintain prior output values 
	m_code << "  if (";
	for(int i = 0; i < stmt.src[0].size(); i++)
	  {
	  if (i != 0) m_code << " || ";
	  m_code << "("
		 << resolve(stmt.src[0], i) 
		 << " > 0)";
	  }
	m_code << ")" << std::endl;
	m_code << "    return;" << std::endl;
	break;
	}
      case SH::SH_OP_OPTBRA:
	{
	SH_DEBUG_ASSERT(false);
	break;
	}
      case SH::SH_OP_FETCH:
	{
	for(int i = 0; i < stmt.dest.size(); i++)
	  {
	  m_code << "  "
		 << resolve(stmt.dest, i)
		 << " = "
		 << resolve(stmt.src[0], i)
		 << ";" << std::endl;
	  }

	break;
	}
      default:
	{
	m_code << "  // *** unhandled operation "
	       << SH::opInfo[stmt.op].name
	       << " ***" << std::endl;
	break;
	}
      }
    }

  void CPUBackendCode::emit(SH::ShBasicBlockPtr block)
    {
    if (!block)
      {
      m_code << "  // empty basic block" << std::endl;
      }
    else
      {
      m_code << "  // start basic block" << std::endl;
      for(SH::ShBasicBlock::ShStmtList::const_iterator itr = block->begin();
	  itr != block->end();
	  itr++)
	{
	emit((*itr));
	}
      m_code << "  // end basic block" << std::endl;
      }
    }

  void CPUBackendCode::emit(SH::ShCtrlGraphNodePtr node)
    {
    m_code << "label_" << m_label_map[node] << ":" << std::endl
	   << "  ;" << std::endl;
	
    if (node->block)
      {
      emit(node->block);
      }

    for(std::vector<SH::ShCtrlGraphBranch>::iterator itr = node->successors.begin();
	itr != node->successors.end();
	itr++)
      {
      m_code << "  if (";
      for(int i = 0; i < (*itr).cond.size(); i++)
	{
	if (i != 0) 
	  m_code << " || ";

	m_code << "(";
	m_code << resolve((*itr).cond, i) << " > 0.0f";
	m_code << ")";
	}
      m_code << ")";

      m_code << " goto label_"
	     << m_label_map[(*itr).node]
	     << ";" << std::endl;
      }

    if (node->follower)
      {
      m_code << "  goto label_" << m_label_map[node->follower] << ";" << std::endl;
      }

    if (node->successors.empty() && !node->follower)
      {
      // Last block, need to return from the function
      m_code << "  return;" << std::endl;
      }
    }

  bool CPUBackendCode::generate(void)
    {
    SH_DEBUG_PRINT("Creating label map...");
    LabelFunctor fl(m_label_map);
    m_program->ctrlGraph->dfs(fl);

    allocate_consts();
    allocate_inputs();
    allocate_outputs();
    allocate_streams();
    allocate_textures();
    allocate_uniforms();

    // emit code
    SH_DEBUG_PRINT("Emitting code...");
    EmitFunctor fe(this);
    m_program->ctrlGraph->dfs(fe);

    // prologue
    std::stringstream prologue;
    prologue << "#include <math.h>" << std::endl;
    prologue << std::endl;
    prologue << "extern \"C\" void __cpu_lookup(void*, float*, float*);" << std::endl;
    prologue << "extern \"C\" void __cpu_lookupi(void*, float*, float*);" << std::endl;
    prologue << std::endl;
    prologue << "extern \"C\" "
	   << " void func("
	   << "float** inputs, "
	   << "float** params, "
	   << "float** streams, "
	   << "void** textures, "
	   << "float** outputs)" << std::endl;
    prologue << "  {" << std::endl;

    // output temp declarations
    for(std::vector<CPUVariable>::iterator itr = m_temps.begin();
	itr != m_temps.end();
	itr++)
      {
      // output temp declaration
      prologue << "  float "
	     << (*itr).m_name << "[" << (*itr).m_size << "]"
	     << ";" << std::endl;
      }

    // epilogue
    std::stringstream epilogue;
    epilogue << "  }" << std::endl;

    // output code for debugging...
    SH_DEBUG_PRINT(prologue.str());
    SH_DEBUG_PRINT(m_code.str());
    SH_DEBUG_PRINT(epilogue.str());

    char name[32];
    tmpnam(name);
    char ccfile[32];
    char sofile[32];
    sprintf(ccfile, "%s.cc", name);
    sprintf(sofile, "%s.so", name);
    FILE* f = fopen(ccfile, "w");
    fprintf(f, "%s", prologue.str().c_str());
    fprintf(f, "%s", m_code.str().c_str());
    fprintf(f, "%s", epilogue.str().c_str());
    fflush(f);

    pid_t pid = fork();
    if (pid == 0)
      {
      // child
      execlp("gcc", "gcc", "-g", "-shared", "-o", sofile, ccfile, NULL);
      SH_DEBUG_PRINT("exec failed (" << errno << ")");
      exit(-1);
      }
    else if (pid > 0)
      {
      int status;
      pid_t ret = waitpid(pid, &status, 0);
      if (ret == -1)
	{
	SH_DEBUG_PRINT("wait failed...");
	return false;
	}
      else
	{
	SH_DEBUG_PRINT("status = " << status);
	}

      m_handle = dlopen(sofile, RTLD_NOW);
      if (m_handle == NULL)
	{
	SH_DEBUG_PRINT("dlopen failed...");
	return false;
	}
      else
	{
	m_func = (CPUFunc)dlsym(m_handle, "func");

	if (m_func == NULL)
	  {
	  SH_DEBUG_PRINT("dlsym failed...");
	  return false;
	  }
	}

      return true;
      }
    else 
      {
      // fork failed
      SH_DEBUG_PRINT("fork failed...");
      return false;
      }
    }

  bool CPUBackendCode::execute(SH::ShStream& dest)
    {
    if (!m_func)
      {
      if (!generate())
	{
	SH_DEBUG_PRINT("failed to generate program..."); 
	return false;
	}
      }

    int num_outputs = dest.size();
    int num_streams = m_streams.size();
    float** inputs = NULL;
    float** params = m_params;
    float** streams = new float*[m_streams.size()];
    void** textures = new void*[m_textures.size()];
    float** outputs = new float*[dest.size()];
    std::vector<int> output_sizes(num_outputs);
    std::vector<int> stream_sizes(num_streams);
    
    int sidx = 0;
    for(std::vector<SH::ShChannelNodePtr>::iterator itr = m_streams.begin();
	itr != m_streams.end();
	itr++, sidx++)
      {
      SH::ShChannelNodePtr channel = (*itr);
      SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(channel->memory()->findStorage("host"));
      if (!storage)
	{
	storage = new SH::ShHostStorage(channel->memory().object(),
					sizeof(float)*channel->size()*channel->count());
	}
      storage->dirty();
      streams[sidx] = (float*)storage->data();
      stream_sizes[sidx] = channel->size();
      }

    int tidx = 0;
    for(std::vector<SH::ShTextureNodePtr>::iterator itr = m_textures.begin();
	itr != m_textures.end();
	itr++, tidx++)
      {
      SH::ShTextureNodePtr texture = (*itr);
      textures[tidx] = (void*)(texture.object());
      }

    int oidx = 0;
    int count = 0;
    for(SH::ShStream::NodeList::iterator itr = dest.begin();
	itr != dest.end();
	itr++, oidx++)
      {
      SH::ShChannelNodePtr channel = (*itr);
      SH::ShHostStoragePtr storage = SH::shref_dynamic_cast<SH::ShHostStorage>(channel->memory()->findStorage("host"));
      if (!storage)
	{
	storage = new SH::ShHostStorage(channel->memory().object(),
					sizeof(float)*channel->size()*channel->count());
	}
      storage->dirty();
      outputs[oidx] = (float*)storage->data();
      output_sizes[oidx] = channel->size();

      if (count == 0) count = channel->count();
      else
	{
	if (count != channel->count())
	  {
	  SH_DEBUG_PRINT("channel count discrepancy...");
	  return false;
	  }
	}
      }

      
    for(int i = 0; i < count; i++)
      {
      SH_DEBUG_PRINT("execution " << i << " of " << count);
      m_func(inputs, params, streams, textures, outputs);

      for(int j = 0; j < num_streams; j++)
	{
	SH_DEBUG_PRINT("advancing input stream "
		       << j
		       << " by "
		       << stream_sizes[j]);
	streams[j] += stream_sizes[j];
	}
      for(int j = 0; j < num_outputs; j++)
	{
	SH_DEBUG_PRINT("advancing output stream "
		       << j
		       << " by "
		       << output_sizes[j]);
	outputs[j] += output_sizes[j];
	}
      }

    return true;
    }


  CPUBackend::CPUBackend(void)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  CPUBackend::~CPUBackend(void)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    }

  std::string CPUBackend::name(void) const
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    return "cpu";
    }

  SH::ShBackendCodePtr CPUBackend::generateCode(const std::string& target,
						const SH::ShProgram& program)
    {
    SH_DEBUG_PRINT(__FUNCTION__);
    CPUBackendCodePtr backendcode = new CPUBackendCode(program);
    backendcode->generate();
    return backendcode;
    }

  void CPUBackend::execute(const SH::ShProgram& program, SH::ShStream& dest)
    {
    SH_DEBUG_PRINT(__FUNCTION__);

    CPUBackendCodePtr backendcode = new CPUBackendCode(program);
    backendcode->execute(dest);
    }

  static CPUBackend* backend = new CPUBackend();

}

