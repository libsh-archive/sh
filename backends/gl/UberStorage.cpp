#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
//#undef GL_GLEXT_PROTOTYPES
//
#include "glati.h"
#include "extensions.h"
#include "UberStorage.hpp"

#include "GlBackend.hpp"
#include "ShDebug.hpp"

namespace shgl {

UberUberTransfer* UberUberTransfer::instance = new UberUberTransfer();
UberHostTransfer* UberHostTransfer::instance = new UberHostTransfer();
HostUberTransfer* HostUberTransfer::instance = new HostUberTransfer();


// this should be rep[lace by a correct onbe
#define CHECK_GL_ERROR()  

int UberStorage::temp_fb[4] = {-1, -1, -1, -1};

UberStorage::UberStorage(SH::ShMemory* memory, 
			     int width, int height, int pitch)
  : SH::ShStorage(memory),
    m_width(width), m_height(height), m_pitch(pitch), 
    m_mem(0), m_bound(0), m_boundto(0) 
{
  static int bFirstTime = 1;

  if (bFirstTime) {
    bFirstTime = 0;
  }
}

GLenum getFormat(int pitch)
{
  GLenum format;
  switch(pitch) {
  case 1: format = GL_LUMINANCE_FLOAT32_ATI; break;
  case 2: format = GL_LUMINANCE_ALPHA_FLOAT32_ATI; break;
  case 3: format = GL_RGB_FLOAT32_ATI; break;
  case 4: format = GL_RGBA_FLOAT32_ATI; break;
  default: format =  GL_RGBA_FLOAT32_ATI;
  }
  return format;
}



void UberStorage::bindAsTexture(unsigned int target){
  unsigned int mem = alloc();// it use m_mem if already allocated
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, mem)); 
}

void UberStorage::bindAsAux(unsigned int n){
  unsigned int mem = alloc();// it uses m_mem if already allocated
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, allocfb(0)));
  SH_GL_CHECK_ERROR(glAttachMemATI( GL_DRAW_FRAMEBUFFER_ATI, n + GL_AUX0, mem));
}

void UberStorage::unbind(){

  switch(m_bound){
  case 0:
    // not bound
    break;
  case 1:
    SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI,m_boundto, 0));
    break;
  case 2:
    SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, temp_fb[0]));
    SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI,m_boundto+GL_AUX0, 0));
    break;
  default:
    // invalid bound parameter
    SH_DEBUG_WARN("Invalid  bind parameter!");
  }
}

//TO DO:
void UberStorage::unbindall(){

}

bool UberStorage::isBoundAsTexture(unsigned int target){
  if(m_bound!=1)
    return false;
  if(m_boundto!= target)
    return false;
  return true;
}

bool UberStorage::isBoundAsAux(unsigned int n){
  if(m_bound!=2)
    return false;
  if(m_boundto!=n)
    return false;
  return true;
}

bool UberStorage::isBound(){
  return m_bound==0;
}

int UberStorage::allocfb(int n, bool bForce){
  // create the temporary frame buffer
  if (temp_fb[n]<0 || bForce) {
    temp_fb[n] = SH_GL_CHECK_ERROR(glCreateFramebufferATI());
  }

  return temp_fb[n];
}

unsigned int UberStorage::alloc(int bForce)
{
  if (m_mem && !bForce) {
    return m_mem;
  }
  /// property array is constant for the time being
  GLint propsArray[4] = {GL_TEXTURE_2D, GL_TRUE, GL_COLOR_BUFFER_ATI, GL_TRUE};
  GLenum format = getFormat(m_pitch);
  m_mem = SH_GL_CHECK_ERROR(glAllocMem2DATI(format, m_width, m_height, 2, propsArray));

  return m_mem;
}

unsigned int UberStorage::remove()
{
  if (m_mem) {
    SH_GL_CHECK_ERROR(glDeleteMemATI(m_mem));
  }
  
  return 0;
}


