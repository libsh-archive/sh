#include <sh/sh.hpp>

using namespace SH;


class Texture3D{
public:

 Texture3D(ShBaseTexture2D<ShColor4f>* ptex2d, unsigned int sqrns/*number of slices in one row, total number of slices would be ns^2*/){
    ptexture = ptex2d;	
    sqrnumslices = sqrns;
    ss = sqrnumslices*sqrnumslices; // real number of slices
    rss = 1.0/ss; // reciprocal of ss
    res3d = ss;
    fo = 1.0/(sqrnumslices*res3d);

    
 }
 ShColor4f operator() (ShVector3f tc3d){
     ShTexCoord2f tc, tc2;
     ShAttrib1f a =  (tc3d(2)/(2.0)+0.5)/rss ;
     ShAttrib1f a1 = floor( (tc3d(2)/(2.0)+0.5)/rss );
     ShAttrib1f a2 = a1+1.0;

     ShAttrib1f lx = tc3d(0)/sqrnumslices;
     ShAttrib1f ly = tc3d(1)/sqrnumslices;

     ShAttrib1f r2n = 2.0/sqrnumslices;

     ShAttrib1f b1 = -(sqrnumslices-1.0)/2;

     tc  = ShTexCoord2f((lx + r2n*( b1 +        a1%sqrnumslices  ) )/2+ShAttrib1f(0.5),
     			(ly + r2n*( b1 +  floor(a1/sqrnumslices) ) )/2+ShAttrib1f(0.5));
   
     /*tc2 = ShTexCoord2f((lx + r2n*( b1 +        a2%sqrnumslices  ) )/2+ShAttrib1f(0.5),
     			(ly + r2n*( b1 +  floor(a2/sqrnumslices) ) )/2+ShAttrib1f(0.5));
   
   
     
     return lerp(  a-a1  ,(*ptexture)(tc2), (*ptexture)(tc));*/
     return (*ptexture)(tc);
  } 

 ShPoint3f scatter(ShPoint3f pos, ShAttrib1f scale){
      
    return ShPoint3f(pos(0)/(sqrnumslices*scale) + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor(       (pos(2)/(2.0*scale)+0.5) * ss)%sqrnumslices),
        	     pos(1)/(sqrnumslices*scale) + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor((pos(2)/(2.0*scale)+0.5) * ss)/sqrnumslices)),
    	ShAttrib1f(0.0));
 }

 ShTexCoord2f get2DTexCoord(ShPoint3f tc3d){
 
	 return ShTexCoord2f((tc3d(0)/sqrnumslices + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * ss)%sqrnumslices)) )/2+ShAttrib1f(0.5),
     			     (tc3d(1)/sqrnumslices + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * ss)/sqrnumslices)) )/2+ShAttrib1f(0.5));

 }

// find neighbors:
ShColor4f find10(ShTexCoord2f tc){ // (0,1,0)
 tc(1) -= fo; 	
 return (*ptexture)(tc);
} 
ShColor4f find12(ShTexCoord2f tc){ // (-1,0,0)
 tc(0) -= fo; 	
 return (*ptexture)(tc);
}
ShColor4f find13(ShTexCoord2f tc){ // (1,0,0)
 tc(0) += fo; 	
 return (*ptexture)(tc);
} 
ShColor4f find15(ShTexCoord2f tc){ // (0,-1,0)
 tc(1) += fo; 	
 return (*ptexture)(tc);
}

// data	 
 ShBaseTexture2D<ShColor4f>* ptexture;
 unsigned int sqrnumslices;
 //unsigned int res2d; // size of a 2d dimension
 unsigned int ss;
 unsigned int rss;
 unsigned int res3d;
 float fo; // fragment offset

 };//class

