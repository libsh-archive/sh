////
//
//
//     (C) Copyright 2003  ATI Research, Inc.    All rights reserved  
//     
//      James Percy, August 2003.
//
////

#include <stdio.h>
#include <stdlib.h>
#include "extensions.h"
#include <GL/glx.h>

//
// ATI_uber_buffer
//
PFNGLALLOCMEM3DATIPROC           ____glAllocMem3DATI;
PFNGLALLOCMEM3DATIPROC           glAllocMem3DATI = NULL;
PFNGLALLOCMEM2DATIPROC           glAllocMem2DATI;
PFNGLALLOCMEM1DATIPROC           glAllocMem1DATI;
PFNGLDELETEMEMATIPROC            glDeleteMemATI;
PFNGLGETMEMPROPERTYATIPROC       glGetMemPropertyATI;
PFNGLGETMEMATIPROC               glGetMemATI;
PFNGLCOPYMEMIMAGE2DATIPROC       glCopyMemImage2DATI;
PFNGLCOPYMEMIMAGE1DATIPROC       glCopyMemImage1DATI;
PFNGLMEMSUBIMAGE3DATIPROC        glMemSubImage3DATI;
PFNGLMEMSUBIMAGE2DATIPROC        glMemSubImage2DATI;
PFNGLMEMSUBIMAGE1DATIPROC        glMemSubImage1DATI;
PFNGLCOPYMEMSUBIMAGE2DATIPROC    glCopyMemSubImage2DATI;
PFNGLCOPYMEMSUBIMAGE1DATIPROC    glCopyMemSubImage1DATI;
PFNGLMEMCOPY3DATIPROC            glMemCopy3DATI;
PFNGLMEMCOPY2DATIPROC            glMemCopy2DATI;
PFNGLMEMCOPY1DATIPROC            glMemCopy1DATI;
PFNGLATTACHMEMATIPROC              glAttachMemATI;
PFNGLVERTEXARRAYMEMATIPROC       glVertexArrayMemATI;
PFNGLDRAWELEMENTARRAYATIPROC     glDrawElementArrayATI;
PFNGLGETMEMINFOLOGATIPROC    glGetMemInfoLogATI;

//
// ARB_fragment_program
//
PFNGLPROGRAMSTRINGARBPROC				glProgramStringARB;
PFNGLBINDPROGRAMARBPROC					glBindProgramARB;
PFNGLDELETEPROGRAMSARBPROC				glDeleteProgramsARB;
PFNGLGENPROGRAMSARBPROC					glGenProgramsARB;
PFNGLPROGRAMENVPARAMETER4FARBPROC		glProgramEnvParameter4fARB;
PFNGLPROGRAMENVPARAMETER4DARBPROC		glProgramEnvParameter4dARB;
PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fvARB;
PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dvARB;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4fARB;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4dARB;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC	glProgramLocalParameter4dvARB;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC	glGetProgramEnvParameterfvARB;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC	glGetProgramEnvParameterdvARB;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfvARB;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdvARB;
PFNGLGETPROGRAMIVARBPROC				glGetProgramivARB;
PFNGLGETPROGRAMSTRINGARBPROC			glGetProgramStringARB;
PFNGLISPROGRAMARBPROC					glIsProgramARB;
PFNGLDRAWBUFFERS						glDrawBuffersATI;

//
// ARB_vertex_buffer_object
//
PFNGLBINDBUFFERARBPROC           glBindBufferARB;
PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB;
PFNGLGENBUFFERSARBPROC           glGenBuffersARB;
PFNGLISBUFFERARBPROC             glIsBufferARB;
PFNGLBUFFERDATAARBPROC           glBufferDataARB;
PFNGLBUFFERSUBDATAARBPROC        glBuffersSubDataARB;
PFNGLGETBUFFERSUBDATAARBPROC     glGetBuffersSubDataARB;
PFNGLMAPBUFFERARBPROC            glMapBufferARB;
PFNGLUNMAPBUFFERARBPROC          glUnMapBufferARB;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB;

