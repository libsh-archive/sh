#include <iomanip>
#include <ctype.h>
#include "ExprParser.hpp"
#include <sh/ShEvaluate.hpp>

using namespace SH;
using namespace std;

bool DBG =  false;

#define DOUT(s) { if(DBG) { std::cout << setw(m_indent) << " " << s << endl; } }

ShVariable ExprParser::registerVar(std::string name, ShVariable var) {
  if(m_streams.find(name) != m_streams.end()) { 
    std::cerr << "Warning: Replacing stream " << name << endl; 
    m_streams.erase(name);
  } 
  m_vars[name] = var;
  var.name(name);
  std::cerr << "Registered variable " << var.node()->nameOfType() << " " << name << endl;
  return var;
}

bool ExprParser::hasVar(std::string name) {
  return m_vars.find(name) != m_vars.end(); 
}

SH::ShVariable ExprParser::getVar(std::string name) {
  if(!hasVar(name)) return ShVariable();
  return m_vars[name];
}

ShStream ExprParser::registerStream(std::string name, ShStream stream) {
  if(m_vars.find(name) != m_vars.end()) {
    std::cerr << "Warning: Replacing var " << name << endl; 
    m_vars.erase(name);
  } 
  m_streams[name] = stream;
  std::cerr << "Registered stream " << name << endl; 
  return stream;
}

bool ExprParser::hasStream(std::string name) {
  return m_streams.find(name) != m_streams.end() ||
         (m_vars.find(name) != m_vars.end() && m_vars[name].node()->hasValues()); 
}

SH::ShStream ExprParser::getStream(std::string name) {
  if(m_streams.find(name) == m_streams.end()) {
    if(hasVar(name) && m_vars[name].node()->hasValues()) { 
      return varToStream(m_vars[name]);
    } else {
      return ShStream();
    }
  }
  return m_streams[name];
}

void ExprParser::registerFunc(std::string name, ShProgram func) {
  func.name(name);
  m_funcs[name] = func;
  func.name(name);
}

ShProgram& ExprParser::operator[](std::string name) {
  return m_funcs[name];
}

void ExprParser::dump(std::ostream& out) {
  out << "Variables: " << endl;
  for(VarMap::iterator V = m_vars.begin(); V != m_vars.end(); ++V) {
    out << "  " << V->second.node()->nameOfType() << " " << V->first << " = " << V->second << endl; 
  }

  out << "Streams: " << endl;
  for(StreamMap::iterator S = m_streams.begin(); S != m_streams.end(); ++S) {
    out << "  " << S->first << endl; 
  }

  out << "Functions: " << endl;
  for(FuncMap::iterator F = m_funcs.begin(); F != m_funcs.end(); ++F) {
    out << "*  " << F->first << endl 
        << F->second.describe_interface() << endl; 
  }
}

void ExprParser::parse(std::string stmtlist) {
  // strip whitespace
  m_stmtlist = stmtlist;
  m_cur = 0;
  m_end = stmtlist.length();
  m_indent = 0;
  parse_stmtlist();
}

SH::ShVariable ExprParser::parse_expr(std::string expr) {
  m_stmtlist = expr; 
  m_cur = 0;
  m_end = expr.length(); 
  m_indent = 0;
  return parse_expr();
}

void ExprParser::parse_stmtlist() { 
  debugEnter("stmtlist");
  for(;m_cur != m_end; parse_stmt()); 
  debugExit();
}

void ExprParser::parse_stmt() { 
  debugEnter("stmt");
  if(peek() == '#') {
    parse_directive();
  } else if(peek() == '@') {
    parse_decl();
  } else if (peek() == ';') {
  } else if(!parse_asmop()) {
    parse_expr();
  }
  eatChar(';');
  eatWs();
  debugExit();
}

void ExprParser::parse_directive() {
  debugEnter("directive");
  eatChar('#');
  string name = token();
  if(name == "debug") {
    DBG = !DBG;
    std::cout << "Debug Mode = " << DBG << endl;
  }
  debugExit();
}

ShVariable ExprParser::parse_decl() {
  debugEnter("decl");
  eatChar('@');
  string shtype = token();
  string name = token();
  ShVariable result = registerVar(name, makeVar(shtype));
  eatWs();
  if(peek() == '=') {
    eatChar('=');
    ShVariable init = parse_expr();
    shASN(result, init);
    DOUT("Initialized " << name << " = " << init);
  }
  debugExit();
  return result;
}

