// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Bryan Chan 
//          
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
#ifndef SHGA_BASE_HPP 
#define SHGA_BASE_HPP 

#include <string>
#include <vector>
#include <pair>
#include "ShgaConstants.hpp"

namespace Shga {

/** \file ShgaBase.hpp
 * This class determines the sequences of operations on coefficients required
 * to compute different versor products 
 */
template<typename T, int P, int N>
class ShgaBase {
  protected:
    static const int DIMENSION = P + N; 
    static const int VECTOR_SIZE = T::typesize;
    static const int NUM_GRADES= DIMENSION + 1;
    static const int MAX_GRADE = ShgaGradeMap[DIMENSION] << 1; 
    static const int MAX_BASIS = ShgaBasisMap[DIMENSION] << 1;

    struct SignedBasisElement {
      int element;
      bool neg;
      SignedBasisElement( int element, bool neg );
    }

    //< geometric product matrix 
    //(each matrix entry is a sum of signed basis elements
    // each element in the signed BasisElementVec is an integer with 
    // bits set for each basis used, and sign bit set if it is negative
    typedef std::vector<SignedBasisElement> SignedBasisElementVec;

    struct Position {
      int vec;
      int offset;
      Position(); // default position is (-1, -1)
      Position( int vec, int offset );
    }

    struct ScalarBinaryOp {
      Position dest;
      Position src1, src2;
      bool neg1, neg2;
      ScalarBinaryOp(); // uninitialized 
      ScalarBinaryOp( Position dest, Position src1, bool neg1, 
                       Position src2, bool neg2 ); 
    };

    struct ScalarUnaryOp {
      bool neg;
      Position src;
      Position dest;
      ScalarUnaryOp(); // uninitialized
      ScalarUnaryOp( Position dest, Position src, bool neg );
    };

    // a single sub-operation used for one of the products 
    struct VectorBinaryOp {
      int size;
      bool neg1, neg2;
      int vec1, vec2, destVec; 
      int swiz1[VECTOR_SIZE], swiz2[VECTOR_SIZE], destSwiz[VECTOR_SIZE]; 

      VectorBinaryOp(); // uninitialized
      // creates a binary op from scalar ops - neg flags and src vecs must be the 
      // same for all the scalar ops
      VectorBinaryOp( std::vector<ScalarBinaryOp> ops ); 
    };

    struct VectorUnaryOp {
      int size;
      bool neg;
      int srcVec, destVec;
      int srcSwiz[VECTOR_SIZE], destSwiz[VECTOR_SIZE];

      VectorUnaryOp(); // uninitialized
      VectorUnaryOp( std::vector<ScalarUnaryOp> ops ); 
    };

    template<typename OpType> 
    struct OpSequence {
      typedef std::vector< OpType > OpVec; 

      bool init; // whether this op has been initialized
      int resultGrade; // bit representation of resulting grades
      OpVec ops; 
      OpSequence();
    }
    typedef OpSequence<VectorBinaryOp> BinaryOpSequence;
    typedef OpSequence<VectorUnaryOp> UnaryOpSequence;


    //< sequences of operations for different products

    //TODO make initialization in the constructor thread safe
    ShgaBase();
    static bool initDone;

    static int numBases[NUM_GRADES];  //< Number of basis elements in each grade
    static int grade[MAX_BASIS]; //< grade of each basis element 
    static string basisName[MAX_BASIS]; //< string names for each basis

    //< coefficients are stored in a linear array of vectors, each holding
    // VECTOR_SIZE coefficients.
    // elementPos gives a pair with the vector and offset (component) within that vector
    // for a given basis for a given grade usage.
    static Position elementPosition[MAX_GRADE][MAX_BASIS]; 

    //< basis element in a given position
    static std::vector<int> elements[MAX_GRADE]; 
    static int numElements[MAX_GRADE];
    static int numVecs[MAX_GRADE];

    //< geometric product matrix for an element in the GA
    static SignedBasisElementVec gpMatrix[MAX_BASIS][MAX_BASIS];
    static SignedBasisElementVec lcpMatrix[MAX_BASIS][MAX_BASIS];
    static SignedBasisElementVec opMatrix[MAX_BASIS][MAX_BASIS];

    // (each matrix entry is 
    static BinaryOpSequence gpOps[MAX_GRADE][MAX_GRADE];
    static BinaryOpSequence lcpOps[MAX_GRADE][MAX_GRADE];
    static BinaryOpSequence opOps[MAX_GRADE][MAX_GRADE];

    // extractGrade[dest][src] contains ops to extract dest grade from src 
    static UnaryOpSequence extractGrade[MAX_GRADE][MAX_GRADE]; 
    static UnaryOpSequence gradeInvolutionOps[MAX_GRADE]; 
    static UnaryOpSequence reverseOps[MAX_GRADE]; 


  private:
    // generate a product from the given product matrix
    // into the given OperationVector for the given input grades
    virtual generateProductOp( SignedBasisElementVec productMatrix[ MAX_BASIS ][ MAX_BASIS ],
        BinaryOpSequence &opseq,
        &int src1Grades, int src2Grades );
};

}

#include "ShgaBaseImpl.hpp"

#endif