//
// ARB_multitexture
//
#if 0
PFNGLCLIENTACTIVETEXTUREARBPROC			glClientActiveTextureARB;
PFNGLACTIVETEXTUREARBPROC				glActiveTextureARB;
PFNGLMULTITEXCOORD3FVARBPROC			glMultiTexCoord4fvARB;
PFNGLMULTITEXCOORD3FARBPROC				glMultiTexCoord4fARB;
PFNGLMULTITEXCOORD3FVARBPROC			glMultiTexCoord3fvARB;
PFNGLMULTITEXCOORD3FARBPROC				glMultiTexCoord3fARB;
PFNGLMULTITEXCOORD2FVARBPROC			glMultiTexCoord2fvARB;
PFNGLMULTITEXCOORD2FARBPROC				glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD1FARBPROC				glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD3DVARBPROC			glMultiTexCoord3dvARB;
PFNGLMULTITEXCOORD3DARBPROC		    	glMultiTexCoord3dARB;
PFNGLMULTITEXCOORD2DVARBPROC			glMultiTexCoord2dvARB;
PFNGLMULTITEXCOORD2DARBPROC				glMultiTexCoord2dARB;
PFNGLMULTITEXCOORD1DARBPROC				glMultiTexCoord1dARB;
#endif

//
// ATI_vertex_array_object
//
PFNGLNEWOBJECTBUFFERATIPROC      glNewObjectBufferATI;
PFNGLISOBJECTBUFFERATIPROC       glIsObjectBufferATI;
PFNGLUPDATEOBJECTBUFFERATIPROC   glUpdateObjectBufferATI;
PFNGLGETOBJECTBUFFERFVATIPROC    glGetObjectBufferfvATI;
PFNGLGETOBJECTBUFFERIVATIPROC    glGetObjectBufferivATI;
PFNGLFREEOBJECTBUFFERATIPROC     glDeleteObjectBufferATI;
PFNGLARRAYOBJECTATIPROC          glArrayObjectATI;
PFNGLGETARRAYOBJECTFVATIPROC     glGetArrayObjectfvATI;
PFNGLGETARRAYOBJECTIVATIPROC     glGetArrayObjectivATI;
PFNGLVARIANTARRAYOBJECTATIPROC   glVariantArrayObjectATI;
PFNGLGETVARIANTARRAYOBJECTFVATIPROC  glGetVariantArrayObjectfvATI;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC  glGetVariantArrayObjectivATI;

#if 0
//
// WGL_ARB_pbuffer
//
PFNWGLCREATEPBUFFERARBPROC               wglCreatePbufferARB;
PFNWGLGETPBUFFERDCARBPROC                wglGetPbufferDCARB;
PFNWGLRELEASEPBUFFERDCARBPROC            wglReleasePbufferDCARB;
PFNWGLDESTROYPBUFFERARBPROC              wglDestroyPbufferARB;
PFNWGLQUERYPBUFFERARBPROC                wglQueryPbufferARB;
PFNWGLMAKECONTEXTCURRENTARBPROC			wglMakeContextCurrentARB;

//
// WGL_ARB_pixel_format
//
PFNWGLGETPIXELFORMATATTRIBIVARBPROC      wglGetPixelFormatAttribivARB;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC      wglGetPixelFormatAttribfvARB;
PFNWGLCHOOSEPIXELFORMATARBPROC           wglChoosePixelFormatARB;

//
// ARB_render_texture.
//
PFNWGLBINDTEXIMAGEARBPROC                wglBindTexImageARB;
PFNWGLRELEASETEXIMAGEARBPROC             wglReleaseTexImageARB;
PFNWGLSETPBUFFERATTRIBARBPROC            wglSetPbufferAttribARB;
#endif

static GLboolean
IsExtSupported(const char *ext)
{
    const char *exts;
    const char *start;
    const char *where, *terminator;

    where = strchr(ext, ' ');

    if ((where != NULL) || (*ext == '\0'))
    {
        return GL_FALSE;
    }

    exts = (const char*) glGetString(GL_EXTENSIONS);
    start = exts;

    for ( ; ; )
    {
        where = strstr(start, ext);

        if (!where)
        {
            break;
        }

        terminator = where + strlen(ext);

        if ((where == start) || (*(where - 1) == ' '))
        {
            if ((*terminator == ' ') || (*terminator == '\0'))
            {
                return GL_TRUE;
            }
        }

        start = terminator;
    }

    return GL_FALSE;
}

