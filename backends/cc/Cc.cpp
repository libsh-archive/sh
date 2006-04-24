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

#ifdef _WIN32
#include <math.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>
#endif /* _WIN32 */

#include <cstdio>
#include <iostream>
#include <fstream>

#include "Cc.hpp" 
#include "Debug.hpp" 
#include "Stream.hpp" 
#include "Variant.hpp"
#include "VariantFactory.hpp"
#include "TypeInfo.hpp"
#include "Optimizations.hpp"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SH_CC_DEBUG
#  define SH_CC_DEBUG_PRINT(x) SH_DEBUG_PRINT(x)
#else
#  define SH_CC_DEBUG_PRINT(x) do { } while(0)
#endif


namespace Cc {

using namespace SH;

#include "CcTexturesString.hpp"

std::string encode(const Variable& v)
{
  std::stringstream ret;
  if (v.neg()) ret << "-";
  ret << v.name();
  ret << v.swizzle();
  return ret.str();
}

// internal use only 
const char* makeVarname(const char* prefix, int suffix)
{
  static char buffer[128];
  sprintf(buffer, "%s%d", prefix, suffix); 
  return buffer;
}

const char* InputPrefix = "var_i_";
const char* OutputPrefix = "var_o_";
const char* TempPrefix = "var_t_";
const char* ConstPrefix = "var_c_";
const char* TexturePrefix = "var_tex_";
const char* StreamPrefix = "var_s_";
const char* UniformPrefix= "var_u_";

CcVariable::CcVariable(void) 
  : m_num(-1), 
    m_size(-1), 
    m_valueType(VALUETYPE_END)
{}
  
CcVariable::CcVariable(int num, const std::string& name, int size, ValueType valueType) 
  : m_num(num),
    m_name(name),
    m_size(size),
    m_valueType(valueType) 
{}

CcBackendCode::LabelFunctor::LabelFunctor(std::map<CtrlGraphNodePtr, int>& label_map) 
  : m_cur_label(0),
    m_label_map(label_map) 
{}

void CcBackendCode::LabelFunctor::operator()(CtrlGraphNode* node) 
{
  m_label_map[node] = m_cur_label++;
}
  
CcBackendCode::EmitFunctor::EmitFunctor(CcBackendCode* bec) 
  : m_bec(bec) 
{}
  
void CcBackendCode::EmitFunctor::operator()(CtrlGraphNode* node) 
{
  m_bec->emit(node);
}
  
CcBackendCode::CcBackendCode(const ProgramNodeCPtr& program) 
  : m_original_program(program),
    m_program(0),
#ifdef _WIN32
    m_hmodule(NULL),
#else
    m_handle(NULL),
#endif
    m_shader_func(NULL),
    m_cur_temp(0),
    m_params(NULL) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);

  // convert half floats, fractionals to types we can use
  m_convertMap[SH_HALF] = SH_FLOAT;
  m_convertMap[SH_FINT] = SH_FLOAT;
  m_convertMap[SH_FSHORT] = SH_FLOAT;
  m_convertMap[SH_FBYTE] = SH_FLOAT;
  m_convertMap[SH_FUINT] = SH_FLOAT;
  m_convertMap[SH_FUSHORT] = SH_FLOAT;
  m_convertMap[SH_FUBYTE] = SH_FLOAT;
}

CcBackendCode::~CcBackendCode(void) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

bool CcBackendCode::allocateRegister(const VariableNodePtr& var) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
  return false;
}

void CcBackendCode::freeRegister(const VariableNodePtr& var) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

void CcBackendCode::upload(void) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

void CcBackendCode::bind(void) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

void CcBackendCode::unbind(void)
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

void CcBackendCode::update(void) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

void CcBackendCode::updateUniform(const VariableNodePtr& uniform) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

std::ostream& CcBackendCode::print(std::ostream& out) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
  return out;
}

std::ostream& CcBackendCode::describe_interface(std::ostream& out) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
  return out;
}

