// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "ShOptimizations.hpp"
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShVariant.hpp"
#include "ShEvaluate.hpp"
#include "ShContext.hpp"
#include "ShSyntax.hpp"
#include "ShInfo.hpp"
#include <sstream>
#include <fstream>

// Uncomment to enable constant/uniform propagation debugging (verbose!)
// #define SH_DEBUG_CONSTPROP

#ifdef SH_DEBUG_OPTIMIZER
#ifndef SH_DEBUG_CONSTPROP
#define SH_DEBUG_CONSTPROP
#endif
#endif

namespace {

using namespace SH;

typedef std::queue<ValueTracking::Def> ConstWorkList;

struct ConstProp : public ShInfo {
  ConstProp(ShStatement* stmt,
            ShProgramNodeCPtr prog,
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
        case SH_PALETTE:
          src[i].push_back(Cell(Cell::BOTTOM));
          break;
        case SH_TEMP:
          if (stmt->src[i].uniform()) {
            // Don't lift computations dependent on uniforms which
            // have been marked with "opt:lifting" == "never"
            if (stmt->src[i].meta("opt:lifting") != "never") {
              src[i].push_back(Cell(Cell::UNIFORM, stmt->src[i], j));
            } else {
              src[i].push_back(Cell(Cell::BOTTOM));
            }
          } else {
            src[i].push_back(Cell(Cell::TOP));
          }
          break;
        case SH_CONST:
          src[i].push_back(Cell(Cell::CONSTANT, stmt->src[i].getVariant(j)));
          break;
        default:
          SH_DEBUG_ASSERT(0 && "Invalid ShBindingType");
          return;
        }
      }
    }
    updateDest(worklist);
  }

  ShInfo* clone() const;

  int idx(int destindex, int source)
  {
    return (stmt->src[source].size() == 1 ? 0 : destindex);
  }

  void updateDest(ConstWorkList& worklist)
  {
    if(dest.empty()) {
      dest.resize(stmt->dest.size(), Cell(Cell::TOP)); 
    }

    // Ignore KIL, optbra, etc.
    if (opInfo[stmt->op].result_source == ShOperationInfo::IGNORE) return;

    if (stmt->op == SH_OP_ASN) {
      for (int i = 0; i < stmt->dest.size(); i++) {
        setDest(i, src[0][i], worklist); // assume src[0][i] cannot move up lattice
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::EXTERNAL) {
      // This statement never results in a constant
      // E.g. texture fetches, stream fetches.
      for (int i = 0; i < stmt->dest.size(); i++) {
        setDest(i, Cell(Cell::BOTTOM), worklist);
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::LINEAR) {
      // The strategy here is to ensure that 
      // a) whenever one src becomes bottom, dest becomes bottom
      // b) uniform only gets set when ALL src are uniform (because value
      // tracking requires it)
      // c) otherwise, propagate CONST state per element
      // @todo range (CONST may move across to UNIFORM, check that this is okay)

      // Consider each tuple element in turn.
      // Dest and sources are guaranteed to be of the same length.
      // Except that sources might be scalar.
      bool all_fields_uniform = true;
      bool some_field_bottom = false;
      for (int i = 0; i < stmt->dest.size(); i++) {
        bool alluniform = true;
        bool allconst = true;
        bool somebottom = false;
        for (int s = 0; s < opInfo[stmt->op].arity; s++) {
          if (src[s][idx(i,s)].state == Cell::BOTTOM) {
            somebottom = true;
          }
          if (src[s][idx(i,s)].state != Cell::CONSTANT) {
            allconst = false;
            if (src[s][idx(i,s)].state != Cell::UNIFORM) {
              alluniform = false;
            }
          }
        }
        some_field_bottom |= somebottom;
        if (!(alluniform && !allconst)) all_fields_uniform = false;
        if (allconst) {
          ShVariable tmpdest(new ShVariableNode(SH_CONST, 1, stmt->dest.valueType()));
          ShStatement eval(*stmt);
          eval.dest = tmpdest;
          for (int k = 0; k < opInfo[stmt->op].arity; k++) {
            ShVariantCPtr srcValue = src[k][idx(i,k)].value;
            ShVariable tmpsrc(new ShVariableNode(SH_CONST, 1, srcValue->valueType()));
            tmpsrc.setVariant(srcValue, 0);
            eval.src[k] = tmpsrc;
          }
          evaluate(eval);
          setDest(i, Cell(Cell::CONSTANT, tmpdest.getVariant(0)), worklist);
        } else if (somebottom) {
          setDest(i, Cell(Cell::BOTTOM), worklist);
        }
      } 


      // Because making a uniform cell based on a ConstProp requires
      // generating a value for the entire statement (not just one
      // field of the destination), we only push said cells if ALL of
      // the indices are uniform for ALL of their corresponding sources.
      if (all_fields_uniform) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          setDest(i, Cell(Cell::UNIFORM, this, i), worklist);
        }
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::ALL) {
      // build statement ONLY if ALL elements of ALL sources are constant
      bool allconst = true;
      bool alluniform = true; // all statements are either uniform or constant
      bool somebottom = false;
      for (int s = 0; s < opInfo[stmt->op].arity && !somebottom; s++) {
        for (unsigned int k = 0; k < src[s].size(); k++) {
          if(src[s][k].state == Cell::BOTTOM) {
            somebottom = true;
          }
          if (src[s][k].state != Cell::CONSTANT) {
            allconst = false;
            if (src[s][k].state != Cell::UNIFORM) {
              alluniform = false;
            }
          }
        }
      }
      if (allconst) { 
        ShVariable tmpdest(new ShVariableNode(SH_CONST, stmt->dest.size(), stmt->dest.valueType()));
        ShStatement eval(*stmt);
        eval.dest = tmpdest;
        for (int i = 0; i < opInfo[stmt->op].arity; i++) {
          SH_DEBUG_ASSERT(src[i][0].value); // @todo type DEBUGGING
          ShValueType srcValueType = src[i][0].value->valueType(); 
          ShVariable tmpsrc(new ShVariableNode(SH_CONST, stmt->src[i].size(), srcValueType));
          for (int j = 0; j < stmt->src[i].size(); j++) {
            tmpsrc.setVariant(src[i][j].value, j);
          }
          eval.src[i] = tmpsrc;
        }
        evaluate(eval);
        for (int i = 0; i < stmt->dest.size(); i++) {
          setDest(i, Cell(Cell::CONSTANT, tmpdest.getVariant(i)), worklist);
        }
      } else if (alluniform) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          setDest(i, Cell(Cell::UNIFORM, this, i), worklist);
        }
      } else if (somebottom) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          setDest(i, Cell(Cell::BOTTOM), worklist);
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

    // @todo type...this is my current understanding:
    // May be constant or if !constval, value is not
    // known to be constant
    Uniform(const ShVariantCPtr& cval)
      : constant(true),
        constval(cval ? cval->get() : ShVariantPtr(0))
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
        // @todo type
        if(!constval) return false;
        // Check with Stefanus whether this modification is correct
    //    SH_DEBUG_ASSERT(constval); // @todo type debugging
        return constval->equals(other.constval);
      } else {
        if (valuenum != other.valuenum) return false;
        if (index != other.index) return false;
        if (neg != other.neg) return false;
        return true;
      }
    }

    ShValueType valueType() const 
    {
      if(constant) return constval->valueType();
      return Value::get(valuenum)->valueType();
    }

    bool operator!=(const Uniform& other) const
    {
      return !(*this == other);
    }


    bool constant;

    ValueNum valuenum;
    int index;
    bool neg;

    ShVariantPtr constval;
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
    ShValueType destValueType;
    std::vector<Uniform> src[3];

    static void clear()
    {
      m_values.clear();
    }
    
    static ValueNum lookup(const ShVariableNodePtr& node)
    {
      for (std::size_t i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type == NODE && m_values[i]->node == node) return i;
      }
      m_values.push_back(new Value(node)); // FIXME: Never gets deleted
      return m_values.size() - 1;
    }

    bool operator==(const Value& other) const
    {
      if (type != other.type) return false;
      if (type == NODE) {
        return node == other.node;
      } else if (type == STMT) {
        if (op != other.op || destsize != other.destsize || destValueType != other.destValueType) return false;
        for (int i = 0; i < opInfo[op].arity; i++) {
          if (src[i].size() != other.src[i].size()) return false;
          for (std::size_t j = 0; j < src[i].size(); j++) {
            if (src[i][j] != other.src[i][j]) return false;
          }
        }
        return true;
      }
      return false;
    }

    bool operator!=(const Value& other) const
    {
      return !(*this == other);
    }

    static ValueNum lookup(ConstProp* cp)
    {
      Value* val = new Value(cp); // FIXME: Never gets deleted
      
      for (std::size_t i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type != STMT) continue;
        if (m_values[i]->op != cp->stmt->op) continue;

        if (*val == *m_values[i]) {
          delete val;
          return i;
        }
      }
      m_values.push_back(val);
      return m_values.size() - 1;
    }

    ShValueType valueType() {
      if(type == NODE) {
        return node->valueType();
      } 
      return destValueType;
    }

    static Value* get(ValueNum n)
    {
      return m_values[n];
    }

    static void dump(std::ostream& out);

    std::string name() const
    {
      if(type == NODE) return node->name();
      else return ""; 
    }

  private:
    Value(const ShVariableNodePtr& node)
      : type(NODE), node(node)
    {
    }

    Value(ConstProp* cp)
      : type(STMT), node(0), op(cp->stmt->op), destsize(cp->stmt->dest.size()), destValueType(cp->stmt->dest.valueType())
    {
      for (int i = 0; i < opInfo[cp->stmt->op].arity; i++) {
        for (std::size_t j = 0; j < cp->src[i].size(); j++) {
          if (cp->src[i][j].state == Cell::UNIFORM) {
            src[i].push_back(cp->src[i][j].uniform);
          } else {
            SH_DEBUG_ASSERT(cp->src[i][j].state == Cell::CONSTANT); // @todo type should be fixed
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

    // @todo comments added, but may not be correct
   
    // Construct a CONSTANT Cell with non-null value
    // or a TOP/BOTTOM
    Cell(State state, ShVariantPtr value = 0)
      : state(state)
    {
      if(value) this->value = value->get(); 
      SH_DEBUG_ASSERT(this->value || (state != CONSTANT));
    }

    // Construct a UNIFORM cell from a variable
    Cell(State state, const ShVariable& var, int index) 
      : state(state), value(0),
        uniform(Value::lookup(var.node()), var.swizzle()[index], var.neg())
    {
    }

    // Construct a UNIFORM cell as a result of some
    // statement where all relevant sources are uniforms/constants.
    Cell(State state, ConstProp* cp, int index) 
      : state(state), value(0),
        uniform(Value::lookup(cp), index, false)
    {
    }

    bool operator==(const Cell& other) const
    {
      if(state != other.state) return false;
      if(value) return value->equals(other.value);
      // @todo type remove debug
      SH_DEBUG_ASSERT(!value);
      return value == other.value; // null
    }

    bool operator!=(const Cell& other) const
    {
      return !(*this == other);
    }
    
    State state;
    ShVariantPtr value; // Only for state == CONSTANT

    Uniform uniform; // Only for state == UNIFORM
  };

  // If dest[index] != cell, sets dest[index] to cell and updates the worklist 
  // Caller must ensure that cell does not move up the lattice. 
  void setDest(int index, const Cell &cell, ConstWorkList &worklist)
  {
    if(dest[index] == cell) return; 
    dest[index] = cell;
    worklist.push(ValueTracking::Def(stmt, index));
  }

  ShStatement* stmt;
  std::vector<Cell> dest;
  std::vector<Cell> src[3];
};

ShInfo* ConstProp::clone() const
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
      SH_DEBUG_ASSERT(a.value); // @todo type debugging
      if (a.value->equals(b.value)) {
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
    SH_DEBUG_ASSERT(uniform.constval); // @todo type DEBUGGING
    out << uniform.constval->encode();
  } else {
    if (uniform.neg) out << '-';
    out << "v" << uniform.valuenum << "[" << uniform.index << "]";
  }
  return out;
}

void ConstProp::Value::dump(std::ostream& out)
{
  out << "--- uniform values ---" << std::endl;
  for (std::size_t i = 0; i < m_values.size(); i++) {
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
    out << "[" << cell.value->encode() << "]";
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
  InitConstProp(const ShProgramNodeCPtr& prog, ConstWorkList& worklist)
    : prog(prog), worklist(worklist)
  {
  }

  // assignment operator could not be generated: declaration only
  InitConstProp& operator=(InitConstProp const&);

  void operator()(ShCtrlGraphNode* node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      I->destroy_info<ConstProp>();
      ConstProp* cp = new ConstProp(&(*I), prog, worklist);
      I->add_info(cp);
    }
  }

  ShProgramNodeCPtr prog;
  ConstWorkList& worklist;
};

struct DumpConstProp {
  void operator()(ShCtrlGraphNode* node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      std::cerr << "{" << *I << "} --- ";
      ConstProp* cp = I->get_info<ConstProp>();

      if (!cp) {
        std::cerr << "NO CP INFORMATION" << std::endl;
        continue;
      }

      std::cerr << "dest = {";
      for (std::size_t i = 0; i < cp->dest.size(); i++) {
        std::cerr << cp->dest[i];
      }
      std::cerr << "}; ";
      for (int s = 0; s < opInfo[I->op].arity; s++) {
        if (s) std::cerr << ", ";
        std::cerr << "src" << s << " = {";
        for (std::size_t i = 0; i < cp->src[s].size(); i++) {
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
  
  void operator()(ShCtrlGraphNode* node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      ConstProp* cp = I->get_info<ConstProp>();

      if (!cp) continue;

      if (!cp->dest.empty()) {
        // if all dest fields are constants, replace this with a
        // constant assignment

        if (I->op != SH_OP_ASN || I->src[0].node()->kind() != SH_CONST) {
          bool allconst = true;
          for (int i = 0; i < I->dest.size(); i++) {
            if (cp->dest[i].state != ConstProp::Cell::CONSTANT) {
              allconst = false;
              break;
            }
          }
          if (allconst) {
            SH_DEBUG_ASSERT(cp->dest[0].value); // @todo type debugging
            ShValueType destValueType = cp->dest[0].value->valueType(); 
            ShVariable newconst(new ShVariableNode(SH_CONST, I->dest.size(), destValueType));
            for(int i = 0; i < I->dest.size(); ++i) {
              newconst.setVariant(cp->dest[i].value, i);
            }
#ifdef SH_DEBUG_CONSTPROP
            SH_DEBUG_PRINT("Replaced {" << *I << "} with " << newconst);
#endif
            *I = ShStatement(I->dest, SH_OP_ASN, newconst);
          } else {
            // otherwise, do the same for each source field.
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              if (I->src[s].node()->kind() == SH_CONST) continue;
            
              ShValueType srcValueType = I->src[s].valueType();
              ShVariable newconst(new ShVariableNode(SH_CONST, I->src[s].size(), srcValueType));
              bool allconst = true;
              for (int i = 0; i < I->src[s].size(); i++) {
                if (cp->src[s][i].state != ConstProp::Cell::CONSTANT) {
                  allconst = false;
                  break;
                }
                newconst.setVariant(cp->src[s][i].value, i);
              }
              if (allconst) {
#ifdef SH_DEBUG_CONSTPROP
                SH_DEBUG_PRINT("Replaced {" << *I << "}.src[" << s << "] with " << newconst);
#endif
                I->src[s] = newconst;
              }
            }
          }

          if (!lift_uniforms || allconst) {
            //SH_DEBUG_PRINT("Skipping uniform lifting");
            continue;
          }

          bool alluniform = true;
          for (int s = 0; s < opInfo[I->op].arity; s++) {
            for (int i = 0; i < I->src[s].size(); i++) {
              if (cp->src[s][i].state != ConstProp::Cell::UNIFORM
                  && cp->src[s][i].state != ConstProp::Cell::CONSTANT) {
                alluniform = false;
                break;
              }
            }
            if (!alluniform) break;
          }
          if (!alluniform || I->dest.node()->kind() == SH_OUTPUT
              || I->dest.node()->kind() == SH_INOUT) {
#ifdef SH_DEBUG_CONSTPROP
            SH_DEBUG_PRINT("Considering " << *I << " for uniform lifting");
#endif          
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              if (I->src[s].uniform()) {
#ifdef SH_DEBUG_CONSTPROP
                SH_DEBUG_PRINT(*I << ".src[" << s << "] is already a uniform");
#endif
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
                } else {
                  // Can't lift this, unless we introduce intermediate instructions.
                  mixed = true;
                }
              }
              
              if (uniform < 0) {
#ifdef SH_DEBUG_CONSTPROP
                SH_DEBUG_PRINT("{" << *I << "}.src[" << s << "] is not uniform");
#endif
                continue;
              }
              if (mixed) {
#ifdef SH_DEBUG_CONSTPROP
                SH_DEBUG_PRINT("{" << *I << "}.src[" << s << "] is mixed");
#endif
                continue;
              }
              ConstProp::Value* value = ConstProp::Value::get(uniform);


#ifdef SH_DEBUG_CONSTPROP
              SH_DEBUG_PRINT("Lifting {" << *I << "}.src[" << s << "]: " << uniform);
#endif
              int srcsize;
              if (value->type == ConstProp::Value::NODE) {
                srcsize = value->node->size();
              } else {
                srcsize = value->destsize;
              }
              ShSwizzle swizzle(srcsize, indices.size(), &(*indices.begin()));
              // Build a uniform to represent this computation.
              ShVariableNodePtr node = build_uniform(value, uniform);
              if (node) {
                I->src[s] = ShVariable(node, swizzle, neg);
              } else {
#ifdef SH_DEBUG_CONSTPROP
                SH_DEBUG_PRINT("Could not lift " << *I << ".src[" << s << "] for some reason");
#endif
              }
            }
          }
        }
      }
    }

    // Clean up
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      // Remove constant propagation information.
      I->destroy_info<ConstProp>();
    }
  }

  ShVariableNodePtr build_uniform(ConstProp::Value* value,
                                  ConstProp::ValueNum valuenum)
  {
    if (value->type == ConstProp::Value::NODE) {
      return value->node;
    }
    
    ShContext::current()->enter(0);
    ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize, value->destValueType);
    {
    std::ostringstream s;
    s << "dep_" << valuenum << "_" << value->name();
    node->name(s.str());
    }
    ShContext::current()->exit();

