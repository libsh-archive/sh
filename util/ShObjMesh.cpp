// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include "ShObjMesh.hpp"

// ShObjMesh converted from old ShObjFile

namespace ShUtil {

static const float EPS = 1e-6;

struct ObjVertLess {
 bool operator()( const ShObjMesh::VertexType *a, const ShObjMesh::VertexType *b ) const {
   float aval[3], bval[3];
   a->data.getValues(aval); b->data.getValues(bval);

   if( aval[0] < bval[0] - EPS ) return true;
   else if( aval[0] < bval[0] + EPS ) {
     if( aval[1] < bval[1] - EPS ) return true;
     else if( aval[1] < bval[1] + EPS ) {
       if( aval[2] < bval[2] - EPS ) return true;
     }
   }
   return false;
 }
};

struct Triple {
    int idx[3];

    // Reads OBJ indices for a face vertex from in.
    // Keeps '\n' if it hits end of line.
    // If nothing is read, then idx[0] = 0 (OBJ indices start from 1)
    Triple(std::istream &in) {
      idx[0] = idx[1] = idx[2] = 0;
      while(in.peek() != '\n' && isspace(in.peek())) in.ignore();

      // each vertex spec must not have spaces (just copying what the old ShObjFile did...)
      in >> std::noskipws;  
      for(int i = 0; i < 3;) {
        in >> idx[i];
        if( !in ) {
          in.clear();
          if(in.peek() == '/') {
            ++i;
            in.ignore(); 
          }else {
            return;
          }
        } 
      }
      in >> std::skipws;
    }

