#ifndef SHADERTREE_HPP
#define SHADERTREE_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include "scanner.hpp"

class ShaderColumns: public Gtk::TreeModel::ColumnRecord
{
  public:
    ShaderColumns();

  Gtk::TreeModelColumn<long long> idx; 
  Gtk::TreeModelColumn<SH::ShStatement*> stmt;
  Gtk::TreeModelColumn<Glib::ustring> label;
  Gtk::TreeModelColumn<double> error; 
  Gtk::TreeModelColumn<bool> remove; 
};

// A treeview representation of the current scanner/simplification state 
class ShaderTree : public Gtk::TreeView {
public:
  ShaderTree(Scanner& scanner);
  virtual ~ShaderTree();

  void update();
  
protected:

  // inserts the section into the treemodel row
  void insertSection(Gtk::TreeModel::Row& row,
             SH::ShSectionNodePtr sectionNode);

  /* handles toggling of the remove event for a given row */
  void on_remove_toggled(const Gtk::TreeModel::Path& path, 
                         const Gtk::TreeModel::iterator& row);
  int m_handler_depth;
  bool m_changed;

  ShaderColumns m_columns;
  Glib::RefPtr<Gtk::TreeStore> m_treeModel; 

  Scanner& m_scanner;
};

#endif
