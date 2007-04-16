#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <stack>

#include <sh/sh.hpp>

/* Reverse Polish notation parser executes Sh statements using the given 
 * variables in the symbol table when it encounters strings
 * and generating constants when it encounters floats.
 * (If execute is called within a Program definition, then 
 * the ops are collected into the program)
 *
 * Currently supported operators:
 * assignment: =, b a = denotes b assigned to a, push a on stack
 * arithmetic: *, +, /, -
 *
 * @todo handle vars can be tuples, but we need to handle
 * swizzed variables
 */
class RPN {
  public:
    RPN();

    void addDecl(std::string name, SH::Variable var); 
    void execute(std::string code);

  private:
    typedef std::map<std::string, SH::Variable> SymbolTable; 
    SymbolTable symtab;

    typedef std::stack<SH::Variable> VarStack;
    VarStack stk;

    SH::Variable pop();
    void push(SH::Variable v);
    void clear();

    // binary operation that pushes result as a temp onto stack
    void tempBinaryOp(SH::Operation op, int destSize = 0);
    void tempUnaryOp(SH::Operation op);
};

#endif
