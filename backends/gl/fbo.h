#ifndef FBO_H
#define FBO_H

// Accepted by the <target> parameter of BindFramebufferEXT,
// CheckFramebufferStatusEXT, FramebufferTexture{1D|2D|3D}EXT, and
// FramebufferRenderbufferEXT:

#define GL_FRAMEBUFFER_EXT                     0x8D40 

// Accepted by the <target> parameter of BindRenderbufferEXT,
// RenderbufferStorageEXT, and GetRenderbufferParameterivEXT,
// and returned by GetFramebufferAttachmentParameterivEXT:

#define GL_RENDERBUFFER_EXT                    0x8D41

// Accepted by the <internalformat> parameter of
// RenderbufferStorageEXT:

#define GL_STENCIL_INDEX_EXT                   0x8D45
#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49

// Accepted by the <pname> parameter of GetRenderbufferParameterivEXT:

#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44

// Accepted by the <pname> parameter of
// GetFramebufferAttachmentParameterivEXT:

#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4

// Accepted by the <attachment> parameter of
// FramebufferTexture{1D|2D|3D}EXT, FramebufferRenderbufferEXT, and
// GetFramebufferAttachmentParameterivEXT

#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT               0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT               0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT               0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT               0x8CED
#define GL_COLOR_ATTACHMENT14_EXT               0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT               0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20

// Returned by CheckFramebufferStatusEXT():

#define GL_FRAMEBUFFER_COMPLETE_EXT                         0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT            0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT    0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT  0x8CD8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT            0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT               0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT           0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT           0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                      0x8CDD
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT                     0x8CDE

// Accepted by GetIntegerv():

#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8

// Returned by GetError():

#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT   0x0506


#ifndef GL_EXT_framebuffer_object 
#define GL_EXT_framebuffer_object 1
#ifdef GL_GLEXT_PROTOTYPES

// New Procedures and Functions
extern void APIENTRY GLboolean glIsRenderbufferEXT(GLuint);
extern void APIENTRY void glBindRenderbufferEXT(GLenum, GLuint);
extern void APIENTRY void glDeleteRenderbuffersEXT(GLsizei, const GLuint*);
extern void APIENTRY void glGenRenderbuffersEXT(GLsizei, GLuint*);
extern void APIENTRY void glRenderbufferStorageEXT(GLenum, GLenum, GLsizei, GLsizei);
extern void APIENTRY void glGetRenderbufferParameterivEXT(GLenum, GLenum, GLint*);
extern void APIENTRY GLboolean glIsFramebufferEXT(GLuint);
extern void APIENTRY void glBindFramebufferEXT(GLenum, GLuint);
extern void APIENTRY void glDeleteFramebuffersEXT(GLsizei, const GLuint*);
extern void APIENTRY void glGenFramebuffersEXT(GLsizei, GLuint*);
extern GLenum APIENTRY GLenum glCheckFramebufferStatusEXT(GLenum target);
extern void APIENTRY void glFramebufferTexture1DEXT(GLenum, GLenum, GLenum, GLint, GLint);
extern void APIENTRY void glFramebufferTexture2DEXT(GLenum, GLenum, GLenum, GLint, GLint);
extern void APIENTRY void glFramebufferTexture3DEXT(GLenum, GLenum, GLenum, GLint, GLint, GLint);
extern void APIENTRY void glFramebufferRenderbufferEXT(GLenum, GLenum, GLenum, GLint);
extern void APIENTRY void glGetFramebufferAttachmentParameterivEXT(GLenum, GLenum, GLenum, GLint*);
extern void APIENTRY void glGenerateMipmapEXT(GLenum target);
#endif /* GL_GLEXT_PROTOTYPES */

typedef void (APIENTRY * PFNGLISRENDERBUFFEREXTPROC) (GLuint);
typedef void (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC) (GLenum, GLuint);
typedef void (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei, const GLuint*);
typedef void (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei, GLuint*);
typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum, GLenum, GLsizei, GLsizei);
typedef void (APIENTRY * PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) (GLenum, GLenum, GLint*);
typedef void (APIENTRY * PFNGLISFRAMEBUFFEREXTPROC) (GLuint);
typedef void (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum, GLuint);
typedef void (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei, const GLuint*);
typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei, GLuint*);
typedef GLenum (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) (GLenum, GLenum, GLenum, GLint, GLint);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum, GLenum, GLenum, GLint, GLint);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) (GLenum, GLenum, GLenum, GLint, GLint, GLint);
typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum, GLenum, GLenum, GLint);
typedef void (APIENTRY * PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) (GLenum, GLenum, GLenum, GLint*);
typedef void (APIENTRY * PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);
#endif

#endif
