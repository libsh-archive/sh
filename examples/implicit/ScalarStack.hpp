#ifndef SCALARSTACK
#define SCALARSTACK

#include <sh/sh.hpp>

using namespace SH;

template<int N, typename T>
struct ScalarStack {
  ShAttrib<N, SH_TEMP, T> m_stack;
  ShAttrib1f m_count;
  int *incSwiz; // swizzle 1, 2, 3, .., N, 0 - used for both pop/push

  ScalarStack() 
    : m_count(0.0)
  {
    m_stack.name("stack");
    m_count.name("stack_count");
    incSwiz = new int[N];
    for(int i = 0; i < N; ++i) incSwiz[i] = (i+1) % N;
  }

  template<typename T2>
  void push(const ShGeneric<1, T2> &value) {
    m_count += 1;
    m_stack.template swiz<N>(incSwiz) = m_stack;
    m_stack(0) = value;
  }

  void pop() {
    m_count -= 1;
    m_stack = m_stack.template swiz<N>(incSwiz);
  }

  // stack must not be empty
  template<typename T2>
  void settop(const ShGeneric<1, T2> &value) {
    m_stack(0) = value;
  }

  ShGeneric<1, T> top() {
    return m_stack(0);
  }


  ShGeneric<1, T> operator[](int i) {
    return m_stack[i];
  }

  ShGeneric<1, float> full() {
    return m_count >= N; 
  }

  ShGeneric<1, float> empty() {
    return m_count <= 0;
  }

  ShGeneric<1, float> count() {
    return m_count;
  }
};

#endif
