#include <sh.hpp>

using namespace SH;

int main() {
  shInit();
  ShMatrix4x4f m;
  int i, j, k;
  for (i = 0; i < 100000; i++ )
    for (j = 0;j < 4;j++)
      for (k = 0; k < 4; k++)
      m[j][k] = 0;
}
