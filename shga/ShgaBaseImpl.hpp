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
#include <iostream>
#include <sstream>
#include "ShDebug.hpp"
#include "ShgaBase.hpp"

namespace Shga {

template<typename T, int P, int N>
bool ShgaBase<T, P, N>::initDone = false;

template<typename T, int P, int N>
int ShgaBase<T, P, N>::numBases[NUM_GRADES];  

template<typename T, int P, int N>
int ShgaBase<T, P, N>::basisGrade[MAX_BASIS]; 

template<typename T, int P, int N>
int ShgaBase<T, P, N>::basisGradeBits[MAX_BASIS]; 

template<typename T, int P, int N>
std::string* ShgaBase<T, P, N>::basisName = 0; 

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::Position** ShgaBase<T, P, N>::elementPosition = 0;

//< basis element in a given position
template<typename T, int P, int N>
std::vector<int>* ShgaBase<T, P, N>::elements = 0; 

template<typename T, int P, int N>
int ShgaBase<T, P, N>::numElements[MAX_GRADE];

template<typename T, int P, int N>
int ShgaBase<T, P, N>::numVecs[MAX_GRADE];

// TODO fix these pointer issues - 
//< geometric product matrix for an element in the GA
template<typename T, int P, int N>
typename ShgaBase<T, P, N>::SignedBasisElementVec** ShgaBase<T, P, N>::gpMatrix = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::SignedBasisElementVec** ShgaBase<T, P, N>::lcpMatrix = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::SignedBasisElementVec** ShgaBase<T, P, N>::opMatrix = 0;

// (each matrix entry is 
template<typename T, int P, int N>
typename ShgaBase<T, P, N>::BinaryOpSequence** ShgaBase<T, P, N>::gpOps = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::BinaryOpSequence** ShgaBase<T, P, N>::lcpOps = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::BinaryOpSequence** ShgaBase<T, P, N>::opOps = 0;

// extractGrade[dest][src] contains ops to extract dest grade from src 
template<typename T, int P, int N>
typename ShgaBase<T, P, N>::UnaryOpSequence** ShgaBase<T, P, N>::extractGradeOps = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::UnaryOpSequence** ShgaBase<T, P, N>::addOps[2];

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::UnaryOpSequence* ShgaBase<T, P, N>::gradeInvolutionOps = 0;

template<typename T, int P, int N>
typename ShgaBase<T, P, N>::UnaryOpSequence* ShgaBase<T, P, N>::reverseOps = 0;

template<typename T, int P, int N>
ShgaBase<T, P, N>::SignedBasisElement::SignedBasisElement( int element, bool neg )
  : element( element ), neg( neg ) {}

template<typename T, int P, int N>
ShgaBase<T, P, N>::Position::Position() 
  : vec( 0 ), offset( 0 ) {} 

template<typename T, int P, int N>
ShgaBase<T, P, N>::Position::Position( unsigned char vec, unsigned char offset ) 
  : vec( vec ), offset( offset ) {} 

template<typename T, int P, int N>
ShgaBase<T, P, N>::ScalarBinaryOp::ScalarBinaryOp( Position dest, 
    Position src1, bool neg1, Position src2, bool neg2 )
  : dest( dest ), src1( src1 ), neg1( neg1 ), src2( src2 ), neg2( neg2 ) {
}

/* Operations are compatible only if 
 *    (I) src1.vec, neg, src2.vec, neg, and dest.vec are equal.  
 * The second condition is that
 *    (II) only one offset of a dest.vec can be written to at a time
 * The comparison operator sorts so that each set of scalar ops
 * that pairwise satisfy condition (I) are in a block in the sorted sequence.
 *
 * Also, within the block, the ops are sorted according to condition (II)
 * to make vectorization easier.
 */ 
template<typename T, int P, int N>
bool ShgaBase<T, P, N>::ScalarBinaryOp::operator<( 
    const ShgaBase<T, P, N>::ScalarBinaryOp &b ) const {
  return src1.vec < b.src1.vec ||
         ( src1.vec == b.src1.vec && 
         ( ( !neg1 && b.neg1 ) ||
         ( neg1 == b.neg1 &&
         ( src2.vec < b.src2.vec ||
         ( src2.vec == b.src2.vec &&
         ( ( !neg2 && b.neg2 ) ||
         ( neg2 == b.neg2 && 
         ( dest.vec == b.dest.vec &&
         ( dest.offset < b.dest.offset )))))))));
}
template<typename T, int P, int N>
bool ShgaBase<T, P, N>::ScalarBinaryOp::matches( const ScalarBinaryOp &b ) const {
  return( src1.vec == b.src1.vec && neg1 == b.neg1 &&
          src2.vec == b.src2.vec && neg2 == b.neg2 &&
          dest.vec == b.dest.vec );
}

template<typename T, int P, int N>
ShgaBase<T, P, N>::ScalarUnaryOp::ScalarUnaryOp( Position dest, 
    Position src, bool neg ) 
  : dest( dest ), src( src ), neg( neg ) {
}

template<typename T, int P, int N>
bool ShgaBase<T, P, N>::ScalarUnaryOp::operator<( 
    const ShgaBase<T, P, N>::ScalarUnaryOp &b ) const {
  return src.vec < b.src.vec ||
         ( src.vec == b.src.vec && 
         ( ( !neg && b.neg ) ||
         ( neg == b.neg &&
         ( dest.vec == b.dest.vec &&
         ( dest.offset < b.dest.offset )))));
}
template<typename T, int P, int N>
bool ShgaBase<T, P, N>::ScalarUnaryOp::matches( const ScalarUnaryOp &b ) const {
  return( src.vec == b.src.vec && neg == b.neg &&
          dest.vec == b.dest.vec );
}

template<typename T, int P, int N>
ShgaBase<T, P, N>::VectorBinaryOp::VectorBinaryOp( 
    const std::vector<ScalarBinaryOp> &ops ) 
  : size( ops.size() ), neg1( ops[0].neg1 ), neg2( ops[0].neg2 ),
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
std::string ShgaBase<T, P, N>::VectorBinaryOp::toString() const {
  std::ostringstream out;

  // TODO make this nice
  out << "<VectorBinaryOp size: " << (int)size << " vec1: " 
    << ( neg1 ? "-" : " " ) << (int)vec1 << "(";
  for( int i = 0; i < size; ++i ) out << swiz1[i] << ( i == size - 1 ? ")" : ","); 
  out << " vec2: " << ( neg2 ? "-" : " " ) << (int)vec2 << "(";
  for( int i = 0; i < size; ++i ) out << swiz2[i] << ( i == size - 1 ? ")" : ","); 
  out << " destVec: " << (int)destVec << "(";
  for( int i = 0; i < size; ++i ) out << destSwiz[i] << ( i == size - 1 ? ")" : ","); 
  out << ">";

  return out.str();
}

template<typename T, int P, int N>
ShgaBase<T, P, N>::VectorUnaryOp::VectorUnaryOp( 
    const std::vector<ScalarUnaryOp> &ops ) 
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
std::string ShgaBase<T, P, N>::VectorUnaryOp::toString() const {
  std::ostringstream out;

  // TODO make this nice
  out << "<VectorUnaryOp size: " << (int)size 
    << " src: " << ( neg ? "-" : " " ) << (int)srcVec << "(";
  for( int i = 0; i < size; ++i ) out << srcSwiz[i] << ( i == size - 1 ? ")" : ","); 
  out << " destVec: " << (int)destVec << "(";
  for( int i = 0; i < size; ++i ) out << destSwiz[i] << ( i == size - 1 ? ")" : ","); 
  out << ">";

  return out.str();
}

template<typename T, int P, int N>
template<typename OpType>
ShgaBase<T, P, N>::OpSequence<OpType>::OpSequence()
  : init( false ), resultGrade( 0 ) {}

template<typename T, int P, int N>
template<typename OpType>
std::string ShgaBase<T, P, N>::OpSequence<OpType>::toString() const {
  std::string result;
  for( typename OpVec::iterator it = ops.begin(); it != ops.end();
      ++it ) {
    result += it->toString();
    result += "\n";
  }
  return result;
}


#define ALLOCATE_2D_ARRAY( varname, type, dim1, dim2 ) { \
  varname = new type*[ dim1 ]; \
  for( int i = 0; i < dim1; ++i ) varname[i] = new type[dim2]; \
} \

