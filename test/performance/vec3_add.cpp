#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  shInit();
  ShVector3f foo(10, 10, 10);
  ShVector3f a(8, 9, 10);

  for(int i = 0; i < 1000000; ++i) {
    foo = a + foo; 
  }
  std::cout  << foo << std::endl;
  return 0;
}

