#include <complex>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/Optimizations.hpp>
#include <shutil/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include "Shell.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

int main(int argc, char** argv)
{
  setBackend("arb");
  Shell s;
  vecs args;
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  s.start(cin, args, 0, true);
}
