#include "ShOptimizations.hpp"
#include <map>
#include <set>
#include <utility>
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShEvaluate.hpp"
#include "ShContext.hpp"
#include "ShSyntax.hpp"
#include <sstream>
#include <fstream>


namespace {

using namespace SH;

typedef std::queue<ValueTracking::Def> ConstWorkList;

struct ConstProp : public ShStatementInfo {
  ConstProp(ShStatement* stmt,
            ConstWorkList& worklist)
    : stmt(stmt)
  {
    for (int i = 0; i < opInfo[stmt->op].arity ; i++) {
      for (int j = 0; j < stmt->src[i].size(); j++) {
        switch(stmt->src[i].node()->kind()) {
        case SH_INPUT:
        case SH_OUTPUT:
        case SH_INOUT:
        case SH_TEXTURE:
        case SH_STREAM:
          src[i].push_back(Cell(Cell::BOTTOM));
          break;
        case SH_TEMP:
          if (stmt->src[i].uniform()) {
            src[i].push_back(Cell(Cell::UNIFORM, stmt->src[i], j));
          } else {
            src[i].push_back(Cell(Cell::TOP));
          }
          break;
        case SH_CONST:
          src[i].push_back(Cell(Cell::CONSTANT, stmt->src[i].getValue(j)));
          break;
        }
      }
    }
    updateDest(worklist);
  }

  ShStatementInfo* clone() const;

  int idx(int destindex, int source)
  {
    return (stmt->src[source].size() == 1 ? 0 : destindex);
  }