#ifdef SH_DEBUG_CONSTPROP
    SH_DEBUG_PRINT("Lifting value #" << valuenum);
#endif

    bool broken = false;
    
    ShProgram prg = SH_BEGIN_PROGRAM("uniform") {
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
        if (stmt.src[i].null()) broken = true;
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    } SH_END;

#ifdef SH_DEBUG_CONSTPROP
    {
      std::ostringstream s;
      s << "lifted_" << valuenum;
      std::string dotfilename(s.str() + ".dot");
      std::ofstream dot(dotfilename.c_str());
      prg.node()->ctrlGraph->graphviz_dump(dot);
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
    std::vector<ShVariantCPtr> constvals;
    for (std::size_t i = 0; i < src.size(); i++) {
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
      // Make intermediate variables, combine them together.
      ShVariable r = ShVariable(new ShVariableNode(SH_TEMP, src.size(), src[0].valueType()));
      
      for (std::size_t i = 0; i < src.size(); i++) {
        std::vector<ConstProp::Uniform> v;
        v.push_back(src[i]);
        ShVariable scalar = compute(v);
        ShStatement stmt(r(i), SH_OP_ASN, scalar);
        ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      }
      return r;
    }

    if (!constvals.empty()) {
      ShVariable var(new ShVariableNode(SH_CONST, constvals.size(), constvals[0]->valueType()));
      for(std::size_t i = 0; i < constvals.size(); ++i) var.setVariant(constvals[i], i);
      return var;
    }
    
    ConstProp::Value* value = ConstProp::Value::get(v);
    
    if (value->type == ConstProp::Value::NODE) {
      ShSwizzle swizzle(value->node->size(), indices.size(), &*indices.begin());
      return ShVariable(value->node, swizzle, neg);
    }
    if (value->type == ConstProp::Value::STMT) {
      ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize, value->destValueType);
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
        if (stmt.src[i].null()) return ShVariable();
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      ShSwizzle swizzle(node->size(), indices.size(), &*indices.begin());
      return ShVariable(node, swizzle, neg);
    }

