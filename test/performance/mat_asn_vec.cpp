#include <sh.hpp>

using namespace SH;

int main() {
  shInit();
  ShMatrix4x4f m;
  int i, j, k;
  for (i = 0; i < 100000; i++ )
    for (j = 0;j < 4;j++)
      m[j] = ShConstAttrib4f(0, 0, 0, 0);
}