bool ExprParser::parse_asmop() {
  save();
  string first = token();
  eatWs();

  size_t i = 0;
  for(i = 0; i < first.length(); first[i] = toupper(first[i]), ++i);

  for(;opInfo[i].name != 0; ++i) {
    if(first == opInfo[i].name) {
      int arity = opInfo[i].arity;
      ShVariable dest = parse_expr(); 
      ShStatement stmt(dest, static_cast<ShOperation>(i));

      for(int j = 0; j < arity; ++j) {
        eatChar(',');
        stmt.src[j] = parse_expr();
      }

      // decision taken from ShInstructions.cpp
      if(ShContext::current()->parsing()) {
        ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      } else {
        evaluate(stmt);
      }

      return true;
    }
  }
  // not found
  restore();
  return false;
}

ShVariable ExprParser::parse_expr() { 
  debugEnter("expr = ");
  ShVariable lhs = parse_expr0();
  eatWs();
  char next = peek();
  if(next != '=') { 
    debugExit();
    return lhs;
  }
  DOUT("operator " << next);
  eatChar('=');
  ShVariable rhs = parse_expr();
  SH_DEBUG_ASSERT(lhs.size() == rhs.size());
  shASN(lhs, rhs);

  DOUT("expr = " << lhs);
  debugExit();
  return lhs;
}

ShVariable ExprParser::parse_expr0() { 
  debugEnter("expr0 = ");
  ShVariable lhs = parse_expr1();
  eatWs();
  char next = peek();
  if(next != '|') { 
    debugExit();
    return lhs;
  }
  DOUT("operator " << next);
  eatChar('|');
  ShVariable rhs = parse_expr0();
  ShValueType rvt = lhs.valueType();
  ShVariable result(new ShVariableNode(SH_TEMP, 1, rvt));
  SH_DEBUG_ASSERT(lhs.size() == rhs.size());
  shDOT(result, lhs, rhs);
  DOUT("expr0 = " << result);
  debugExit();
  return result;
}

ShVariable ExprParser::parse_expr1() { 
  debugEnter("expr +-");
  ShVariable result = parse_expr2();
  while(true) {
    eatWs();
    char next = peek();
    if(next != '+' && next != '-') {
      break;
    }
    DOUT("operator " << next);
    ++m_cur;
    ShVariable rhs = parse_expr2();
    int rsize = max(result.size(), rhs.size());
    ShValueType rvt = result.valueType();
    ShVariable temp(new ShVariableNode(SH_TEMP, rsize, rvt));
    switch(next) {
      case '+': shADD(temp, result, rhs); break;
      case '-': shADD(temp, result, -rhs); break;
    }
    result = temp;
  }
  DOUT("expr1 = " << result);
  debugExit();
  return result;
}

ShVariable ExprParser::parse_expr2() { 
  debugEnter("expr2 */%");
  ShVariable result = parse_expr3();
  while(true) {
    eatWs();
    char next = peek();
    if(next != '*' && next != '/' && next != '%') {
      break;
    }
    DOUT("operator " << next);
    ++m_cur;
    ShVariable rhs = parse_expr3();
    int rsize = max(result.size(), rhs.size());
    ShValueType rvt = result.valueType();
    ShVariable temp(new ShVariableNode(SH_TEMP, rsize, rvt));
    switch(next) {
      case '*': shMUL(temp, result, rhs); break;
      case '/': shDIV(temp, result, rhs); break;
      case '%': shMOD(temp, result, rhs); break;
    }
    result = temp;
  }
  DOUT("expr2 = " << result);
  debugExit();
  return result;
}

ShVariable ExprParser::parse_expr3() { 
  debugEnter("expr3 unary -");
  eatWs();
  char next = peek();
  bool negate = false;
  while(next == '-') {
    eatChar('-');
    negate = !negate;
    eatWs();
    next = peek();
  }
  ShVariable result = parse_expr4(); 
  result = negate ? -result : result;

  DOUT("expr3 = " << result);
  debugExit();
  return result;
}