void UberStorage::clearBuffer(float* col){
  
  if(m_mem<=0) return;
  CHECK_GL_ERROR();
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, allocfb(3)));

  // whi aux2? don't know
  glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI, GL_AUX2, m_mem);
   	 
  glDrawBuffer(GL_AUX2); 
  
  float local[4];
   if(!col){
     col = local;
     glGetFloatv(GL_COLOR_CLEAR_VALUE, col);
   }

   /// draw a quad...
   //glClearColor(col[0], col[1], col[2], col[3]);
   //glClear(GL_COLOR_BUFFER_BIT);
   glClear(GL_DEPTH_BUFFER_BIT);

   // setup matrices 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, width(), 0.0, height());
 
  CHECK_GL_ERROR();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  CHECK_GL_ERROR();

  // save the state 
  SH_GL_CHECK_ERROR(glPushAttrib(GL_COLOR_BUFFER_BIT | /* GL_DRAW_BUFFER */
				 GL_PIXEL_MODE_BIT | /* GL_READ_BUFER */
				 GL_POLYGON_BIT | /* GL_FRONT_FACE */
				 GL_ENABLE_BIT /* Hopefully, all enables: */
				 /* programs + lighting */));
    
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);

  glBegin(GL_QUADS);  
  glColor4f(col[0], col[1], col[2], col[3]);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(width(), 0.0 , 0.0);
  glVertex3f(width(), height(), 0.0);
  glVertex3f(0.0, height(), 0.0);
  glEnd(); 
  CHECK_GL_ERROR();

  SH_GL_CHECK_ERROR(glPopAttrib());
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glDrawBuffer(GL_BACK); 
  glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, 0);
  glDrawBuffer(GL_BACK); 

}

bool UberUberTransfer::transfer(const SH::ShStorage* from, SH::ShStorage* to)
{
  const UberStorage* src_storage = dynamic_cast<const UberStorage*>(from);
  UberStorage* dst_storage = dynamic_cast<UberStorage*>(to);
  
  if (!dst_storage || !src_storage) {
    SH_DEBUG_WARN("Invalid Storage");
    return false;
  }

  unsigned int srcmem = src_storage->mem();;
  unsigned int destmem = dst_storage->alloc();
  int width = src_storage->width();
  int height = src_storage->height();
  int pitch = src_storage->pitch();
  GLenum format = getFormat(src_storage->pitch());

  if (!srcmem || !destmem) {
    SH_DEBUG_WARN("Cannot copy unininitialized uber buffer\n");
  }

  // save the state 
  SH_GL_CHECK_ERROR(glPushAttrib(GL_COLOR_BUFFER_BIT | /* GL_DRAW_BUFFER */
				 GL_PIXEL_MODE_BIT | /* GL_READ_BUFER */
				 GL_POLYGON_BIT | /* GL_FRONT_FACE */
				 GL_ENABLE_BIT /* Hopefully, all enables: */
				 /* programs + lighting */));
    
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);
  glDisable(GL_LIGHTING);
  glFrontFace(GL_CCW);

  // attach ubuf to texture
  GLuint texBinding;
  glGenTextures(1, &texBinding);
  glActiveTextureARB(GL_TEXTURE7);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texBinding);
  //TODO save all this state before messing with it (for now just assume nobody uses
  //texture unit 7
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, 0);
  // use this dummy memory object to force GL_AUXn to be
  // a float framebuffer, so ReadPixels can return unclamped values with full precision
  UberStorage::allocfb(2);

  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, srcmem));

  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, UberStorage::temp_fb[2]));


  SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI, GL_AUX3, destmem));
  SH_DEBUG_PRINT("Attach GL_AUX3 temporarily to dest mem " << destmem);

  SH_GL_CHECK_ERROR(glDrawBuffer(GL_AUX3)); 

  SH_GL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // setup matrices 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // find tex coords && draw quad
  glBegin(GL_QUADS);
  glMultiTexCoord2fARB(GL_TEXTURE7, 0, 1); 
  glVertex3f(0, 0, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, 1, 1);
  glVertex3f(width, 0, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, 1, 0); 
  glVertex3f(width, height, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, 0, 0); 
  glVertex3f(0, height, 0.0);
  glEnd(); 

  // detach/free mem objects
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, 0));
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI, GL_AUX3, 0));
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, 0));

  /// restore all the info
  glPopAttrib();

  glDeleteTextures(1, &texBinding);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

}


