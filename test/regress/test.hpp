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
    std::cout << varname << " = [ ";
    std::cout << std::setiosflags(std::ios::right);
    for(int j = 0; j < size; j++) {
      if (j != 0) std::cout << ",";
      std::cout << std::setw(10) << values[j];
    }
    std::cout << " ]" << std::endl;;
  }

  template<typename T>
  void mem_from_host(typename T::mem_type mem[], T &host) {
    SH::ShVariantPtr memVariant = new SH::ShDataVariant<typename T::storage_type, SH::SH_MEM>(mem, host.size(), false);
    memVariant->set(host.getVariant());
  }

  template <class INPUT1, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const OUTPUT& res)
  {
    typedef typename INPUT1::mem_type IT;
    typedef typename OUTPUT::mem_type OT;

    std::string name = program.name();
    IT* _in1 = new IT[in1.size()];
    mem_from_host(_in1, in1);

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(IT), _in1);
    SH::ShChannel<typename INPUT1::TempType> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    chan_out = program << chan_in1;

    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        print_fail(name);
        pretty_print("  A", in1.size(), _in1);
        pretty_print("out", res.size(), _out);
        pretty_print("exp", res.size(), _res);
        return;
      }
    }
#ifdef SH_REGRESS_SHOWALL
      /* DEBUG */ pretty_print("out", res.size(), _out);
      /* DEBUG */ pretty_print("exp", res.size(), _res);
#endif
    print_pass(name);
  }

  template <class INPUT1, class INPUT2, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const INPUT2& in2,
           const OUTPUT& res)
  {
    typedef typename INPUT1::mem_type IT1;
    typedef typename INPUT2::mem_type IT2;
    typedef typename OUTPUT::mem_type OT;

    std::string name = program.name();
    IT1* _in1 = new IT1[in1.size()];
    mem_from_host(_in1, in1);

    IT2* _in2 = new IT2[in2.size()];
    mem_from_host(_in2, in2);

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(IT1), _in1);
    SH::ShChannel<typename INPUT1::TempType> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_in2 = new SH::ShHostMemory(in2.size()*sizeof(IT2), _in2);
    SH::ShChannel<typename INPUT2::TempType> chan_in2(mem_in2, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    chan_out = program << chan_in1 << chan_in2;

    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        print_fail(name);
        pretty_print("  A", in1.size(), _in1);
        pretty_print("  B", in2.size(), _in2);
        pretty_print("out", res.size(), _out);
        pretty_print("exp", res.size(), _res);
        return;
      }
    }
#ifdef SH_REGRESS_SHOWALL
      /* DEBUG */ pretty_print("out", res.size(), _out);
      /* DEBUG */ pretty_print("exp", res.size(), _res);
#endif    
    print_pass(name);
  }

  template <class INPUT1, class INPUT2, class INPUT3, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const INPUT2& in2,
           const INPUT3& in3,
           const OUTPUT res)
  {
    typedef typename INPUT1::mem_type IT1;
    typedef typename INPUT2::mem_type IT2;
    typedef typename INPUT3::mem_type IT3;
    typedef typename OUTPUT::mem_type OT;

    std::string name = program.name();
  
    IT1* _in1 = new IT1[in1.size()];
    mem_from_host(_in1, in1);

    IT2* _in2 = new IT2[in2.size()];
    mem_from_host(_in2, in2);

    IT3* _in3 = new IT3[in3.size()];
    mem_from_host(_in3, in3);

    OT* _out = new OT[res.size()];
    mem_from_host(_out, res);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10;

    OT* _res = new OT[res.size()];
    mem_from_host(_res, res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(IT1), _in1);
    SH::ShChannel<typename INPUT1::TempType> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_in2 = new SH::ShHostMemory(in2.size()*sizeof(IT2), _in2);
    SH::ShChannel<typename INPUT2::TempType> chan_in2(mem_in2, 1);

    SH::ShHostMemoryPtr mem_in3 = new SH::ShHostMemory(in3.size()*sizeof(IT3), _in3);
    SH::ShChannel<typename INPUT3::TempType> chan_in3(mem_in3, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(OT), _out);
    SH::ShChannel<typename OUTPUT::TempType> chan_out(mem_out, 1);

    chan_out = program << chan_in1 << chan_in2 << chan_in3;
  
    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        print_fail(name);
        pretty_print("  A", in1.size(), _in1);
        pretty_print("  B", in2.size(), _in2);
        pretty_print("  C", in2.size(), _in2);
        pretty_print("out", res.size(), _out);
        pretty_print("exp", res.size(), _res);
        return;
      }
    }
#ifdef SH_REGRESS_SHOWALL
      /* DEBUG */ pretty_print("out", res.size(), _out);
      /* DEBUG */ pretty_print("exp", res.size(), _res);
#endif
  }

  /// Checks results from running ops on the host
  template <class OUTPUT, class EXPECTED>
  void check(std::string name, const OUTPUT &out, const EXPECTED &res)
  {
      typedef typename OUTPUT::host_type OT;
      OT* _out = new OT[out.size()];
      out.getValues(_out);

      OT* _res = new OT[res.size()];
      res.getValues(_res);

      if(out.size() != res.size()) {
        print_fail(name);
        std::cout << "Test data size mismatch" << std::endl;
        return;
      }

      for(int i = 0; i < out.size(); ++i) {
        if (fabs(_out[i] - _res[i]) > EPSILON) {
          print_fail(name);
          pretty_print("out", out.size(), _out); 
          pretty_print("exp", res.size(), _res); 
          return;
        }
      }
#ifdef SH_REGRESS_SHOWALL
        /* DEBUG */ pretty_print("out", res.size(), _out);
        /* DEBUG */ pretty_print("exp", res.size(), _res);
#endif
      print_pass(name);
  }

  bool on_host()
  {
    return m_backend == "host";
  }

  
private:
  void print_fail(std::string name);
  void print_pass(std::string name);
  
  std::string m_backend;
};

void init_tests(int argc, char** argv);
         

#endif