    int operator[](int i) { return idx[i]; }
};

typedef std::vector<Triple> ShObjIndexedFace;

ShObjMesh::ShObjMesh() {
}

ShObjMesh::ShObjMesh(std::istream &in) {
  readObj(in);
}

std::istream& ShObjMesh::readObj(std::istream &in) {
  char ch = 0;
  typedef std::vector<VertexType*> VertexVec;
  typedef std::vector<ShTexCoord2f> TexCoordVec;
  typedef std::vector<ShNormal3f> NormalVec;
  typedef std::vector<ShVector3f> TangentVec;
  typedef std::vector<ShObjIndexedFace> FaceVec;

  VertexVec vertexVec;
  TexCoordVec tcVec;
  NormalVec normVec;
  TangentVec tangentVec;
  FaceVec faceVec;

  // remove old mesh
  clear();

  // read in verts,tangents,normals, etc. first 
  while (in) {
    in >> std::ws >> ch;
    if (!in) break; // TODO: Check for error conditions.
    switch (ch) {
      case 'v': {
        ch = in.get();
        switch (ch) {
          case ' ': { // vertex
            float x, y, z;
            in >> x >> y >> z;
            vertexVec.push_back(new VertexType(ShPoint3f(x, y, z)));
            break;
          }
          case 't': { // Texture coordinate
            float u, v;
            in >> u >> v;
            tcVec.push_back(ShTexCoord2f(u, v));
            break;
          }
          case 'n': {
            // Normal
            float x, y, z;
            in >> x >> y >> z;
            normVec.push_back(ShNormal3f(x, y, z));
            break;
          }
        }
        break;
      }
      case 'r': { // Tangent
        float x, y, z;
        in >> x >> y >> z;
        tangentVec.push_back(ShVector3f(x, y, z));
        break;
      }
      case 'f': { // Face
        ShObjIndexedFace f;
        for(;;) {
          Triple triple(in);
          if( triple[0] == 0 ) break; 
          f.push_back(triple);
        }
        faceVec.push_back(f);
        break;
      }
      case '#': // Comment
      case '$': // Apparently this is also used for comments sometimes
      case 'l': // Line, ignore
      case 'p': // Point, ignore
      case 'm': // material library, ignore
      case 'g': // group, ignore
      case 's': // smoothing group, ignore
      case 'u': // material, ignore
      default: // anything else we ignore as well
        break;
    }
    while (in && in.get() != '\n') ; // Ignore rest of line
  }

  // convert faceVec to mesh
  for(int i = 0; i < faceVec.size(); ++i) {
    VertexList vl;
    for(ShObjIndexedFace::iterator I = faceVec[i].begin(); I != faceVec[i].end(); ++I) {
      int vi = (*I)[0] - 1;
      if( vi == -1 || vi >= vertexVec.size() ) {
        std::ostringstream os;
        os << "Invalid vertex index " << vi << " in OBJ file.";
        ShError( ShException(os.str()));
      }
      vl.push_back(vertexVec[vi]);
    }
    FaceType *face = addFace(vl);

    // go through face again and set up edge properties
    EdgeType *edge = face->edge;
    for(ShObjIndexedFace::iterator I = faceVec[i].begin(); I != faceVec[i].end(); ++I, edge = edge->next) {
      int tci = (*I)[1] - 1;
      int ni = (*I)[2] - 1; 

      if( tci == -1 ) {
        // TODO make a texture coordinate
      } else {
        if( tci >= tcVec.size() ) {
          std::ostringstream os;
          os << "Invalid texcoord index " << tci << " in OBJ file.";
          ShError( ShException(os.str()));
        }
        edge->data.tc = tcVec[tci]; 
      }

      if( ni == -1 ) {
        // TODO make a normal  
      } else {
        if( ni >= normVec.size() ) {
          std::ostringstream os;
          os << "Invalid normal index " << ni << " in OBJ file.";
          ShError( ShException(os.str()));
        }
        edge->data.n = normVec[ni];

        if( ni >= tangentVec.size() ) {
          // TODO make a tangent
        } else {
          edge->data.tgt = tangentVec[ni];
        }
      }
    }
    ShVector3f v01 = face->edge->end->data - face->edge->start->data;
    ShVector3f v02 = face->edge->next->end->data - face->edge->start->data; 
    face->data = cross(v01, v02);  
  }

  mergeVertices<ObjVertLess>();
  mergeEdges<std::less<VertexType*> >();
  if(generateVertexNormals()) SH_DEBUG_PRINT("Warning: Some vertex normals were missing in OBJ file.");
  if(generateSphericalTexCoords()) SH_DEBUG_PRINT("Warning: Some texcoords were missing in OBJ file.");
  normalizeNormals();
  // TODO flip faces that have vert normals not matching the face normal
  // TODO triangulate

  return in;
}

bool ShObjMesh::generateVertexNormals(bool force) {
  typedef std::map<VertexType*, ShPoint3f> NormalSumMap;
  typedef std::map<VertexType*, int> NormalSumCount;
  NormalSumMap nsm;
  NormalSumCount nscount;
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    EdgeType &e = **I;
    if( force || dot(e.data.n, e.data.n).getValue(0) < EPS ) {
      nsm[e.start] = ShConstant3f(0.0f, 0.0f, 0.0f);
      nscount[e.start] = 0;
    }
  }
  if( nsm.empty() ) return false;

  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    VertexType *v = (*I)->start;
    if( nsm.count(v) > 0 ) {
      nsm[v] += (*I)->face->data; 
      nscount[v]++;
    }
  }

  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    VertexType *v = (*I)->start;
    if( nsm.count(v) > 0 ) {
      (*I)->data.n = nsm[v] / (float)nscount[v];
    }
  }
  return true;
}

bool ShObjMesh::generateSphericalTexCoords(bool force) {
  ShPoint3f center;
  bool changed = false;
  for(VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    center += (*I)->data;
  }
  center *= 1.0f / verts.size();

  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    EdgeType &e = **I;
    if( force || dot(e.data.tc, e.data.tc).getValue(0) < EPS ) {
      ShVector3f cv = normalize(e.start->data - center);
      e.data.tc(0) = atan2(cv.getValue(2), cv.getValue(0));
      e.data.tc(1) = acos(cv.getValue(1));
      changed = true;
    }
  }
  return changed; 
}

void ShObjMesh::normalizeNormals() {
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    (*I)->data.n = normalize((*I)->data.n);
  }

  for(FaceSet::iterator J = faces.begin(); J != faces.end(); ++J) {
    (*J)->data = normalize((*J)->data);
  }
}


std::istream& operator>>(std::istream &in, ShObjMesh &mesh) {
  return mesh.readObj(in); 
}

}