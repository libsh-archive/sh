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
#include "ShgaConstants.hpp"

namespace Shga {

/** TODO this is a minor mess...
 * Most of the vectorization code should be generalized and whacked into the
 * Sh optimizer.
 */

/** \file ShgaBase.hpp
 * This class determines the sequences of operations on coefficients required
 * to compute different versor products 
 */
template<typename T, int P, int N>
class ShgaBase {
  public:
    enum {
      DIMENSION = P + N,
      VECTOR_SIZE = T::typesize,
      NUM_GRADES= DIMENSION + 1,
      MAX_GRADE = 1 << ( DIMENSION + 1 ),
      MAX_BASIS = 1 << DIMENSION 
    };

  protected:

    struct SignedBasisElement {
      int element;
      bool neg;
      SignedBasisElement( int element, bool neg );
    };

    //< geometric product matrix 
    //(each matrix entry is a sum of signed basis elements
    // each element in the signed BasisElementVec is an integer with 
    // bits set for each basis used, and sign bit set if it is negative
    typedef std::vector<SignedBasisElement> SignedBasisElementVec;

    struct Position {
      unsigned char vec;
      unsigned char offset;
      Position(); // default position is (0, 0);
      Position( unsigned char vec, unsigned char offset );
    };

    struct ScalarBinaryOp {
      Position dest;
      Position src1, src2;
      bool neg1, neg2;
      ScalarBinaryOp(); // uninitialized 
      ScalarBinaryOp( Position dest, Position src1, bool neg1, 
                       Position src2, bool neg2 ); 
      bool operator<( const ScalarBinaryOp &b ) const;
      bool matches( const ScalarBinaryOp &b ) const;
    };

    struct ScalarUnaryOp {
      bool neg;
      Position src;
      Position dest;
      ScalarUnaryOp(); // uninitialized
      ScalarUnaryOp( Position dest, Position src, bool neg );
      bool operator<( const ScalarUnaryOp &b ) const;
      bool matches( const ScalarUnaryOp &b ) const;
    };


    // a single sub-operation used for one of the products 
    struct VectorBinaryOp {
      unsigned char size;
      bool neg1, neg2;
      unsigned char vec1, vec2, destVec; 
      int swiz1[VECTOR_SIZE], swiz2[VECTOR_SIZE], destSwiz[VECTOR_SIZE]; 

      VectorBinaryOp(); // uninitialized
      // creates a binary op from scalar ops - neg flags and src vecs must be the 
      // same for all the scalar ops
      VectorBinaryOp( const std::vector<ScalarBinaryOp> &ops ); 
      std::string toString() const;
    };

    struct VectorUnaryOp {
      unsigned char size;
      bool neg;
      unsigned char srcVec, destVec;
      int srcSwiz[VECTOR_SIZE], destSwiz[VECTOR_SIZE];

      VectorUnaryOp(); // uninitialized
      VectorUnaryOp( const std::vector<ScalarUnaryOp> &ops ); 
      std::string toString() const;
    };

    template<typename OpType> 
    struct OpSequence {
      typedef std::vector< OpType > OpVec; 
      typedef OpType VecType;

      bool init; // whether this op has been initialized
      int resultGrade; // bit representation of resulting grades
      OpVec ops; 
      OpSequence();
      std::string toString() const;
    };
    typedef OpSequence<VectorBinaryOp> BinaryOpSequence;
    typedef OpSequence<VectorUnaryOp> UnaryOpSequence;

    //< sequences of operations for different products

    //TODO make initialization in the constructor thread safe
    ShgaBase();
    static bool initDone;

    static int numBases[NUM_GRADES];  //< Number of basis elements in each grade
    static int basisGrade[MAX_BASIS]; //< grade of each basis element (as anumber) 
    static int basisGradeBits[MAX_BASIS]; //< grade of each basis element (as bit-shifted SHGA_GRADE0) 
    static std::string* basisName; //< string names for each basis

    //< coefficients are stored in a linear array of vectors, each holding
    // VECTOR_SIZE coefficients.
    // elementPos gives a pair with the vector and offset (component) within that vector
    // for a given basis for a given grade usage.
    // [MAX_GRADE][MAX_BASIS]
    static Position **elementPosition;

    //< basis element in a given position [MAX_GRADE];
    static std::vector<int>* elements;
    static int numElements[MAX_GRADE];
    static int numVecs[MAX_GRADE];

    //< geometric product MAX_BASIS * MAX_BASIS matrix for an element in the GA
    static SignedBasisElementVec** gpMatrix;
    static SignedBasisElementVec** lcpMatrix;
    static SignedBasisElementVec** opMatrix;

    // binary ops indexed by two src grades [MAX_GRADE][MAX_GRADE];
    static BinaryOpSequence** gpOps;
    static BinaryOpSequence** lcpOps;
    static BinaryOpSequence** opOps;

    // extractGrade[dest][src] contains ops to extract dest grade from src 
    // ([MAX_GRADE][MAX_GRADE])
    static UnaryOpSequence** extractGradeOps;
    static UnaryOpSequence** addOps[2]; // a sequence of unary ops from each src op 

    // unaryop[src] ([MAX_GRADE])
    static UnaryOpSequence* gradeInvolutionOps;
    static UnaryOpSequence* reverseOps;

  private:
    // generate a product from the given product matrix
    // into the given OperationVector for the given input grades
    static void generateProductOp( SignedBasisElementVec **productMatrix,
        int src1Grades, int src2Grades, BinaryOpSequence& opSeq ); 

    static void generateExtractOp( int destGrade, int srcGrade );
    static void generateAddOp( int src1Grade, int src2Grade ); 
    static void generateGradeInvolutionOp( int srcGrade );
    static void generateReverseOp( int srcGrade );

    template<typename ScalarOpType, typename OpSequenceType>
    static void makeOpSequence( OpSequenceType& opSeq, int resultGrade, std::vector< ScalarOpType > ops );
};

}

#include "ShgaBaseImpl.hpp"

#endif
