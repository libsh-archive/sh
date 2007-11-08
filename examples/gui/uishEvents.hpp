#ifndef UISH_EVENT_HPP
#define UISH_EVENT_HPP

struct uishMouseEvent {
  guint32 time;
  double x, y;
  guint state;

  uishMouseEvent(guint32 time, double x, double y, guint state)
    : time(time), x(x), y(y), state(state) {}
};

struct uishButtonEvent: public uishMouseEvent {
  guint button;

  uishButtonEvent(guint32 time, double x, double y, guint state, guint button)
    : uishMouseEvent(time, x, y, state), button(button) {} 
};

struct uishResizeEvent {
  double width, height;

  uishResizeEvent(double width, double height)
    : width(width), height(height) {}
};

struct uishToggleEvent {
  bool value;
  uishToggleEvent(bool value)
    : value(value) {}
};

#endif
