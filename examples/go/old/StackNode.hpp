#ifndef STACKNODE_HPP
#define STACKNODE_HPP

#include <wx/defs.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/dcclient.h>
#include "man.hpp"

/* A node in the stack for composing parametric functions */

class StackNode {
  virtual ~StackNode() {}

  /* Renders the stack node.
   * Node sets up its own transformation and shaders */
  virtual void render() = 0;

  /* Returns the current state of this node.
   * Each node is responsible for maintaining a monotonically increasing
   * state, updating it each time it has changed 
   *
   * (This is only used for handling of parametric functions that are 
   * computed partly on the host, as they may need to redo the host computation
   * when one of the subfunctions has updated) */
  virtual int state() = 0;

  /* Reacts to mouse motion.
   *
   * This is called when this node has been selected, and it may optionally
   * react to mouse input */
  virtual void onMotion(const wxMouseEvent& e) {}
};

/* Nodes for ManStackNodes and HostManStackNodes */

class ManStackNode: public StackNode {
  /* Built initially using 
  template<int I, int O>
  ManStackNode
  void render();
  int state();
};

class HostManStackNode: public StackNode { 
};

#endif
