#ifndef UBERBUFFERS_HPP
#define UBERBUFFERS_HPP

#ifdef __cplusplus
extern "C" {
#endif

/*
** GL_ATI_uber_buffers
**
** Support:
**   Rage 128 based    : Not Supported
**   Radeon 7xxx based : Not Supported
**   Radeon 8xxx based : Not Supported
**   Radeon 9xxx based : Supported
*/
#ifndef GL_ATI_uber_buffers
#define GL_ATI_uber_buffers                 1

#define GL_COLOR_BUFFER_ATI                 0x00010000  // TEMPORARY VALUE
#define GL_DEPTH_BUFFER_ATI                 0x00020000  // TEMPORARY VALUE
#define GL_STENCIL_BUFFER_ATI               0x00030000  // TEMPORARY VALUE
#define GL_GENERIC_ARRAY_ATI                0x00040000  // TEMPORARY VALUE
#define GL_ACCUMULATION_BUFFER_ATI          0x00050000  // TEMPORARY VALUE
#define GL_MIPMAP_ATI                       0x00060000  // TEMPORARY VALUE
#define GL_BORDER_ATI                       0x00070000  // TEMPORARY VALUE
#define GL_USAGE_ATI                        0x00080000  // TEMPORARY VALUE
#define GL_PROXY_ATI                        0x001f0000  // TEMPORARY VALUE
#define GL_MATCH_SIZE_ATI                   0x00090000  // TEMPORARY VALUE
#define GL_MATCH_COMPONENT_SET_ATI          0x000a0000  // TEMPORARY VALUE
#define GL_MATCH_COMPONENT_SIZE_ATI         0x000b0000  // TEMPORARY VALUE
#define GL_MATCH_SAMPLES_ATI                0x000c0000  // TEMPORARY VALUE
#define GL_ATTACH_COMPATIBILITY_ATI         0x000d0000  // TEMPORARY VALUE
#define GL_VALIDATE_ONLY_ATI                0x000e0000  // TEMPORARY VALUE
#define GL_MEMORY_DIMENSIONS_ATI            0x000f0000  // TEMPORARY VALUE
#define GL_MEMORY_WIDTH_ATI                 0x00100000  // TEMPORARY VALUE
#define GL_MEMORY_HEIGHT_ATI                0x00110000  // TEMPORARY VALUE
#define GL_MEMORY_DEPTH_ATI                 0x00120000  // TEMPORARY VALUE
#define GL_MEMORY_LEVELS_ATI                0x00130000  // TEMPORARY VALUE
#define GL_MEMORY_FORMAT_ATI                0x00140000  // TEMPORARY VALUE
#define GL_SUB_MEM_ATI                      0x00150000  // TEMPORARY VALUE
#define GL_WS_ALLOCATED_ATI                 0x00160000  // TEMPORARY VALUE
#define GL_EXACT_ATI                        0x00170000  // TEMPORARY VALUE
#define GL_MIN_ATI                          0x00180000  // TEMPORARY VALUE
#define GL_MAX_ATI                          0x00190000  // TEMPORARY VALUE
#define GL_MIN_ELEMENTS_ATI                 0x001a0000  // TEMPROARY VALUE
#define GL_MEMORY_3D_ATI                    0x001b0000  // TEMPORARY VALUE
#define GL_FRAMEBUFFER_ATI                  0x001c0000  // TEMPORARY VALUE
#define GL_STENCIL_COMPONENT_ATI            0x001d0000  // TEMPORARY VALUE
#define GL_STENCIL_COMPONENT8_ATI           0x001e0000  // TEMPORARY VALUE

#define GL_PROXY_ATI                        0x001f0000  // TEMPORARY VALUE
#define GL_CUBE_MAP_ATI                     0x00200000  // TEMPORARY VALUE
#define GL_3D_DEPTH_SLICE_ATI               0x00210000  // TEMPORARY VALUE
#define GL_MATCH_COMPONENT_TYPE_ATI         0x00220000  // TEMPORARY VALUE
#define GL_IS_BASE_MEM_ATI                  0x00230000  // TEMPORARY VALUE
#define GL_IMAGES_ATI                       0x00240000  // TEMPORARY VALUE
#define GL_DRAW_FRAMEBUFFER_ATI             0x00250000  // TEMPORARY VALUE
#define GL_READ_FRAMEBUFFER_ATI             0x00260000  // TEMPORARY VALUE
#define GL_SWAP_SRC_FRAMEBUFFER_ATI         0x00270000  // TEMPORARY VALUE
#define GL_SWAP_DST_FRAMEBUFFER_ATI         0x00280000  // TEMPORARY VALUE
#define GL_SWAP_PRESERVE_ATI                0x00290000  // TEMPORARY VALUE
#define GL_SWAP_SYNC_ATI                    0x002a0000  // TEMPORARY VALUE
#define GL_SWAP_INTERVAL_ATI                0x002b0000  // TEMPORARY VALUE
#define GL_VIDEO_ATI                        0x002c0000  // TEMPORARY VALUE

#define GL_BASE_ATI                         0x00000001
#define GL_LEAVES_ATI                       0x00000002
#define GL_MIPMAP_FALSE_ATI                 0x00000004
#define GL_MIPMAP_TRUE_ATI                  0x00000008
#define GL_MIPMAP_EITHER_ATI                0x0000000c
#define GL_CUBE_MAP_FALSE_ATI               0x00000010
#define GL_CUBE_MAP_TRUE_ATI                0x00000020
#define GL_CUBE_MAP_EITHER_ATI              0x00000030
#define GL_3D_DEPTH_SLICE_FALSE_ATI         0x00000040
#define GL_3D_DEPTH_SLICE_TRUE_ATI          0x00000080
#define GL_3D_DEPTH_SLICE_EITHER_ATI        0x000000c0

#define GL_MAX_SWAP_INTERVAL_ATI            16

#ifndef GL_CHAR_ARB_DEFINED
typedef char   GLcharARB;
#define GL_CHAR_ARB_DEFINED
#endif
typedef GLuint GLmem;

#ifdef GL_GLEXT_PROTOTYPES
GLAPI GLmem glAllocMem3DATI(GLenum format,
                                   GLsizei width, GLsizei height, GLsizei depth,
                                   GLsizei np, const GLint *properties);
GLAPI GLmem glAllocMem2DATI(GLenum format,
                                   GLsizei width, GLsizei height,
                                   GLsizei np, const GLint *properties);
GLAPI GLmem glAllocMem1DATI(GLenum format,
                                   GLsizei width,
                                   GLsizei np, const GLint *properties);
GLAPI GLvoid glGetMemInfoLogATI(GLmem mem,
                                       GLsizei maxLength,
                                       GLsizei* length,
                                       GLcharARB* infoLog);
GLAPI GLmem glCloneMemATI(GLmem mem);
GLAPI GLvoid glDeleteMemATI(GLmem mem);
GLAPI GLint glGetMemPropertyATI(GLmem mem, GLenum property);
GLAPI GLmem glGetSubMemATI(GLmem mem, GLenum type, GLuint child);
GLAPI GLmem glGetBaseMemATI(GLmem mem);
GLAPI GLmem glGetMemATI(GLenum target);
GLAPI GLboolean glIsMemATI(GLmem mem);
GLAPI GLvoid glMemImageATI(GLmem mem, GLenum format, GLenum type, GLvoid *data);
GLAPI GLvoid glCopyMemImage2DATI(GLmem mem, GLint x, GLint y);
GLAPI GLvoid glCopyMemImage1DATI(GLmem mem, GLint x, GLint y);
GLAPI GLvoid glMemSubImage3DATI(GLmem mem,
                                       GLint xoffset, GLint yoffset, GLint zoffset,
                                       GLsizei width, GLsizei height, GLsizei depth,
                                       GLenum format, GLenum type,
                                       GLvoid *data);
GLAPI GLvoid glMemSubImage2DATI(GLmem mem,
                                       GLint xoffset, GLint yoffset,
                                       GLsizei width, GLsizei height,
                                       GLenum format, GLenum type,
                                       GLvoid *data);
GLAPI GLvoid glMemSubImage1DATI(GLmem mem,
                                       GLint xoffset,
                                       GLsizei width,
                                       GLenum format, GLenum type,
                                       GLvoid *data);
GLAPI GLvoid glCopyMemSubImage2DATI(GLmem mem,
                                           GLint xoffset, GLint yoffset,
                                           GLint x, GLint y,
                                           GLsizei width, GLsizei height);
GLAPI GLvoid glCopyMemSubImage1DATI(GLmem mem,
                                           GLint xoffset,
                                           GLint x, GLint y,
                                           GLsizei width);
GLAPI GLvoid glMemCopy3DATI(GLmem src, GLint xsrc, GLint ysrc, GLint zsrc,
                                   GLmem dst, GLint xdst, GLint ydst, GLint zdst,
                                   GLsizei width, GLsizei height, GLsizei depth);
GLAPI GLvoid glMemCopy2DATI(GLmem src, GLint xsrc, GLint ysrc,
                                   GLmem dst, GLint xdst, GLint ydst,
                                   GLsizei width, GLsizei height);
GLAPI GLvoid glMemCopy1DATI(GLmem src, GLint xsrc,
                                   GLmem dst, GLint xdst,
                                   GLsizei width);
GLAPI GLvoid glGetMemImageATI(GLmem mem, GLenum format, GLenum type, GLvoid *data);
GLAPI GLvoid glGetMemSubImage3DATI(GLmem mem,
                                          GLint xoffset, GLint yoffset, GLint zoffset,
                                          GLsizei width, GLsizei height, GLsizei depth,
                                          GLenum format, GLenum type,
                                          GLvoid *data);
GLAPI GLvoid glGetMemSubImage2DATI(GLmem mem,
                                          GLint xoffset, GLint yoffset,
                                          GLsizei width, GLsizei height,
                                          GLenum format, GLenum type,
                                          GLvoid *data);
GLAPI GLvoid glGetMemSubImage1DATI(GLmem mem,
                                          GLint xoffset,
                                          GLsizei width,
                                          GLenum format, GLenum type,
                                          GLvoid *data);
GLAPI GLvoid glInvalidateMemATI(GLmem mem);
GLAPI GLvoid glAttachMemATI(GLenum target,
                                   GLenum attachment,
                                   GLmem mem);
GLAPI GLboolean glDetachMemATI(GLmem mem);

GLAPI GLuint glCreateFramebufferATI(GLvoid);
GLAPI GLvoid glDeleteFramebufferATI(GLuint framebuffer);
GLAPI GLvoid glBindFramebufferATI(GLenum target, GLuint framebuffer);
GLAPI GLuint glGetFramebufferATI(GLenum target);
GLAPI GLboolean glIsFramebufferATI(GLuint framebuffer);
GLAPI GLvoid glFramebufferParameterfATI(GLenum target, GLenum pname, GLfloat param);
GLAPI GLvoid glFramebufferParameterfvATI(GLenum target, GLenum pname, const GLfloat *params);
GLAPI GLvoid glFramebufferParameteriATI(GLenum target, GLenum pname, GLint param);
GLAPI GLvoid glFramebufferParameterivATI(GLenum target, GLenum pname, const GLint *params);
GLAPI GLvoid glGetFramebufferParameterfvATI(GLenum target, GLenum pname, GLfloat *params);
GLAPI GLvoid glGetFramebufferParameterivATI(GLenum target, GLenum pname, GLint *params);
GLAPI GLvoid glSwapBuffersATI(GLvoid);
GLAPI GLvoid glVertexArrayMemATI(GLenum array, GLint size, GLmem mem, GLuint offset);
GLAPI GLvoid glVertexAttribMemATI(GLuint index, GLint size, GLboolean normalized,
                                         GLmem mem, GLuint offset);
GLAPI GLvoid glDrawMemElementsATI(GLenum mode, GLsizei count, GLmem mem, GLuint offset);
GLAPI GLvoid glMultiDrawMemElementsATI(GLenum mode, GLsizei *count, GLmem *mems,
                                              GLuint *offsets, GLsizei primcount);
#endif /* GL_GLEXT_PROTOTYPES */

typedef GLmem             (APIENTRY *PFNGLALLOCMEM3DATIPROC)(GLenum format,
                                                             GLsizei width, GLsizei height, GLsizei depth,
                                                             GLsizei np, const GLint *properties);
typedef GLmem             (APIENTRY *PFNGLALLOCMEM2DATIPROC)(GLenum format,
                                                             GLsizei width, GLsizei height,
                                                             GLsizei np, const GLint *properties);
typedef GLmem             (APIENTRY *PFNGLALLOCMEM1DATIPROC)(GLenum format,
                                                             GLsizei width,
                                                             GLsizei np, const GLint *properties);
typedef GLvoid            (APIENTRY *PFNGLGETMEMINFOLOGATIPROC)(GLmem mem,
                                                                GLsizei maxLength,
                                                                GLsizei* length,
                                                                GLcharARB* infoLog);
typedef GLmem             (APIENTRY *PFNGLCLONEMEMATIPROC)(GLmem mem);
typedef GLvoid            (APIENTRY *PFNGLDELETEMEMATIPROC)(GLmem mem);
typedef GLint             (APIENTRY *PFNGLGETMEMPROPERTYATIPROC)(GLmem mem, GLenum property);
typedef GLmem             (APIENTRY *PFNGLGETSUBMEMATIPROC)(GLmem mem, GLenum type, GLuint child);
typedef GLmem             (APIENTRY *PFNGLGETBASEMEMATIPROC)(GLmem mem);
typedef GLmem             (APIENTRY *PFNGLGETMEMATIPROC)(GLenum target);
typedef GLboolean         (APIENTRY *PFNGLISMEMATIPROC)(GLmem mem);
typedef GLvoid            (APIENTRY *PFNGLMEMIMAGEATIPROC)(GLmem mem, GLenum format, GLenum type, GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLCOPYMEMIMAGE2DATIPROC)(GLmem mem, GLint x, GLint y);
typedef GLvoid            (APIENTRY *PFNGLCOPYMEMIMAGE1DATIPROC)(GLmem mem, GLint x, GLint y);
typedef GLvoid            (APIENTRY *PFNGLMEMSUBIMAGE3DATIPROC)(GLmem mem,
                                                                GLint xoffset, GLint yoffset, GLint zoffset,
                                                                GLsizei width, GLsizei height, GLsizei depth,
                                                                GLenum format, GLenum type,
                                                                GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLMEMSUBIMAGE2DATIPROC)(GLmem mem,
                                                                GLint xoffset, GLint yoffset,
                                                                GLsizei width, GLsizei height,
                                                                GLenum format, GLenum type,
                                                                GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLMEMSUBIMAGE1DATIPROC)(GLmem mem,
                                                                GLint xoffset,
                                                                GLsizei width,
                                                                GLenum format, GLenum type,
                                                                GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLCOPYMEMSUBIMAGE2DATIPROC)(GLmem mem,
                                                                    GLint xoffset, GLint yoffset,
                                                                    GLint x, GLint y,
                                                                    GLsizei width, GLsizei height);
typedef GLvoid            (APIENTRY *PFNGLCOPYMEMSUBIMAGE1DATIPROC)(GLmem mem,
                                                                    GLint xoffset,
                                                                    GLint x, GLint y,
                                                                    GLsizei width);
typedef GLvoid            (APIENTRY *PFNGLMEMCOPY3DATIPROC)(GLmem src, GLint xsrc, GLint ysrc, GLint zsrc,
                                                            GLmem dst, GLint xdst, GLint ydst, GLint zdst,
                                                            GLsizei width, GLsizei height, GLsizei depth);
typedef GLvoid            (APIENTRY *PFNGLMEMCOPY2DATIPROC)(GLmem src, GLint xsrc, GLint ysrc,
                                                            GLmem dst, GLint xdst, GLint ydst,
                                                            GLsizei width, GLsizei height);
typedef GLvoid            (APIENTRY *PFNGLMEMCOPY1DATIPROC)(GLmem src, GLint xsrc,
                                                            GLmem dst, GLint xdst,
                                                            GLsizei width);
typedef GLvoid            (APIENTRY *PFNGLGETMEMIMAGEATIPROC)(GLmem mem, GLenum format, GLenum type, GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLGETMEMSUBIMAGE3DATIPROC)(GLmem mem,
                                                                   GLint xoffset, GLint yoffset, GLint zoffset,
                                                                   GLsizei width, GLsizei height, GLsizei depth,
                                                                   GLenum format, GLenum type,
                                                                   GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLGETMEMSUBIMAGE2DATIPROC)(GLmem mem,
                                                                   GLint xoffset, GLint yoffset,
                                                                   GLsizei width, GLsizei height,
                                                                   GLenum format, GLenum type,
                                                                   GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLGETMEMSUBIMAGE1DATIPROC)(GLmem mem,
                                                                   GLint xoffset,
                                                                   GLsizei width,
                                                                   GLenum format, GLenum type,
                                                                   GLvoid *data);
typedef GLvoid            (APIENTRY *PFNGLINVALIDATEMEMATIPROC)(GLmem mem);
typedef GLvoid            (APIENTRY *PFNGLATTACHMEMATIPROC)(GLenum target,
							    GLenum attachment,
							    GLmem mem);
typedef GLboolean         (APIENTRY *PFNGLDETACHMEMATIPROC)(GLmem mem);

typedef GLuint            (APIENTRY *PFNGLCREATEFRAMEBUFFERATIPROC)(GLvoid);
typedef GLvoid            (APIENTRY *PFNGLDELETEFRAMEBUFFERATIPROC)(GLuint framebuffer);
typedef GLvoid            (APIENTRY *PFNGLBINDFRAMEBUFFERATIPROC)(GLenum target, GLuint framebuffer);
typedef GLuint            (APIENTRY *PFNGLGETFRAMEBUFFERATIPROC)(GLenum target);
typedef GLboolean         (APIENTRY *PFNGLISFRAMEBUFFERATIPROC)(GLuint framebuffer);
typedef GLvoid            (APIENTRY *PFNGLFRAMEBUFFERPARAMETERFATIPROC)(GLenum target, GLenum pname, GLfloat param);
typedef GLvoid            (APIENTRY *PFNGLFRAMEBUFFERPARAMETERFVATIPROC)(GLenum target, GLenum pname, const GLfloat *params);
typedef GLvoid            (APIENTRY *PFNGLFRAMEBUFFERPARAMETERIATIPROC)(GLenum target, GLenum pname, GLint param);
typedef GLvoid            (APIENTRY *PFNGLFRAMEBUFFERPARAMETERIVATIPROC)(GLenum target, GLenum pname, const GLint *params);
typedef GLvoid            (APIENTRY *PFNGLGETFRAMEBUFFERPARAMETERFVATIPROC)(GLenum target, GLenum pname, GLfloat *params);
typedef GLvoid            (APIENTRY *PFNGLGETFRAMEBUFFERPARAMETERIVATIPROC)(GLenum target, GLenum pname, GLint *params);
typedef GLvoid            (APIENTRY *PFNGLSWAPBUFFERSATIPROC)(GLvoid);
typedef GLvoid            (APIENTRY *PFNGLVERTEXARRAYMEMATIPROC)(GLenum array, GLint size, GLmem mem, GLuint offset);
typedef GLvoid            (APIENTRY *PFNGLVERTEXATTRIBMEMATIPROC)(GLuint index, GLint size, GLboolean normalized,
                                                                  GLmem mem, GLuint offset);
typedef GLvoid            (APIENTRY *PFNGLDRAWMEMELEMENTSATIPROC)(GLenum mode, GLsizei count, GLmem mem, GLuint offset);
typedef GLvoid            (APIENTRY *PFNGLMULTIDRAWMEMELEMENTSATIPROC)(GLenum mode, GLsizei *count, GLmem *mems,
                                                                       GLuint *offsets, GLsizei primcount);


#endif /* GL_ATI_uber_buffers */

#ifdef __cplusplus
}
#endif

#endif
