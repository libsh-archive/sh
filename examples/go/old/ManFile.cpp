#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <libxml++/libxml++.h>
#include "man.hpp"
#include "ManFile.hpp"

using namespace SH;
using namespace std;

/* Shapes read in from file
 * Currently supported:
 *  - SVG curves
 *  - BEZIER_SURFACE http://people.scs.fsu.edu/~burkardt/data/bezier_surface/bezier_surface.html
 */

namespace {
/* Reads all paths in the SVG file and build a curve */
class PathFinder : public xmlpp::SaxParser
{
  public:
    PathFinder() {}
    virtual ~PathFinder() {}

    Curve2D allPaths() {
      return m_uniform_concat(m_paths, 0);
    }

  protected:
    //overrides:
    virtual void on_start_element(const Glib::ustring& name,
                                  const AttributeList& attributes) {
      if(name == "path") {
        for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
          if(iter->name == "d") addPath(iter->value);
        }
      }
    }

    ConstAttrib1f getScalar(istringstream& pin) {
      float f;
      pin >> f;
      return f;
    }

    ConstPoint2f getPoint(istringstream& pin) {
      float x, y;
      pin >> x;
      pin.ignore(256, ',');
      pin >> y;
      return ConstPoint2f(x, y);
    }

    /* Split up the elements in the string into curves and add them to the PathVec */
    void addPath(const Glib::ustring& s) {
      istringstream pin(s);
      bool done = false;
      ConstPoint2f first, last;
      for(int i = 0;!done; ++i) {
        char command;
        pin >> skipws >> command; 
        if(pin.eof()) break;

        /* Make sure we get *new* constants for each segment 
         * (instead of reusing th esame uniform - which causes big problems) */
        ConstPoint2f p1, p2, end; /* parameters used in different curves */
        ConstPoint2f lastCopy = last;

        switch(command) {
          case 'M': /* moveto */
            first = last = end = getPoint(pin); 
            break;

          case 'L': /* lineto */
            end = getPoint(pin);
            m_paths.push_back(m_line(mx_<1>(0), m_(lastCopy), m_(end)));
            break;

          case 'H': /* horizontal line */
            end = last;
            end(0) = getScalar(pin);
            m_paths.push_back(m_line(mx_<1>(0), m_(lastCopy), m_(end)));
            break;

          case 'V': /* vertical line */
            end = last;
            end(1) = getScalar(pin);
            m_paths.push_back(m_line(mx_<1>(0), m_(lastCopy), m_(end)));
            break;

          case 'C': /* cubic bezier to */ {
              Man<0, 2> p[4]; 
              p[0] = last;
              p[1] = getPoint(pin);
              p[2] = getPoint(pin);
              end = p[3] = getPoint(pin);
              m_paths.push_back(m_cubic_bezier(mx_<1>(0), p));
              break;
            }
          case 'Z': /* done*/
            done = true;
            break;
        }
        last = end;
      }
    }

    typedef vector<Curve2D> PathVec;
    PathVec m_paths;
};
}

/* Reads all paths in the SVG file and build a curve */
Curve2D m_svg_crv(const std::string& filename) {
  PathFinder pf;
  try {
    pf.set_substitute_entities(true);
    pf.parse_file(filename);
  } catch(const xmlpp::exception& ex) {
    std::cout << "libxml++ exception: " << ex.what() << std::endl;
  }
  return pf.allPaths();
}

/* Reads in the patch format of the original teapot/teaset */ 
Surface3D m_read_tea_surface(const char* filename) {
  ifstream fin(filename);
  int count;

  /* read in patch indices */
  fin >> count;
  typedef vector<int> veci;
  typedef vector<veci> vecvi; 
  vecvi patch_index; 
  for(int i = 0; i < count; ++i) {
    patch_index.push_back(veci());
    veci& vi = patch_index.back();
    int idx;
    for(int j = 0; j < 16; ++j) {
      if(j > 0) fin.ignore(256, ',');
      fin >> idx; 
      vi.push_back(idx - 1);
    }
  }

  /* read in points */
  fin >> count;
  vector<ConstPoint3f> patch_verts;
  float x, y, z;
  for(int i = 0; i < count; ++i) {
    fin >> x;
    fin.ignore(256, ',');
    fin >> y;
    fin.ignore(256, ',');
    fin >> z;
    patch_verts.push_back(ConstPoint3f(x, y, z));
  }

  vector<Surface3D> patches;
  for(vecvi::iterator P = patch_index.begin(); P != patch_index.end(); ++P) {
    Man<0, 3> p[4][4]; 
    for(int i = 0; i < 4; ++i) for(int j = 0; j < 4; ++j) {
      p[i][j] = m_(patch_verts[(*P)[i * 4 + j]]);
    }
    patches.push_back(m_cubic_bezier_patch(mx_<2>(0), mx_<2>(1), p)); 
  }

  /* doesn't maintain continuity across boundaries, but oh well */
  return m_uniform_concat(patches, 0); 
}
