////
//
//
//     (C) Copyright 2003  ATI Research, Inc.    All rights reserved  
//     
//      James Percy, August 2003.
//
////

#ifndef EXTENSTIONS_INCLUDED
#define EXTENSTIONS_INCLUDED

// #include "_windows.h"
#include <GL/glut.h>
#include "glati.h"
// #include "wglati.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// ATI_uber_buffer
//
extern PFNGLALLOCMEM3DATIPROC           glAllocMem3DATI;
extern PFNGLALLOCMEM2DATIPROC           glAllocMem2DATI;
extern PFNGLALLOCMEM1DATIPROC           glAllocMem1DATI;
extern PFNGLDELETEMEMATIPROC            glDeleteMemATI;
extern PFNGLGETMEMPROPERTYATIPROC       glGetMemPropertyATI;
extern PFNGLGETMEMATIPROC               glGetMemATI;
extern PFNGLCOPYMEMIMAGE2DATIPROC       glCopyMemImage2DATI;
extern PFNGLCOPYMEMIMAGE1DATIPROC       glCopyMemImage1DATI;
extern PFNGLMEMSUBIMAGE3DATIPROC        glMemSubImage3DATI;
extern PFNGLMEMSUBIMAGE2DATIPROC        glMemSubImage2DATI;
extern PFNGLMEMSUBIMAGE1DATIPROC        glMemSubImage1DATI;
extern PFNGLCOPYMEMSUBIMAGE2DATIPROC    glCopyMemSubImage2DATI;
extern PFNGLCOPYMEMSUBIMAGE1DATIPROC    glCopyMemSubImage1DATI;
extern PFNGLMEMCOPY3DATIPROC            glMemCopy3DATI;
extern PFNGLMEMCOPY2DATIPROC            glMemCopy2DATI;
extern PFNGLMEMCOPY1DATIPROC            glMemCopy1DATI;
extern PFNGLATTACHMEMATIPROC              glAttachMemATI;
extern PFNGLVERTEXARRAYMEMATIPROC       glVertexArrayMemATI;
extern PFNGLDRAWELEMENTARRAYATIPROC     glDrawElementArrayATI;
extern PFNGLGETMEMINFOLOGATIPROC    glGetMemInfoLogATI;


//
// ATI_vertex_array_object
//
extern PFNGLNEWOBJECTBUFFERATIPROC      glNewObjectBufferATI;
extern PFNGLISOBJECTBUFFERATIPROC       glIsObjectBufferATI;
extern PFNGLUPDATEOBJECTBUFFERATIPROC   glUpdateObjectBufferATI;
extern PFNGLGETOBJECTBUFFERFVATIPROC    glGetObjectBufferfvATI;
extern PFNGLGETOBJECTBUFFERIVATIPROC    glGetObjectBufferivATI;
extern PFNGLFREEOBJECTBUFFERATIPROC     glDeleteObjectBufferATI;
extern PFNGLARRAYOBJECTATIPROC          glArrayObjectATI;
extern PFNGLGETARRAYOBJECTFVATIPROC     glGetArrayObjectfvATI;
extern PFNGLGETARRAYOBJECTIVATIPROC     glGetArrayObjectivATI;
extern PFNGLVARIANTARRAYOBJECTATIPROC   glVariantArrayObjectATI;
extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC  glGetVariantArrayObjectfvATI;
extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC  glGetVariantArrayObjectivATI;

//
// ARB_vertex_buffer_object
//
extern PFNGLBINDBUFFERARBPROC           glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC           glGenBuffersARB;
extern PFNGLISBUFFERARBPROC             glIsBufferARB;
extern PFNGLBUFFERDATAARBPROC           glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC        glBuffersSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC     glGetBuffersSubDataARB;
extern PFNGLMAPBUFFERARBPROC            glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC          glUnMapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB;



//
// ARB_fragment_program
//
extern PFNGLPROGRAMSTRINGARBPROC				glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC					glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC				glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC					glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC		glProgramEnvParameter4fARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC		glProgramEnvParameter4dARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4dARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	    glProgramLocalParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC	    glProgramLocalParameter4dvARB;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC	    glGetProgramEnvParameterfvARB;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC	    glGetProgramEnvParameterdvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdvARB;
extern PFNGLGETPROGRAMIVARBPROC				    glGetProgramivARB;
extern PFNGLGETPROGRAMSTRINGARBPROC			    glGetProgramStringARB;
extern PFNGLISPROGRAMARBPROC					glIsProgramARB;
extern PFNGLDRAWBUFFERS                         glDrawBuffersATI;

GLboolean InitExtensions(void);

#ifdef __cplusplus
}
#endif

#endif //  EXTENSTIONS_INCLUDED