std::ostream& CcBackendCode::describe_bindings(std::ostream& out) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
  return out;
}

template<typename T>
void CcBackendCode::allocate_varlist(const std::list<T> &varList, const char* varPrefix, const char* arrayName, const char* typePrefix)
{
  int num = 0;

  m_code << "  // Assigning locals for " << arrayName << std::endl;
  for(typename std::list<T>::const_iterator I = varList.begin()
        ; I != varList.end(); ++I, ++num) {
    VariableNodePtr node = *I; 

    const char* name = makeVarname(varPrefix, num);
    const char* type = ctype(node->valueType());
    m_code << "  " << typePrefix << " " << type << " *" << name 
	   << " = ((" << type << "*) " << arrayName << "[" << num << "]); // " 
	   << node->name() << std::endl; 

    m_varmap[node] = CcVariable(num, name, node->size(), node->valueType()); 
  }
  m_code << std::endl;

  SH_CC_DEBUG_PRINT("Found " << num << " " << arrayName << "...");
}

void CcBackendCode::allocate_consts(void) 
{
  int num = 0;
    
  m_code << "  // Declaring constants" << std::endl; 
  for (ProgramNode::VarList::const_iterator I = m_program->constants.begin()
	 ; I != m_program->constants.end(); ++I, ++num) {
    VariableNodePtr node = (*I);
    const char* name = makeVarname(ConstPrefix, num);
      
    m_code << "const " << ctype(node->valueType()) << " " << name << "[" << node->size() << "] = {" 
	   << node->getVariant()->encodeArray() << "}; // " << node->name() << std::endl;

    m_varmap[node] = CcVariable(num, name, node->size(), node->valueType()); 
  }
  m_code << std::endl;

  SH_CC_DEBUG_PRINT("Found " << num << " consts...");
}

void CcBackendCode::allocate_inputs(void) 
{
  allocate_varlist(m_program->inputs, InputPrefix, "inputs"); 
}

void CcBackendCode::allocate_outputs(void) 
{
  allocate_varlist(m_program->outputs, OutputPrefix, "outputs"); 
  
  m_code << "  // Initializing output variables to zero " << std::endl;
  int num = 0;
  for (ProgramNode::VarList::const_iterator I = m_program->outputs.begin(); 
       I != m_program->outputs.end(); ++I, ++num) {
    VariableNodePtr node = *I; 

    const char* name = makeVarname(OutputPrefix, num);
    const char* type = ctype(node->valueType());
    for (int i=0; i < node->size(); i++) {
      m_code << "  " << name << "[" << i << "] = (" << type << ")0;" << std::endl;
    }
  }
  m_code << std::endl;
}

void CcBackendCode::allocate_textures(void) 
{
  allocate_varlist(m_program->textures, TexturePrefix, "textures", "const"); 
}

void CcBackendCode::allocate_uniforms(void) 
{
  allocate_varlist(m_program->uniforms, UniformPrefix, "params"/*, "const"*/);  // let uniforms be assigned to...
    
  ProgramNode::VarList &uniforms = m_program->uniforms;
  m_params = new void*[uniforms.size()]; 


  // @todo type Trying to use the variants' data arrays directly. Hope this works. (<-- stupid comment)
  // @todo note that since ray assigns to uniforms, we're going to allow
  // this to be non-const...
  ProgramNode::VarList::iterator I = uniforms.begin(); 
  for (int i = 0; I != m_program->uniforms.end(); ++I, ++i) {
    VariableNodePtr node = *I; 
    m_params[i] = node->getVariant()->array(); 
  }
}

void CcBackendCode::allocate_temps() 
{
  ProgramNode::VarList::const_iterator I = m_program->temps.begin();
  for (; I != m_program->temps.end(); ++I, ++m_cur_temp) {
    VariableNodePtr node = (*I);
    const char* name = makeVarname(TempPrefix, m_cur_temp);
      
    m_code << ctype(node->valueType()) << " " << name << "[" << node->size() << "]"
	   << "; // " << node->name() << std::endl;
    m_varmap[node] = CcVariable(m_cur_temp, name, node->size(), node->valueType()); 
  }

  SH_CC_DEBUG_PRINT("Found " << m_cur_temp << " temps...");
}