ShVariable ExprParser::parse_expr4() { 
  debugEnter("expr4");
  ShVariable result; 
  eatWs();
  char next = peek();
  if(isalpha(next)) {
    string tok = token();
    if(m_vars.find(tok) != m_vars.end()) {
      result = m_vars[tok];
      eatWs();
      if(peek() == '(') {
        ShSwizzle swiz = parse_swiz(result.size());
        result = result(swiz);
      }
    } else if(m_funcs.find(tok) != m_funcs.end()) {
      ShProgram func = m_funcs[tok];
      ShRecord args = parse_arglist();
      result = ShVariable((*func.outputs_begin())->clone(
            SH_TEMP, 0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END,
            true, false));
      result = func(args); 
    } else {
      cerr << "Unexpected token " << tok << " in expression " << endl; 
    }
  } else if(next == '@') {
    eatChar('@');
    result = makeVar(token());
    std::vector<float> consts = parse_constlist();
    if(shIsInterval(result.valueType())) { 
      DOUT("const list - interval"); 
      // @todo factor out and merge with branch below 
      // (virtually the same code)
      SH_DEBUG_ASSERT(consts.size() * 2 == result.size());
      size_t N = result.size();
      
      ShDataVariant<ShInterval<float> > val(N);
      for(size_t i = 0; i < N; ++i) {
        val[i].lo() = consts[2 * i];
        val[i].hi() = consts[2 * i + 1];
      }

      if(result.node()->kind() == SH_CONST) {
        result.setVariant(&val);
      } else {
        ShVariable newconst(new ShVariableNode(SH_CONST, N, SH_I_FLOAT, SH_ATTRIB));
        newconst.setVariant(&val);
        shASN(result, newconst);
      }
    } else {
      DOUT("const list - real"); 
      SH_DEBUG_ASSERT(consts.size() == result.size());
      size_t N = result.size();

      ShDataVariant<float> val(N);
      for(size_t i = 0; i < N; ++i) {
        val[i] = consts[i];
        DOUT(i << ": " << consts[i]);
      }

      if(result.node()->kind() == SH_CONST) {
        result.setVariant(&val);
      } else {
        ShVariable newconst(new ShVariableNode(SH_CONST, N, SH_FLOAT, SH_ATTRIB));
        newconst.setVariant(&val);
        shASN(result, newconst);
      }
    }
  } else if(isdigit(next)) {
    result = ShConstAttrib1f(real()); 
  } else if (next == '(') {
    eatChar('(');
    result = parse_expr();
    eatWs(); eatChar(')');
  } else {
    cerr << "Unexpected char " << peek() << " at pos " << m_cur << endl;
    m_cur++;
  }
  DOUT("expr4 = " << result);
  debugExit();
  return result;
}

ShRecord ExprParser::parse_arglist() { 
  debugEnter("arglist");
  ShRecord result;
  eatWs();
  if(!eatChar('(')) return result;
  for(;;) {
    eatWs();
    char next = peek();
    if(next == ')') break; 
    result.append(parse_expr());
    eatWs();
    if(peek() == ',') eatChar(',');
  }
  eatChar(')');
  debugExit();
  return result;
}

std::vector<float> ExprParser::parse_constlist() { 
  debugEnter("constlist");
  std::vector<float> result;

  eatWs();
  if(!eatChar('(')) return result;
  for(;;) {
    eatWs();
    char next = peek();
    if(next == ')') break; 
    result.push_back(real());
    eatWs();
    if(peek() == ',') eatChar(',');
  }
  eatChar(')');
  debugExit();
  return result;
}

ShSwizzle ExprParser::parse_swiz(int srcSize) {
  vector<int> indices;
  eatChar('(');
  for(;;) {
    eatWs();
    if(peek() == ')') break; 
    else if(peek() == ',') {
      eatChar(',');
      eatWs();
    }
    int ind = number();
    if(ind == -1) {
      cerr << "Expecting digit in swizzle" << endl;
    }
    indices.push_back(ind);
  }
  int *temp = new int[indices.size()];
  std::copy(indices.begin(), indices.end(), temp);
  ShSwizzle result(srcSize, indices.size(), temp);
  delete[] temp;
  eatWs();
  eatChar(')');
  return result;
}

void ExprParser::eatWs() {
  for(; m_cur != m_end && isspace(peek()); ++m_cur);
}

bool ExprParser::eatChar(char expected) {
  if(m_cur == m_end) return false;
  bool result = (m_stmtlist[m_cur] == expected);
  if(!result) {
    cerr << "Expected " << expected << " at position " << m_cur 
      << " found [" << peek() << "]  Context: <" 
      << m_stmtlist.substr(max(0, m_cur - 10), min(10, m_cur + 1)) 
      << "##" << endl;
    m_cur++;
    return false;
  }
  m_cur++;
  return true;
}

char ExprParser::peek() {
  return (m_cur == m_end) ? 0 : m_stmtlist[m_cur]; 
}

