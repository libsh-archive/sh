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
#include <iostream>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <sys/wait.h>

#include "Gcc.hpp" 
#include "ShDebug.hpp" 
#include "ShStream.hpp" 
#include "ShVariant.hpp"
#include "ShVariantFactory.hpp"
#include "ShTypeInfo.hpp"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SH_GCC_DEBUG
#  define SH_GCC_DEBUG_PRINT(x) SH_DEBUG_PRINT(x)
#else
#  define SH_GCC_DEBUG_PRINT(x) do { } while(0)
#endif





namespace ShGcc {
  using namespace SH;

  std::string encode(const ShVariable& v) {
    std::stringstream ret;
    if (v.neg()) ret << "-";
    ret << v.name();
    ret << v.swizzle();
    return ret.str();
  }

  // internal use only 
  const char* makeVarname(const char* prefix, int suffix) {
    static char buffer[128];
    sprintf(buffer, "%s%d", prefix, suffix); 
    return buffer;
  }

  const char* InputPrefix = "var_i_";
  const char* OutputPrefix = "var_o_";
  const char* TempPrefix = "var_t_";
  const char* ConstPrefix = "var_c_";
  const char* TexturePrefix = "var_t_";
  const char* StreamPrefix = "var_s_";
  const char* UniformPrefix= "var_u_";

  GccVariable::GccVariable(void) 
    : m_num(-1), 
      m_size(-1), 
      m_typeIndex(-1)
  {}
  
  GccVariable::GccVariable(int num, const std::string& name, int size, int typeIndex) 
    : m_num(num),
      m_name(name),
      m_size(size),
      m_typeIndex(typeIndex) 
  {}

  GccBackendCode::LabelFunctor::LabelFunctor(std::map<ShCtrlGraphNodePtr, int>& label_map) 
    : m_cur_label(0),
      m_label_map(label_map) 
  {}

  void GccBackendCode::LabelFunctor::operator()(ShCtrlGraphNode* node) 
  {
    m_label_map[node] = m_cur_label++;
  }
  
  GccBackendCode::EmitFunctor::EmitFunctor(GccBackendCode* bec) 
    : m_bec(bec) 
  {}
  
  void GccBackendCode::EmitFunctor::operator()(ShCtrlGraphNode* node) 
  {
    m_bec->emit(node);
  }
  
