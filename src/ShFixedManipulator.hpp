#ifndef SHFIXEDMANIPULATOR_HPP
#define SHFIXEDMANIPULATOR_HPP

#include "ShProgram.hpp"
#include <vector>

namespace SH {

typedef ShProgramNode::VarList::const_iterator ShManipVarIterator;
/** \brief A ShFixedManipulator is a ShProgram output manipulator. 
 * Fixed size manipulators can be combined with each other
 * to give "wider" fixed size manipulators that handle more 
 * ShProgram outputs.
 */

class ShFixedManipulatorNode: public ShRefCountable { 
  public:
    ShFixedManipulatorNode();
    virtual ~ShFixedManipulatorNode(); 

    /** Define the manipulator's behaviour on inputs
     * Consumes a number of inputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an ShAlgebraException
     */
    virtual ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const = 0;

    /** Defines the manipulator's behaviour on outputs 
     * Consumes a number of outputs in the variable list, changing finger.
     * This function must ensure that it doesn't pass the end iterator.
     * If it does, it must throw an ShAlgebraException
     */
    virtual ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const = 0;
};
typedef ShRefCount<ShFixedManipulatorNode> ShFixedManipulator;

class ShKeepNode: public ShFixedManipulatorNode {
  public:
    ShKeepNode(int numChannels); 
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numChannels;
};
ShFixedManipulator shKeep(int numChannels = 1);

class ShLoseNode: public ShFixedManipulatorNode {
  public:
    ShLoseNode(int numChannels); 
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numChannels;
};
ShFixedManipulator shLose(int numChannels = 1);

class ShDupNode: public ShFixedManipulatorNode {
  public:
    ShDupNode(int numDups);
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    int m_numDups;
};
ShFixedManipulator shDup(int numDups = 2);

// TODO make class names less clunky
// This node can only be created by using the & operator with another fixed manipulator
//
// This is the only manip node that allows number of channels
// not to match when connected to a ShProgram. (extras are handled in the
// default connect way when inputs != outpus)
//
class ShProgramManipNode: public ShFixedManipulatorNode {
  public:
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;

  private:
    ShProgram p;

    ShProgramManipNode(const ShProgram &p);

    friend ShFixedManipulator operator&(const ShFixedManipulator &m, const ShProgram &p );
    friend ShFixedManipulator operator&( const ShProgram &p, const ShFixedManipulator &m);
};

// This node can only be created using the & operator with another fixed manipulator
class ShTreeManipNode: public ShFixedManipulatorNode {
  public:
    ShProgram applyToInputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
    ShProgram applyToOutputs(ShManipVarIterator &finger, ShManipVarIterator end) const;
  private:
    ShFixedManipulator a, b;

    ShTreeManipNode(const ShFixedManipulator &a, const ShFixedManipulator &b); 

    friend ShFixedManipulator operator&(const ShFixedManipulator &m, const ShFixedManipulator &n);
};

extern ShProgram operator<<(const ShProgram &p, const ShFixedManipulator &m); 
extern ShProgram operator<<(const ShFixedManipulator &m, const ShProgram &p);
extern ShFixedManipulator operator&(const ShFixedManipulator &m, const ShFixedManipulator &n);
extern ShFixedManipulator operator&(const ShFixedManipulator &m, const ShProgram &p );
extern ShFixedManipulator operator&(const ShProgram &p, const ShFixedManipulator &m);

}

#endif