std::string CcBackendCode::resolve(const Variable& v) 
{
  CcVariable &var = m_varmap[v.node()];
  SH_DEBUG_ASSERT(var.m_num != -1);

  std::stringstream buf;
  if (v.neg()) buf << "-";
  buf << var.m_name;
  return buf.str();
}

std::string CcBackendCode::resolve(const Variable& v, int idx) 
{
  CcVariable &var = m_varmap[v.node()];
  SH_DEBUG_ASSERT(var.m_num != -1);

  std::stringstream buf;
  if (v.neg()) buf << "-";
  buf << var.m_name << "[" << v.swizzle()[idx] << "]";
  return buf.str();
}

const char* CcBackendCode::ctype(ValueType valueType)
{
  switch(valueType) {
  case SH_HALF:
  case SH_FLOAT:
  case SH_FBYTE:
  case SH_FSHORT:
  case SH_FINT:
  case SH_FUBYTE:
  case SH_FUSHORT:
  case SH_FUINT:
    return "float";

  case SH_DOUBLE: return "double";
  case SH_BYTE:   return "char";
  case SH_SHORT:  return "short";
  case SH_INT:    return "int";
  case SH_UBYTE:  return "unsigned char";
  case SH_USHORT: return "unsigned short";
  case SH_UINT:   return "unsigned int";
  default:
    SH_DEBUG_PRINT("Invalid value type: " << valueTypeName(valueType));
    SH_DEBUG_ASSERT(0); 
  }
  return "unknown"; 
}

void CcBackendCode::emit(const BasicBlockPtr& block) 
{
  if (!block) {
    m_code << "  // empty basic block" << std::endl;
  }
  else {
    m_code << "  // start basic block" << std::endl;
    BasicBlock::StmtList::const_iterator I = block->begin();
    for(;I != block->end(); ++I) {
      emit((*I));
    }
    m_code << "  // end basic block" << std::endl;
  }
}