template<typename T, int P, int N>
ShgaBase<T, P, N>::ShgaBase( ) {
  if( initDone ) return;

  // allocate all the static arrays
  basisName = new std::string[ MAX_BASIS ];
  elements = new std::vector<int>[ MAX_GRADE ];
  ALLOCATE_2D_ARRAY( elementPosition, Position, MAX_GRADE, MAX_BASIS );
  ALLOCATE_2D_ARRAY( gpMatrix, SignedBasisElementVec, MAX_BASIS, MAX_BASIS ); 
  ALLOCATE_2D_ARRAY( lcpMatrix, SignedBasisElementVec, MAX_BASIS, MAX_BASIS ); 
  ALLOCATE_2D_ARRAY( opMatrix, SignedBasisElementVec, MAX_BASIS, MAX_BASIS ); 
  ALLOCATE_2D_ARRAY( gpOps, BinaryOpSequence, MAX_GRADE, MAX_GRADE); 
  ALLOCATE_2D_ARRAY( lcpOps, BinaryOpSequence, MAX_GRADE, MAX_GRADE); 
  ALLOCATE_2D_ARRAY( opOps, BinaryOpSequence, MAX_GRADE, MAX_GRADE); 
  ALLOCATE_2D_ARRAY( extractGradeOps, UnaryOpSequence, MAX_GRADE, MAX_GRADE); 
  ALLOCATE_2D_ARRAY( addOps[0], UnaryOpSequence, MAX_GRADE, MAX_GRADE); 
  ALLOCATE_2D_ARRAY( addOps[1], UnaryOpSequence, MAX_GRADE, MAX_GRADE); 
  gradeInvolutionOps = new UnaryOpSequence[ MAX_GRADE ];
  reverseOps = new UnaryOpSequence[ MAX_GRADE ];

  int i, j, k;

  // initialize number of elements of each grade
  for( i = 0; i < NUM_GRADES; ++i ) {
    numBases[ i ] = 1; 
    for( j = i - 1; j > 0; --j ) {
      numBases[ j ] = numBases[ j ] + numBases[ j-1 ];
    }
  }

  // TODO get rid of printout
  std::cout << "numBases: " ; 
  for( int i = 0; i < NUM_GRADES; ++i ) {
    std::cout << numBases[ i ] << " "; 
  }
  std::cout << std::endl;

  // initialize grade of each basis
  memset( basisGrade, 0, MAX_BASIS * sizeof( int ) ); 
  for( i = 0; i < MAX_BASIS; ++i ) {
    for( j = 0; j < DIMENSION; ++j ) {
      if( i & ShgaBasisElementMap[ j ] ) basisGrade[ i ]++;
    }
    basisGradeBits[i] = ShgaGradeMap[ basisGrade[ i ] ];
  }

  // initialize basis names
  basisName[0] = "1";
  for( i = 1; i < MAX_BASIS; ++i ) for( j = 0; j < DIMENSION; ++j ) {
    if( i & ShgaBasisElementMap[ j ] ) {
      if( !basisName[i].empty() ) basisName[i] += "^";
      basisName[ i ] += ShgaBasisElementName[ j ];
    }
  }

  // TODO get rid of printout
  std::cout << "basisName/Grade: " << std::endl; 
  for( int i = 0; i < MAX_BASIS; ++i ) { 
    std::cout << "Basis: " << basisName[i] <<  " grade: " << basisGrade[ i ] << std::endl; 
  }

  // initialize position map & elements vectors
  int pos;
  // TODO check if this is necessary.  Can't remember if default initization is 0.
  for( i = 0; i < MAX_GRADE; ++i ) {
    for( pos = 0, j = 0; j < NUM_GRADES; ++j ) { 
      for( k = 0; k < MAX_BASIS; ++k ) {
        if( basisGrade[ k ] == j && ( basisGradeBits[k] & i ) ) {
          elementPosition[ i ][ k ] = Position( pos / VECTOR_SIZE , pos % VECTOR_SIZE ); 
          pos++;
          elements[ i ].push_back( k );
        }
      }
    }
  }
  std::cout << "element positions: " << std::endl;
  for( i = 0; i < MAX_GRADE; ++i ) {
    std::cout << "Grade: "; 
    for( j = 0; j < NUM_GRADES; ++j ) if( i & ( 1 << j ) ) std::cout << j << " ";
    std::cout << std::endl;

    for( j = 0; j < MAX_BASIS; ++j ) {
      std::cout << "    " << basisName[ j ] 
        << " vec: " << (int)elementPosition[ i ][ j ].vec
        << " offset: " << (int)elementPosition[ i ][ j ].offset << std::endl;
    }
  }

  std::cout << "\nelements: " << std::endl; 
  for( i = 0; i < MAX_GRADE; ++i ) { 
    std::cout << "Grade: "; 
    for( j = 0; j < NUM_GRADES; ++j ) if( i & ( 1 << j ) ) std::cout << j << " ";
    std::cout << " basis elements: ";
    for( j = 0; j < elements[i].size(); ++j ) {
      std::cout << basisName[ elements[i][j] ] << " ";
    }
    std::cout << std::endl;
  }

  for( i = 0; i < MAX_GRADE; ++i ) {
    numElements[i] = elements[i].size();
    numVecs[i] = (int) ceil( numElements[i] / (double) VECTOR_SIZE );

  }

  // TODO get rid of this
  for( i = 0; i < MAX_GRADE; ++i ) {
    std::cout << "Grade: "; 
    for( j = 0; j < NUM_GRADES; ++j ) if( i & ( 1 << j ) ) std::cout << j << " ";
    std::cout << " num elements: " << numElements[i] << " num vecs: " << numVecs[i] << std::endl;
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
    int tempi = i;
    for( k = 0; k < DIMENSION; ++k ) {
      int basisK = ShgaBasisElementMap[ k ];
      if( tempi & basisK ) tempi ^= basisK;
      if( basisK & j ) {
        // TODO check if this bit bashing actually works!!!
        int swaps = basisGrade[ tempi ];
        if( swaps & 1 ) sign *= -1;
      }
    }

    if( sign != 0 ) {
      // TODO depending on grade conditions, 
      if( basisGrade[ ij ] == basisGrade[ i ] + basisGrade[ j ] ) {
        opMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) );
      }
      if( basisGrade[ ij ] == basisGrade[ i ] - basisGrade[ j ] ) {
        lcpMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) );
      }
      gpMatrix[ ij ][ j ].push_back( SignedBasisElement( i, sign < 0 ) ); 
    }
  }

  std::cout << "Geometric Product Matrix: " << std::endl;
  for( i = 0; i < numElements[ MAX_GRADE - 1 ]; ++i ) {
    for( j = 0; j < numElements[ MAX_GRADE - 1 ]; ++j ) {
      int basisi = elements[ MAX_GRADE - 1 ][ i ];
      int basisj = elements[ MAX_GRADE - 1 ][ j ];
      if( !gpMatrix[basisi][basisj].empty() ) {
        std::cout << ( gpMatrix[basisi][basisj].front().neg ? "-" : " " ) 
          << basisName[ gpMatrix[basisi][basisj].front().element ] ; 
      } else {
        std::cout << "0";
      }
      std::cout << ", ";
    }
    std::cout << std::endl;
  }

  // Step 2 - merge into vector operations
  // TODO may want to delay this until the particular op is actually used
  for( i = 0; i < MAX_GRADE; ++i ) {
    for( j = 0; j < MAX_GRADE; ++j ) {
      generateProductOp( gpMatrix, i, j, gpOps[i][j] );
      generateProductOp( lcpMatrix, i, j, lcpOps[i][j] );
      generateProductOp( opMatrix, i, j, opOps[i][j] );
      generateExtractOp( i, j );
      generateAddOp( i, j );
    }
    generateGradeInvolutionOp( i );
    generateReverseOp( i );
  }

  initDone = true;
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateProductOp( 
    SignedBasisElementVec** productMatrix,
    int src1Grades, int src2Grades, BinaryOpSequence &opSeq ) { 
  if( opSeq.init ) return;
  int resultGrade = 0; // grade usage of the result
  int destGrade; // a single grade in the result to check for 
  std::vector< ScalarBinaryOp > ops;

  int src1Elm, src2Elm, destElm;
  // determine grades from results based on non-zero src elements
  for( destElm = 0; destElm < MAX_BASIS; ++destElm ) {
    destGrade = basisGradeBits[ destElm ];
    for( src2Elm = 0; !( resultGrade & destGrade ) && src2Elm < MAX_BASIS; ++src2Elm ) {
      if( ( basisGradeBits[ src2Elm ] & src2Grades ) == 0  ) continue;

      SignedBasisElementVec &matrixEntry = productMatrix[destElm][src2Elm];
      for( typename SignedBasisElementVec::iterator sbev = matrixEntry.begin();
          sbev != matrixEntry.end(); ++sbev ) {
        src1Elm = sbev->element;
        if( basisGradeBits[ src1Elm ] & src1Grades ) {
          resultGrade |= destGrade;
          break;
        }
      }
    }
  }

  // add scalar operations to a list (will vectorize later) 
  for( destElm = 0; destElm < MAX_BASIS; ++destElm ) {
    if( !( basisGradeBits[ destElm ] & resultGrade ) ) continue;
    for( src2Elm = 0; src2Elm < MAX_BASIS; ++src2Elm ) {
      if( !( basisGradeBits[ src2Elm ] & src2Grades ) ) continue;

      SignedBasisElementVec &matrixEntry = productMatrix[destElm][src2Elm];
      for( typename SignedBasisElementVec::iterator sbev = matrixEntry.begin();
          sbev != matrixEntry.end(); ++sbev ) {
        src1Elm = sbev->element;
        if( basisGradeBits[ src1Elm ] & src1Grades ) {
          ops.push_back( ScalarBinaryOp( elementPosition[ resultGrade ][ destElm ],
                elementPosition[ src1Grades ][ src1Elm ], sbev->neg,
                elementPosition[ src2Grades ][ src2Elm ], false ) );
        }
      }
    }
  }
  makeOpSequence( opSeq, resultGrade, ops );
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateExtractOp( int destGrade, int srcGrade ) {

  int commonGrade = destGrade & srcGrade;
  std::vector< ScalarUnaryOp > scalarOps;
  for( int i = 0; i < MAX_BASIS; ++i ) {
    if( basisGradeBits[i] & commonGrade ) {
      scalarOps.push_back( ScalarUnaryOp( elementPosition[ destGrade ][ i ],
            elementPosition[ srcGrade ][ i ], false ) );
    }
  }
  makeOpSequence( extractGradeOps[destGrade][srcGrade], destGrade, scalarOps );
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateAddOp( int src1Grade, int src2Grade ) {
  int resultGrade = src1Grade | src2Grade;

  std::vector< ScalarUnaryOp > scalarOps1, scalarOps2;
  for( int i = 0; i < MAX_BASIS; ++i ) {
    if( basisGradeBits[i] & src1Grade ) {
      scalarOps1.push_back( ScalarUnaryOp( elementPosition[ resultGrade ][ i ],
            elementPosition[ src1Grade ][ i ], false ) );
    }
    if( basisGradeBits[i] & src2Grade ) {
      scalarOps2.push_back( ScalarUnaryOp( elementPosition[ resultGrade ][ i ],
            elementPosition[ src2Grade ][ i ], false ) );
    }
  }
  makeOpSequence( addOps[0][src1Grade][src2Grade], resultGrade, scalarOps1 );
  makeOpSequence( addOps[1][src1Grade][src2Grade], resultGrade, scalarOps2 );
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateGradeInvolutionOp( int srcGrade ) {
  int destGrade = srcGrade; 

  std::vector< ScalarUnaryOp > scalarOps;
  for( int i = 0; i < MAX_BASIS; ++i ) {
    if( basisGradeBits[i] & srcGrade ) {
      int k = basisGrade[i];
      scalarOps.push_back( ScalarUnaryOp( elementPosition[ destGrade ][ i ],
            elementPosition[ srcGrade ][ i ], k & 1 ) );
    }
  }
  makeOpSequence( gradeInvolutionOps[srcGrade], destGrade, scalarOps );
}

template<typename T, int P, int N>
void ShgaBase<T, P, N>::generateReverseOp( int srcGrade ) {
  int destGrade = srcGrade; 

  std::vector< ScalarUnaryOp > scalarOps;
  for( int i = 0; i < MAX_BASIS; ++i ) {
    if( basisGradeBits[i] & srcGrade ) {
      int k = basisGrade[i];
      int sign = k * ( k - 1 ) / 2;
      scalarOps.push_back( ScalarUnaryOp( elementPosition[ destGrade ][ i ],
            elementPosition[ srcGrade ][ i ], sign & 1 ) );
    }
  }
  makeOpSequence( reverseOps[srcGrade], destGrade, scalarOps );
}

template<typename T, int P, int N>
template<typename ScalarOpType, typename OpSequenceType>
void ShgaBase<T, P, N>::makeOpSequence( OpSequenceType& opSeq, int resultGrade,
    std::vector< ScalarOpType > ops ) {
  opSeq.init = true;
  opSeq.resultGrade = resultGrade;

  typedef std::multiset< ScalarOpType > ScalarOpSet; 
  ScalarOpSet opBins; 
  for( typename std::vector< ScalarOpType >::iterator opIt = ops.begin();
      opIt != ops.end(); ++opIt ) {
    opBins.insert( *opIt );
  }

  for( typename ScalarOpSet::const_iterator it = opBins.begin();
      it != opBins.end();) {
    typename ScalarOpSet::const_iterator blockStart, blockEnd, finger, next; 
    blockStart = blockEnd = it;
    for(++blockEnd; blockEnd != opBins.end() && blockStart->matches( *blockEnd ); ++blockEnd ) {
    }
    it = blockEnd;

    // each subvector of scalar ops can be merged into a vector op (i.e. have the 
    // same src1 vec, neg, src2 vec, neg, dest vec, and have different dest offsets
    std::vector< std::vector< ScalarOpType > > vectorOps; 
    finger = blockStart;
    next = blockStart;
    ++next;
    int insertToVec = 0;
    for(;; finger = next, ++next ) {
      if( insertToVec == vectorOps.size() ) vectorOps.push_back( std::vector< ScalarOpType >() ); 
      vectorOps[ insertToVec ].push_back( *finger );
      ++insertToVec;

      if( next == blockEnd ) break;
      if( *finger < *next ) insertToVec = 0;
    }

    for( int i = 0; i < vectorOps.size(); ++i ) {
      opSeq.ops.push_back( (typename OpSequenceType::VecType)( vectorOps[i] ) );
    }
  }
//  std::cout << "# sops: " << opBins.size() << " # vops: " << opSeq.ops.size() << std::endl;
}




} // namespace Shga

#endif