  void updateDest(ConstWorkList& worklist)
  {
    dest.clear();

    // Ignore KIL, optbra, etc.
    if (opInfo[stmt->op].result_source == ShOperationInfo::IGNORE) return;

    if (stmt->op == SH_OP_ASN) {
      for (int i = 0; i < stmt->dest.size(); i++) {
        dest.push_back(Cell(src[0][i]));
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::EXTERNAL) {
      // This statement never results in a constant
      // E.g. texture fetches, stream fetches.
      for (int i = 0; i < stmt->dest.size(); i++) {
        dest.push_back(Cell(Cell::BOTTOM));
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::LINEAR) {
      // Consider each tuple element in turn.
      // Dest and sources are guaranteed to be of the same length.
      // Except that sources might be scalar.
      for (int i = 0; i < stmt->dest.size(); i++) {
        bool alluniform = true;
        bool allconst = true;
        for (int s = 0; s < opInfo[stmt->op].arity; s++) {
          if (src[s][idx(i,s)].state != Cell::CONSTANT) {
            allconst = false;
            if (src[s][idx(i,s)].state != Cell::UNIFORM) {
              alluniform = false;
            }
          }
        }
        if (allconst) {
          ShVariable tmpdest(new ShVariableNode(SH_CONST, 1));
          ShStatement eval(*stmt);
          eval.dest = tmpdest;
          for (int k = 0; k < opInfo[stmt->op].arity; k++) {
            ShVariable tmpsrc(new ShVariableNode(SH_CONST, 1));
            tmpsrc.setValue(0, src[k][idx(i,k)].value);
            eval.src[k] = tmpsrc;
          }
          evaluate(eval);
          dest.push_back(Cell(Cell::CONSTANT, tmpdest.getValue(0)));
          worklist.push(ValueTracking::Def(stmt, i));
        } else if (alluniform) {
          dest.push_back(Cell(Cell::UNIFORM, this, i));
          worklist.push(ValueTracking::Def(stmt, i));
        } else {
          dest.push_back(Cell(Cell::BOTTOM));
        }
      }      
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::ALL) {
      // build statement ONLY if ALL elements of ALL sources are constant
      bool allconst = true;
      bool alluniform = true; // all statements are either uniform or constant
      for (int s = 0; s < opInfo[stmt->op].arity; s++) {
        for (unsigned int k = 0; k < src[s].size(); k++) {
          if (src[s][k].state != Cell::CONSTANT) {
            allconst = false;
            if (src[s][k].state != Cell::UNIFORM) {
              alluniform = false;
            }
          }
        }
      }
      if (allconst) {
        ShVariable tmpdest(new ShVariableNode(SH_CONST, stmt->dest.size()));
        ShStatement eval(*stmt);
        eval.dest = tmpdest;
        for (int i = 0; i < opInfo[stmt->op].arity; i++) {
          ShVariable tmpsrc(new ShVariableNode(SH_CONST, stmt->src[i].size()));
          for (int j = 0; j < stmt->src[i].size(); j++) {
            tmpsrc.setValue(j, src[i][j].value);
          }
          eval.src[i] = tmpsrc;
        }
        evaluate(eval);
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::CONSTANT, tmpdest.getValue(i)));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      } else if (alluniform) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::UNIFORM, this, i));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      } else {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::BOTTOM));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      }
    } else {
      SH_DEBUG_ASSERT(0 && "Invalid result source type");
    }
  }

  typedef int ValueNum;

  struct Uniform {
    Uniform()
      : constant(false),
        valuenum(-1)
    {
    }

    Uniform(float constval)
      : constant(true),
        constval(constval)
    {
    }
    
    Uniform(int valuenum, int index, bool neg)
      : constant(false),
        valuenum(valuenum), index(index), neg(neg)
    {
    }

    bool operator==(const Uniform& other) const
    {
      if (constant != other.constant) return false;

      if (constant) {
        return constval != other.constval;
      } else {
        if (valuenum != other.valuenum) return false;
        if (index != other.index) return false;
        if (neg != other.neg) return false;
        return true;
      }
    }

    bool operator!=(const Uniform& other) const
    {
      return !(*this == other);
    }

    ValueNum valuenum;
    int index;
    bool neg;

    bool constant;
    float constval;
  };

  class Value {
  public:
    enum Type {
      NODE,
      STMT
    };

    Type type;
    
    // Only for type == NODE:
    ShVariableNodePtr node;

    // Only for type == STMT:
    ShOperation op;
    int destsize;
    std::vector<Uniform> src[3];

    static void clear()
    {
      m_values.clear();
    }
    
    static ValueNum lookup(const ShVariableNodePtr& node)
    {
      for (int i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type == NODE && m_values[i]->node == node) return i;
      }
      m_values.push_back(new Value(node));
      return m_values.size() - 1;
    }

    static ValueNum lookup(ConstProp* cp)
    {
      for (int i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type != STMT) continue;
        if (m_values[i]->op != cp->stmt->op) continue;

        bool mismatch = false;
        for (int j = 0; j < opInfo[cp->stmt->op].arity; j++) {
          if (cp->src[j].size() != m_values[i]->src[j].size()) {
            mismatch = true;
            break;
          }
          
          for (int k = 0; k < cp->src[j].size(); k++) {
            if (cp->src[j][k].uniform != m_values[i]->src[j][k]) {
              mismatch = true;
              break;
            }
          }
          if (mismatch) break;
        }
        if (!mismatch) return i;
      }
      m_values.push_back(new Value(cp));
      return m_values.size() - 1;
    }

    static Value* get(ValueNum n)
    {
      return m_values[n];
    }

    static void dump(std::ostream& out);
    
  private:
    Value(const ShVariableNodePtr& node)
      : type(NODE), node(node)
    {
    }

    Value(ConstProp* cp)
      : type(STMT), node(0), op(cp->stmt->op), destsize(cp->stmt->dest.size())
    {
      for (int i = 0; i < opInfo[cp->stmt->op].arity; i++) {
        for (int j = 0; j < cp->src[i].size(); j++) {
          if (cp->src[i][j].state == Cell::UNIFORM) {
            src[i].push_back(cp->src[i][j].uniform);
          } else {
            src[i].push_back(Uniform(cp->src[i][j].value));
          }
        }
      }
    }
    
    static std::vector<Value*> m_values;
  };
  
  struct Cell {
    enum State {
      BOTTOM,
      CONSTANT,
      UNIFORM,
      TOP
    };

    Cell(State state, float value = 0.0)
      : state(state), value(value)
    {
    }

    Cell(State state, const ShVariable& var, int index) 
      : state(state), value(0.0),
        uniform(Value::lookup(var.node()), var.swizzle()[index], var.neg())
    {
    }

    Cell(State state, ConstProp* cp, int index) 
      : state(state), value(0.0),
        uniform(Value::lookup(cp), index, false)
    {
    }

    bool operator==(const Cell& other) const
    {
      return state == other.state && value == other.value;
    }
    bool operator!=(const Cell& other) const
    {
      return !(*this == other);
    }
    
    State state;
    float value; // Only for state == CONSTANT

    Uniform uniform; // Only for state == UNIFORM
  };

  ShStatement* stmt;
  std::vector<Cell> dest;
  std::vector<Cell> src[3];
};

