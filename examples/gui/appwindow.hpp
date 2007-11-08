#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <sh/sh.hpp>
#include <gtkmm.h>
#include "scanner.hpp"
#include "shaderTree.hpp"
#include "shaderGraph.hpp"
#include "viewer.hpp"

extern "C" typedef SH::ShProgram (*ShaderFunc)();

class AppWindow : public Gtk::Window {
public:
  AppWindow();

  void update_simplefsh(SH::ShProgram simple);
  
protected:

private:
  void on_open();

  void load_fsh(const std::string& filename);

  SH::ShProgram m_fsh;

  Scanner m_scanner;

  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

    // The menubar, with all the menus at the top of the window
    Gtk::MenuBar m_menubar;

    // Each menu itself
    Gtk::Menu m_menu_file;

    // holds tree view and main OpenGL area
    Gtk::HPaned m_hpaned;

      // The shader information pane 
      Gtk::Notebook m_infoNotebook;

        // Tree view of the shader
        ShaderTree m_shaderTree;

        // Graph view of the shader
        ShaderGraph m_shaderGraph;

      // original and new shader
      //Gtk::VPaned m_vpaned;
        Gtk::VBox m_viewerBox;

        // The main OpenGL area
        ViewerState m_viewerState;
        Viewer m_viewerOriginal;
        Viewer m_viewerSimple;
};

#endif
