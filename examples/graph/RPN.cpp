#include <sstream>
#include <sh/ShEvaluate.hpp>
#include <sh/ShEval.hpp>
#include "RPN.hpp"

using namespace std;
using namespace SH;

void RPN::addDecl(std::string name, ShVariable var) {
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
              ShVariable a, b;
              a = pop();
              b = pop();
              shASN(a, b);
              push(a);
              break;
            }
          case '*': tempBinaryOp(SH_OP_MUL); break;
          case '/': tempBinaryOp(SH_OP_DIV); break;
          case '+': tempBinaryOp(SH_OP_ADD); break;
          case '-': 
            {
              ShVariable a = pop();
              ShVariable b = pop();
              push(-b); push(a);
              tempBinaryOp(SH_OP_ADD); break;
            }
        }
      }
    } else {
      std::cout << "Read float " << val;
      push(ShConstAttrib1f(val));
    }
  }
}

ShVariable RPN::pop() {
  ShVariable result = stk.top();
  stk.pop();
  return result;
}

void RPN::push(ShVariable v) {
  stk.push(v);
}

void RPN::clear() {
  for(;!stk.empty(); stk.pop());
}

void RPN::tempBinaryOp(SH::ShOperation op, int destSize)
{
  ShVariable a = pop();
  ShVariable b = pop();
  // figure out type of result
  const ShEvalOpInfo* info = ShEval::instance()->getEvalOpInfo(op, SH_VALUETYPE_END,
      a.valueType(), b.valueType());
  if(destSize == 0) {
    if(opInfo[op].result_source != ShOperationInfo::LINEAR) {
      std::cout << "Person who wrote RPN.cpp made an error." << endl;
    }
    destSize = std::max(a.size(), b.size());
  } 
  ShVariable dest(a.node()->clone(SH_TEMP, destSize, info->m_dest, SH_SEMANTICTYPE_END, false, false));
  ShStatement stmt(dest, a, op, b);
  if(!ShContext::current()->parsing()) {
    evaluate(stmt);
  } else {
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  }
  push(dest);
}

void RPN::tempUnaryOp(SH::ShOperation op)
{
  ShVariable a = pop();
  // figure out type of result
  const ShEvalOpInfo* info = ShEval::instance()->getEvalOpInfo(op, SH_VALUETYPE_END,
      a.valueType());
  ShVariable dest(a.node()->clone(SH_TEMP, 0, info->m_dest, SH_SEMANTICTYPE_END, false, false));
  ShStatement stmt(dest, op, a); 
  if(!ShContext::current()->parsing()) {
    evaluate(stmt);
  } else {
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  }
  push(dest);
}