ShStatementInfo* ConstProp::clone() const
{
  return new ConstProp(*this);
}

ConstProp::Cell meet(const ConstProp::Cell& a, const ConstProp::Cell& b)
{
  if (a.state == ConstProp::Cell::BOTTOM || b.state == ConstProp::Cell::BOTTOM) {
    return ConstProp::Cell(ConstProp::Cell::BOTTOM);
  }
  if (a.state != b.state
      && (a.state != ConstProp::Cell::TOP && b.state != ConstProp::Cell::TOP)) {
    return ConstProp::Cell(ConstProp::Cell::BOTTOM);
  }
  // At this point either the cells are the same or one of them is
  // top.
  if (a.state == b.state) {
    if (a.state == ConstProp::Cell::CONSTANT) {
      if (a.value == b.value) {
        return a;
      } else {
        return ConstProp::Cell(ConstProp::Cell::BOTTOM);
      }
    }
    if (a.state == ConstProp::Cell::UNIFORM) {
      if (a.uniform == b.uniform) return a;
      return ConstProp::Cell(ConstProp::Cell::BOTTOM);
    }
  }

  if (a.state != ConstProp::Cell::TOP) return a;
  if (b.state != ConstProp::Cell::TOP) return b;
    
  return ConstProp::Cell(ConstProp::Cell::TOP);
}

std::vector<ConstProp::Value*> ConstProp::Value::m_values = std::vector<ConstProp::Value*>();

std::ostream& operator<<(std::ostream& out, const ConstProp::Uniform& uniform)
{
  if (uniform.constant) {
    out << uniform.constval;
  } else {
    if (uniform.neg) out << '-';
    out << "v" << uniform.valuenum << "[" << uniform.index << "]";
  }
  return out;
}

void ConstProp::Value::dump(std::ostream& out)
{
  out << "--- uniform values ---" << std::endl;
  for (int i = 0; i < m_values.size(); i++) {
    out << i << ": ";
    if (m_values[i]->type == NODE) {
      out << "node " << m_values[i]->node->name() << std::endl;
    } else if (m_values[i]->type == STMT) {
      out << "stmt [" << m_values[i]->destsize << "] " << opInfo[m_values[i]->op].name << " ";
      for (int j = 0; j < opInfo[m_values[i]->op].arity; j++) {
        if (j) out << ", ";
        for (std::vector<Uniform>::iterator U = m_values[i]->src[j].begin();
             U != m_values[i]->src[j].end(); ++U) {
          out << *U;
        }
      }
      out << ";" << std::endl;
    }
  }
}

std::ostream& operator<<(std::ostream& out, const ConstProp::Cell& cell)
{
  switch(cell.state) {
  case ConstProp::Cell::BOTTOM:
    out << "[bot]";
    break;
  case ConstProp::Cell::CONSTANT:
    out << "[" << cell.value << "]";
    break;
  case ConstProp::Cell::TOP:
    out << "[top]";
    break;
  case ConstProp::Cell::UNIFORM:
    out << "<" << cell.uniform << ">";
    break;
  }
  return out;
}

