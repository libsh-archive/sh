#ifndef SHFIXEDMANIPULATOR_HPP
#define SHFIXEDMANIPULATOR_HPP

#include "ShProgram.hpp"
#include <vector>

namespace SH {

/** \brief A ShFixedManipulator is a ShProgram output manipulator. 
 * Fixed size manipulators can be combined with each other
 * to give "wider" fixed size manipulators that handle more 
 * ShProgram outputs.
 */
class ShFixedManipulator { 
  public:
    ShFixedManipulator();
    ShFixedManipulator(int );
    
    ShFixedManipulator operator&(const ShFixedManipulator &b) const;
    ShFixedManipulator operator&(const ShProgram &p ) const;

    friend ShFixedManipulator operator&( const ShProgram &p, const ShFixedManipulator &m);
    friend ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m); 
    friend ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p);

  protected:
    std::list<int> m_channels; // stores -k for lose k, +k for keep k, 0 for ShProgram
    std::list<ShProgram> m_programs; 
};

/// keep(n) is a fixed size manipulator that passes through n shader outputs (n > 0)
extern ShFixedManipulator shKeep(int n = 1);

/// lose(n) is a fixed size manipulator that discards n shader outputs (n > 0) 
/// inputs (n > 0) 
extern ShFixedManipulator shLose(int n = 1);

/// combines a ShProgram with this manipulator
extern ShFixedManipulator operator&( const ShProgram &p, const ShFixedManipulator &m);

extern ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m); 
extern ShProgram operator>>(const ShFixedManipulator &m, const ShProgram &p); /// Equiv. to p << m 
extern ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p);
extern ShProgram operator>>(const ShProgram &p, const ShFixedManipulator &m); /// Equiv. to m << p 

}

#endif