void CcBackendCode::emit(const CtrlGraphNodePtr& node) 
{
  m_code << "label_" << m_label_map[node] << ":" << std::endl
	 << "  ;" << std::endl;
	
  if (node->block) {
    emit(node->block);
  }

  std::vector<CtrlGraphBranch>::iterator I = node->successors.begin();
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

bool CcBackendCode::generate(void) 
{
  // Transform the code to remove types this backend cannot handle
  m_program = m_original_program->clone();
  Context::current()->enter(m_program);
  Transformer transform(m_program);

  transform.convertToFloat(m_convertMap);
  transform.stripDummyOps();
  if(transform.changed()) {
    optimize(m_program);
  } else {
    m_program = shref_const_cast<ProgramNode>(m_original_program);
  }
  Context::current()->exit();

  // @todo type add conversion code on 
  // transformer already fixes the Program.  Now we just need to set up
  // some storages that hold a copy of texture and input stream data in 
  // our computation type, and make a temp buffer for output if it needs 
  // to be type converted.
  //
  // a) texture lookups
  // b) convert input data array to a useable computation type
  // c) convert output data array to the memory storage type

  SH_CC_DEBUG_PRINT("Creating label map...");
  LabelFunctor f(m_label_map);
  m_program->ctrlGraph->dfs(f);

  allocate_consts();
  allocate_inputs();
  allocate_outputs();
  allocate_textures();
  allocate_uniforms();
  allocate_temps();

  // emit code
  SH_CC_DEBUG_PRINT("Emitting code...");
  EmitFunctor fe(this);
  m_program->ctrlGraph->dfs(fe);

  // prologue
  std::stringstream prologue;
  prologue << "#include <math.h>" << std::endl;
  prologue << "#ifdef __APPLE__" << std::endl;
  prologue << "double exp10(double a) { return pow(10.0, a); }" << std::endl;
  prologue << "#endif" << std::endl;

  for(int i = 0; cc_texture_string[i][0] != 0; ++i) {
    prologue << cc_texture_string[i]; 
  }
  // @todo output the CcTextures.hpp file here
  prologue << std::endl;
  prologue << std::endl;
  prologue << "extern \"C\" "
#ifdef _WIN32
	   << " void __declspec(dllexport) cc_shader("
#else
	   << " void cc_shader("
#endif /* _WIN32 */
	   << "void** inputs, "
	   << "void** params, "
	   << "void** channels, "
	   << "void** textures, "
	   << "void** outputs)" << std::endl;
  prologue << "{" << std::endl;


  // epilogue
  std::stringstream epilogue;
  epilogue << "}" << std::endl;

#ifdef CC_DEBUG
  SH_CC_DEBUG_PRINT("Outputting generated C++ code to ccstream.cpp");
  std::ofstream dbgout("ccstream.cpp");
  dbgout << prologue.str();
  dbgout << m_code.str();
  dbgout << epilogue.str();
  dbgout.flush();
  dbgout.close();
#endif

  return load_shader_func(prologue, epilogue);
}

bool CcBackendCode::load_shader_func(const std::stringstream& prologue,
				     const std::stringstream& epilogue)
{
#ifdef _WIN32
  // generate temporary names for the
  // code and the dll
  char path[1024];
  GetTempPath(1024, path);

  char prefix[1024];
  GetTempFileName(path, "shcc", 0, prefix);

  char cppfile[1024];
  char dllfile[1024];
  sprintf(cppfile, "%s.cpp", prefix);
  sprintf(dllfile, "%s.dll", prefix);
  m_code_filename = cppfile;
  m_lib_filename  = dllfile;

  // write the code out to cppfile
  std::ofstream fout(cppfile);
  fout << prologue.str();
  fout << m_code.str();
  fout << epilogue.str();
  fout.flush();
  fout.close();

  // run cl on cppfile and generate dllfile
  char cmdline[1024];
  sprintf(cmdline, "cl /EHsc /LD /Fe\"%s\" \"%s\"", dllfile, cppfile);

  SH_CC_DEBUG_PRINT("cmdline: \"" << cmdline << "\"");

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(STARTUPINFO));   
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));   
  si.cb = sizeof(STARTUPINFO);

  if (!CreateProcess(NULL, cmdline, NULL, NULL,
		     TRUE, 0, NULL, NULL, &si, &pi)) {
    SH_CC_DEBUG_PRINT("CreateProcess failed!" << GetLastError());
    return false;
  }

  // TODO: do not use INFINITE
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Attempt to load the dll and fetch our function
  m_hmodule = LoadLibrary(dllfile);
  if (m_hmodule == NULL) {
    SH_CC_DEBUG_PRINT("LoadLibrary failed: " << GetLastError());
    return false;
  } else {
    m_shader_func = (CcShaderFunc)GetProcAddress(m_hmodule, "cc_shader");

    if (m_shader_func == NULL) {
      SH_CC_DEBUG_PRINT("GetProcAddress failed: " << GetLastError());
      return false;
    }
  }

  return true;

