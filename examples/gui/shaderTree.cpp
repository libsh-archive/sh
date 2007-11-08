#include <iostream>

#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#undef GL_GLEXT_PROTOTYPES
#include <GL/glu.h>
#include <sh/ShInclusion.hpp>
#include "shaderTree.hpp"

using namespace SH;
using namespace ShUtil;

ShaderColumns::ShaderColumns()
{
  add(idx);
  add(stmt);
  add(label);
  add(error);
  add(remove);
}

ShaderTree::ShaderTree(Scanner& scanner)
  : m_handler_depth(0), 
    m_treeModel(Gtk::TreeStore::create(m_columns)),
    m_scanner(scanner)
{
  set_model(m_treeModel);

  append_column("Index", m_columns.idx); 
  append_column("Label", m_columns.label); 
  append_column("Error", m_columns.error); 
  append_column_editable("Remove", m_columns.remove);

  m_treeModel->signal_row_changed().connect(
      SigC::slot(*this, &ShaderTree::on_remove_toggled));
}

ShaderTree::~ShaderTree()
{
  // Nothing to do here right now.
}

void ShaderTree::update()
{
  // build section tree and dump it
  m_treeModel->clear();
  ShSectionTree& stree = m_scanner.getSectionTree();

  Gtk::TreeModel::Row row = *m_treeModel->append();
  std::string fshName = m_scanner.fsh().name();
  row[m_columns.idx] = -2; // @todo remove special weird values
  row[m_columns.label] = fshName == "" ? "Main Program" :  fshName;

  insertSection(row, stree.root);

  expand_all();
  show_all_children();
}

void ShaderTree::insertSection(Gtk::TreeModel::Row& row, 
    ShSectionNodePtr sectionNode)
{
  for(ShSectionNode::cfg_iterator C = sectionNode->cfgBegin();
      C != sectionNode->cfgEnd(); ++C) {
    if(!(*C)->block) continue;

    ShBasicBlockPtr block = (*C)->block;
    for(ShBasicBlock::iterator B = block->begin(); B != block->end(); ++B) {
      // ignore STARTSEC/ENDSEC
      if(B->op == SH_OP_STARTSEC || B->op == SH_OP_ENDSEC) {
        continue;
      }

      Gtk::TreeModel::Row newRow = *m_treeModel->append(row.children());  

      ShStmtIndex* idx = B->get_info<ShStmtIndex>(); 
      if(idx) {
        newRow[m_columns.idx] = idx->index(); 
        newRow[m_columns.error] = m_scanner.getError(*B);
      } else {
        newRow[m_columns.idx] = -1;
        newRow[m_columns.error] = 1313.0;
      }

      newRow[m_columns.stmt] = &*B;

      std::ostringstream out;
      out << *B;
      newRow[m_columns.label] = out.str();

      // fill in the other columns
      newRow[m_columns.remove] = m_scanner.getState(*B);
    }
  }

  for(ShSectionNode::iterator S = sectionNode->begin();
      S != sectionNode->end(); ++S) {
    Gtk::TreeModel::Row sectionRow = *m_treeModel->append(row.children());
    sectionRow[m_columns.label] = (*S)->name();
    sectionRow[m_columns.idx] = -2; // @todo don't use stupid untraceable constants...
    sectionRow[m_columns.stmt] = (*S)->getStart(); 
    insertSection(sectionRow, *S);
  }
}

void ShaderTree::on_remove_toggled(const Gtk::TreeModel::Path& path, 
                                   const Gtk::TreeModel::iterator& row) {
  std::cout << "remove toggled on row " << path.to_string() << std::endl;

  bool remove = (*row).get_value(m_columns.remove);

  if(m_handler_depth == 0) {
    m_changed = false;
  }

  ShStatement* stmt = (*row)[m_columns.stmt]; 
  if(stmt) {
    std::cout << "  checking remove " << remove << " state " << m_scanner.getState(*stmt) << std::endl; 
    if(m_scanner.getState(*stmt) != remove) {
      m_changed = true;
      std::cout << "  setting remove " << remove << " on stmt = " << *stmt << std::endl; 
      m_scanner.setState(*stmt, remove);
    }
  }

  // if the row is for a section, toggle the children to the same value 
  if((*row)[m_columns.idx] == -2) { 
    m_handler_depth++;
    Gtk::TreeNodeChildren::iterator I = (*row).children().begin();
    for(;I != (*row).children().end(); ++I) { 
      I->set_value(m_columns.remove, remove); 
    }
    m_handler_depth--;
  }

  if(m_handler_depth == 0 && m_changed) {
    m_scanner.simplify();
  }
}


