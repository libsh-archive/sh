#include <complex>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <sh/ShOptimizations.hpp>
#include <sh/shutil.hpp>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include "Shell.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;

void dump(ShProgram &foo, string desc) {
  cout << "ShProgram " << desc << endl;
  cout << foo.node()->describe_interface() << endl; 

  ofstream out(desc.c_str()); 
  foo.node()->ctrlGraph->graphvizDump(out);
  string cmd = string("dot -Tps < ") + desc.c_str() + " > " + desc.c_str() + ".ps";
  system(cmd.c_str());
}

int main(int argc, char** argv)
{
  shSetBackend("cc");
  Shell s;
  vecs args;
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  s.start(cin, args, 0, true);
}
