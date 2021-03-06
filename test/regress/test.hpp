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
#ifndef REGRESS_TEST_HPP
#define REGRESS_TEST_HPP

#include <iostream>
#include <iomanip>
#include <sh/sh.hpp>
#include <shutil/shutil.hpp>

#define SH_COLOR_GREEN "[32m"
#define SH_COLOR_RED "[31m"
#define SH_COLOR_BLUE "[34m"
#define SH_COLOR_YELLOW "[33m"
#define SH_COLOR_NORMAL "[0m"

// turns on output of all results, even if they pass 
//#define SH_REGRESS_SHOWALL 

class Test {
public:
  // reads backend from command line (default gcc) 
  Test(int argc, char** argv);

  void ignore_backend(std::string backend);

  template <class T>
  void pretty_print(std::string varname, int size, const T *values)
  {
    std::stringstream s;
    s << std::setiosflags(std::ios::right);
    for(int j = 0; j < size; j++) {
      if (j != 0) s << ",";
      s << std::setw(10) << values[j];
    }
    print_values(varname.c_str(), s.str());
  }

  template<int N, SH::BindingType Binding, typename T, SH::SemanticType Semantic, bool swizzled>
  void mem_from_host(typename SH::Attrib<N, Binding, T, Semantic, swizzled>::mem_type mem[], const SH::Attrib<N, Binding, T, Semantic, swizzled>& host)
  {
    using namespace SH;
    VariantPtr memVariant = new DataVariant<typename Attrib<N, Binding, T, Semantic, swizzled>::storage_type, MEM>(host.size(), mem, false);
    memVariant->set(host.getVariant());
  }

  template<int Rows, int Cols, SH::BindingType Binding, typename T>
  void mem_from_host(typename SH::Matrix<Rows, Cols, Binding, T>::mem_type mem[], const SH::Matrix<Rows, Cols, Binding, T>& host)
  {
    // TODO: add support for matrices
  }

  /// Apply the input parameter to the program
  template<class T>
  SH::Program bind_input(SH::Program& program, const T& in, std::string& str)
  {
    using namespace std;
    using namespace SH;
    
    typedef typename T::mem_type IT;
    IT* _in = new IT[in.size()];
    mem_from_host(_in, in);
    
    HostMemoryPtr mem_in = new HostMemory(in.size()*sizeof(IT), _in, T::value_type);
    Channel<typename T::TempType> chan_in(mem_in, 1);
    
    stringstream s;
    s << setiosflags(ios::right);
    for(int j = 0; j < in.size(); j++) {
	if (j != 0) s << ",";
	s << setw(10) << _in[j];
    }
    str = s.str();
    
    return program << chan_in;
  }

  /// Check results and output differences
  template<typename T>
  int output_result(const std::string& name, const std::vector<std::string>& inputs,
		    const T& out, const T& exp, int out_size, const double epsilon)
  {
    for(int i=0; i < out_size; i++) {
      if ((out[i] != out[i]) || (fabs(out[i] - exp[i]) > epsilon)) {
        print_fail(name);
	for (unsigned j=0; j < inputs.size(); j++) {
	  char label[4] = "  A";
	  label[2] += j;
	  print_values(label, inputs[j]);
	}
        pretty_print("out", out_size, out);
        pretty_print("exp", out_size, exp);
        return 1;
      }
    }
#ifdef SH_REGRESS_SHOWALL
    print_pass(name);
    for (unsigned j=0; j < inputs.size(); j++) {
      char label[4] = "  A";
      label[2] += j;
      print_values(label, inputs[j]);
    }
    pretty_print("out", out_size, out);
    pretty_print("exp", out_size, exp);
#endif
    return 0;
  }

  /// Run stream test on current backend (1 input parameter)
  template <class SH_INPUT1, class SH_OUTPUT>
  int run(SH::Program& program, const SH_INPUT1& in1, const SH_OUTPUT& res, const double epsilon)
  {
    if (on_host()) return 0; // skip this test

    typedef typename SH_OUTPUT::mem_type OT;

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::HostMemoryPtr mem_out = new SH::HostMemory(res.size()*sizeof(OT), _out, SH_OUTPUT::value_type);
    SH::Channel<typename SH_OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(1);
    SH::Program program1 = bind_input(program, in1, inputs[0]);
    chan_out = program1;
    mem_out->hostStorage()->sync();

    return output_result(program.name(), inputs, _out, _res, res.size(), epsilon);
  }

