#ifndef SHMANIPULATOR_HPP
#define SHMANIPULATOR_HPP

#include "ShProgram.hpp"
#include <vector>

namespace SH {

/** \brief A ShManipulator is a ShProgram output manipulator. 
 * This kind of manipulator permutes the outputs of 
 * a ShProgram based on given integer indices. 
 */
class ShManipulator {
  public:
    /** \brief Creates empty manipulator of given size
     */
    ShManipulator();
    ShManipulator( const ShManipulator &m );
    
  protected:
    typedef std::pair<int, int> IndexRange; 
    typedef std::vector< IndexRange > IndexRangeVector;

    void append( int i );
    void append( int start, int end );
    void append( IndexRange r ); 

    IndexRangeVector m_ranges; 

    friend ShProgram operator<<( const ShProgram &p, const ShManipulator &m );
};

/** \brief Applies a manipulator to a ShProgram
 */
ShProgram operator<<( const ShProgram &p, const ShManipulator &m );


#if 0
/** \brief A ShFixedSizeManipulator is a ShProgram output manipulator. 
 * Fixed size manipulators can be combined with each other
 * to give "wider" fixed size manipulators that handle more 
 * ShProgram outputs.
 */
class ShFixedSizeManipulator: public ShManipulator {
  public:
    ShFixedSizeManipulator( int srcSize );
    ShFixedSizeManipulator operator&( const ShFixedSizeManipulator &b ) const;

  protected:
    int m_srcSize;
};

/// keep(n) is a fixed size manipulator that passes through n shader outputs (n > 0)
class keep: public ShFixedSizeManipulator {
  public:
    keep( int n = 1 );
};

/// lose(n) is a fixed size manipulator that discards n shader outputs (n > 0) 
/// inputs (n > 0) 
class lose: public ShFixedSizeManipulator {
  public:
    lose( int n = 1 );
};

#endif

/// permute( a1, ... ) is a manipulator that permutes 
// shader outputs based on given indices
//
class permute: public ShManipulator {
  public:
    /** \brief creates a permutation manipulator which
     * gives outputSize outputs when applied to a ShProgram
     *
     * if an index >= 0, then uses index'th output
     * if index < 0, then uses program size + index'th output
     */
    permute( int outputSize, ... ); 
    permute( std::vector<int> indices );

    permute operator*( const permute &p ) const;
};

/// range manipulator that permutes ranges of shader
// outputs based on given indices
class range: public ShManipulator {
  public:
    range( int i );
    range( int start, int end );

    range operator()( int i );
    range operator()( int start, int end );
};

/** extract is a manipulator that removes the kth output
 * and appends it before all other outputs. 
 *
 * if k >= 0, then take element k (indices start at 0) 
 * if k < 0, take element outputs.size + k
 */
class extract: public ShManipulator {
  public:
    extract( int k ); 
};

/** drop is a manipulator that discards the k outputs.
 *
 * if k >= 0 then discards outputs 0 to k 
 * if k < 0 then discards outputs size + k to size - 1
 */
class drop: public ShManipulator {
  public:
    drop( int k );
};

}

#endif