string ExprParser::token() {
  string result;
  eatWs();
  for(; isalnum(peek()) || peek() == '_'; result = result + peek(), ++m_cur); 
  return result;
}

int ExprParser::number() {
  if(!isdigit(peek())) return -1;
  int result = 0;
  for(; isdigit(peek()); result = result * 10 + peek() - '0', ++m_cur);
  return result;
}

float ExprParser::real() {
  bool neg = false;
  if(peek() == '-') {
    neg = true; 
    eatChar('-');
  }

  // significand
  if(!isdigit(peek())) return 0;
  float sig = 0;
  for(; isdigit(peek()); sig = sig* 10 + peek() - '0', ++m_cur);

  // mantissa
  float mant = 0;
  float mult = 0.1;
  if(peek() == '.') {
    eatChar('.');
    for(; isdigit(peek()); mant = mant + mult * (peek() - '0'), mult *= 0.1, ++m_cur);
  }

  float result = mant + sig;
  if(neg) result *= -1;
  return result; 
}

void ExprParser::save() {
  m_save = m_cur;
}

void ExprParser::restore() {
  m_cur = m_save;
}

ShVariable ExprParser::makeVar(string shtype) {
  int size = 1;
  ShValueType vt = SH_FLOAT;
  ShSemanticType type = SH_ATTRIB;
  ShBindingType kind = SH_TEMP;
  size_t i = 0;
  string foo;
  int temp = 0;
  shtype = shtype + 'A'; // put a guard at the end

  for(;i < shtype.length();++i) {
    char ci = shtype[i];

    bool dofoo = false;
    if(isdigit(ci)) {
      temp = temp * 10 + (ci - '0');
      dofoo = true;
    } else {
      if(isupper(ci)) {
        if(temp != 0) size = temp;
        dofoo = true;
      }
    }
    if(dofoo) {
      DOUT("dofoo " << foo);
      // TODO use the tables instead of doing this manually
      if (foo == "Sh" || foo == "") {
      } else if(foo == "Attrib") { type = SH_ATTRIB;
      } else if(foo == "Point") { type = SH_POINT;
      } else if(foo == "Position" || foo == "Pos") { type = SH_POSITION;
      } else if(foo == "Vector" || foo == "Vec") { type = SH_VECTOR;
      } else if(foo == "Normal") { type = SH_NORMAL;
      } else if(foo == "Texcoord" || foo == "Tc") { type = SH_TEXCOORD;
      } else if(foo == "Color") { type = SH_COLOR;

      } else if(foo == "Input" || foo == "In") { kind = SH_INPUT;
      } else if(foo == "Output" || foo == "Out") { kind = SH_OUTPUT;
      } else if(foo == "InOut" || foo == "IO") { kind = SH_INOUT;
      } else if(foo == "Const") { kind = SH_CONST;

      } else if(foo == "d") { vt = SH_DOUBLE; 
      } else if(foo == "f") { vt = SH_FLOAT; 
      } else if(foo == "i") { vt = SH_INT; 
      } else if(foo == "i_d") { vt = SH_I_DOUBLE; 
      } else if(foo == "a_d") { vt = SH_A_DOUBLE; 
      } else if(foo == "i_f") { vt = SH_I_FLOAT; 
      } else if(foo == "a_f") { vt = SH_A_FLOAT; 
      } else if(!isdigit(foo[0])) {
        cerr << "Unrecognized part of Sh type <" << foo << ">" << endl;
      }
      foo = "";
    }

    // @todo clean this up... (tokenize by splitting at digits and capitals)  
    if(!isdigit(ci)) {
      foo = foo + ci;
    }
  }
  return ShVariable(new ShVariableNode(kind, size, vt, type));
}

ShStream ExprParser::varToStream(ShVariable& var) {
  DOUT("Converting var --> stream " << var.node()->nameOfType() << " " << var.name());
  SH_DEBUG_ASSERT(var.node()->hasValues());
  ShVariantPtr variant = var.node()->getVariant();
  ShMemoryPtr mem = new ShHostMemory(var.size() * variant->datasize(), variant->array());
  ShChannelNodePtr channel = new ShChannelNode(var.node()->specialType(),
                                    var.size(),
                                    var.valueType(),
                                    mem,
                                    1);
  return ShStream(channel);
}

void ExprParser::debugEnter(std::string node) {
  DOUT(node << " { " << m_cur << "," << m_end);
  m_indent += 2;
}

void ExprParser::debugExit() {
  m_indent -= 2;
  DOUT("}" << m_cur << "," << m_end);
}
