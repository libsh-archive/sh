// -*- C++ -*-
#include <iostream>
#include <fstream>
#include "sh.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"

using namespace SH;

ShTexCoord2f posn;

ShProgram u(const ShProgram& a, const ShProgram& b)
{
  return max<ShAttrib1f>() << (a & b);
}

ShProgram i(const ShProgram& a, const ShProgram& b)
{
  return min<ShAttrib1f>() << (a & b);
}

ShProgram s(const ShProgram& a, const ShProgram& b)
{
  return i(a, sub<ShAttrib1f>() << ShConstant1f(1.0) << b);
}

ShProgram rect(float x, float y,
               float w, float h)
{
  ShProgram res = SH_BEGIN_PROGRAM() {
    ShVector2f o = posn - ShConstant2f(x, y);
    ShOutputAttrib1f result = min((o(0) > 0.0)*(o(1) > 0.0), (o(0) < w)*(o(1) < h));
  } SH_END;
  return res;
}

ShProgram srect(float x, float y,
                float w, float h,
                float skew)
{
  ShProgram res = SH_BEGIN_PROGRAM() {
    ShVector2f o = posn - ShConstant2f(x, y);
    o(0) -= o(1) * (skew/h);
    ShOutputAttrib1f result = min((o(0) > 0.0)*(o(1) > 0.0), (o(0) < w)*(o(1) < h));
  } SH_END;
  return res;
}

ShProgram circ(float x, float y, float r)
{
  ShProgram res = SH_BEGIN_PROGRAM() {
    ShVector2f o = posn - ShConstant2f(x, y);
    ShOutputAttrib1f result = ((o | o) < (r*r));
  } SH_END;
  return res;
}

ShProgram doText(const std::string& text)
{
  float px = 0.0;
  float py = 0.0;
  float sep = 7.0;
  float lineheight = 60.0;
  float linesep = 20.0;
  ShProgram phrase = SH_BEGIN_PROGRAM() { ShOutputAttrib1f result = 0.0; } SH_END;
  for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
    ShProgram letter = SH_BEGIN_PROGRAM() { ShOutputAttrib1f result = 0.0; } SH_END;
    switch (*c) {
    case 'S':
      {
        float ro = 18.0;
        float w = 12.0;
        float ri = ro - w;
        letter = u(s(s(circ(px + ro, py + ro, ro), circ(px + ro, py + ro, ri)),
                   rect(px, py + ro, ro, ro)),
                 s(s(circ(px + ro, py + ro + ro + ri, ro), circ(px + ro, py + ro + ro + ri, ri)),
                   rect(px + ro, py + ro + ri, ro, ro)));
        px += ro + ro;
      }
      break;
    default:
      break;
    }
    phrase = u(phrase, letter);
    px += sep;
  }
  return phrase >> posn;
}


int main(int argc, char** argv)
{
  std::string backendName("arb");
  if (argc > 1) {
    backendName = argv[1];
  }

  try {
    ShColor3f SH_DECL(color) = ShColor3f(1.0, 1.0, 1.0);
  
    ShProgram s_renderer = SH_BEGIN_FRAGMENT_PROGRAM {
      ShInputAttrib1f in;
      ShOutputColor3f out = color;
      ShAttrib1f x = in < 0.1;
      kill(x);
    } SH_END;

    ShProgram foo = SH_BEGIN_PROGRAM() {
      ShOutputAttrib1f foo = 0.0;
    } SH_END;
    
    ShProgram shader = s_renderer << doText("S");
    
    ShBackendPtr backend = ShBackend::lookup(backendName);

    if (!backend) {
      std::cerr << "Error opening " << backendName << " backend." << std::endl;
      return -1;
    }

    SH_DEBUG_ASSERT(!shader->target().empty());

    ShBackendCodePtr code = shader->code(backend);
    std::cout << "Printing Input/Output Format:" << std::endl;
    code->printInputOutputFormat(std::cout);

    std::cout << "Printing code:" << std::endl;
    code->print(std::cout);

    std::string dumpfilename = argv[0];

    // Get rid of some libtool annoyingness
    int slash;
    for (slash = dumpfilename.size() - 1; slash >= 0; slash--) {
      if (dumpfilename[slash] == '/') break;
    }
    if (dumpfilename[slash] == '/') dumpfilename = dumpfilename.substr(slash + 1);
    if (dumpfilename.substr(0, 3) == "lt-") dumpfilename = dumpfilename.substr(3);

    dumpfilename += "-control.dot";

    std::ofstream dumpfile(dumpfilename.c_str());
    shader->ctrlGraph->graphvizDump(dumpfile);
 
    return 0;
  } catch (const ShException& e) {
    std::cerr << "SH Exception: " << e.message() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown exception caught." << std::endl;
    return -1;
  }
}