#ifdef SH_DEBUG_CONSTPROP
    SH_DEBUG_PRINT("Reached invalid point");
#endif

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
  
  InitConstProp init(p.node(), worklist);
  graph->dfs(init);

#ifdef SH_DEBUG_CONSTPROP
  SH_DEBUG_PRINT("Const Prop Initial Values:");
  DumpConstProp dump_pre;
  graph->dfs(dump_pre);
#endif

  
  while (!worklist.empty()) {
    ValueTracking::Def def = worklist.front(); worklist.pop();
    ValueTracking* vt = def.stmt->get_info<ValueTracking>();
    if (!vt) {
#ifdef SH_DEBUG_CONSTPROP
      SH_DEBUG_PRINT(*def.stmt << " on worklist does not have VT information?");
#endif
      continue;
    }


    for (ValueTracking::DefUseChain::iterator use = vt->uses[def.index].begin();
         use != vt->uses[def.index].end(); ++use) {
      if (use->kind != ValueTracking::Use::STMT) continue;
      ConstProp* cp = use->stmt->get_info<ConstProp>();
      if (!cp) {
#ifdef SH_DEBUG_CONSTPROP
        SH_DEBUG_PRINT("Use " << *use->stmt << " does not have const prop information!");
#endif
        continue;
      }

      ConstProp::Cell cell = cp->src[use->source][use->index];

      ValueTracking* ut = use->stmt->get_info<ValueTracking>();
      if (!ut) {
        // Should never happen...
#ifdef SH_DEBUG_CONSTPROP
        SH_DEBUG_PRINT("Use " << *use->stmt << " on worklist does not have VT information?");
#endif
        continue;
      }

#ifdef SH_DEBUG_CONSTPROP
      SH_DEBUG_PRINT("Meeting cell for {" << *use->stmt
                     << "}.src" << use->source << "[" << use->index << "]");
#endif
      
      ConstProp::Cell new_cell(ConstProp::Cell::TOP);
      
      for (ValueTracking::UseDefChain::iterator possdef
             = ut->defs[use->source][use->index].begin();
           possdef != ut->defs[use->source][use->index].end(); ++possdef) {
        ConstProp* dcp = possdef->stmt->get_info<ConstProp>();
        if (!dcp) {
#ifdef SH_DEBUG_CONSTPROP
          SH_DEBUG_PRINT("Possible def " << *dcp->stmt << " on worklist does not have CP information?");
#endif
          continue;
        }
        
        ConstProp::Cell destcell = dcp->dest[possdef->index];

        // If the use is negated, we need to change the cell
        if (use->stmt->src[use->source].neg()) {
          if (destcell.state == ConstProp::Cell::CONSTANT) {
            SH_DEBUG_ASSERT(destcell.value); // @todo type DEBUGGING
            destcell.value->negate();
          } else if (destcell.state == ConstProp::Cell::UNIFORM) {
            destcell.uniform.neg = !destcell.uniform.neg;
          }
        }
#ifdef SH_DEBUG_CONSTPROP
        SH_DEBUG_PRINT("  meet(" << new_cell << ", " << destcell << ") = " <<
                       meet(new_cell, destcell));
#endif
        new_cell = meet(new_cell, destcell);
      }
      
      if (cell != new_cell) {
#ifdef SH_DEBUG_CONSTPROP
        SH_DEBUG_PRINT("  ...replacing cell");
#endif
        cp->src[use->source][use->index] = new_cell;
        cp->updateDest(worklist);
      }
    }
  }
  // Now do something with our glorious newfound information.

  
#ifdef SH_DEBUG_CONSTPROP
  ConstProp::Value::dump(std::cerr);

  DumpConstProp dump;
  graph->dfs(dump);
#endif
  
  FinishConstProp finish(p.node()->target().find("gpu:") == 0
                         && !ShContext::current()->optimization_disabled("uniform lifting"));
  graph->dfs(finish);
  
}

}
