AM_CXXFLAGS = -O0 -g
INCLUDES = $(LTDLINCL)

lib_LTLIBRARIES = libsh.la

libsh_la_LIBADD = $(LIBLTDL)
libsh_la_LDFLAGS = -export-dynamic -no-undefined

libsh_la_SOURCES = ShBackend.cpp ShBasicBlock.cpp ShBlock.cpp ShCtrlGraph.cpp \
                   ShEnvironment.cpp ShException.cpp ShParser.cpp ShProgram.cpp \
                   ShStatement.cpp ShSwizzle.cpp ShSyntax.cpp ShToken.cpp \
                   ShTokenizer.cpp ShUtility.cpp ShVariable.cpp ShVariableNode.cpp \
		   ShTextureNode.cpp ShImage.cpp \
                   ShLinearAllocator.cpp \
                   ShDomTree.cpp \
                   ShOptimizer.cpp \
		   ShAttrib.hpp ShAttribImpl.hpp ShBackend.hpp ShBasicBlock.hpp \
                   ShBlock.hpp ShConstant.hpp ShConstantImpl.hpp ShCtrlGraph.hpp \
                   ShDebug.hpp ShEnvironment.hpp ShException.hpp ShLib.hpp \
                   ShParser.hpp ShRefCount.hpp ShProgram.hpp ShStatement.hpp \
                   ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp \
                   ShUtility.hpp ShVariable.hpp ShVariableNode.hpp sh.hpp \
                   ShMatrix.hpp ShMatrixImpl.hpp ShTexture.hpp ShTextureImpl.hpp \
                   ShTextureNode.hpp ShImage.hpp ShLinearAllocator.hpp \
                   ShLibAttrib.hpp ShSpecialType.hpp \
                   ShVector.hpp ShLibVector.hpp \
                   ShPoint.hpp ShLibPoint.hpp \
		   ShPosition.hpp ShLibPosition.hpp \
                   ShColor.hpp ShLibColor.hpp \
                   ShNormal.hpp ShLibNormal.hpp \
                   ShTexCoord.hpp ShLibTexCoord.hpp \
                   ShDomTree.hpp ShOptimizer.hpp

incincdir = $(includedir)/sh

incinc_HEADERS = ShAttrib.hpp ShAttribImpl.hpp ShBackend.hpp ShBasicBlock.hpp \
                   ShBlock.hpp ShConstant.hpp ShConstantImpl.hpp ShCtrlGraph.hpp \
                   ShDebug.hpp ShEnvironment.hpp ShException.hpp ShLib.hpp \
                   ShParser.hpp ShRefCount.hpp ShProgram.hpp ShStatement.hpp \
                   ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp \
                   ShUtility.hpp ShVariable.hpp ShVariableNode.hpp sh.hpp \
                   ShMatrix.hpp ShMatrixImpl.hpp ShTexture.hpp ShTextureImpl.hpp \
                   ShTextureNode.hpp ShImage.hpp ShLinearAllocator.hpp \
                   ShLibAttrib.hpp ShSpecialType.hpp \
                   ShVector.hpp ShLibVector.hpp \
                   ShPoint.hpp ShLibPoint.hpp \
                   ShColor.hpp ShLibColor.hpp \
		   ShPosition.hpp ShLibPosition.hpp \
                   ShNormal.hpp ShLibNormal.hpp \
                   ShTexCoord.hpp ShLibTexCoord.hpp \
                   ShDomTree.hpp ShOptimizer.hpp 