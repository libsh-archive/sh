// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "GlBackend.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"

#include <sstream>

#ifdef WIN32

PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = 0;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = 0;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = 0;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = 0;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = 0;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = 0;

PFNGLTEXIMAGE3DPROC glTexImage3D = 0;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = 0;

// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = 0;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = 0;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0;

// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = 0;
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = 0;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = 0;
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = 0;
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = 0;

#endif /* WIN32 */

namespace shgl {

using namespace SH;

#ifdef WIN32
LRESULT CALLBACK shGlWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
  // An empty WindowProc, we need one to create a window. For now
  // just pass everything through DefWindowProc.
  // TODO: Should we actually process any messages? There will
  // definitely be atleast a WM_CREATE coming through here and
  // maybe some others.
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
#endif /* WIN32 */

void shGlCheckError(const char* desc, const char* file, int line)
{
  GLenum errnum = glGetError();
  char* error = 0;
  switch (errnum) {
  case GL_NO_ERROR:
    return;
  case GL_INVALID_ENUM:
    error = "GL_INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    error = "GL_INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    error = "GL_INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    error = "GL_STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    error = "GL_STACK_UNDERFLOW";
    break;
  case GL_OUT_OF_MEMORY:
    error = "GL_OUT_OF_MEMORY";
    break;
  case GL_TABLE_TOO_LARGE:
    error = "GL_TABLE_TOO_LARGE";
    break;
  default:
    error = "Unknown error!";
    break;
  }
  SH_DEBUG_PRINT("GL ERROR on " << file << ": " <<line<<": "<< error);
  SH_DEBUG_PRINT("GL ERROR call: " << desc);
}

#ifdef WIN32
#define GET_WGL_PROCEDURE(x, T) \
if ((x = reinterpret_cast<PFN ## T ## PROC>(wglGetProcAddress(#x))) == NULL) \
  { \
  std::stringstream msg; \
  msg << "wglGetProcAddress failed (" << GetLastError() << ")"; \
  shError(ShException(msg.str())); \
  }
#endif /* WIN32 */

GlBackend::GlBackend(CodeStrategy* code, TextureStrategy* texture, StreamStrategy* stream) :
  m_code(code),
  m_texture(texture),
  m_stream(stream)
  {

#ifdef WIN32
  // wglGetProcessAddress will fail outright if there isn't a current
  // context. So, If there isn't a current context we need to create one.
  // To create a context we need just one thing, an HDC. But not just
  // any HDC, an HDC that can be assigned an accelerated pixel format and
  // subsequently bound to an OpenGL context. Creating a "DISPLAY" DC
  // (via CreateDC) can have its pixel format set, but is incapable of
  // being assigned to an HGLRC. A memory DC (via CreateCompatibleDC) is
  // only good for PFD_DRAW_TO_BITMAP pixel formats which are inherently
  // not accelerated (and then all the wglGetProcAddress fail because
  // its not the ICD driver). The only solution seems to be creating an
  // actual window which gets us a real HDC. The window is temporary and
  // only exist long enough to fetch the functions needed. 
  // TODO: cleanup this hackery. I try to create a simple a window as 
  // possible to just get an HDC, the Win32 documentation is a little
  // sparse on what the requirements are (i.e settings for dwStyle,
  // WNDCLASSEX, PIXELFORMATDESCRIPTOR, etc). Should this window perhaps
  // be kept around for the lifetime of the GlBackend? It would simplify
  // the pbuffer code. What are the implications of having a hidden window
  // lying around, what about the event loop and shGlWindowProc. How does
  // this interact with a UI toolkit. If someone loads the backend before
  // initializing glut does this stuff conflict with the WGL stuff done 
  // by GLUT. The documentation explicit states that "extension function
  // addresses are unique for each pixel format", a little troubling, although
  // I'm not sure I believe it.
  HWND hWnd = NULL;
  HDC hdc = wglGetCurrentDC();
  HGLRC hglrc = wglGetCurrentContext();
  if (hdc == NULL && hglrc == NULL)
    {
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof (WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = shGlWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "shGlWindow";
    wc.hIconSm = NULL;

    if (!RegisterClassEx(&wc))
      {
      std::stringstream msg;
      msg << "RegisterClassEx failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

    hWnd = CreateWindowEx(0, "shGlWindow", "shGlWindow", WS_POPUP,
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, NULL, NULL);

    if (hWnd == NULL)
      {
      std::stringstream msg;
      msg << "CreateWindowEx failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

    hdc = GetDC(hWnd);
    if (hdc == NULL)
      {
      DestroyWindow(hWnd);

      std::stringstream msg;
      msg << "GetDC failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

	  PIXELFORMATDESCRIPTOR pfd;
	  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	  pfd.nVersion = 1;
	  pfd.dwFlags = PFD_SUPPORT_OPENGL;
	  pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;

	  int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0)
      {
      std::stringstream msg;
      msg << "ChoosePixelFormat failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

	  if (!SetPixelFormat(hdc, pf, &pfd))
      {
      std::stringstream msg;
      msg << "SetPixelFormat failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

    hglrc = wglCreateContext(hdc);
    if (hglrc == NULL)
      {
      ReleaseDC(hWnd, hdc);
      DestroyWindow(hWnd);

      std::stringstream msg;
      msg << "wglCreateContext failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }

    if (!wglMakeCurrent(hdc, hglrc))
      {
      wglDeleteContext(hglrc);
      ReleaseDC(hWnd, hdc);
      DestroyWindow(hWnd);

      std::stringstream msg;
      msg << "wglMakeCurrent failed (" << GetLastError() << ")";
      shError(ShException(msg.str()));
      }
    }
  else
    {
    hdc = NULL;
    hglrc = NULL;
    }

  GET_WGL_PROCEDURE(glProgramStringARB, GLPROGRAMSTRINGARB);
  GET_WGL_PROCEDURE(glBindProgramARB, GLBINDPROGRAMARB);
  GET_WGL_PROCEDURE(glGenProgramsARB, GLGENPROGRAMSARB);
  GET_WGL_PROCEDURE(glActiveTextureARB, GLACTIVETEXTUREARB);
  GET_WGL_PROCEDURE(glProgramEnvParameter4fvARB, GLPROGRAMENVPARAMETER4FVARB);
  GET_WGL_PROCEDURE(glProgramLocalParameter4fvARB, GLPROGRAMLOCALPARAMETER4FVARB);
  GET_WGL_PROCEDURE(glGetProgramivARB, GLGETPROGRAMIVARB);

  GET_WGL_PROCEDURE(glTexImage3D, GLTEXIMAGE3D);
  GET_WGL_PROCEDURE(glTexSubImage3D, GLTEXSUBIMAGE3D);

  // WGL_ARB_pixel_format
  GET_WGL_PROCEDURE(wglGetPixelFormatAttribivARB, WGLGETPIXELFORMATATTRIBIVARB);
  GET_WGL_PROCEDURE(wglGetPixelFormatAttribfvARB, WGLGETPIXELFORMATATTRIBFVARB);
  GET_WGL_PROCEDURE(wglChoosePixelFormatARB, WGLCHOOSEPIXELFORMATARB);

  // WGL_ARB_pbuffer
  GET_WGL_PROCEDURE(wglCreatePbufferARB, WGLCREATEPBUFFERARB);
  GET_WGL_PROCEDURE(wglGetPbufferDCARB, WGLGETPBUFFERDCARB);
  GET_WGL_PROCEDURE(wglReleasePbufferDCARB, WGLRELEASEPBUFFERDCARB);
  GET_WGL_PROCEDURE(wglDestroyPbufferARB, WGLDESTROYPBUFFERARB);
  GET_WGL_PROCEDURE(wglQueryPbufferARB, WGLQUERYPBUFFERARB);

  if (hWnd)
    {
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(hWnd, hdc);
    wglDeleteContext(hglrc);
    DestroyWindow(hWnd);
    }
  
#endif /* WIN32 */
  }

SH::ShBackendCodePtr
GlBackend::generateCode(const std::string& target,
                        const SH::ShProgramNodeCPtr& shader)
{
  return m_code->generate(target, shader, m_texture);
}

void
GlBackend::execute(const SH::ShProgramNodeCPtr& program,
                   SH::ShStream& dest)
{
 m_stream->execute(program, dest);
}

}
