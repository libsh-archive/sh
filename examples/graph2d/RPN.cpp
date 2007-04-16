#include <sstream>
#include <sh/Evaluate.hpp>
#include <sh/Eval.hpp>
#include "RPN.hpp"

using namespace std;
using namespace SH;

void RPN::addDecl(std::string name, Variable var) {
  symtab[name] = var;
}

void RPN::execute(std::string code) {
  istringstream in(code);

  float val;
  char name[100];

  clear();
  in.width(99);
  for(;;) {
    in >> val;
    if(in.fail()) {
      in >> name;
      if(in.fail()) {
        std::cout << "DONE";
        break;
      }
      if(isalpha(name[0])) {
        std::cout << "Read var " << name;
        if(symtab[name].null()) {
          std::cout << "No such variable" << endl;
        }
        push(symtab[name]);
      } else {
        std::cout << "Read op " << name;
        switch(name[0]) {
          case '=':
            {
              Variable a, b;
              a = pop();
              b = pop();
              shASN(a, b);
              push(a);
              break;
            }
          case '*': tempBinaryOp(OP_MUL); break;
          case '/': tempBinaryOp(OP_DIV); break;
          case '+': tempBinaryOp(OP_ADD); break;
          case '-': 
            {
              Variable a = pop();
              Variable b = pop();
              push(-b); push(a);
              tempBinaryOp(OP_ADD); break;
            }
        }
      }
    } else {
      std::cout << "Read float " << val;
      push(ConstAttrib1f(val));
    }
  }
}

Variable RPN::pop() {
  Variable result = stk.top();
  stk.pop();
  return result;
}

void RPN::push(Variable v) {
  stk.push(v);
}

void RPN::clear() {
  for(;!stk.empty(); stk.pop());
}

void RPN::tempBinaryOp(SH::Operation op, int destSize)
{
  Variable a = pop();
  Variable b = pop();
  // figure out type of result
  const EvalOpInfo* info = Eval::instance()->getEvalOpInfo(op, VALUETYPE_END,
      a.valueType(), b.valueType());
  if(destSize == 0) {
    if(opInfo[op].result_source != OperationInfo::LINEAR) {
      std::cout << "Person who wrote RPN.cpp made an error." << endl;
    }
    destSize = std::max(a.size(), b.size());
  } 
  Variable dest(a.node()->clone(SH_TEMP, destSize, info->m_dest, SEMANTICTYPE_END, false, false));
  Statement stmt(dest, a, op, b);
  if(!Context::current()->parsing()) {
    evaluate(stmt);
  } else {
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  }
  push(dest);
}

void RPN::tempUnaryOp(SH::Operation op)
{
  Variable a = pop();
  // figure out type of result
  const EvalOpInfo* info = Eval::instance()->getEvalOpInfo(op, VALUETYPE_END,
      a.valueType());
  Variable dest(a.node()->clone(SH_TEMP, 0, info->m_dest, SEMANTICTYPE_END, false, false));
  Statement stmt(dest, op, a); 
  if(!Context::current()->parsing()) {
    evaluate(stmt);
  } else {
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  }
  push(dest);
}