  /// Run stream test on current backend (2 input parameters)
  template <class SH_INPUT1, class SH_INPUT2, class SH_OUTPUT>
  int run(SH::Program& program, const SH_INPUT1& in1, const SH_INPUT2& in2,
	  const SH_OUTPUT& res, const double epsilon)
  {
    if (on_host()) return 0; // skip this test

    typedef typename SH_OUTPUT::mem_type OT;

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::HostMemoryPtr mem_out = new SH::HostMemory(res.size()*sizeof(OT), _out, SH_OUTPUT::value_type);
    SH::Channel<typename SH_OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(2);
    SH::Program program1 = bind_input(program, in1, inputs[0]);
    SH::Program program2 = bind_input(program1, in2, inputs[1]);
    chan_out = program2;
    mem_out->hostStorage()->sync();

    return output_result(program.name(), inputs, _out, _res, res.size(), epsilon);
  }

  /// Run stream test on current backend (3 input parameters)
  template <class SH_INPUT1, class SH_INPUT2, class SH_INPUT3, class SH_OUTPUT>
  int run(SH::Program& program, const SH_INPUT1& in1, const SH_INPUT2& in2,
           const SH_INPUT3& in3, const SH_OUTPUT res, const double epsilon)
  {
    if (on_host()) return 0; // skip this test

    typedef typename SH_OUTPUT::mem_type OT;
  
    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);
  
    SH::HostMemoryPtr mem_out = new SH::HostMemory(res.size()*sizeof(OT), _out, SH_OUTPUT::value_type);
    SH::Channel<typename SH_OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(3);
    SH::Program program1 = bind_input(program, in1, inputs[0]);
    SH::Program program2 = bind_input(program1, in2, inputs[1]);
    SH::Program program3 = bind_input(program2, in3, inputs[2]);
    chan_out = program3;
    mem_out->hostStorage()->sync();

    return output_result(program.name(), inputs, _out, _res, res.size(), epsilon);
  }

  /// Run stream test on current backend (4 input parameters)
  template <class SH_INPUT1, class SH_INPUT2, class SH_INPUT3, class SH_INPUT4, class SH_OUTPUT>
  int run(SH::Program& program, const SH_INPUT1& in1, const SH_INPUT2& in2,
           const SH_INPUT3& in3, const SH_INPUT4& in4, const SH_OUTPUT res, const double epsilon)
  {
    if (on_host()) return 0; // skip this test

    typedef typename SH_OUTPUT::mem_type OT;
  
    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);
  
    SH::HostMemoryPtr mem_out = new SH::HostMemory(res.size()*sizeof(OT), _out, SH_OUTPUT::value_type);
    SH::Channel<typename SH_OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(4);
    SH::Program program1 = bind_input(program, in1, inputs[0]);
    SH::Program program2 = bind_input(program1, in2, inputs[1]);
    SH::Program program3 = bind_input(program2, in3, inputs[2]);
    SH::Program program4 = bind_input(program3, in4, inputs[3]);
    chan_out = program4;
    mem_out->hostStorage()->sync();

    return output_result(program.name(), inputs, _out, _res, res.size(), epsilon);
  }

  /// Check results from running ops on the host
  template <class SH_OUTPUT, class EXPECTED>
  int check(std::string name, const SH_OUTPUT &out, const EXPECTED &res, const double epsilon)
  {
    if (!on_host()) return 0; // skip this test
 
    typedef typename SH_OUTPUT::host_type OT;
    OT* _out = new OT[out.size()];
    out.getValues(_out);
    
    OT* _res = new OT[res.size()];
    res.getValues(_res);
    
    if(out.size() != res.size()) {
      print_fail(name);
        std::cout << "Test data size mismatch" << std::endl;
        return 2;
    }
    
    std::vector<std::string> inputs(0);
    return output_result(name, inputs, _out, _res, res.size(), epsilon);
  }

  const std::string backend() const { return m_backend; }
  
private:
  void print_values(const char* varname, const std::string& values);
  void print_fail(std::string name);
  void print_pass(std::string name);

  bool on_host() { return m_backend == "host"; }
  
  std::string m_backend;
};

void init_tests(int argc, char** argv);

#endif
