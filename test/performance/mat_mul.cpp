#include <cmath>
#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  shInit();
  ShMatrix4x4f foo;
  ShMatrix4x4f a;

  for(int i = 0; i < 4; ++i) for(int j = 0; j < 4; ++j) {
    foo[i][j] = i * M_PI / j;
    a[i][j] = sin(i + j * 2.14);
  }

  for(int i = 0; i < 30000; ++i) {
    foo = a * foo; 
  }
  std::cout  << foo << std::endl;
  return 0;
}