#ifdef EXIT_ON_MISSING_EXTENSION
#define GET_PROC(x)                                 \
{                                                   \
    x = (void*) glXGetProcAddressARB(#x);              \
    if (x == NULL)                                  \
    {                                               \
        printf("Extension %s not found.\n", #x);    \
		exit(-1);                                   \
    }                                               \
}
#else
#define GET_PROC(x)                                 \
{                                                   \
    x = (void*) glXGetProcAddressARB(#x);              \
}
#endif


GLboolean
InitExtensions(void)
{
    // GL_ATI_uber_buffers

/* extension string not yet exported
    if (!IsExtSupported("GL_ATI_uber_buffers"))
    {
        fprintf(stderr, "InitExtensions: GL_ATI_uber_buffers not supported\n");
        return GL_FALSE;
    }
*/

    ____glAllocMem3DATI = (PFNGLALLOCMEM3DATIPROC)glXGetProcAddressARB("glAllocMem3DATI");
    glAllocMem3DATI = (PFNGLALLOCMEM3DATIPROC)glXGetProcAddressARB("glAllocMem3DATI");
    glAllocMem2DATI = (PFNGLALLOCMEM2DATIPROC)glXGetProcAddressARB("glAllocMem2DATI");
    glAllocMem1DATI = (PFNGLALLOCMEM1DATIPROC)glXGetProcAddressARB("glAllocMem1DATI");
    glDeleteMemATI  = (PFNGLDELETEMEMATIPROC)glXGetProcAddressARB("glDeleteMemATI");
    glGetMemPropertyATI = (PFNGLGETMEMPROPERTYATIPROC)glXGetProcAddressARB("glGetMemPropertyATI");
    glGetMemATI = (PFNGLGETMEMATIPROC)glXGetProcAddressARB("glGetMemATI");
    glCopyMemImage2DATI = (PFNGLCOPYMEMIMAGE2DATIPROC)glXGetProcAddressARB("glCopyMemImage2DATI");
    glCopyMemImage1DATI = (PFNGLCOPYMEMIMAGE1DATIPROC)glXGetProcAddressARB("glCopyMemImage1DATI");
    glMemSubImage3DATI = (PFNGLMEMSUBIMAGE3DATIPROC)glXGetProcAddressARB("glMemSubImage3DATI");
    glMemSubImage2DATI = (PFNGLMEMSUBIMAGE2DATIPROC)glXGetProcAddressARB("glMemSubImage2DATI");
    glMemSubImage1DATI = (PFNGLMEMSUBIMAGE1DATIPROC)glXGetProcAddressARB("glMemSubImage1DATI");
    glCopyMemSubImage2DATI = (PFNGLCOPYMEMSUBIMAGE2DATIPROC)glXGetProcAddressARB("glCopyMemSubImage2DATI");
    glCopyMemSubImage1DATI = (PFNGLCOPYMEMSUBIMAGE1DATIPROC)glXGetProcAddressARB("glCopyMemSubImage1DATI");
    glMemCopy3DATI = (PFNGLMEMCOPY3DATIPROC)glXGetProcAddressARB("glMemCopy3DATI");
    glMemCopy2DATI = (PFNGLMEMCOPY2DATIPROC)glXGetProcAddressARB("glMemCopy2DATI");
    glMemCopy1DATI = (PFNGLMEMCOPY1DATIPROC)glXGetProcAddressARB("glMemCopy1DATI");
    glAttachMemATI = (PFNGLATTACHMEMATIPROC)glXGetProcAddressARB("glAttachMemATI");
    glVertexArrayMemATI = (PFNGLVERTEXARRAYMEMATIPROC) glXGetProcAddressARB("glVertexArrayMemATI");
	glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC) glXGetProcAddressARB("glDrawElementArrayATI");
    glGetMemInfoLogATI = (PFNGLGETMEMINFOLOGATIPROC)glXGetProcAddressARB("glGetMemInfoLogATI");

    GET_PROC(glProgramStringARB);
    GET_PROC(glBindProgramARB);
    GET_PROC(glDeleteProgramsARB);
    GET_PROC(glGenProgramsARB);
    GET_PROC(glProgramEnvParameter4fARB);
    GET_PROC(glProgramEnvParameter4dARB);
    GET_PROC(glProgramEnvParameter4fvARB);
    GET_PROC(glProgramEnvParameter4dvARB);
    GET_PROC(glProgramLocalParameter4fARB);
    GET_PROC(glProgramLocalParameter4dARB);
    GET_PROC(glProgramLocalParameter4fvARB);
    GET_PROC(glProgramLocalParameter4dvARB);
    GET_PROC(glGetProgramEnvParameterfvARB);
    GET_PROC(glGetProgramEnvParameterdvARB);
    GET_PROC(glGetProgramLocalParameterfvARB);
    GET_PROC(glGetProgramLocalParameterdvARB);
    GET_PROC(glGetProgramivARB);
    GET_PROC(glGetProgramStringARB);
    GET_PROC(glIsProgramARB);
	GET_PROC(glDrawBuffersATI);

    GET_PROC(glNewObjectBufferATI);
    GET_PROC(glIsObjectBufferATI);
    GET_PROC(glUpdateObjectBufferATI);
    GET_PROC(glGetObjectBufferfvATI);
    GET_PROC(glGetObjectBufferivATI);
    GET_PROC(glDeleteObjectBufferATI);
    GET_PROC(glArrayObjectATI);
    GET_PROC(glGetArrayObjectfvATI);
    GET_PROC(glGetArrayObjectivATI);
    GET_PROC(glVariantArrayObjectATI);
    GET_PROC(glGetVariantArrayObjectfvATI);
    GET_PROC(glGetVariantArrayObjectivATI);

    GET_PROC(glBindBufferARB);
    GET_PROC(glDeleteBuffersARB);
    GET_PROC(glGenBuffersARB);
    GET_PROC(glIsBufferARB);
    GET_PROC(glBufferDataARB);
    GET_PROC(glBuffersSubDataARB);
    GET_PROC(glGetBuffersSubDataARB);
    GET_PROC(glMapBufferARB);
    GET_PROC(glUnMapBufferARB);
    GET_PROC(glGetBufferParameterivARB);
    GET_PROC(glGetBufferParameterivARB);
    GET_PROC(glGetBufferPointervARB);


#if 0
    GET_PROC(glClientActiveTextureARB);
    GET_PROC(glActiveTextureARB);
    GET_PROC(glMultiTexCoord4fARB);
    GET_PROC(glMultiTexCoord4fvARB);
    GET_PROC(glMultiTexCoord3fvARB);
    GET_PROC(glMultiTexCoord3fARB);
    GET_PROC(glMultiTexCoord2fvARB);
    GET_PROC(glMultiTexCoord2fARB);
    GET_PROC(glMultiTexCoord1fARB);
    GET_PROC(glMultiTexCoord3dvARB);
    GET_PROC(glMultiTexCoord3dARB);
    GET_PROC(glMultiTexCoord2dvARB);
    GET_PROC(glMultiTexCoord2dARB);
    GET_PROC(glMultiTexCoord1dARB);

    GET_PROC(wglCreatePbufferARB);
    GET_PROC(wglGetPbufferDCARB);
    GET_PROC(wglReleasePbufferDCARB);
    GET_PROC(wglDestroyPbufferARB);
    GET_PROC(wglQueryPbufferARB);
    GET_PROC(wglMakeContextCurrentARB);

    GET_PROC(wglGetPixelFormatAttribivARB);
    GET_PROC(wglGetPixelFormatAttribfvARB);
    GET_PROC(wglChoosePixelFormatARB);

    GET_PROC(wglBindTexImageARB);
    GET_PROC(wglReleaseTexImageARB);
    GET_PROC(wglSetPbufferAttribARB);
#endif

    return GL_TRUE;
}
