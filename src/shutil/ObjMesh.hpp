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
#ifndef SHUTIL_SHOBJMESH_HPP
#define SHUTIL_SHOBJMESH_HPP

#include <iosfwd>

#include "sh/sh.hpp"
#include "Mesh.hpp"

namespace ShUtil {

using namespace SH;

struct ObjVertex; 
struct ObjFace;
struct ObjEdge;

typedef MeshType<ObjVertex, ObjFace, ObjEdge> ObjMeshType;

struct ObjVertex: public MeshVertex<ObjMeshType> {
  Point3f pos; // vertex position

  /** \brief Constructs a vertex with the given position */
  ObjVertex(const Point3f &p);
};

struct ObjFace: public MeshFace<ObjMeshType> {
  Normal3f normal; // face normal
};

struct ObjEdge: public MeshEdge<ObjMeshType> {
  // properties for start vertex in this edge's face
  Normal3f normal;  
  Vector3f tangent;
  TexCoord2f texcoord;
};

/* OBJ file mesh where each vertex stores its position,
 * Each edge stores the normal/tc/tangent for its start vertex,
 * and each face stores its face normal
 *
 * Each face in the object mesh is a triangle. (Triangulation
 * happens on loading from the OBJ file)
 */
class ObjMesh: public Mesh<ObjMeshType> {
  public:
    typedef Mesh<ObjMeshType> ParentType;

    /** \brief Constructs an empty mesh */
    ObjMesh();

    /** \brief Constructs ObjMesh from an input stream of an OBJ file */
    ObjMesh(std::istream &in);

    /** \brief Deletes current mesh and reads in a new mesh from an OBJ file */
    std::istream& readObj(std::istream &in);

    /** \brief Generates face normals by cross product  */
    void generateFaceNormals();

    /** \brief Generates normals by averaging adjacent face normals
     * for vertices that have zero normals (or all vertices if force = true) 
     *
     * returns the number of fixed vertex normals */ 
    int generateVertexNormals(bool force = false);

    /** \brief Generates tangents by cross product with (0,1,0)
     * for vertices that have zero tangents (or all vertices if force = true)
     *
     * returns the number of tangents generated
     */
    int generateTangents(bool force = false);

    /** \brief Generates texcoords in a "spherical" shrink map centered at
     * the average of all vertex positions only when all vertex texcoords are 0. 
     * (or if force = true)
     *
     * returns the number of texture coordinates generated */ 
    int generateSphericalTexCoords(bool force = false);

    /** \brief Normalizes all the normals held in this mesh */
    void normalizeNormals();

    /** \brief Consolidates vertices whose coordinates are within 1e-5 of each 
     * other componentwise
     */
    void consolidateVertices();

    /** \brief Sets mesh data to data from an OBJ file */
    friend std::istream& operator>>(std::istream &in, ObjMesh &mesh);
};

}

#endif
