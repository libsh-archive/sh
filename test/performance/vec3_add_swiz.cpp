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
    foo(0,2,1) = a(1,2,0) + foo(1,0,2); 
  }
  std::cout  << foo << std::endl;
  return 0;
}