struct InitConstProp {
  InitConstProp(ConstWorkList& worklist)
    : worklist(worklist)
  {
  }

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      I->template destroy_info<ConstProp>();
      ConstProp* cp = new ConstProp(&(*I), worklist);
      I->add_info(cp);
    }
  }

  ConstWorkList& worklist;
};

struct DumpConstProp {
  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      std::cerr << "{" << *I << "} --- ";
      ConstProp* cp = I->template get_info<ConstProp>();

      if (!cp) {
        std::cerr << "NO CP INFORMATION" << std::endl;
        continue;
      }

      std::cerr << "dest = {";
      for (int i = 0; i < cp->dest.size(); i++) {
        std::cerr << cp->dest[i];
      }
      std::cerr << "}; ";
      for (int s = 0; s < opInfo[I->op].arity; s++) {
        if (s) std::cerr << ", ";
        std::cerr << "src" << s << " = {";
        for (int i = 0; i < cp->src[s].size(); i++) {
          std::cerr << cp->src[s][i];
        }
        std::cerr << "}";
      }
      std::cerr << std::endl;
    }
    
  }
};

struct FinishConstProp
{
  FinishConstProp(bool lift_uniforms)
    : lift_uniforms(lift_uniforms)
  {
  }
  
  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      ConstProp* cp = I->template get_info<ConstProp>();

      if (!cp) continue;

      if (!cp->dest.empty()) {
        // if all dest fields are constants, replace this with a
        // constant assignment

        if (I->op != SH_OP_ASN || I->src[0].node()->kind() != SH_CONST) {
          ShVariable newconst(new ShVariableNode(SH_CONST, I->dest.size()));
          bool allconst = true;
          for (int i = 0; i < I->dest.size(); i++) {
            if (cp->dest[i].state != ConstProp::Cell::CONSTANT) {
              allconst = false;
              break;
            }
            newconst.setValue(i, cp->dest[i].value);
          }
          if (allconst) {
            //SH_DEBUG_PRINT("Replaced {" << *I << "} with " << newconst);
            *I = ShStatement(I->dest, SH_OP_ASN, newconst);
          } else {
            // otherwise, do the same for each source field.
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              if (I->src[s].node()->kind() == SH_CONST) continue;
            
              ShVariable newconst(new ShVariableNode(SH_CONST, I->src[s].size()));
              bool allconst = true;
              for (int i = 0; i < I->src[s].size(); i++) {
                if (cp->src[s][i].state != ConstProp::Cell::CONSTANT) {
                  allconst = false;
                  break;
                }
                newconst.setValue(i, cp->src[s][i].value);
              }
              if (allconst) {
                //SH_DEBUG_PRINT("Replaced {" << *I << "}.src[" << s << "] with " << newconst);
                I->src[s] = newconst;
              }
            }
          }

          if (!lift_uniforms) {
            //SH_DEBUG_PRINT("Skipping uniform lifting");
            continue;
          }

          //SH_DEBUG_PRINT("Considering " << *I << " for uniform lifting");
          
          bool alluniform = true;
          for (int s = 0; s < opInfo[I->op].arity; s++) {
            for (int i = 0; i < I->src[s].size(); i++) {
              if (cp->src[s][i].state != ConstProp::Cell::UNIFORM
                  || cp->src[s][i].state != ConstProp::Cell::CONSTANT) {
                alluniform = false;
                break;
              }
            }
            if (!alluniform) break;
          }
          if (!alluniform || I->dest.node()->kind() == SH_OUTPUT
              || I->dest.node()->kind() == SH_INOUT) {
            //SH_DEBUG_PRINT("Really considering " << *I << " for uniform lifting");
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              if (I->src[s].uniform()) {
                //SH_DEBUG_PRINT(*I << ".src[" << s << "] is already a uniform");
                continue;
              }

              bool mixed = false;
              bool neg = false;
              ConstProp::ValueNum uniform = -1;
              std::vector<int> indices;
              
              for (int i = 0; i < I->src[s].size(); i++) {
                if (cp->src[s][i].state == ConstProp::Cell::UNIFORM) {
                  if (uniform < 0) {
                    uniform = cp->src[s][i].uniform.valuenum;
                    neg = cp->src[s][i].uniform.neg;
                    indices.push_back(cp->src[s][i].uniform.index);
                  } else {
                    if (uniform != cp->src[s][i].uniform.valuenum
                        || neg != cp->src[s][i].uniform.neg) {
                      mixed = true;
                      break;
                    }
                    indices.push_back(cp->src[s][i].uniform.index);
                  }
                }
              }
              
              
              if (mixed) {
                //SH_DEBUG_PRINT(*I << ".src[" << s << "] is mixed");
                continue;
              }
              if (uniform < 0) {
                //SH_DEBUG_PRINT(*I << ".src[" << s << "] is not uniform");
                continue;
              }
              ConstProp::Value* value = ConstProp::Value::get(uniform);


              //SH_DEBUG_PRINT("Lifting {" << *I << "}.src[" << s << "]: " << uniform);
              
              ShSwizzle swizzle(value->destsize, indices.size(), &(*indices.begin()));
              //SH_DEBUG_PRINT("Swizzle = " << swizzle);
              // Build a uniform to represent this computation.
              ShVariableNodePtr node = build_uniform(value, uniform);
              if (node) {
                I->src[s] = ShVariable(node, swizzle, neg);
              } else {
                //SH_DEBUG_PRINT("Could not lift " << *I << ".src[" << s << "] for some reason");
              }
            }
          }
        }
      }

      // Remove constant propagation information.
      I->template destroy_info<ConstProp>();
    }
  }

  ShVariableNodePtr build_uniform(ConstProp::Value* value,
                                  ConstProp::ValueNum valuenum)
  {
    if (value->type == ConstProp::Value::NODE) {
      return value->node;
    }
    
    ShContext::current()->enter(0);
    ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize);
    {
    std::ostringstream s;
    s << "dep_" << valuenum;
    node->name(s.str());
    }
    ShContext::current()->exit();

    //SH_DEBUG_PRINT("Lifting value #" << valuenum);

    bool broken = false;
    
    ShProgram prg = SH_BEGIN_PROGRAM("uniform") {
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
        if (stmt.src[i].null()) broken = true;
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    } SH_END;

