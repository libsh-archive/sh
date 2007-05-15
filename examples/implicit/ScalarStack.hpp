#ifndef SCALARSTACK
#define SCALARSTACK

#include <sh/sh.hpp>

using namespace SH;

template<int N, typename T>
struct ScalarStack {
  Attrib<N, SH_TEMP, T> m_stack;
  Attrib1f m_count;
  int *incSwiz; // swizzle 1, 2, 3, .., N, 0 - used for both pop/push

  ScalarStack() 
    : m_count(0.0)
  {
    m_stack.name("stack");
    m_count.name("stack_count");
    incSwiz = new int[N];
  }

  void buildIncSwiz(int offset) {
    for(int i = 0; i < N; ++i) incSwiz[i] = (i+offset) % N;
  }
  
  ~ScalarStack() {
    delete[] incSwiz;
  }

  template<typename T2>
  void push(const Generic<1, T2> &value) {
    m_count += 1;
    buildIncSwiz(1);
    m_stack.template swiz<N>(incSwiz) = m_stack;
    m_stack(0) = value;
  }

  template<typename T2>
  void push(const Generic<1, T2> &v1, const Generic<1, T2> &v2) {
    m_count += 2;
    buildIncSwiz(2);
    m_stack.template swiz<N>(incSwiz) = m_stack;
    m_stack(0) = v1;
    m_stack(1) = v2;
  }

  template<typename T2>
  void push(const Generic<1, T2> &v1, const Generic<1, T2> &v2, const Generic<1, T2> &v3, const Generic<1, T2> &v4) { 
    m_count += 4;
    buildIncSwiz(4);
    m_stack.template swiz<N>(incSwiz) = m_stack;
    m_stack(0) = v1;
    m_stack(1) = v2;
    m_stack(2) = v3;
    m_stack(3) = v4;
  }

  void pop(int offset=1) {
    m_count -= offset;
    buildIncSwiz(offset);
    m_stack = m_stack.template swiz<N>(incSwiz);
  }

  void clear() {
    m_count = 0;
  }

  // stack must not be empty
  template<typename T2>
  void settop(const Generic<1, T2> &value) {
    m_stack(0) = value;
  }

  Generic<1, T> top() {
    return m_stack(0);
  }


  Generic<1, T> operator[](int i) {
    return m_stack[i];
  }

  Generic<1, float> full() {
    return m_count >= N; 
  }

  Generic<1, float> empty() {
    return m_count <= 0;
  }

  Generic<1, float> count() {
    return m_count;
  }
};

#endif
