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
#ifndef SH_REGRESS_TEST_HPP
#define SH_REGRESS_TEST_HPP

#include <iostream>
#include <iomanip>
#include <sh.hpp>

// @todo type fix epsilon checks
#define EPSILON 1e-2

#define COLOR_GREEN "[32m"
#define COLOR_RED "[31m"
#define COLOR_BLUE "[34m"
#define COLOR_YELLOW "[33m"
#define COLOR_NORMAL "[0m"

// turns on output of all results, even if they pass 
//#define SH_REGRESS_SHOWALL 

class Test {
public:
  // reads backend from command line (default gcc) 
  Test(int argc, char** argv);

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

  template<typename T>
  void mem_from_host(typename T::mem_type mem[], T &host) {
    SH::ShVariantPtr memVariant = new SH::ShDataVariant<typename T::storage_type, SH::SH_MEM>(mem, host.size(), false);
    memVariant->set(host.getVariant());
  }

  template<class T>
  SH::ShProgram bind_input(SH::ShProgram& program, const T& in, std::string& str)
  {
    using namespace std;
    using namespace SH;
    
    typedef typename T::mem_type IT;
    IT* _in = new IT[in.size()];
    mem_from_host(_in, in);
    
    ShHostMemoryPtr mem_in = new ShHostMemory(in.size()*sizeof(IT), _in);
    ShChannel<typename T::TempType> chan_in(mem_in, 1);
    
    stringstream s;
    s << setiosflags(ios::right);
    for(int j = 0; j < in.size(); j++) {
	if (j != 0) s << ",";
	s << setw(10) << _in[j];
    }
    str = s.str();
    
    return program << chan_in;
  }

  template<typename T>
  int output_result(const std::string& name, const std::vector<std::string>& inputs,
		    const T& out, const T& exp, int out_size)
  {
    for(int i=0; i < out_size; i++) {
      if (fabs(out[i] - exp[i]) > EPSILON) {
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
    pretty_print("out", size, out);
    pretty_print("exp", size, exp);
    print_pass(name);
#endif
    return 0;
  }

  template <class INPUT1, class OUTPUT>
  int run(SH::ShProgram& program, const INPUT1& in1, const OUTPUT& res)
  {
    if (on_host()) return 0; // skip this test

    typedef typename OUTPUT::mem_type OT;

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(1);
    SH::ShProgram program1 = bind_input(program, in1, inputs[0]);
    chan_out = program1;

    return output_result(program.name(), inputs, _out, _res, res.size());
  }

  template <class INPUT1, class INPUT2, class OUTPUT>
  int run(SH::ShProgram& program, const INPUT1& in1, const INPUT2& in2,
	  const OUTPUT& res)
  {
    if (on_host()) return 0; // skip this test

    typedef typename OUTPUT::mem_type OT;

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(2);
    SH::ShProgram program1 = bind_input(program, in1, inputs[0]);
    SH::ShProgram program2 = bind_input(program1, in2, inputs[1]);
    chan_out = program2;

    return output_result(program.name(), inputs, _out, _res, res.size());
  }

  template <class INPUT1, class INPUT2, class INPUT3, class OUTPUT>
  int run(SH::ShProgram& program, const INPUT1& in1, const INPUT2& in2,
           const INPUT3& in3, const OUTPUT res)
  {
    if (on_host()) return 0; // skip this test

    typedef typename OUTPUT::mem_type OT;
  
    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);
  
    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    std::vector<std::string> inputs(3);
    SH::ShProgram program1 = bind_input(program, in1, inputs[0]);
    SH::ShProgram program2 = bind_input(program1, in2, inputs[1]);
    SH::ShProgram program3 = bind_input(program2, in3, inputs[2]);
    chan_out = program3;

    return output_result(program.name(), inputs, _out, _res, res.size());
  }

  /// Checks results from running ops on the host
  template <class OUTPUT, class EXPECTED>
  int check(std::string name, const OUTPUT &out, const EXPECTED &res)
  {
      typedef typename OUTPUT::host_type OT;
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
      return output_result(name, inputs, _out, _res, res.size());
  }
  
private:
  void print_values(const char* varname, const std::string& values);
  void print_fail(std::string name);
  void print_pass(std::string name);

  bool on_host() { return m_backend == "host"; }
  
  std::string m_backend;
};

void init_tests(int argc, char** argv);

#endif