  GccBackendCode::GccBackendCode(const ShProgramNodeCPtr& program) 
    : m_program(program),
      m_handle(NULL),
      m_func(NULL),
      m_cur_temp(0),
      m_params(NULL) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  GccBackendCode::~GccBackendCode(void) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  bool GccBackendCode::allocateRegister(const ShVariableNodePtr& var) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
    return false;
  }

  void GccBackendCode::freeRegister(const ShVariableNodePtr& var) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  void GccBackendCode::upload(void) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  void GccBackendCode::bind(void) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  void GccBackendCode::updateUniform(const ShVariableNodePtr& uniform) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  std::ostream& GccBackendCode::print(std::ostream& out) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
    return out;
  }

  std::ostream& GccBackendCode::describe_interface(std::ostream& out) 
  {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
    return out;
  }
  
  template<typename T>
  void GccBackendCode::allocate_varlist(const std::list<T> &varList, const char* varPrefix, const char* arrayName, const char* typePrefix)
  {
    int num = 0;

    m_code << "  // Assigning locals for " << arrayName << std::endl;
    for(typename std::list<T>::const_iterator I = varList.begin()
        ; I != varList.end(); ++I, ++num) {
      ShVariableNodePtr node = *I; 

      const char* name = makeVarname(varPrefix, num);
      const char* type = ctype(node->typeIndex());
      m_code << "  " << typePrefix << type << " *" << name 
        << " = ((" << type << "*) " << arrayName << "[" << num << "]); // " 
        << node->name() << std::endl; 

      m_varmap[node] = GccVariable(num, name, node->size(), node->typeIndex()); 
    }
    m_code << std::endl;

    SH_GCC_DEBUG_PRINT("Found " << num << " " << arrayName << "...");
  }

  void GccBackendCode::allocate_consts(void) 
  {
    int num = 0;
    
    m_code << "  // Declaring constants" << std::endl; 
    for (ShProgramNode::VarList::const_iterator I = m_program->constants.begin()
        ; I != m_program->constants.end(); ++I, ++num) {
      ShVariableNodePtr node = (*I);
      const char* name = makeVarname(ConstPrefix, num);
      
      m_code << ctype(node->typeIndex()) << name << "[" << node->size() << "] = {" 
        << node->getVariant()->encodeArray() << "}; // " << node->name() << std::endl;

      m_varmap[node] = GccVariable(num, name, node->size(), node->typeIndex()); 
    }
    m_code << std::endl;

    SH_GCC_DEBUG_PRINT("Found " << num << " consts...");
  }

  void GccBackendCode::allocate_inputs(void) {
    allocate_varlist(m_program->inputs, InputPrefix, "inputs"); 
  }

  void GccBackendCode::allocate_outputs(void) {
    allocate_varlist(m_program->outputs, OutputPrefix, "outputs"); 
  }

  void GccBackendCode::allocate_channels(void) {
    allocate_varlist(m_program->channels, StreamPrefix, "channels"); 
  }

  void GccBackendCode::allocate_textures(void) {
    allocate_varlist(m_program->textures, TexturePrefix, "textures"); 
  }

  void GccBackendCode::allocate_uniforms(void) {
    allocate_varlist(m_program->uniforms, UniformPrefix, "params", "const"); 
    
    const ShProgramNode::VarList &uniforms = m_program->uniforms;
    m_params = new const void*[uniforms.size()]; // @todo type assume that all pointers are the same size (probably valid?)


    // @todo type Trying to use the variants' data arrays directly. Hope this works. (<-- stupid comment)
    ShProgramNode::VarList::const_iterator I = uniforms.begin(); 
    for (int i = 0; I != m_program->uniforms.end(); ++I, ++i) {
      ShVariableNodePtr node = *I; 
      m_params[i] = node->getVariant()->array(); 
    }
  }

  void GccBackendCode::allocate_temps() 
  {
    ShProgramNode::VarList::const_iterator I = m_program->temps.begin();
    for (; I != m_program->temps.end(); ++I, ++m_cur_temp) {
      ShVariableNodePtr node = (*I);
      const char* name = makeVarname(TempPrefix, m_cur_temp);
      
      m_code << ctype(node->typeIndex()) << " " << name << "[" << node->size() << "]"
        << "; // " << node->name() << std::endl;
      m_varmap[node] = GccVariable(m_cur_temp, name, node->size(), node->typeIndex()); 
    }

    SH_GCC_DEBUG_PRINT("Found " << m_cur_temp << " temps...");
  }

  std::string GccBackendCode::resolve(const ShVariable& v) {
    GccVariable &var = m_varmap[v.node()];
    SH_DEBUG_ASSERT(var.m_num != -1);

    std::stringstream buf;
    if (v.neg()) buf << "-";
    buf << var.m_name;
    return buf.str();
  }

  std::string GccBackendCode::resolve(const ShVariable& v, int idx) {
    GccVariable &var = m_varmap[v.node()];
    SH_DEBUG_ASSERT(var.m_num != -1);

    std::stringstream buf;
    if (v.neg()) buf << "-";
    buf << var.m_name << "[" << v.swizzle()[idx] << "]";
    return buf.str();
  }

  const char* GccBackendCode::ctype(int typeIndex)
  {
    // outputs a c++ type corresponding to the given type index
    // @todo type enter these strings into the ShTypeInfo objects 
    static const char* ctype_table[] = {
      "Invalid",
      "ShInterval<double>",
      "ShInterval<float>",
      "double",
      "float",
      "int",
      "short",
      "char",
      "unsigned int",
      "unsigned short",
      "unsigned char"
    };

    SH_DEBUG_ASSERT(typeIndex > 0 && typeIndex <= 10); // @todo type remove
    return ctype_table[typeIndex];
  }

  void GccBackendCode::emit(ShBasicBlockPtr block) {
    if (!block) {
      m_code << "  // empty basic block" << std::endl;
    }
    else {
      m_code << "  // start basic block" << std::endl;
      ShBasicBlock::ShStmtList::const_iterator I = block->begin();
      for(;I != block->end(); ++I) {
        emit((*I));
      }
      m_code << "  // end basic block" << std::endl;
    }
  }

  void GccBackendCode::emit(ShCtrlGraphNodePtr node) {
    m_code << "label_" << m_label_map[node] << ":" << std::endl
	   << "  ;" << std::endl;
	
    if (node->block) {
      emit(node->block);
    }

    std::vector<ShCtrlGraphBranch>::iterator I = node->successors.begin();
    for(;I != node->successors.end(); ++I) {

      m_code << "  if (";
      for(int i = 0; i < (*I).cond.size(); i++) {
        if (i != 0) {
          m_code << " || ";
        }

        m_code << "(";
        m_code << resolve((*I).cond, i) << " > 0"; // @todo type fix this truth condition 
        m_code << ")";
      }
      m_code << ")";

      m_code << " goto label_" << m_label_map[(*I).node] << ";" << std::endl;
    }

    if (node->follower) {
      m_code << "  goto label_" << m_label_map[node->follower] << ";" << std::endl;
    }

    if (node->successors.empty() && !node->follower) {
      // Last block, need to return from the function
      m_code << "  return;" << std::endl;
    }
  }

  bool GccBackendCode::generate(void) {
    SH_GCC_DEBUG_PRINT("Creating label map...");
    LabelFunctor fl(m_label_map);
    m_program->ctrlGraph->dfs(fl);

    allocate_consts();
    allocate_inputs();
    allocate_outputs();
    allocate_channels();
    allocate_textures();
    allocate_uniforms();
    allocate_temps();

    // emit code
    SH_GCC_DEBUG_PRINT("Emitting code...");
    EmitFunctor fe(this);
    m_program->ctrlGraph->dfs(fe);

    // prologue
    std::stringstream prologue;
    prologue << "#include <math.h>" << std::endl;
    prologue << "#include <iostream>" << std::endl;
    // @todo output the GccTextures.hpp file here
    prologue << std::endl;
    prologue << std::endl;
    prologue << "extern \"C\" "
	   << " void func("
	   << "void** inputs, "
	   << "const void** params, "
	   << "void** channels, "
	   << "void** textures, "
	   << "void** outputs)" << std::endl;
    prologue << "{" << std::endl;


    // epilogue
    std::stringstream epilogue;
    epilogue << "}" << std::endl;

    // output code for debugging...
    SH_GCC_DEBUG_PRINT(prologue.str());
    SH_GCC_DEBUG_PRINT(m_code.str());
    SH_GCC_DEBUG_PRINT(epilogue.str());

    char name[32];
    tmpnam(name);
    char ccfile[32];
    char sofile[32];
    sprintf(ccfile, "%s.cc", name);
    sprintf(sofile, "%s.so", name);

    std::ofstream fout(ccfile);
    fout << prologue.str();
    fout << m_code.str();
    fout << epilogue.str();
    fout.flush();
    fout.close();

#ifdef SH_GCC_DEBUG
    std::ofstream dbgout("gccstream.cpp");
    dbgout << prologue.str();
    dbgout << m_code.str();
    dbgout << epilogue.str();
    dbgout.flush();
    dbgout.close();
#endif

    pid_t pid = fork();
    if (pid == 0) {
      // child
      execlp("gcc", "gcc", "-O2", "-shared", "-o", sofile, ccfile,
             "-L", SH_INSTALL_PREFIX "/lib/sh", "-lshgcc", NULL);
      SH_GCC_DEBUG_PRINT("exec failed (" << errno << ")");
      exit(-1);
    } else if (pid > 0) {
      int status;
      pid_t ret = waitpid(pid, &status, 0);
      if (ret == -1) {
        SH_GCC_DEBUG_PRINT("wait failed...");
        return false;
      } else {
        SH_GCC_DEBUG_PRINT("status = " << status);
      }

      m_handle = dlopen(sofile, RTLD_NOW);
      if (m_handle == NULL) {
        SH_GCC_DEBUG_PRINT("dlopen failed: " << dlerror());
        return false;
      } else {
        m_func = (GccFunc)dlsym(m_handle, "func");
        if (m_func == NULL) {
          SH_GCC_DEBUG_PRINT("dlsym failed: " << dlerror());
          return false;
        }
      }
      return true;
    } else  {
      // fork failed
      SH_GCC_DEBUG_PRINT("fork failed...");
      return false;
    }
  }

  bool GccBackendCode::execute(ShStream& dest) 
  {
    if (!m_func) {
      if (!generate()) {
        SH_GCC_DEBUG_PRINT("failed to generate program..."); 
        return false;
      }
    }

    int num_outputs = dest.size();
    int num_streams = m_program->channels.size();
    int num_textures = m_program->textures.size();
    void** inputs = NULL;
    void** outputs = new void*[num_outputs]; 
    void** streams = new void*[num_streams];
    void** textures = new void*[num_textures];
    std::vector<int> output_sizes(num_outputs); // sizes of each output element in bytes 
    std::vector<int> output_types(num_outputs);
    std::vector<int> stream_sizes(num_streams); // sizes of each stream element in bytes 
    std::vector<int> stream_types(num_streams);
    
    int sidx = 0;
    
    SH_GCC_DEBUG_PRINT("Assigning input channels to arrays");
    for(ShProgramNode::ChannelList::const_iterator I = m_program->channels_begin()
        ;I != m_program->channels_end(); ++I, ++sidx) {
      ShChannelNodePtr channel = (*I);
      ShHostStoragePtr storage = shref_dynamic_cast<ShHostStorage>(channel->memory()->findStorage("host"));
      
      int datasize = shTypeInfo(channel->typeIndex())->datasize();
      stream_sizes[sidx] = datasize * channel->size();
      stream_types[sidx] = channel->typeIndex();

      if (!storage) {
        storage = new ShHostStorage(channel->memory().object(),
           datasize * channel->size() * channel->count());
      }
      storage->dirty();
      streams[sidx] = storage->data();
    }

    int tidx = 0;
    for(ShProgramNode::TexList::const_iterator I = m_program->textures_begin()
        ;I != m_program->textures_end(); ++I, ++tidx) {
      ShTextureNodePtr texture = (*I);

      ShHostStoragePtr storage = shref_dynamic_cast<ShHostStorage>(
          texture->memory()->findStorage("host"));

      // @todo type this doesn't work with cube maps
      // but should be taken care of
      textures[tidx] = storage->data(); 
    }

    // @todo code below is *exactly* the same as the code above for streams...
    // factor this out
    int oidx = 0;
    int count = 0;

    
    SH_GCC_DEBUG_PRINT("Assigning output channels to arrays");
    for(ShStream::NodeList::iterator I = dest.begin()
        ;I != dest.end(); ++I, ++oidx) {
      ShChannelNodePtr channel = (*I);
      ShHostStoragePtr storage = shref_dynamic_cast<ShHostStorage>(
          channel->memory()->findStorage("host"));

      int datasize = shTypeInfo(channel->typeIndex())->datasize();
      output_sizes[oidx] = datasize * channel->size();
      output_types[oidx] = channel->typeIndex();

      if (!storage) {
        SH_GCC_DEBUG_PRINT("  Allocating new storage?");
        storage = new ShHostStorage(channel->memory().object(),
           datasize * channel->size() * channel->count());
      }
      storage->dirty();
      outputs[oidx] = storage->data();
      SH_GCC_DEBUG_PRINT("  outputs[" << oidx << "] = " << outputs[oidx]);

      if (count == 0) {
        count = channel->count();
      } else if (count != channel->count()) {
        SH_GCC_DEBUG_PRINT("channel count discrepancy...");
        return false;
      }
    }

      
    for(int i = 0; i < count; i++) {
      SH_GCC_DEBUG_PRINT("execution " << i << " of " << count);
      m_func(inputs, m_params, streams, textures, outputs);

      for(int j = 0; j < num_streams; j++) {
        SH_GCC_DEBUG_PRINT("advancing input stream "
		       << j
		       << " by "
		       << stream_sizes[j] 
           << " bytes." );
        // @todo type - not sure if void pointers inc by 1 byte,
        // so cast
        streams[j] = reinterpret_cast<char*>(streams[j]) + stream_sizes[j]; 
      }
      for(int j = 0; j < num_outputs; j++) {
        SH_GCC_DEBUG_PRINT("advancing output stream "
		       << j
		       << " by "
		       << output_sizes[j]
           << " bytes." );
        outputs[j] = reinterpret_cast<char*>(outputs[j]) + output_sizes[j]; 
      }
    }

    return true;
  }


  GccBackend::GccBackend(void) {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  GccBackend::~GccBackend(void) {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
  }

  std::string GccBackend::name(void) const {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
    return "gcc";
  }

  ShBackendCodePtr GccBackend::generateCode(const std::string& target,
						const ShProgramNodeCPtr& program) {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);
    GccBackendCodePtr backendcode = new GccBackendCode(program);
    backendcode->generate();
    return backendcode;
  }

  void GccBackend::execute(const ShProgramNodeCPtr& program, ShStream& dest) {
    SH_GCC_DEBUG_PRINT(__FUNCTION__);

    GccBackendCodePtr backendcode = new GccBackendCode(program);
    backendcode->execute(dest);
  }

  static GccBackend* backend = new GccBackend();

}

