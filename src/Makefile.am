INCLUDES = $(LTDLINCL)

EXTRA_DIST = Makefile.win32

lib_LTLIBRARIES = libsh.la

libsh_la_LIBADD = $(LIBLTDL)
libsh_la_LDFLAGS = -export-dynamic -no-undefined

incincdir = $(includedir)/sh
incinc_HEADERS  = ShAttrib.hpp ShAttribImpl.hpp ShBackend.hpp ShBasicBlock.hpp

# General/internals
libsh_la_SOURCES  = ShBasicBlock.cpp ShBlock.cpp ShCtrlGraph.cpp
libsh_la_SOURCES += ShEnvironment.cpp ShException.cpp ShError.cpp
libsh_la_SOURCES += ShParser.cpp 
libsh_la_SOURCES += ShProgram.cpp ShProgram.hpp
libsh_la_SOURCES += ShProgramNode.cpp ShProgramNode.hpp
libsh_la_SOURCES += ShStatement.cpp ShSwizzle.cpp ShSyntax.cpp ShToken.cpp
libsh_la_SOURCES += ShTokenizer.cpp ShUtility.cpp ShVariable.cpp ShVariableNode.cpp
libsh_la_SOURCES += ShInternals.hpp ShInternals.cpp
libsh_la_SOURCES += ShBasicBlock.hpp
libsh_la_SOURCES += ShBlock.hpp ShCtrlGraph.hpp
libsh_la_SOURCES += ShDebug.hpp ShEnvironment.hpp ShError.hpp
libsh_la_SOURCES += ShException.hpp
libsh_la_SOURCES += ShParser.hpp ShRefCount.hpp ShStatement.hpp
libsh_la_SOURCES += ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp
libsh_la_SOURCES += ShUtility.hpp ShVariable.hpp ShVariableNode.hpp sh.hpp
libsh_la_SOURCES += ShInstructions.hpp ShInstructions.cpp
libsh_la_SOURCES += ShMeta.hpp ShMeta.cpp
libsh_la_SOURCES += ShMetaForwarder.hpp ShMetaForwarder.cpp
libsh_la_SOURCES += ShContext.hpp ShContext.cpp
libsh_la_SOURCES += ShDllExport.hpp
incinc_HEADERS   += ShDllExport.hpp

# Backend
libsh_la_SOURCES += ShBackend.cpp ShBackend.hpp 
libsh_la_SOURCES += ShLinearAllocator.hpp ShLinearAllocator.cpp

# Optimizer and compiler internals
libsh_la_SOURCES += ShTransformer.hpp ShBitSet.hpp
libsh_la_SOURCES += ShTransformer.cpp ShBitSet.cpp
incinc_HEADERS   += ShTransformer.hpp ShBitSet.hpp
libsh_la_SOURCES += ShOptimizations.cpp ShOptimizations.hpp
incinc_HEADERS   += ShOptimizations.hpp
libsh_la_SOURCES += ShEvaluate.cpp ShEvaluate.hpp
incinc_HEADERS   += ShEvaluate.hpp
libsh_la_SOURCES += ShStructural.hpp ShStructural.cpp
incinc_HEADERS   += ShStructural.hpp

# Library functions
libsh_la_SOURCES += ShLib.hpp
libsh_la_SOURCES += ShLibArith.hpp ShLibArithImpl.hpp
libsh_la_SOURCES += ShLibBoolean.hpp ShLibBooleanImpl.hpp
libsh_la_SOURCES += ShLibClamp.hpp ShLibClampImpl.hpp
libsh_la_SOURCES += ShLibGeometry.hpp ShLibGeometryImpl.hpp
libsh_la_SOURCES += ShLibMatrix.hpp ShLibMatrixImpl.hpp
libsh_la_SOURCES += ShLibMisc.hpp ShLibMiscImpl.hpp
libsh_la_SOURCES += ShLibTrig.hpp ShLibTrigImpl.hpp
libsh_la_SOURCES += ShLibDeriv.hpp ShLibDerivImpl.hpp

# Textures and arrays
libsh_la_SOURCES += ShBaseTexture.hpp ShBaseTextureImpl.hpp ShBaseTexture.cpp
libsh_la_SOURCES += ShArray.hpp ShTable.hpp ShTexture.hpp
libsh_la_SOURCES += ShWrap.hpp ShClamping.hpp ShInterp.hpp
libsh_la_SOURCES += ShTextureNode.hpp ShImage.hpp ShImage3D.hpp
libsh_la_SOURCES += ShMemory.hpp ShMemory.cpp
libsh_la_SOURCES += ShTextureNode.cpp ShImage.cpp ShImage3D.cpp

# Types
libsh_la_SOURCES += ShGeneric.hpp ShGenericImpl.hpp
libsh_la_SOURCES += ShAttrib.hpp ShAttribImpl.hpp
libsh_la_SOURCES += ShLibAttrib.hpp
libsh_la_SOURCES += ShVector.hpp ShVectorImpl.hpp ShLibVector.hpp
libsh_la_SOURCES += ShPoint.hpp ShPointImpl.hpp ShLibPoint.hpp
libsh_la_SOURCES += ShPosition.hpp ShPositionImpl.hpp ShLibPosition.hpp
libsh_la_SOURCES += ShColor.hpp ShColorImpl.hpp ShLibColor.hpp
libsh_la_SOURCES += ShNormal.hpp ShNormalImpl.hpp ShLibNormal.hpp
libsh_la_SOURCES += ShTexCoord.hpp ShTexCoordImpl.hpp ShLibTexCoord.hpp
libsh_la_SOURCES += ShMatrix.hpp ShMatrixImpl.hpp
libsh_la_SOURCES += ShQuaternion.hpp ShQuaternionImpl.hpp

