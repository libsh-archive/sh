#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <libxml++/libxml++.h>
#include <sh/sh.hpp>
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
  bool pathdone; 
  public:
    PathFinder(): pathdone(false) {}
    virtual ~PathFinder() {}

    Man allPaths() {
      size_t numCP = m_conpoint.size();
      size_t numCurves = (numCP - 1) / 3;
      ArrayRect<Point2f> control_point(numCurves, 4);
      HostMemoryPtr mem = new HostMemory(numCurves * 4 * 2 * sizeof(float), SH_FLOAT);
      float* data = reinterpret_cast<float*>(mem->hostStorage()->data());
      //Palette<Point2f> control_point(numCP);
      for(size_t i = 0; i < numCurves; ++i) {
        for(size_t j = 0; j < 4; ++j) {
          //cout << m_conpoint[i * 3 + j] << endl;
//          data[(i * 4 + j) * 2] = m_conpoint[i * 3 + j].getValue(0); 
//          data[(i * 4 + j) * 2 + 1] = m_conpoint[i * 3 + j].getValue(1); 
          data[(j * numCurves + i) * 2] = m_conpoint[i * 3 + j].getValue(0); 
          data[(j * numCurves + i) * 2 + 1] = m_conpoint[i * 3 + j].getValue(1); 
        }
        // control_point[i] = m_conpoint[i];
      }
      /*
      for(size_t i = 0; i < numCurves * 4 * 2; ++i) {
        cout << i << " " << data[i] << endl;
      }
      */
      //cout << endl;
      control_point.memory(mem);
      Program result = SH_BEGIN_PROGRAM() {
        InputAttrib1f SH_DECL(t); 
        OutputPoint2f SH_DECL(p); 
#if 0
        //p = m_conpoint.front();
        p = lerp(t < 0.5314159, m_conpoint.front(), m_conpoint.back()); 

        Attrib1f SH_DECL(i) = 0;
        Attrib2f SH_DECL(idx);
        t *= numCurves;
        SH_WHILE(i < numCurves) {
          idx(0) = i; 
          SH_IF((t >= i) && (t <= i + 1)) {
            Attrib1f SH_DECL(tt) = t - i;
            Attrib4f SH_DECL(bt) = bernstein<4>(tt);
            for(int j = 0; j < 4; ++j) {
                //p += bt(j) * control_point[i * 3 + j]; 
                idx(1) = j; 
                Point2f SH_DECL(inc) = bt(j) * control_point[idx];
                if(j == 0) p = inc; 
                else p += inc; 
            }
          } SH_ENDIF;
          i += 1;
        } SH_ENDWHILE;
#else
        //p = m_conpoint.front();
        p = m_conpoint.front();

        Attrib1f SH_DECL(i) = 0;
        Attrib2f SH_DECL(idx);
        t *= numCurves;
        SH_WHILE(i < numCurves) {
        //for(int i = 0; i < numCurves; ) {
          idx(0) = i; 
          SH_IF(t < i - 1) {
          } SH_ELSE {
            SH_IF(t > i) {
              Attrib1f SH_DECL(tt) = t - i;
              Attrib4f SH_DECL(bt) = bernstein<4>(tt);
              for(int j = 0; j < 4; ++j) {
                  //p += bt(j) * control_point[i * 3 + j]; 
                  idx(1) = j; 
                  Point2f SH_DECL(inc) = bt(j) * control_point[idx];
                  if(j == 0) p = inc; 
                  else p += inc; 
              }
            } SH_ENDIF;
          } SH_ENDIF;
          i += 1;
        //}
        } SH_ENDWHILE;
#endif
      } SH_END;

      return Man(result)(m_u(0)); 
    }

  protected:
    //overrides:
    virtual void on_start_element(const Glib::ustring& name,
                                  const AttributeList& attributes) {
      if(name == "path" && !pathdone) {
        for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
          if(iter->name == "d") addPath(iter->value);
        }
        pathdone = true;
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
      /* reflect y to match OpenGL */
      return ConstPoint2f(x, -y);
    }

    /* Split up the elements in the string into curves and add them to the PathVec */
    void addPath(const Glib::ustring& s) {
      istringstream pin(s);
      bool done = false;
      ConstPoint2f last, end;
      for(int i = 0;!done; ++i) {
        char command;
        pin >> skipws >> command; 
        if(pin.eof()) break;

        /* Make sure we get *new* constants for each segment 
         * (instead of reusing th esame uniform - which causes big problems) */
        ConstPoint2f lastCopy = last;

        switch(command) {
          case 'M': /* moveto */
            assert(m_conpoint.empty());
            end = last = getPoint(pin); 
            m_conpoint.push_back(last);
            break;

          case 'L': /* lineto */
            end = getPoint(pin);
            m_conpoint.push_back(last);
            //m_conpoint.push_back(lerp(0.25, end, last)); 
            //m_conpoint.push_back(lerp(0.75, end, last)); 
            m_conpoint.push_back(end);
            m_conpoint.push_back(end);
            break;

          case 'H': /* horizontal line */
            end = last;
            end(0) = getScalar(pin);
            m_conpoint.push_back(lerp(0.25, end, last)); 
            m_conpoint.push_back(lerp(0.75, end, last)); 
            m_conpoint.push_back(end);
            break;

          case 'V': /* vertical line */
            end = last;
            end(1) = getScalar(pin);
            m_conpoint.push_back(lerp(0.25, end, last)); 
            m_conpoint.push_back(lerp(0.75, end, last)); 
            m_conpoint.push_back(end);
            break;

          case 'C': /* cubic bezier to */ {
              for(int i = 0; i < 3; ++i) m_conpoint.push_back(getPoint(pin));
              end = m_conpoint.back();
              break;
            }
          case 'Z': /* done*/
            done = true;
            break;
        }
        last = end;
      }
    }

    //typedef vector<Man> PathVec;
    //PathVec m_paths;
    typedef vector<Point2f> ControlPointVec;
    ControlPointVec m_conpoint;
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
  return pf.allPaths().name("m_svg_crv(" + filename + ")");
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

  vector<Man> patches;
  for(vecvi::iterator P = patch_index.begin(); P != patch_index.end(); ++P) {
    Point3D p[4][4]; 
    for(int i = 0; i < 4; ++i) for(int j = 0; j < 4; ++j) {
      p[i][j] = m_(patch_verts[(*P)[i * 4 + j]]);
    }
    patches.push_back(m_cubic_bezier_patch(m_u(0), m_u(1), p)); 
  }

  /* doesn't maintain continuity across boundaries, but oh well */
  return m_uniform_concat(patches, 0).name(string("m_read_tea_surface(")+ filename +")"); 
}