#ifdef SH_DEBUG_OPTIMIZER
    {
      std::ostringstream s;
      s << "lifted_" << valuenum;
      std::string dotfilename(s.str() + ".dot");
      std::ofstream dot(dotfilename.c_str());
      prg.node()->ctrlGraph->graphvizDump(dot);
      dot.close();
      std::string cmdline = std::string("dot -Tps -o ") + s.str() + ".ps " + s.str() + ".dot";
      system(cmdline.c_str());
    }
#endif
    
    if (broken) return 0;
    node->attach(prg.node());

    value->type = ConstProp::Value::NODE;
    value->node = node;
    
    return node;
  }

  ShVariable compute(const std::vector<ConstProp::Uniform>& src)
  {
    ConstProp::ValueNum v = -1;
    
    bool allsame = true;
    bool neg = false;
    std::vector<int> indices;
    std::vector<float> constvals;
    for (int i = 0; i < src.size(); i++) {
      if (src[i].constant) {
        if (v >= 0) {
          allsame = false;
          break;
        }
        constvals.push_back(src[i].constval);
      } else {
        if (v < 0 && constvals.empty()) {
          v = src[i].valuenum;
          neg = src[i].neg;
        } else {
          if (v != src[i].valuenum || neg != src[i].neg || !constvals.empty()) {
            allsame = false;
          }
        }
        indices.push_back(src[i].index);
      }
    }
    if (!allsame) {
      //SH_DEBUG_PRINT("Source is not heterogenous");
      // Make intermediate variables, combine them together.
      ShVariable r = ShVariable(new ShVariableNode(SH_TEMP, src.size()));
      
      for (int i = 0; i < src.size(); i++) {
        std::vector<ConstProp::Uniform> v;
        v.push_back(src[i]);
        ShVariable scalar = compute(v);
        ShStatement stmt(r(i), SH_OP_ASN, scalar);
        ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      }
      return r;
    }

    if (!constvals.empty()) {
      ShVariable var(new ShVariableNode(SH_CONST, constvals.size()));
      var.setValues(&*constvals.begin());
      return var;
    }
    
    ConstProp::Value* value = ConstProp::Value::get(v);
    
    if (value->type == ConstProp::Value::NODE) {
      //SH_DEBUG_PRINT("compute: Returning simple uniform with " << indices.size() << " indices");
      ShSwizzle swizzle(value->node->size(), indices.size(), &*indices.begin());
      //SH_DEBUG_PRINT("compute: Swizzle = " << swizzle);
      return ShVariable(value->node, swizzle, neg);
    }
    if (value->type == ConstProp::Value::STMT) {
      ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize);
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
        if (stmt.src[i].null()) return ShVariable();
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      //SH_DEBUG_PRINT("compute: Returning computed uniform with " << indices.size() << " indices");
      ShSwizzle swizzle(node->size(), indices.size(), &*indices.begin());
      //SH_DEBUG_PRINT("compute: Swizzle = " << swizzle);
      return ShVariable(node, swizzle, neg);
    }

    //SH_DEBUG_PRINT("Reached invalid point");

    // Should never reach here.
    return ShVariable();
  }

  bool lift_uniforms;
};

}

