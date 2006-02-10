// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include "ShObjMesh.hpp"

// ShObjMesh converted from old ShObjFile

namespace ShUtil {

struct Triple {
    int idx[3];

    // Reads OBJ indices for a face vertex from in.
    // Keeps '\n' if it hits end of line.
    // If nothing is read, then idx[0] = 0 (OBJ indices start from 1)
    Triple(std::istream &in) {
      idx[0] = idx[1] = idx[2] = 0;
      while(in.peek() != '\n' && isspace(in.peek())) in.ignore();

      if (in.peek() == '\n') return;
      
      // each vertex spec must not have spaces (just copying what the old ShObjFile did...)
      in >> std::noskipws;  
      for(int i = 0; i < 3;) {
        in >> idx[i];
        if( !in ) {
          in.clear();
          if(in.peek() == '/') {
            ++i;
            in.ignore();
          } else if (isspace(in.peek())) {
            break;
          } else {
            // ?
            break;
          }
        } 
      }
      in >> std::skipws;
    }

    int operator[](int i) { return idx[i]; }
};

typedef std::vector<Triple> ShObjIndexedFace;

ShObjVertex::ShObjVertex(const ShPoint3f &p)
  : pos(p) {}

ShObjMesh::ShObjMesh() {
}

ShObjMesh::ShObjMesh(std::istream &in) {
  readObj(in);
}

std::istream& ShObjMesh::readObj(std::istream &in) {
  char ch = 0;
  typedef std::vector<Vertex*> VertexVec;
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
    if (!in) {
      break; // TODO: Check for error conditions.
    }
    switch (ch) {
      case 'v': {
        ch = in.get();
        switch (ch) {
          case ' ': { // vertex
            float x, y, z;
            in >> x >> y >> z;
            vertexVec.push_back(new Vertex(ShPoint3f(x, y, z)));
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
  for(std::size_t i = 0; i < faceVec.size(); ++i) {
    VertexList vl;
    for(ShObjIndexedFace::iterator I = faceVec[i].begin(); I != faceVec[i].end(); ++I) {
      int vi = (*I)[0] - 1;
      if( vi == -1 || vi >= (int)vertexVec.size() ) {
        std::ostringstream os;
        os << "Invalid vertex index " << vi << " in OBJ file.";
        shError( ShException(os.str()));
      }
      vl.push_back(vertexVec[vi]);
    }
    Face* face = addFace(vl);

    // go through face again and set up edge properties
    Edge* edge = face->edge;
    for(ShObjIndexedFace::iterator I = faceVec[i].begin(); I != faceVec[i].end(); ++I, edge = edge->next) {
      int tci = (*I)[1] - 1;
      int ni = (*I)[2] - 1; 

      if( tci != -1 ) {
        if( tci >= (int)tcVec.size() ) {
          std::ostringstream os;
          os << "Invalid texcoord index " << tci << " in OBJ file.";
          shError( ShException(os.str()));
        }
        edge->texcoord = tcVec[tci]; 
      } else edge->texcoord *= 0.0f;

      if( ni != -1 ) {
        if( ni >= (int)normVec.size() ) {
          std::ostringstream os;
          os << "Invalid normal index " << ni << " in OBJ file.";
          shError( ShException(os.str()));
        }
        edge->normal = normVec[ni];

        if( ni >= (int)tangentVec.size() ) {
          edge->tangent *= 0.0f;
        } else {
          edge->tangent = tangentVec[ni];
        }
      }
    }
  }

  earTriangulate();
  generateFaceNormals();

  int badNorms = generateVertexNormals();
  if(badNorms > 0) SH_DEBUG_WARN("OBJ file has " << badNorms << " vertices without normals"); 

  int badTangents = generateTangents();
  if(badTangents > 0) SH_DEBUG_WARN("OBJ file has " << badTangents << " vertices without tangents"); 

  int badTexCoords = generateSphericalTexCoords();
  if(badTexCoords > 0) SH_DEBUG_WARN("OBJ file has " << badTexCoords << " vertices without texture coordinates.");

  // TODO flip faces that have vert normals not matching the face normal
  return in;
}

void ShObjMesh::generateFaceNormals() {
  for(FaceSet::iterator I = faces.begin(); I != faces.end(); ++I) {
    Face& face = **I;
    Edge& e01 = *(face.edge);
    Edge& e02 = *(e01.next);
    ShVector3f v01 = e01.end->pos - e01.start->pos;
    ShVector3f v02 = e02.end->pos - e02.start->pos; 
    face.normal = cross(v01, v02);  
  }
}

int ShObjMesh::generateVertexNormals(bool force) {
  typedef std::map<Vertex*, ShPoint3f> NormalSumMap;
  NormalSumMap nsm;
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    Edge &e = **I;
    if( force || dot(e.normal, e.normal).getValue(0) == 0 ) { 
      nsm[e.start] = ShConstAttrib3f(0.0f, 0.0f, 0.0f);
    }
  }
  if( nsm.empty() ) return 0;

  /* For each pair of edges e1->e2 in a face, add the following 
   * scaling of the face vector to that vertex's cumulative normal */ 
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    Vertex *v = (*I)->end;

    if(nsm.count(v) > 0) {
      Edge* e1 = *I;
      Edge* e2 = e1->next;
      ShVector3f ve1 = e1->end->pos - e1->start->pos;
      ShVector3f ve2 = e2->end->pos - e2->start->pos;
      ShAttrib1f scale = length(cross(ve1, ve2)) / ((ve1 | ve1) + (ve2 | ve2));
      nsm[v] += scale * (*I)->face->normal; 
    }
  }

  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    Vertex *v = (*I)->start;
    if(nsm.count(v) > 0) {
      (*I)->normal = normalize((nsm[v])); 
    }
  }
  return nsm.size();
}

int ShObjMesh::generateTangents(bool force) {
  int changed = 0;
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    Edge &e = **I;
    if( force || dot(e.tangent, e.tangent).getValue(0)  == 0) {
      e.tangent = cross(e.normal, ShVector3f(0.0f, 1.0f, 0.0f));
      changed++;
    }
  }
  return changed; 
}

int ShObjMesh::generateSphericalTexCoords(bool force) {
  if( !force ) {
    // If some vertex has texcoords, don't mess with them
    for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
      if( dot((*I)->texcoord, (*I)->texcoord).getValue(0) != 0) {
        return 0;
      }
    }
  }