bool UberHostTransfer::transfer(const SH::ShStorage* from, SH::ShStorage* to)
{
  const UberStorage* ustorage = dynamic_cast<const UberStorage*>(from);
  SH::ShHostStorage* hstorage = dynamic_cast<SH::ShHostStorage*>(to);
  
  if (!ustorage || !hstorage) {
    SH_DEBUG_WARN("Invalid Storage");
    return false;
  }

  unsigned int mem = ustorage->mem();
  int width = ustorage->width();
  int height = ustorage->height();
  int pitch = ustorage->pitch();
  GLenum format = getFormat(ustorage->pitch());

  if (!mem) {
    SH_DEBUG_WARN("Un-initialized memory");
    return false;
  }

  // save the state 
  SH_GL_CHECK_ERROR(glPushAttrib(GL_COLOR_BUFFER_BIT | /* GL_DRAW_BUFFER */
				 GL_PIXEL_MODE_BIT | /* GL_READ_BUFER */
				 GL_POLYGON_BIT | /* GL_FRONT_FACE */
				 GL_ENABLE_BIT /* Hopefully, all enables: */
				 /* programs + lighting */));

  
  glDisable(GL_VERTEX_PROGRAM_ARB);
  glDisable(GL_FRAGMENT_PROGRAM_ARB);
  glDisable(GL_LIGHTING);
  glFrontFace(GL_CCW);
  SH_DEBUG_PRINT("Temporarily glDisabled vertex/fragment programs and GL lighting");

  // attach ubuf to texture
  GLuint texBinding;
  glGenTextures(1, &texBinding);
  glActiveTextureARB(GL_TEXTURE7);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texBinding);
  //TODO save all this state before messing with it (for now just assume nobody uses
  //texture unit 7
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, 0);
  SH_DEBUG_PRINT("Temporarily unbind framebuffer fb " << ustorage->temp_fb[0]);
  
  // use this dummy memory object to force GL_AUXn to be
  // a float framebuffer, so ReadPixels can return unclamped values with full precision
  GLint propsArray[] = {GL_TEXTURE_2D, GL_TRUE, GL_COLOR_BUFFER_ATI, GL_TRUE};
  GLmem dummyMem = glAllocMem2DATI(format, width, height, 2, propsArray);
  
  UberStorage::allocfb(1);

  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, mem));
  SH_DEBUG_PRINT("Attach GL_TEXTURE_2D temporarily to mem " << mem);
  
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI,ustorage-> temp_fb[1]));
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_READ_FRAMEBUFFER_ATI, ustorage->temp_fb[1]));
  SH_DEBUG_PRINT("Bind GL_DRAW_FRAMEBUFFER_ATI to temp fb " << ustorage->temp_fb[1]);

  SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI, GL_AUX3, dummyMem));
  SH_DEBUG_PRINT("Attach GL_AUX3 temporarily to dummy mem " << dummyMem);
  
  SH_GL_CHECK_ERROR(glDrawBuffer(GL_AUX3)); 

  SH_GL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // setup matrices 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, width, 0.0, height);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // find tex coords && draw quad
  double tcx1 = 0;
  double tcy1 = 0;
  double tcx2 = 1;
  double tcy2 = 1;

  glBegin(GL_QUADS);
  glMultiTexCoord2fARB(GL_TEXTURE7, tcx1, tcy1); 
  glVertex3f(0.0,0.0, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, tcx2, tcy1);
  glVertex3f(0 + width, 0, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, tcx2, tcy2); 
  glVertex3f(0 + width, 0 + height, 0.0);
  glMultiTexCoord2fARB(GL_TEXTURE7, tcx1, tcy2); 
  glVertex3f(0, 0 + height, 0.0);
  glEnd(); 

  SH_GL_CHECK_ERROR(glReadBuffer(GL_AUX3));

  GLenum format2;
  switch (pitch) {
  case 1: format2 = GL_LUMINANCE; break;
  case 2: format2 = GL_LUMINANCE_ALPHA; break;
  case 3: format2 = GL_RGB; break;
  case 4: format2 = GL_RGBA; break;
  default: format2 = 0; break;
  }
 
  // sanity check
  if (hstorage->length() != width*height*pitch*sizeof(float)) {
    std::cout<<"{}{}{}{}{}{}{}{}{}{}{}{}{}Big problem!"<<std::endl;
  }

  SH_GL_CHECK_ERROR(glReadPixels(0, 0, width, height, format2, GL_FLOAT, hstorage->data())); 

  // detach/free mem objects
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, 0));

  SH_GL_CHECK_ERROR(glAttachMemATI(GL_DRAW_FRAMEBUFFER_ATI, GL_AUX3, 0));

  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_DRAW_FRAMEBUFFER_ATI, 0));
  SH_GL_CHECK_ERROR(glBindFramebufferATI(GL_READ_FRAMEBUFFER_ATI, 0));

  // restore state
  glPopAttrib();

  glDeleteTextures(1, &texBinding);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  SH_GL_CHECK_ERROR(glDeleteMemATI(dummyMem));
  SH_DEBUG_PRINT("Delete dummy mem " << dummyMem); 

  return true;
}