#else
  char name[32];
  tmpnam(name);
  char ccfile[32];
  char sofile[32];
  sprintf(ccfile, "%s.cc", name);
  #ifdef __APPLE__
  sprintf(sofile, "%s.dylib", name);
  #else
  sprintf(sofile, "%s.so", name);
  #endif
  m_code_filename = ccfile;
  m_lib_filename  = sofile;

  std::ofstream fout(ccfile);
  fout << prologue.str();
  fout << m_code.str();
  fout << epilogue.str();
  fout.flush();
  fout.close();

  pid_t pid = fork();
  if (pid == 0) {
    // child
#ifdef __APPLE__
    execlp("cc", "cc", "-O2", "-fPIC", "-bundle", "-o", sofile, ccfile, (void*)NULL);
#else
    execlp("cc", "cc", "-O2", "-fPIC", "-shared", "-o", sofile, ccfile, (void*)NULL);
#endif // __APPLE__
    SH_CC_DEBUG_PRINT("exec failed (" << errno << ")");
    exit(-1);
  } else if (pid > 0) {
    int status;
    pid_t ret = waitpid(pid, &status, 0);
    if (ret == -1) {
      SH_CC_DEBUG_PRINT("wait failed...");
      return false;
    } else {
      SH_CC_DEBUG_PRINT("status = " << status);
    }

    m_handle = dlopen(sofile, RTLD_NOW);
    if (m_handle == NULL) {
      SH_CC_DEBUG_PRINT("dlopen failed: " << dlerror());
      return false;
    } else {
      m_shader_func = (CcShaderFunc)dlsym(m_handle, "cc_shader");
      if (m_shader_func == NULL) {
	SH_CC_DEBUG_PRINT("dlsym failed: " << dlerror());
	return false;
      }
    }
    return true;
  } else  {
    // fork failed
    SH_CC_DEBUG_PRINT("fork failed...");
    return false;
  }
#endif /* _WIN32 */
}

void CcBackendCode::delete_temporary_files()
{
  if (!m_code_filename.empty()) {
    remove(m_code_filename.c_str());
    m_code_filename = "";
  }

  if (!m_lib_filename.empty()) {
    remove(m_lib_filename.c_str());
    m_lib_filename = "";
  }
}

