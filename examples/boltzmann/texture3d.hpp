#include <sh/sh.hpp>

using namespace SH;


class Texture3D{
public:

 Texture3D(ShBaseTexture2D<ShColor4f>* ptex2d, unsigned int sqrns/*sqroot of number of slices in one row, total number of slices would be ns^2*/){
    ptexture = ptex2d;	
    sqrnumslices = sqrns;
    ss = sqrnumslices*sqrnumslices; // real number of slices
    rss = 1.0/ss; // reciprocal of ss
    res3d = ss;
    //fo = 1.0/(sqrnumslices*res3d);
    fo = 1.0/res3d;
    
 }
 ShColor4f operator() (ShAttrib3f tc3d){
     ShTexCoord2f tc, tc2;
  /*   ShAttrib1f a =  tc3d(2)*ss ;
     ShAttrib1f a1 = floor( a );

     ShAttrib1f lx = tc3d(0)/sqrnumslices;
     ShAttrib1f ly = tc3d(1)/sqrnumslices;

     ShAttrib1f r2n = 1.0/sqrnumslices;

     ShAttrib1f b1 = 0; //-(sqrnumslices-1.0)/2;

     tc  = ShTexCoord2f((lx + r2n*( b1 +  floor(a1%sqrnumslices)  ) ),
     			(ly + r2n*( b1 +  floor(a1/sqrnumslices) ) ));*/
   
     /*tc2 = ShTexCoord2f((lx + r2n*( b1 +        a2%sqrnumslices  ) )/2+ShAttrib1f(0.5),
     			(ly + r2n*( b1 +  floor(a2/sqrnumslices) ) )/2+ShAttrib1f(0.5));
   
     
     
     return lerp(  a-a1  ,(*ptexture)(tc2), (*ptexture)(tc));*/

     tc = get2DTexCoord(tc3d);
     
     return (*ptexture)(tc);
  } 

 ShPoint3f scatter(ShPoint3f pos, ShAttrib1f scale){
      
    return ShPoint3f(pos(0)/(sqrnumslices*scale) + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor(       (pos(2)/(2.0*scale)+0.5) * ss)%sqrnumslices),
        	     pos(1)/(sqrnumslices*scale) + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor((pos(2)/(2.0*scale)+0.5) * ss)/sqrnumslices)),
    	ShAttrib1f(0.0));
 }

/* ShTexCoord2f get2DTexCoord(ShAttrib3f tc3d){
 
	 return ShTexCoord2f((tc3d(0)/sqrnumslices + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * ss)%sqrnumslices)) )/2+ShAttrib1f(0.5),
     			     (tc3d(1)/sqrnumslices + (2.0/sqrnumslices)*(-(sqrnumslices-1.0)/2 + floor( floor( (tc3d(2)/2.0+0.5) * ss)/sqrnumslices)) )/2+ShAttrib1f(0.5));

 }*/
 ShTexCoord2f get2DTexCoord(ShAttrib3f tc3d){
 
	 return ShTexCoord2f((tc3d(0)/sqrnumslices + (1.0/sqrnumslices)*( ( floor( tc3d(2) * ss)%sqrnumslices)) ),
     			     (tc3d(1)/sqrnumslices + (1.0/sqrnumslices)*( floor( floor( tc3d(2) * ss)/sqrnumslices)) ));

 }

 ShAttrib3f get3DTexCoord(ShTexCoord2f tc2d){
 
	 ShAttrib3f res = ShAttrib3f( frac(tc2d(0)*sqrnumslices), frac(tc2d(1)*sqrnumslices), (floor(tc2d(1)*sqrnumslices)*sqrnumslices + floor(tc2d(0)*sqrnumslices))/res3d );
	 return res;
 }

// find neighbors:
ShColor4f find9(ShTexCoord2f tc){ // (-1,1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 	
 tc3d(1) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
} 
ShColor4f find10(ShTexCoord2f tc){ // (0,1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) += fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
} 
ShColor4f find11(ShTexCoord2f tc){ // (1,1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) += fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
} 
ShColor4f find12(ShTexCoord2f tc){ // (-1,0,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find13(ShTexCoord2f tc){ // (1,0,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
} 
ShColor4f find14(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(1) -= fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find15(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) -= fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find16(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) -= fo; 	
 return (*ptexture)(get2DTexCoord(tc3d));
}

ShColor4f find0(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(1) += fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find1(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) += fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find2(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) += fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find3(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find4(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find5(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find6(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(1) -= fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find7(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) -= fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find8(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) -= fo; 	
 tc3d(2) -= fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}

ShColor4f find17(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(1) += fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find18(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) += fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find19(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) += fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find20(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find21(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find22(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find23(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) -= fo; 
 tc3d(1) -= fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find24(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(1) -= fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
}
ShColor4f find25(ShTexCoord2f tc){ // (0,-1,0)
 ShAttrib3f tc3d = get3DTexCoord(tc);	
 tc3d(0) += fo; 
 tc3d(1) -= fo; 	
 tc3d(2) += fo;
 return (*ptexture)(get2DTexCoord(tc3d));
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

