#include <sstream>
#include <fstream>
#include <ctype.h>
#include "Shell.hpp"

using namespace SH;
using namespace std;


vecs tokenize(string s) { 
  vecs result;
  int i, j, l;
  i = j = 0; l = s.length();
  for(;i < l;) {
    for(; i < l && isspace(s[i]); ++i);
    for(j = i; j < l && !isspace(s[j]); ++j);
    if(i < l) {
      result.push_back(s.substr(i, j - i));
    }
    i = j;
  }
  return result;
}

void untokenize(const vecs& vs, char* result, int max) {
  ostringstream out;
  for(vecs::const_iterator S = vs.begin(); S != vs.end(); ++S) {
    out << *S << " ";
  }
  strncpy(result, out.str().c_str(), max); 
}


void runit(string cmd, string arg) {
  if(fork() == 0) {
    execl("/bin/bash", "/bin/bash", "-c", (cmd + " " + arg).c_str(), (const char *) 0); 
  } else {
    wait();
  }
}

#define EXPECT(N) if(toks.size() < N) { cerr << "Expected " << N << " args " << endl; }

std::istream& Shell::start(std::istream& in, vecs args, bool prompt) {
#define BUFLEN 1000
  char buf[BUFLEN];
  enum State {
    DEFAULT,
    INPROG,
    INCODE,
  };

  State s = DEFAULT;
  vecs toks;
  string prog;
  string name;

  for(;;) {
    if(prompt) {
      switch(s) {
        case INPROG:
            cout << "P>"; break;
        case INCODE:
            cout << "C>"; break;
        default: break;
      }
    }
    in.getline(buf, BUFLEN);
    if(in.eof()) break;

    vecs toks = tokenize(buf);
    if(toks.size() > 0 && (toks[0] == "//" || toks[0] == "#")) {
      continue;
    }
    // arg replacement
    bool change = false; 
    for(size_t i = 0; i < toks.size(); ++i) {
      string oldtok = toks[i];
      string &tok = toks[i];
      for(size_t j = 0; j < tok.length() - 1; ++j) { 
        if(tok[j] == '$' && isdigit(tok[j+1])) {
          int n;
          size_t k = j + 1;
          for(n = 0; k < tok.length() && isdigit(tok[k]); n *= 10, n += tok[k] - '0', k++);
          if(n - 1 >= args.size()) {
            std::cout << "Missing argument " << n << endl; 
          } else {
            tok = tok.substr(0, j) + args[n - 1] + tok.substr(k, tok.length());
            j += args[n - 1].length() - 1;
          }
        }
      }
      if(oldtok != tok) {
        change = true;
      }
    }
    if(change) {
#if 0
      cerr << buf << endl;
      for(size_t i = 0; i < toks.size(); ++i) {
        cerr << toks[i] << " ";
      }
      cerr << endl;
#endif
      untokenize(toks, buf, BUFLEN);
    }

    switch(s) {
      case DEFAULT:
        try {

          if(toks.size() > 0) {
            if(toks[0] == "p") { EXPECT(2); 
              name = toks[1];
              s = INPROG;
            } else if (toks[0] == "c") {
              s = INCODE;
            } else if (toks[0] == "dot") { EXPECT(2);
              name = toks[1];
              ep[name].node()->dump(name);

            } else if (toks[0] == "incl") { EXPECT(3);
              std::cerr << toks[2] << " =  incl " << toks[1] << endl;
              ShProgram result = inclusion(ep[toks[1]]);
              result.name(toks[2]);
              ep.registerFunc(toks[2], result); 

            } else if (toks[0] == "aincl") { EXPECT(3);
              std::cerr << toks[2] << " =  aincl " << toks[1] << endl;
              ShProgram result = affine_inclusion(ep[toks[1]]);
              result.name(toks[2]);
              ep.registerFunc(toks[2], result); 

            } else if (toks[0] == "conv") { EXPECT(3);
              std::cerr << toks[2] << " =  aconv " << toks[1] << endl;
              ShProgram cprog(ep[toks[1]].node()->clone());
              cprog.name(toks[2]);

              ShContext::current()->enter(cprog.node());
                SH::ShTransformer tf(cprog.node());
                tf.handleDbgOps();
                tf.convertInputOutput();
                tf.convertTextureLookups();
                tf.convertAffineTypes();
                tf.convertIntervalTypes();
                optimize(cprog);
              ShContext::current()->exit();

              ep.registerFunc(toks[2], cprog); 

            } else if (toks[0] == "meta") { EXPECT(4);
              std::cerr << toks[1] << ".meta(" << toks[2] << ") = " << toks[3] << endl;
              ep[toks[1]].meta(toks[2], toks[3]);

            } else if (toks[0] == "print") { EXPECT(2);
              for(size_t i = 1; i < toks.size(); ++i) {
                if(ep.hasVar(toks[i])) {
                  std::cout << ep.getVar(toks[i]) << " ";
                } else {
                  std::cout << toks[i] << " ";
                }
              }
              std::cout << endl;
            } else if (toks[0] == "stream") { EXPECT(3);
              name = toks[1];
              ShStream result; 
              for(size_t i = 2; i < toks.size(); ++i) {
                ShStream s = ep.getStream(toks[i]);
                result = result & s;  
              }
              ep.registerStream(name, result);
            } else if (toks[0] == "apply") { EXPECT(4);
              if(!ep.hasStream(toks[1])) {
                std::cout << "Couldn't find stream " << toks[1] << endl; 
              }
              ShStream outStream = ep.getStream(toks[1]);

              name = toks[2];
              ShProgram prog = ep[name];

              if(!ep.hasStream(toks[3])) {
                std::cout << "Couldn't find stream " << toks[3] << endl; 
              }

              ShStream inStream = ep.getStream(toks[3]);
              outStream = prog << inStream; 

            } else if (toks[0] == "r") { EXPECT(2);
              std::cerr << "Reading file " << toks[1] << endl; 
              std::ifstream fin(toks[1].c_str());
              vecs args;
              for(size_t i = 2; i < toks.size(); ++i) {
                args.push_back(toks[i]);
              }
              start(fin, args, false);

            } else if (toks[0] == "b") { EXPECT(2);
              shSetBackend(toks[1]);

            } else if (toks[0] == "dump") {
              ep.dump(std::cout);
              
            // file commands
            } else if (toks[0] == "ls") { EXPECT(2);
              std::cerr << "Listing files *" << toks[1] << endl;
              runit("/bin/ls", "*" + toks[1] + "*");
            } else if (toks[0] == "gv") { EXPECT(2);
              std::cerr << "Viewing file " << endl;
              runit("/usr/bin/gv", toks[1] + ".ps");
            } else if (toks[0] == "vars") { EXPECT(2);
              std::cerr << "Viewing file " << endl;
              runit("/bin/cat", toks[1] + ".vars");

            } else if (toks[0] == "q") {
              cout << endl << "Quitting" << endl;
              exit(EXIT_SUCCESS);
            } else {
              prog = buf;
              prog += ";";
              ep.parse(prog);
            }
          }
        } catch (ShException& se) {
          cerr << "Exception: " << se.message() << endl;
        }
        break;
      case INPROG:
        if(toks.size() == 1 && toks[0] == "p") {
          std::cerr << "Parsing Program:" << endl << prog << endl;
          //ShContext::current()->optimization(0);
          ShProgram newprog;
          try {
            newprog  = SH_BEGIN_PROGRAM("gpu:stream"); {
              ep.parse(prog);
            } SH_END;
          } catch (ShException e) {
            cerr << "Exception:" << e.message() << endl;
          }
          ep.registerFunc(name, newprog);
          cerr << "Added program " << name << endl;
          s = DEFAULT; 
        } else {
          prog += buf;
          prog += ";\n";
        }
        break;
      case INCODE:
        if(toks.size() == 1 && toks[0] == "c") {
          s = DEFAULT;
        } else {
          prog = buf;
          prog += ";\n";
          try {
            ep.parse(prog);
          } catch (ShException e) {
            cerr << "Exception:" << e.message() << endl;
          }
                
        }
        break;
    }
  }
  return in;
}
