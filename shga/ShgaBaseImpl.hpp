// Sh: A GPU metaprogramming language.
//
// Copyright ( c ) 2003 University of Waterloo Computer Graphics Laboratory
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
#ifndef BASEIMPL_HPP 
#define BASEIMPL_HPP 

#include <cstring>
#include "ShgaBase.hpp"

namespace Shga {

template<typename T, int P, int N>
ShgaBase<T, P, N>::SignedBasisElement::SignedBasisElement( int element, bool neg )
  : element( element ), neg( neg ) {}

template<typename T, int P, int N>
ShgaBase<T, P, N>::Position::Position() 
  : vec( -1 ), offset( -1 ) {} 

template<typename T, int P, int N>
ShgaBase<T, P, N>::Position::Position( int vec, int offset ) 
  : vec( vec ), offset( offset ) {} 

template<typename T, int P, int N>
ShgaBase<T, P, N>::ScalarBinaryOp::ScalarBinaryOp( Position dest, 
    Position src1, bool neg1, Position src2, bool neg2 )
  : dest( dest ), src1( src1 ), neg1( neg1 ), src2( src2 ), neg2( neg2 ) {
}

template<typename T, int P, int N>
ShgaBase<T, P, N>::ScalarUnaryOp::ScalarUnaryOp( Position dest, 
    Position src, bool neg ) {
  : dest( dest ), src( src ), neg( neg ) {
}


template<typename T, int P, int N>
ShgaBase<T, P, N>::VectorBinaryOp::VectorBinaryOp( 
    std::vector<ScalarBinaryOp> ops ) 
  : size( vbop.size() ), neg1( ops[0].neg1 ), neg2( ops[0].neg2 ),
  vec1( ops[0].src1.vec ), vec2( ops[0].src2.vec ), destVec( ops[0].dest.vec ) {
  for( int i = 0; i < size; ++i ) {
    // TODO replace with exceptions
    SH_DEBUG_ASSERT( ops[i].neg1 == neg1 && ops[i].neg2 == neg2 );
    SH_DEBUG_ASSERT( ops[i].src1.vec == vec1 && ops[i].src2.vec == vec2 && 
        ops[i].dest.vec == destVec );
    swiz1[i] = ops[i].src1.offset;
    swiz2[i] = ops[i].src2.offset;
    destSwiz[i] = ops[i].dest.offset;
  }
}

template<typename T, int P, int N>
ShgaBase<T, P, N>::VectorUnaryOp::VectorUnaryOp( 
    std::vector<ScalarUnaryOp> ops ) 
  : size( ops.size() ), neg( ops[0].neg),
    srcVec( ops[0].src.vec ), destVec( ops[0].dest.vec ) {
  for( int i = 0; i < size; ++i ) {
    // TODO replace with exceptions
    SH_DEBUG_ASSERT( ops[i].neg == neg ); 
    SH_DEBUG_ASSERT( ops[i].src.vec == srcVec && ops[i].dest.vec == destVec );
    srcSwiz[i] = ops[i].src.offset;
    destSwiz[i] = ops[i].dest.offset;
  }
}

template<typename T, int P, int N>
template<typename OpType>
ShgaBase<T, P, N>::BinaryOpSequence<OpType>::BinaryOpSequence()
  : init( false ), resultGrade( 0 ) {}


template<typename T, int P, int N>
ShgaBase<T, P, N>::ShgaBase( ) {
  if( initDone ) return;

  int i, j, k;

  // initialize number of elements of each grade
  for( i = 0; i < NUM_GRADES; ++i ) {
    numBases[ i ] = 1; 
    for( j = i - 1; j > 0; --j ) {
      numBases[ j ] = numBases[ j ] + numBases[ j-1 ];
    }
  }

  // initialize grade of each basis
  memset( grade, 0, MAX_BASIS * sizeof( int ) ); 
  for( i = 0; i < MAX_BASIS; ++i ) for( j = 0; j < DIMENSION; ++j ) {
    if( i & ShgaBasisElementMap[ j ] ) grade[ i ]++;
  }

  // initialize basis names
  basisName[ 0 ] = ShgaScalarBasisName;
  for( i = 1; i < MAX_BASIS; ++i ) for( j = 0; j <= DIMENSION; ++j ) {
    if( i & ShgaBasisElementMap[ j ] ) {
      basisName[ i ] += basisName[ i ].empty( ) ? "" : "^";
      basisName[ i ] += ShgaBasisElementName[ j ];
    }
  }

  // initialize position map & gradeElement vectors
  int pos;
  // TODO check if this is necessary.  Can't remember if default initization is 0.
  for( i = 0; i < MAX_GRADE; ++i ) {
    for( pos = 0, j = 0; j < NUM_GRADES; ++j ) { 
      for( k = 0; k < MAX_BASIS; ++k ) {
        if( grade[ k ] == j && ( ShgaGradeMap[ grade[k]] & i ) ) {
        elementPosition[ i ][ k ] = Position( ceil( pos / VECTOR_SIZE ), pos % VECTOR_SIZE ); 
        pos++;
        gradeElement[ i ].push_back( k );
      }
    }
  }

  for( i = 0; i < MAX_GRADE; ++i ) {
    numElements[i] = elements[i].size();
    numVecs[i] = (int) ceil( numElements[i] / (double) VECTOR_SIZE );
  }

  // initialize operator matrix entries

  // geometric product 
  // Step 1 - find matrix for GP  
  for( i = 0; i < MAX_BASIS; ++i ) for( j = 0; j < MAX_BASIS; ++j ) {
    int ij = i ^ j; // XOR ( see Gaigen Implementation paper )

    int sign = 1;
    // compute sign due to signature
    int removedBases = i & j;
    for( k = 0; k < DIMENSION; ++k ) {
      if( ShgaBasisElementMap[ k ] & removedBases ) {
        if( P <= k && k < P + N ) sign *= -1; 
        else if( P + N <= k ) sign *= 0;
      }
    }

    // compute sign due to reordering
    for( k = 0; k < DIMENSION; ++k ) {
      int basisK = ShgaBasisElementMap[ k ];
      if( basisK & j ) {
        // TODO check if this bit bashing actually works!!!
        int swaps = grade[ i & !( ( basisK << 1 ) - 1 ) ]; 
        if( swaps & 1 ) sign *= -1;
      }
    }

    if( sign != 0 ) {
      // TODO depending on grade conditions, 
      if( grade[ ij ] == grade[ i ] + grade[ j ] ) {
        opMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) );
      }
      if( grade[ ij ] == gradeOfbasis[ i ] - grade[ j ] ) {
        lcpMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) );
      }
      gpMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) ); 
    }
  }

  for( i = 0; i < MAX_BASIS; ++i ) {
    for( j = 0; j < MAX_BASIS; ++j ) {
      if( !gpMatrix[i][j].empty() ) {
        std::cout << setw( 10 ) << basisName[ gpMatrix[i][j].front().element ] << " * " 
          << gpMatrix[i][j].front().sign;
      } else {
        std::cout << setw( 15 ) << "0";
      }
    }
    std::cout << std::endl;
  }

  // Step 2 - merge into vector operations
  // TODO may want to delay this until the particular op is actually used
  for( i = 0; i < MAX_GRADE; ++i ) {
    for( j = 0; j < MAX_GRADE; ++j ) {
      generateProductOp( gpMatrix, gpOps, i, j );
      generateProductOp( lcpMatrix, lcpOps, i, j );
      generateProductOp( opMatrix, opOps, i, j );
    }
  }
  

  initDone = true;
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateProductOp( SignedBasisElementVec productMatrix[ MAX_BASIS ][ MAX_BASIS ],
    BinaryOpSequence &opseq, int src1Grades, int src2Grades ) {
  int resultGrade = 0; // grade usage of the result
  int destGrade; // a single grade in the result to check for 

  int src1Elm, src2Elm, destElm;
  // determine grades from results based on non-zero src elements
  for( destElm = 0; destElm < MAX_BASIS; ++destElm ) {
    destGrade = ShgaGradeMap[ gradeOfbasis[ destElm ] ];
    for( src2Elm = 0; !( resultGrade & destGrade ) && src2Elm < MAX_BASIS; ++src2Elm ) {
      if( ( ShgaGradeMap[ grade[ src2Elm ] ] & src2Grades ) == 0  ) continue;

      for( SignedBasisElementVec::iterator sbev = productMatrix[i][j].begin();
          sbev != productMatrix[i][j].end(); ++sbev ) {
        src1Elm = sbev->element;
        if( ShgaGradeMap[ grade[ src1Elm ] ] & src1Grades ) {
          resultGrade |= destGrade;
          break;
        }
      }
    }
  }
  OperationVector &opVec = productOperationVector[ src1Grades ][ src2Grades ];
  opVec.resultGrade = resultGrade;

  // add operations 
  // first, classify operations by vector
  //
  // single scalar ops can be combined if the src1, src2, and dest vecs 
  // are the same, and there each dest component is written to at most once.
  // (TODO add different restrictions for other architectures,
  // optimize for minimal swizzling on current generation Radeons)
  int destVec, src1Vec, src2Vec, destOffset, src1Offset, src2Offset; 
  int destBasisElement, src2BasisElement, src2BasisElement;
  for( destVec = 0; destVec < numVecs[ resultGrade ]; ++destVec ) {
    for( src2Vec = 0; src2Vec < numVecs[ src2Grades ]; ++src2Vec ) {

      // list of src1 & src2 pos 
      // classified by sr
      vector< ScalarBinaryOp > scalarOps[ numVecs[ src1Grades ] ][2][VECTOR_SIZE]; 

      // classify operations from productMatrix by vector
      for( destOffset = 0; destOffset < VECTOR_SIZE; ++destOffset ) {
        destBasisElement = elements[resultGrade][destVec * VECTOR_SIZE + destOffset ];

        for( src2Offset = 0; src2Offset < VECTOR_SIZE; ++src2Offset ) {
          src2BasisElement = elements[resultGrade][src2Vec * VECTOR_SIZE + src2Offset ];

          SignedBasisElementVec &productMatrixElms = productMatrix[destBasisElement][src2BasisElement];

          for( SignedBasisElementVec::iterator sbev = productMatrixElms.begin();
              sbev != productMatrixElms.end(); ++sbev ) {
            Position &src1Pos = elementPosition[ src1Grades ][ sbev->element ];
            scalarOps[ src1Pos.vec ][ sbev->neg ? 1 : 0 ][ destOffset ].push_back( 
                ScalarBinaryOp( Position( destVec, destOffset ), src1Pos, sbev->neg,
                  Position( src2Vec, src2Offset ), false ) );
                
          }
        }
      }


      // generate swizzled vector ops by popping scalar ops off 
      int src1Swiz[VECTOR_SIZE]; 
      int src2Swiz[VECTOR_SIZE]; 
      int destSwiz[VECTOR_SIZE]; 
      for( src1Vec = 0; src1Vec < numVecs[ src1Grades ]; ++src1Vec ) {
        for( int isNeg = 0; isNeg <= 1; ++isNeg ) {
          int size = 0;
          for( destOffset = 0; destOffset < VECTOR_SIZE; ++destOffset ) {
            vector< pair< int, int > >& sopVec = scalarOps[ src1Vec ][ isNeg ][ destOffset ];
            if( !sopVec.empty() ) {
              pair<int, int> scalarOp = sopVec.back(); 
              sopVec.pop_back();
              src1Swiz[ size ] = scalarOp.first;
              src2Swiz[ size ] = scalarOp.second;
              destSwiz[ size ] = destOffset;
              size++;
            }
          }
          productOperationVector[ src1Grades ][ src2Grades ].push_back(
              Operation( size, isNeg, src1Vec, src2Vec, destVec,
                src1Swiz, src2Swiz, destSwiz ) );
          if( size == 0 ) break; // no more scalar ops here!
        }
      }
    } 
  }
  
}



} // namespace Shga

#endif
