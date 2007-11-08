#include <dlfcn.h>
#include "appwindow.hpp"

AppWindow::AppWindow()
  : m_scanner(*this), 
    m_shaderTree(m_scanner),
    m_shaderGraph(m_scanner),
    m_viewerOriginal(m_viewerState),
    m_viewerSimple(m_viewerState)
{
  set_title("Foo");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_file.items().push_back(MenuElem("_Open Shader...", 
        SigC::slot(*this, &AppWindow::on_open)));

  m_menu_file.items().push_back(MenuElem("_Quit", SigC::slot(*this, &AppWindow::hide)));


  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_File", m_menu_file));
  
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);
  m_vbox.pack_start(m_hpaned); 


  m_infoNotebook.set_size_request(512, 512);
  m_hpaned.pack1(m_infoNotebook);
    m_infoNotebook.append_page(m_shaderGraph, "Shader Graph");

    m_infoNotebook.append_page(m_shaderTree, "Shader Tree");

  m_viewerOriginal.set_size_request(384, 384);
  m_viewerSimple.set_size_request(384, 384);

  /*
  m_vpaned.pack1(m_viewerOriginal);
  m_vpaned.pack2(m_viewerSimple);
  m_hpaned.pack2(m_vpaned);
  */

  m_viewerBox.pack_start(m_viewerOriginal);
  m_viewerBox.pack_start(m_viewerSimple);
  m_hpaned.pack2(m_viewerBox);

  show_all();
}

void AppWindow::update_simplefsh(SH::ShProgram simple)
{
    m_viewerSimple.setFsh(simple);
}

void AppWindow::on_open() 
{
  /* file open code from gtkmm tutorial */
  Gtk::FileSelection dialog("Please choose a file");
  dialog.set_transient_for(*this);
       
  int result = dialog.run();

  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;

      std::string filename = dialog.get_filename(); //Notice that it is a std::string, not a Glib::ustring.
      load_fsh(filename);
      std::cout << "File selected: " <<  filename << std::endl;
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void AppWindow::load_fsh(const std::string& filename) {
  void* handle = dlopen(filename.c_str(), RTLD_NOW);
  if (handle == NULL) {
    std::cerr << "dlopen failed: " << dlerror() << std::endl;
    return; 
  } else {
    ShaderFunc shader_func = (ShaderFunc)dlsym(handle, "shader");
    if (shader_func == NULL) {
      std::cerr << "dlsym failed: " << dlerror() << std::endl;
      return; 
    }
    try {
      m_fsh = shader_func();
      SH_DEBUG_ASSERT(m_fsh.node());
      m_fsh.node()->dump("foo");

      m_viewerOriginal.setFsh(m_fsh);
      // @todo fix this
      m_viewerSimple.setFsh(m_fsh);

      std::cout << "Setting Scanner fsh" << std::endl;
      m_scanner.setFsh(m_fsh);

      std::cout << "Updating shader tree" << std::endl;
      m_shaderTree.update();

      std::cout << "Updating shader graph" << std::endl;
      m_shaderGraph.update();
    } catch (const SH::ShException& e) {
      std::cerr << "SH Exception " << e.message() << std::endl;
    }
  }
}