bool HostUberTransfer::transfer(const SH::ShStorage* from, SH::ShStorage* to)
{
  const SH::ShHostStorage* hstorage = dynamic_cast<const SH::ShHostStorage*>(from);
  UberStorage* ustorage = dynamic_cast<UberStorage*>(to);
  
  if (!ustorage || !hstorage) {
    SH_DEBUG_WARN("Invalid Storage");
    return false;
  }

  // allocate if needed
  unsigned int mem = ustorage->alloc();

  if (!mem) {
    SH_DEBUG_WARN("Unable to allocate Memory");
    return false;
  }

  int width = ustorage->width();
  int height = ustorage->height();
  int pitch = ustorage->pitch();
  GLenum format = getFormat(ustorage->pitch());

  //bind the texture 
  GLuint texBinding;
  glGenTextures(1, &texBinding);
  glActiveTextureARB(GL_TEXTURE7);

  // TODO!!! Save the texture binding
  
  //TODO handle other dimensions
  glBindTexture(GL_TEXTURE_2D, texBinding); 

  //attach the uber buffer
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, mem)); 
  SH_DEBUG_PRINT("Attach GL_TEXTURE_2D temporarily to mem " << mem); 

  //use glTexImage to upload data
  // TODO support other dimensions
  GLenum format2;
  switch (ustorage->pitch()) {
  case 1: format2 = GL_LUMINANCE; break;
  case 2: format2 = GL_LUMINANCE_ALPHA; break;
  case 3: format2 = GL_RGB; break;
  case 4: format2 = GL_RGBA; break;
  default: format2 = 0; break;
  }

  SH_GL_CHECK_ERROR(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format2, GL_FLOAT,
                                    hstorage->data()));

  //detach uber buffer
  SH_GL_CHECK_ERROR(glAttachMemATI(GL_TEXTURE_2D, GL_IMAGES_ATI, 0));
  SH_DEBUG_PRINT("Detach GL_TEXTURE_2D from mem " << mem); 

  //delete texture
  glDeleteTextures(1, (GLuint*)&texBinding);

}

}