bool CcBackendCode::execute(const Stream& src, Stream& dest) 
{
  if (!m_shader_func) {
    if (!generate()) {
      SH_CC_DEBUG_PRINT("failed to generate program..."); 
      return false;
    }
  }

  int num_outputs = dest.size();
  int num_inputs = src.size();
  int num_textures = m_program->textures.size();
  void** inputs = new void*[num_inputs];
  void** outputs = new void*[num_outputs]; 
  void** streams = NULL;
  void** textures = new void*[num_textures];
  std::vector<int> output_sizes(num_outputs); // sizes of each output element in bytes 
  std::vector<int> output_types(num_outputs);
  std::vector<int> input_sizes(num_inputs); // sizes of each stream element in bytes 
  std::vector<int> input_types(num_inputs);
    
  int iidx = 0;
    
  SH_CC_DEBUG_PRINT("Assigning input channels to arrays");
  for(Stream::const_iterator I = src.begin(); I != src.end(); ++I, ++iidx) {
    HostStoragePtr storage = shref_dynamic_cast<HostStorage>(I->node()->memory(0)->findStorage("host"));

    int datasize = typeInfo(I->node()->valueType(), MEM)->datasize();
    int stride, count, offset;
    I->get_stride(&stride, 1);
    I->get_count(&count, 1);
    I->get_offset(&offset, 1);
    input_sizes[iidx] = datasize * I->node()->size() * stride;
    input_types[iidx] = I->node()->valueType();

    if (!storage) {
      storage = new HostStorage(I->node()->memory(0).object(),
				  datasize * I->node()->size() * count, I->node()->valueType());
    }
//    storage->dirty();
    inputs[iidx] = reinterpret_cast<char*>(storage->data()) +
                    datasize * I->node()->size() * offset;
  }

  int tidx = 0;
  for(ProgramNode::TexList::const_iterator I = m_program->begin_textures()
        ;I != m_program->end_textures(); ++I, ++tidx) {
    TextureNodePtr texture = (*I);

    HostStoragePtr storage = shref_dynamic_cast<HostStorage>(texture->memory(0)->findStorage("host"));

    // @todo type this doesn't work with cube maps
    // but should be taken care of
    textures[tidx] = storage->data(); 
  }

  // @todo code below is *exactly* the same as the code above for streams...
  // factor this out
  int oidx = 0;
  int dest_count = 0;

    
  SH_CC_DEBUG_PRINT("Assigning output channels to arrays");
  for(Stream::NodeList::iterator I = dest.begin()
        ;I != dest.end(); ++I, ++oidx) {
    HostStoragePtr storage = shref_dynamic_cast<HostStorage>(
								 I->node()->memory(0)->findStorage("host"));

    int stride, count, offset;
    I->get_stride(&stride, 1);
    I->get_count(&count, 1);
    I->get_offset(&offset, 1);

    int datasize = typeInfo(I->node()->valueType(), MEM)->datasize();
    output_sizes[oidx] = datasize * I->node()->size() * stride;
    output_types[oidx] = I->node()->valueType();

    if (!storage) {
      SH_CC_DEBUG_PRINT("  Allocating new storage?");
      storage = new HostStorage(I->node()->memory(0).object(),
				  datasize * I->node()->size() * count, I->node()->valueType());
    }
    storage->dirty();
    outputs[oidx] = reinterpret_cast<char*>(storage->data()) +
                    datasize * I->node()->size() * offset;
    SH_CC_DEBUG_PRINT("  outputs[" << oidx << "] = " << outputs[oidx]);

    if (dest_count == 0) {
      dest_count = count;
    } else if (dest_count != count) {
      SH_CC_DEBUG_PRINT("channel count discrepancy...");
      return false;
    }
  }

      
  for(int i = 0; i < dest_count; i++) {
    m_shader_func(inputs, m_params, streams, textures, outputs);

    for(int j = 0; j < num_inputs; j++) {
      SH_CC_DEBUG_PRINT("advancing input stream "
			<< j
			<< " by "
			<< input_sizes[j] 
			<< " bytes." );
      // @todo type - not sure if void pointers inc by 1 byte,
      // so cast
      inputs[j] = reinterpret_cast<char*>(inputs[j]) + input_sizes[j]; 
    }
    for(int j = 0; j < num_outputs; j++) {
      SH_CC_DEBUG_PRINT("advancing output stream "
			<< j
			<< " by "
			<< output_sizes[j]
			<< " bytes." );
      outputs[j] = reinterpret_cast<char*>(outputs[j]) + output_sizes[j]; 
    }
  }

  return true;
}


CcBackend::CcBackend(void)
  : Backend("cc", "1.0")
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

CcBackend::~CcBackend(void) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
}

BackendCodePtr CcBackend::generate_code(const std::string& target,
					  const ProgramNodeCPtr& program) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);
  CcBackendCodePtr backendcode = new CcBackendCode(program);
  backendcode->generate();
  return backendcode;
}

void CcBackend::execute(const Program& program, Stream& dest) 
{
  SH_CC_DEBUG_PRINT(__FUNCTION__);

  ProgramNodePtr prg = shref_const_cast<ProgramNode>(program.node());
  Pointer<Backend> b(this);
    
  CcBackendCodePtr backendcode = shref_dynamic_cast<CcBackendCode>(prg->code(b)); // = new CcBackendCode(program);
  backendcode->execute(program.stream_inputs(), dest);
  backendcode->delete_temporary_files();
}

void CcBackend::gather(const BaseTexture& dest,
                       const BaseTexture& src, 
                       const BaseTexture& index)
{
  SH_DEBUG_WARN("gather not implemented");
}


}


extern "C" {
  using namespace Cc;

#ifdef _WIN32
  __declspec(dllexport) 
#endif
  CcBackend* backend_libshcc_instantiate()
  {
    return new CcBackend();
  }

#ifdef _WIN32
  __declspec(dllexport) 
#endif
  int backend_libshcc_target_cost(const std::string& target)
  {
    if ("cc:stream" == target)  return 1;
    if ("cpu:stream" == target) return 5;
    if ("*:stream" == target)   return 10;
    if ("stream" == target)     return 10;
    return 0;
  }
}
