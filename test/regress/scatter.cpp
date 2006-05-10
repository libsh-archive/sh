#include <sh/sh.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "test.hpp"

using namespace std;
using namespace SH;

int main(int argc, char* argv[])
{
  int errors = 0;
  int total_tests = 0;

  Test test(argc, argv);
  test.ignore_backend("host");

  vector<string> inputs;
  inputs.push_back("src"); inputs.push_back("index");

  {
    ++total_tests;
    Array1D<Attrib3f> dest(5);
    float* dest_data = dest.write_data();
    for (int i = 0; i < 3*5; ++i) {
      dest_data[i] = i/3 * 10 + (i%3);
    }
    Array1D<Attrib1f> index(1);
    index.write_data()[0] = 3;
    Array1D<Attrib3f> data(1);
    float d[] = {123,456,789};
    copy(d, d+3, data.write_data());
    float expected[] = {0,1,2, 10,11,12, 20,21,22, 123,456,789, 40,41,42};
    
    try {
      dest[index] = data;
      if (test.output_result<float*>("single", inputs, dest.read_data(),
                                     expected, 3, 0.0))
        ++errors;
    } catch (const Exception& e) {
      cout << "SH Exception '" << e.message() << "'" << endl;
    } catch (const exception& e) {
      cout << "C++ Exception '" << e.what() << "'" << endl;
    }
  }

  {
    ++total_tests;
    Array1D<Attrib4s> dest(5);
    short* dest_data = dest.write_data();
    for (int i = 0; i < 4*5; ++i) {
      dest_data[i] = i/4 * 10 + (i%4);
    }
    Array1D<Attrib1i> index(3);
    int* index_data = index.write_data();
    index_data[0] = 1; index_data[1] = 3; index_data[2] = 4;
    Array1D<Attrib3s> data(3);
    short d[] = {111,222,333,444,555,666,777,888,999};
    copy(d, d+9, data.write_data());
    short expected[] = {0,1,2, 111,222,333, 20,21,22, 444,555,666, 777,888,999};
    
    try {
      dest[index] = data;
      if (test.output_result<short*>("multiple", inputs, dest.read_data(),
                                     expected, 3, 0.0))
        ++errors;
    } catch (const Exception& e) {
      cout << "SH Exception '" << e.message() << "'" << endl;
    } catch (const exception& e) {
      cout << "C++ Exception '" << e.what() << "'" << endl;
    }
  }

  if (errors != 0) {
    cout << "Total Errors: " << errors << "/" << total_tests << endl;
    return 1;
  }

  return 0;
} 
