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
#ifndef SHUTIL_SHOBJMESH_HPP
#define SHUTIL_SHOBJMESH_HPP

#include <iosfwd>

#ifdef WIN32
#include <sh.hpp>
#else
#include <sh/sh.hpp>
#endif /* WIN32 */

#include "ShMesh.hpp"

namespace ShUtil {

using namespace SH;

/** \file ShObjMesh.hpp
 * A OBJ file reading  
 */
struct ShObjMeshEdgeData {
  // data for the start vertex in an edge 
  ShNormal3f n;  // normal
  ShTexCoord2f tc;  // texture coordinate
  ShVector3f tgt; // tangent
};

/* OBJ file mesh where each vertex stores its position,
 * Each edge stores the normal/tc/tangent for its start vertex,
 * and each face stores its face normal
 */
class ShObjMesh: public ShMesh<ShPoint3f, ShObjMeshEdgeData, ShNormal3f> {
  public:
    typedef ShMesh<ShPoint3f, ShObjMeshEdgeData, ShNormal3f> ParentType;

    /** \brief Constructs an empty mesh */
    ShObjMesh();

    /** \brief Constructs ShObjMesh from an input stream of an OBJ file */
    ShObjMesh(std::istream &in);

    /** \brief Deletes current mesh and reads in a new mesh from an OBJ file */
    std::istream& readObj(std::istream &in);

    /** \brief Generates normals by averaging adjacent face normals
     * for vertices that have zero normals (or all vertices if force = true) 
     *
     * returns true iff some vertex had a zero normal that needed fixing */
    bool generateVertexNormals(bool force = false);

    /** \brief Generates texcoords in a "spherical" shrink map centered at
     * the average of all vertex positions for vertices that have 0 texcoords
     * (or all vertices if force = true)
     *
     * returns true iff some vertex had a zero texcoord that needed fixing */
    bool generateSphericalTexCoords(bool force = false);

    /** \brief Normalizes all the normals held in this mesh */
    void normalizeNormals();

    /** \brief Sets mesh data to data from an OBJ file */
    friend std::istream& operator>>(std::istream &in, ShObjMesh &mesh);

};

}

#endif
