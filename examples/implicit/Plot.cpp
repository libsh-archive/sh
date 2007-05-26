#include <iostream>
#include <fstream>
#include <string>
#include <sh/sh.hpp>
#include <shutil/shutil.hpp>
#include "ColorFinder.hpp"
#include "Plot.hpp"

using namespace std;
using namespace SH;
using namespace ShUtil;


/* Returns a fsh that plots a 2D function and the IA/AA evaluation at a given range */  
Color3f plot(Program func, const Attrib2f& texcoord, const Attrib1i_f &input_range, const Attrib1f& scale, const Attrib2f& offset) {
  comment("Begin plot");
  ConstColor3f aaColor(0.9, 0.6, 0.5); 
  ConstColor3f iaColor(0.5, 0.6, 0.8); 
  ConstColor3f bothColor(0.9, 0.5, 0.7); 
  ConstColor3f noColor(0.75, 0.75, 0.75);
  ConstColor3f TufteOrange(1.0f, 0.62f, .035f);
  ConstAttrib1f lineWidth(1 / 256.0);

  Color3f SH_DECL(color); // result

  Program ifunc = inclusion(func);
  Program afunc = affine_inclusion_syms(func);

  Point2f pos = (texcoord - 0.5f) * 2 * scale + offset; 

  Attrib1f SH_DECL(inAxis) = 0.0f; 
  Attrib1f SH_DECL(inCurve) = 0.0f; 
  Attrib1f SH_DECL(inInput) = 0.0f;
  Attrib1f SH_DECL(inRangeIA) = 0.0f;
  Attrib1f SH_DECL(inRangeAA) = 0.0f;
  Attrib1f SH_DECL(inRangeBoth) = 0.0f;

  // check if in curve
  Attrib1f SH_DECL(val) = func(pos(0));  // evaluate function 
  Attrib2f pnormal;
  pnormal(0) = dx(val);
  pnormal(1) = 1.0f;
  pnormal = normalize(pnormal);
  Attrib1f deltay = abs(dy(pos(1))) * 4;
  Attrib1f pfdelta = pos(1) - val; 
  inCurve = abs(pfdelta * pnormal(1)) < scale * lineWidth; 

  inInput = range_lo(range_contains(input_range, pos(0)));

  SH_IF(inInput) {
    // check if in range

    Attrib1a_f SH_DECL(aa_range) = input_range; 
    Attrib1f SH_DECL(center) = range_center(aa_range);

    // check if in affine range
    Attrib1a_f SH_DECL(aa_result) = afunc(aa_range);
    Attrib1f SH_DECL(result_center) = range_center(aa_result);
    Attrib1f SH_DECL(result_inerr) = affine_lasterr(aa_result, aa_range);
    Attrib1f SH_DECL(result_radius) = range_radius(aa_result);
    Attrib1f SH_DECL(result_other) = result_radius - abs(result_inerr);

    Attrib1f errValue = (pos(0) - center) / range_radius(aa_range);
    
    inRangeAA = abs(errValue * result_inerr + result_center - pos(1)) - deltay < result_other;

    // check if in IA range
    Attrib1i_f SH_DECL(ia_result) = ifunc(input_range);
    inRangeIA = range_lo(range_contains(ia_result, pos(1)));
  } SH_ENDIF;

  inRangeBoth = inRangeIA && inRangeAA;

  // check if in grid
  inAxis = (abs(pos(0)) < scale * lineWidth) + (abs(pos(1)) < scale * lineWidth); 
  SH_IF(inInput) {
    inAxis = (abs(pos(0)) < scale * 3 * lineWidth) + (abs(pos(1)) < scale * 3 * lineWidth); 
  } SH_ENDIF;
  Color3f axisColor = lerp(inInput, TufteOrange, ConstColor3f(0.25, 0.25, 0.25));  

  color = lerp(inRangeBoth, bothColor,
          lerp(inRangeAA, aaColor,
          lerp(inRangeIA, iaColor, noColor)));
  color = cond(inCurve, color * 0.75, color);  
  color = cond(inAxis, color * axisColor, color);
  comment("end plot");
  return color;
}