  // Find center and generate texture coordinates 
  int changed = 0;
  ShPoint3f center;
  for(VertexSet::iterator I = verts.begin(); I != verts.end(); ++I) {
    center += (*I)->pos;
  }
  center *= 1.0f / verts.size();

  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    Edge &e = **I;
    if( force || dot(e.texcoord, e.texcoord).getValue(0) == 0) {
      ShVector3f cv = normalize(e.start->pos - center);
      e.texcoord(0) = atan2(cv.getValue(2), cv.getValue(0));
      e.texcoord(1) = acos(cv.getValue(1));
      changed++; 
    }
  }
  return changed; 
}

void ShObjMesh::normalizeNormals() {
  for(EdgeSet::iterator I = edges.begin(); I != edges.end(); ++I) {
    (*I)->normal = normalize((*I)->normal);
  }

  for(FaceSet::iterator J = faces.begin(); J != faces.end(); ++J) {
    (*J)->normal = normalize((*J)->normal);
  }
}

struct ObjVertLess {
 static const float EPS;

 bool operator()( const ShObjVertex *a, const ShObjVertex *b ) const {
   float aval[3], bval[3];
   a->pos.getValues(aval); b->pos.getValues(bval);

   return (aval[0] < bval[0] - EPS) || ( 
          (aval[0] < bval[0] + EPS) && ( 
          (aval[1] < bval[1] - EPS) || ( 
          (aval[1] < bval[1] + EPS) && 
          (aval[2] < bval[2] - EPS))));
 }
};

const float ObjVertLess::EPS = 1e-5;

void ShObjMesh::consolidateVertices() {
  mergeVertices<ObjVertLess>();
}


std::istream& operator>>(std::istream &in, ShObjMesh &mesh) {
  return mesh.readObj(in); 
}

}
