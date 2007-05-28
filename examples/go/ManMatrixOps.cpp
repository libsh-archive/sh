#include "ManMatrixOps.hpp"

using namespace std;
using namespace SH;


ManMatrix m_scale(const Man& s) { 
  assert(s.output_matches(3));
  Man sf = s.repeat(3);
  Program p = SH_BEGIN_PROGRAM() {
    Variable in = s.outsize_var(SH_INPUT);
    ManMatrix::OutType out;
    Attrib3f temp_s;
    shASN(temp_s, in);
    out = scale(temp_s);
  } SH_END;
  return p << s; 
}

ManMatrix m_translate(const Man& t) { 
  assert(t.output_matches(3));
  Man tf = t.repeat(3);
  Program p = SH_BEGIN_PROGRAM() {
    Variable in = t.outsize_var(SH_INPUT);
    ManMatrix::OutType out;
    Attrib3f temp_t;
    shASN(temp_t, in);
    out = translate(temp_t);
  } SH_END;
  return p << t; 
}

ManMatrix m_rotate(const Man& axis, const Man& angle) {
  assert(axis.output_matches(3));
  assert(angle.output_matches(1));
  Man axisf = m_resize_inputs(axis, angle).fill(3);
  Man anglef = m_resize_inputs(angle, axis).fill(1); 
  Program p = SH_BEGIN_PROGRAM() {
    Variable in_axis = axisf.outsize_var(SH_INPUT);
    Variable in_angle = anglef.outsize_var(SH_INPUT);
    ManMatrix::OutType out;

    Vector3f temp_axis; 
    shASN(temp_axis, in_axis);
    Attrib1f temp_angle; 
    shASN(temp_angle, in_angle);
    out = rotate(temp_axis, temp_angle); 
  } SH_END;
  Program result = p << (axisf & anglef); 
  if(axisf.input_size() > 0) return result << dup(); 
  return result;
}

ManMatrix mul(const ManMatrix& a, const ManMatrix& b) {
  int maxIn = max(a.input_size(), b.input_size());
  ManMatrix af = a.resize_inputs(maxIn);
  ManMatrix bf = b.resize_inputs(maxIn);
  Program p = SH_BEGIN_PROGRAM() {
    ManMatrix::OutType::InputType ma, mb;
    ManMatrix::OutType result; 
    result = ma | mb;
  } SH_END;
  Program result = p << (af & bf); 
  if(af.input_size() > 0) return result << dup(); 
  return result;
}
