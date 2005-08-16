#include <sh/sh.hpp>

using namespace SH;


class Texture3D{
public:

 Texture3D(ShBaseTexture2D<ShColor4f>* ptex2d, unsigned int sqrns/*sqroot of number of slices in one row, total number of slices would be ns^2*/){
    ptexture = ptex2d;	
    sqrnumslices = sqrns;
    ss = sqrnumslices*sqrnumslices; // real number of slices
    rs = 1.0/sqrnumslices;
    rss = 1.0/ss; // reciprocal of ss
    res3d = ss;
    //fo = 1.0/(sqrnumslices*res3d);
    fo = 1.0/res3d;
    
 }
 ShColor4f operator() (ShAttrib3f tc3d){
     ShTexCoord2f tc;
 
     /*ShAttrib1f xlerp, ylerp, zlerp;
     xlerp = frac(tc3d(0)*ss);
     ylerp = frac(tc3d(1)*ss);
     zlerp = frac(tc3d(1)*ss);*/

     ShAttrib3f tc3d0, tc3d1, tc3d2, tc3d3, tc3d4, tc3d5, tc3d6, tc3d7;
     ShAttrib1f tx1, tx2, ty1, ty2, tz1, tz2;

     //tx1 = tc3d(0)-xlerp*rss;
     tx1 = floor(tc3d(0)*ss)*rss;
     tx2 = tx1+rss;
     //ty1 = tc3d(1)-ylerp*rss;
     ty1 = floor(tc3d(1)*ss)*rss;
     ty2 = ty1+rss;
     //tz1 = tc3d(2)-zlerp*rss;
     tz1 = floor(tc3d(2)*ss)*rss;
     tz2 = tz1+rss;

     tc3d0 = ShAttrib3f(tx1, ty2, tz1);
     tc3d1 = ShAttrib3f(tx2, ty2, tz1);
     tc3d2 = ShAttrib3f(tx1, ty1, tz1);
     tc3d3 = ShAttrib3f(tx2, ty1, tz1);

     tc3d4 = ShAttrib3f(tx1, ty2, tz2);
     tc3d5 = ShAttrib3f(tx2, ty2, tz2);
     tc3d6 = ShAttrib3f(tx1, ty1, tz2);
     tc3d7 = ShAttrib3f(tx2, ty1, tz2);
    
     
     ShColor4f c0, c1, c2, c3, c4, c5, c6, c7, c01, c23, c45, c67, c01_23, c45_67, c_final;
     c0 = (*ptexture)(get2DTexCoord(tc3d0));
     c1 = (*ptexture)(get2DTexCoord(tc3d1));
     c2 = (*ptexture)(get2DTexCoord(tc3d2));
     c3 = (*ptexture)(get2DTexCoord(tc3d3));
     c4 = (*ptexture)(get2DTexCoord(tc3d4));
     c5 = (*ptexture)(get2DTexCoord(tc3d5));
     c6 = (*ptexture)(get2DTexCoord(tc3d6));
     c7 = (*ptexture)(get2DTexCoord(tc3d7));

     ShAttrib1f xlerp = (tc3d(0)-tx1)*ss;
     
     c01 = lerp(ShAttrib4f(xlerp, xlerp, xlerp, xlerp), c1, c0);
     c23 = lerp(ShAttrib4f(xlerp, xlerp, xlerp, xlerp), c3, c2);
     c45 = lerp(ShAttrib4f(xlerp, xlerp, xlerp, xlerp), c5, c4);
     c67 = lerp(ShAttrib4f(xlerp, xlerp, xlerp, xlerp), c7, c6);
      
     ShAttrib1f ylerp = (tc3d(1)-ty1)*ss;
      
     c01_23 = lerp(ShAttrib4f(ylerp, ylerp, ylerp, ylerp), c23, c01);
     c45_67 = lerp(ShAttrib4f(ylerp, ylerp, ylerp, ylerp), c45, c67);
       
     ShAttrib1f zlerp = (tc3d(2)-tz1)*ss;
 
     c_final = lerp(ShAttrib4f(zlerp, zlerp, zlerp, zlerp), c45_67, c01_23);
  
     return c_final;

     /*tc = get2DTexCoord(tc3d);
     return (*ptexture)(tc);*/
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
 
	 return ShTexCoord2f((tc3d(0)/sqrnumslices + rs*( floor( floor( tc3d(2) * ss)%sqrnumslices)) ),
     			     (tc3d(1)/sqrnumslices + rs*( floor( floor( tc3d(2) * ss)/sqrnumslices)) ));

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
 float rs;
 unsigned int ss;
 float rss;
 unsigned int res3d;
 float fo; // fragment offset

 };//class