# Shader Algebra
libsh_la_SOURCES += ShAlgebra.hpp ShAlgebra.cpp
libsh_la_SOURCES += ShManipulator.hpp ShManipulatorImpl.hpp ShManipulator.cpp
libsh_la_SOURCES += ShNibbles.hpp ShNibblesImpl.hpp
libsh_la_SOURCES += ShFixedManipulator.hpp ShFixedManipulator.cpp

# Streams
libsh_la_SOURCES += ShChannelNode.hpp ShChannelNode.cpp
libsh_la_SOURCES += ShChannel.hpp ShChannelImpl.hpp
libsh_la_SOURCES += ShStream.hpp ShStreamImpl.hpp ShStream.cpp


incinc_HEADERS += ShBlock.hpp ShCtrlGraph.hpp
incinc_HEADERS += ShDebug.hpp ShEnvironment.hpp ShError.hpp
incinc_HEADERS += ShException.hpp ShLib.hpp
incinc_HEADERS += ShLibArith.hpp ShLibArithImpl.hpp
incinc_HEADERS += ShLibBoolean.hpp ShLibBooleanImpl.hpp
incinc_HEADERS += ShLibClamp.hpp ShLibClampImpl.hpp
incinc_HEADERS += ShLibGeometry.hpp ShLibGeometryImpl.hpp
incinc_HEADERS += ShLibMatrix.hpp ShLibMatrixImpl.hpp
incinc_HEADERS += ShLibMisc.hpp ShLibMiscImpl.hpp
incinc_HEADERS += ShLibTrig.hpp ShLibTrigImpl.hpp
incinc_HEADERS += ShLibDeriv.hpp ShLibDerivImpl.hpp
incinc_HEADERS += ShParser.hpp ShRefCount.hpp ShRefCountImpl.hpp 
incinc_HEADERS += ShProgram.hpp ShProgramNode.hpp ShStatement.hpp
incinc_HEADERS += ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp
incinc_HEADERS += ShUtility.hpp
incinc_HEADERS += ShVariable.hpp ShVariableNode.hpp
incinc_HEADERS += ShGeneric.hpp ShGenericImpl.hpp
incinc_HEADERS += sh.hpp
incinc_HEADERS += ShMatrix.hpp ShMatrixImpl.hpp
incinc_HEADERS += ShArray.hpp ShTable.hpp ShTexture.hpp
incinc_HEADERS += ShWrap.hpp
incinc_HEADERS += ShBaseTexture.hpp ShBaseTextureImpl.hpp
incinc_HEADERS += ShTextureNode.hpp ShImage.hpp ShImage3D.hpp
incinc_HEADERS += ShLinearAllocator.hpp
incinc_HEADERS += ShLibAttrib.hpp
incinc_HEADERS += ShVector.hpp ShVectorImpl.hpp ShLibVector.hpp
incinc_HEADERS += ShPoint.hpp ShPointImpl.hpp ShLibPoint.hpp
incinc_HEADERS += ShColor.hpp ShColorImpl.hpp ShLibColor.hpp
incinc_HEADERS += ShPosition.hpp ShPositionImpl.hpp ShLibPosition.hpp
incinc_HEADERS += ShNormal.hpp ShNormalImpl.hpp ShLibNormal.hpp
incinc_HEADERS += ShTexCoord.hpp ShTexCoordImpl.hpp ShLibTexCoord.hpp
incinc_HEADERS += ShAlgebra.hpp ShInternals.hpp
incinc_HEADERS += ShFixedManipulator.hpp
incinc_HEADERS += ShManipulator.hpp ShManipulatorImpl.hpp
incinc_HEADERS += ShNibbles.hpp ShNibblesImpl.hpp
incinc_HEADERS += ShChannelNode.hpp ShChannel.hpp ShChannelImpl.hpp
incinc_HEADERS += ShStream.hpp ShStreamImpl.hpp
incinc_HEADERS += ShQuaternion.hpp ShQuaternionImpl.hpp
incinc_HEADERS += ShMemory.hpp ShClamping.hpp ShInterp.hpp
incinc_HEADERS += ShInstructions.hpp
incinc_HEADERS += ShMeta.hpp ShMetaForwarder.hpp
incinc_HEADERS += ShContext.hpp

# Generated by python programs in scripts/
GENERATED  = ShAttrib.hpp ShAttribImpl.hpp # ShAttrib.cpp
GENERATED += ShVector.hpp ShVectorImpl.hpp # ShVector.cpp
GENERATED += ShPoint.hpp ShPointImpl.hpp # ShPoint.cpp
GENERATED += ShColor.hpp ShColorImpl.hpp # ShColor.cpp
GENERATED += ShPosition.hpp ShPositionImpl.hpp # ShPosition.cpp
GENERATED += ShNormal.hpp ShNormalImpl.hpp # ShNormal.cpp
GENERATED += ShTexCoord.hpp ShTexCoordImpl.hpp # ShTexCoord.cpp

%.hpp: scripts/%.hpp.py scripts/common.py scripts/semantic.py
	rm -f $@
	python $< > $@
	chmod -w $@

%.cpp: scripts/%.cpp.py scripts/common.py scripts/semantic.py
	rm -f $@
	python $< > $@
	chmod -w $@

%.o: $(GENERATED)
%.Plo: $(GENERATED)

clean-generated:
	rm -f $(GENERATED)
