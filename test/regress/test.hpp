#ifndef SH_REGRESS_TEST_HPP
#define SH_REGRESS_TEST_HPP

#include <iostream>
#include <iomanip>
#include <sh.hpp>

#define EPSILON 1e-5

#define COLOR_GREEN "[32m"
#define COLOR_RED "[31m"
#define COLOR_BLUE "[34m"
#define COLOR_YELLOW "[33m"
#define COLOR_NORMAL "[0m"

class Test {
public:
  Test(int argc, char** argv);

  template <class INPUT1, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const OUTPUT& res)
  {
    std::string name = program.name();
    float* _in1 = new float[in1.size()];
    in1.getValues(_in1);

    float* _out = new float[res.size()];
    res.getValues(_out);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10.0;

    float* _res = new float[res.size()];
    res.getValues(_res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(float), _in1);
    SH::ShChannel<INPUT1> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(float), _out);
    SH::ShChannel<OUTPUT> chan_out(mem_out, 1);

    chan_out = program << chan_in1;

    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
                  << std::setiosflags(std::ios::left) << std::setw(50) << name
                  << COLOR_RED
                  << " FAILED"
                  << COLOR_NORMAL
                  << " [" << m_backend << "]"
                  << std::endl;

        std::cout << "out = [ ";
        std::cout << std::setiosflags(std::ios::right);
        for(int j = 0; j < res.size(); j++)
          {
            if (j != 0) std::cout << ",";
            std::cout << std::setw(10) << _out[j];
          }
        std::cout << " ]" << std::endl;;

        std::cout << "res = [ ";
        std::cout << std::setiosflags(std::ios::right);
        for(int j = 0; j < res.size(); j++)
          {
            if (j != 0) std::cout << ",";
            std::cout << std::setw(10) << _res[j];
          }
        std::cout << " ]" << std::endl;;
        std::cout << std::resetiosflags(std::ios::right);

        return;
      }
    }
    std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name
              << COLOR_GREEN
              << " PASSED"
              << COLOR_NORMAL << std::endl;
  }

  template <class INPUT1, class INPUT2, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const INPUT2& in2,
           const OUTPUT& res)
  {
    std::string name = program.name();
    float* _in1 = new float[in1.size()];
    in1.getValues(_in1);

    float* _in2 = new float[in2.size()];
    in2.getValues(_in2);

    float* _out = new float[res.size()];
    res.getValues(_out);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10.0;

    float* _res = new float[res.size()];
    res.getValues(_res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(float), _in1);
    SH::ShChannel<INPUT1> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_in2 = new SH::ShHostMemory(in2.size()*sizeof(float), _in2);
    SH::ShChannel<INPUT2> chan_in2(mem_in2, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(float), _out);
    SH::ShChannel<OUTPUT> chan_out(mem_out, 1);

    chan_out = program << chan_in1 << chan_in2;

    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
                  << std::setiosflags(std::ios::left) << std::setw(50) << name
                  << COLOR_RED
                  << " FAILED"
                  << COLOR_NORMAL
                  << " [" << m_backend << "]"
                  << std::endl;

        std::cout << "out = [ ";
        std::cout << std::setiosflags(std::ios::right);
        for(int j = 0; j < res.size(); j++)
          {
            if (j != 0) std::cout << ",";
            std::cout << std::setw(10) << _out[j];
          }
        std::cout << " ]" << std::endl;

        std::cout << "res = [ ";
        std::cout << std::setiosflags(std::ios::right);
        for(int j = 0; j < res.size(); j++)
          {
            if (j != 0) std::cout << ",";
            std::cout << std::setw(10) << _res[j];
          }
        std::cout << " ]" << std::endl;
        std::cout << std::resetiosflags(std::ios::right);

        return;
      }
    }
    std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name
              << COLOR_GREEN
              << " PASSED"
              << COLOR_NORMAL << std::endl;
  }

  template <class INPUT1, class INPUT2, class INPUT3, class OUTPUT>
  void run(SH::ShProgram& program,
           const INPUT1& in1,
           const INPUT2& in2,
           const INPUT3& in3,
           const OUTPUT res)
  {
    std::string name = program.name();
  
    float* _in1 = new float[in1.size()];
    in1.getValues(_in1);

    float* _in2 = new float[in2.size()];
    in2.getValues(_in2);

    float* _in3 = new float[in3.size()];
    in3.getValues(_in3);

    float* _out = new float[res.size()];
    res.getValues(_out);
    // Arbitrarily change output values
    for (int i = 0; i < res.size(); i++) _out[i] += 10.0;

    float* _res = new float[res.size()];
    res.getValues(_res);

    SH::ShHostMemoryPtr mem_in1 = new SH::ShHostMemory(in1.size()*sizeof(float), _in1);
    SH::ShChannel<INPUT1> chan_in1(mem_in1, 1);

    SH::ShHostMemoryPtr mem_in2 = new SH::ShHostMemory(in2.size()*sizeof(float), _in2);
    SH::ShChannel<INPUT2> chan_in2(mem_in2, 1);

    SH::ShHostMemoryPtr mem_in3 = new SH::ShHostMemory(in3.size()*sizeof(float), _in3);
    SH::ShChannel<INPUT3> chan_in3(mem_in3, 1);

    SH::ShHostMemoryPtr mem_out = new SH::ShHostMemory(res.size()*sizeof(float), _out);
    SH::ShChannel<OUTPUT> chan_out(mem_out, 1);

    chan_out = program << chan_in1 << chan_in2 << chan_in3;
  
    for(int i = 0; i < res.size(); i++) {
      if (fabs(_out[i] - _res[i]) > EPSILON) {
        std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
                  << std::setiosflags(std::ios::left) << std::setw(50) << name
                  << COLOR_RED
                  << " FAILED"
                  << COLOR_NORMAL
                  << " [" << m_backend << "]"
                  << std::endl;
        return;
      }
    }
    std::cout << COLOR_YELLOW << "Test: " << COLOR_NORMAL
              << std::setiosflags(std::ios::left) << std::setw(50) << name
              << COLOR_GREEN
              << " PASSED"
              << COLOR_NORMAL << std::endl;
  }
  
private:
  
  std::string m_backend;
};

void init_tests(int argc, char** argv);
         

#endif
