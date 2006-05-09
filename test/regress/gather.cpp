#include <sh/sh.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "test.hpp"

using namespace std;
using namespace SH;

template <typename T1, typename T2>
void test_type(Test& test, int& total_tests, int& errors)
{
  Array1D<T1> src(100);
  typename T1::mem_type* src_data = src.write_data();
  for (int i = 0; i < T1::typesize*100; ++i) {
    src_data[i] = i/T1::typesize * 10 + (i % T1::typesize);
  }

  vector<string> inputs;
  inputs.push_back("src"); inputs.push_back("index");

  {
    ++total_tests;
    Array1D<T2> index(1);
    index.write_data()[0] = 10;
    Array1D<T1> result(1);
    typename T1::mem_type expected[T1::typesize];
    for (int i = 0; i < T1::typesize; ++i) {
      expected[i] = 100 + i;
    }
    
    try {
      result = src[index];
      if (test.output_result<typename T1::mem_type*>("single", inputs,
                                                     result.read_data(),
                                                     expected, T1::typesize, 0.0))
        ++errors;
    } catch (const Exception& e) {
      cout << "SH Exception '" << e.message() << "'" << endl;
    } catch (const exception& e) {
      cout << "C++ Exception '" << e.what() << "'" << endl;
    }
  }

  {
    ++total_tests;
    Array1D<T2> index(3);
    typename T2::mem_type index_data[] = {10, 13, 4};
    copy(index_data, index_data+3, index.write_data());
    Array1D<T1> result(3);
    typename T1::mem_type expected[3*T1::typesize];
    for (int i = 0; i < T1::typesize; ++i) {
      expected[0*T1::typesize + i] = 100 + i;
      expected[1*T1::typesize + i] = 130 + i;
      expected[2*T1::typesize + i] = 40 + i;
    }
    
    try {
      result = src[index];
      if (test.output_result<typename T1::mem_type*>("many", inputs, 
                                                     result.read_data(),
                                                     expected, T1::typesize*3, 0.0))
        ++errors;
    } catch (const Exception& e) {
      cout << "SH Exception '" << e.message() << "'" << endl;
    } catch (const exception& e) {
      cout << "C++ Exception '" << e.what() << "'" << endl;
    }
  }
}

int main(int argc, char* argv[])
{
  int errors = 0;
  int total_tests = 0;

  Test test(argc, argv);
  test.ignore_backend("host");

  test_type<Attrib3f, Attrib1f>(test, total_tests, errors);
  test_type<Attrib4f, Attrib1i>(test, total_tests, errors);
  test_type<Attrib2i, Attrib1i>(test, total_tests, errors);

  if (errors != 0) {
    cout << "Total Errors: " << errors << "/" << total_tests << endl;
    return 1;
  }

  return 0;
} 