namespace SH {


void propagate_constants(ShProgram& p)
{
  ShCtrlGraphPtr graph = p.node()->ctrlGraph;

  ConstWorkList worklist;

  //ConstProp::Value::clear();
  
  InitConstProp init(worklist);
  graph->dfs(init);

  while (!worklist.empty()) {
    ValueTracking::Def def = worklist.front(); worklist.pop();
    ValueTracking* vt = def.stmt->template get_info<ValueTracking>();
    if (!vt) {
      //SH_DEBUG_PRINT("Statement on worklist does not have VT information?");
      //SH_DEBUG_PRINT(*def.stmt);
      continue;
    }


    for (ValueTracking::DefUseChain::iterator use = vt->uses[def.index].begin();
         use != vt->uses[def.index].end(); ++use) {
      ConstProp* cp = use->stmt->template get_info<ConstProp>();
      if (!cp) {
        //SH_DEBUG_PRINT("Use does not have const prop information!");
        continue;
      }

      ConstProp::Cell cell = cp->src[use->source][use->index];

      ValueTracking* ut = use->stmt->template get_info<ValueTracking>();
      if (!ut) {
        // Should never happen...
        //SH_DEBUG_PRINT("Statement on worklist does not have VT information?");
        //SH_DEBUG_PRINT(*def.stmt);
        continue;
      }

      ConstProp::Cell new_cell = cell;
      
      for (ValueTracking::UseDefChain::iterator possdef
             = ut->defs[use->source][use->index].begin();
           possdef != ut->defs[use->source][use->index].end(); ++possdef) {
        ConstProp* dcp = possdef->stmt->template get_info<ConstProp>();
        if (!dcp) {
          //SH_DEBUG_PRINT("Statement on worklist does not have CP information?");
          continue;
        }
        
        ConstProp::Cell destcell = dcp->dest[possdef->index];
        if (destcell.state == ConstProp::Cell::CONSTANT
            && use->stmt->src[use->source].neg()) {
          destcell.value = -destcell.value;
        }
        if (destcell.state == ConstProp::Cell::UNIFORM
            && use->stmt->src[use->source].neg()) {
          destcell.uniform.neg = !destcell.uniform.neg;
        }
        new_cell = meet(destcell, new_cell);
      }
      
      if (cell != new_cell) {
        cp->src[use->source][use->index] = new_cell;
        cp->updateDest(worklist);
      }
    }
  }
  // Now do something with our glorious newfound information.

  
#ifdef SH_DEBUG_OPTIMIZER
  ConstProp::Value::dump(std::cerr);

  DumpConstProp dump;
  graph->dfs(dump);
#endif
  
  FinishConstProp finish(p.node()->target().find("gpu:") == 0);
  graph->dfs(finish);
  
}

}
