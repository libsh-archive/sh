INCLUDES = $(LTDLINCL)

lib_LTLIBRARIES = libsh.la

libsh_la_LIBADD = $(LIBLTDL)
libsh_la_LDFLAGS = -export-dynamic -no-undefined

GENERATED  = ShAttrib.hpp ShAttribImpl.hpp ShAttrib.cpp
GENERATED += ShVector.hpp ShVectorImpl.hpp ShVector.cpp
GENERATED += ShPoint.hpp ShPointImpl.hpp ShPoint.cpp
GENERATED += ShColor.hpp ShColorImpl.hpp ShColor.cpp
GENERATED += ShPosition.hpp ShPositionImpl.hpp ShPosition.cpp
GENERATED += ShNormal.hpp ShNormalImpl.hpp ShNormal.cpp
GENERATED += ShTexCoord.hpp ShTexCoordImpl.hpp ShTexCoord.cpp

libsh_la_SOURCES = ShBackend.cpp ShBasicBlock.cpp ShBlock.cpp ShCtrlGraph.cpp \
                   ShEnvironment.cpp ShException.cpp ShError.cpp \
                   ShParser.cpp ShProgram.cpp \
                   ShStatement.cpp ShSwizzle.cpp ShSyntax.cpp ShToken.cpp \
                   ShTokenizer.cpp ShUtility.cpp ShVariable.cpp ShVariableNode.cpp \
		   ShTextureNode.cpp ShImage.cpp ShImage3D.cpp \
                   ShLinearAllocator.cpp \
                   ShDomTree.cpp \
                   ShOptimizer.cpp \
                   ShTransformer.cpp \
                   ShInternals.hpp ShInternals.cpp \
		   ShAttrib.hpp ShAttribImpl.hpp ShAttrib.cpp \
		   ShBackend.hpp ShBasicBlock.hpp \
                   ShBlock.hpp ShCtrlGraph.hpp \
                   ShDebug.hpp ShEnvironment.hpp ShError.hpp \
                   ShException.hpp ShLib.hpp \
		   ShLibArith.hpp ShLibArithImpl.hpp \
		   ShLibBoolean.hpp ShLibBooleanImpl.hpp \
		   ShLibClamp.hpp ShLibClampImpl.hpp \
		   ShLibGeometry.hpp ShLibGeometryImpl.hpp \
		   ShLibMatrix.hpp ShLibMatrixImpl.hpp \
		   ShLibMisc.hpp ShLibMiscImpl.hpp \
		   ShLibTrig.hpp ShLibTrigImpl.hpp \
                   ShParser.hpp ShRefCount.hpp ShProgram.hpp ShStatement.hpp \
                   ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp \
                   ShUtility.hpp ShVariable.hpp ShVariableNode.hpp sh.hpp \
		   ShGeneric.hpp ShGenericImpl.hpp ShGeneric.cpp \
                   ShMatrix.hpp ShMatrixImpl.hpp \
                   ShBaseTexture.hpp ShBaseTextureImpl.hpp \
                   ShArray.hpp ShTable.hpp ShTexture.hpp \
                   ShWrap.hpp \
                   ShTextureNode.hpp ShImage.hpp ShImage3D.hpp \
		   ShLinearAllocator.hpp \
                   ShLibAttrib.hpp \
                   ShVector.hpp ShVectorImpl.hpp ShVector.cpp ShLibVector.hpp \
                   ShPoint.hpp ShPointImpl.hpp ShPoint.cpp ShLibPoint.hpp \
		   ShPosition.hpp ShPositionImpl.hpp ShPosition.cpp ShLibPosition.hpp \
                   ShColor.hpp ShColorImpl.hpp ShColor.cpp ShLibColor.hpp \
                   ShNormal.hpp ShNormalImpl.hpp ShNormal.cpp ShLibNormal.hpp \
                   ShTexCoord.hpp ShTexCoordImpl.hpp ShTexCoord.cpp ShLibTexCoord.hpp \
                   ShDomTree.hpp ShOptimizer.hpp ShTransformer.hpp ShBitSet.hpp ShBitSet.cpp \
		   ShAlgebra.hpp ShAlgebra.cpp \
		   ShManipulator.hpp ShManipulatorImpl.hpp ShManipulator.cpp \
		   ShNibbles.hpp ShNibblesImpl.hpp \
		   ShFixedManipulator.hpp ShFixedManipulator.cpp \
		   ShChannelNode.hpp ShChannelNode.cpp \
		   ShChannel.hpp ShChannelImpl.hpp \
		   ShStream.hpp ShStreamImpl.hpp ShStream.cpp \
		   ShInit.cpp \
		   ShQuaternion.hpp ShQuaternionImpl.hpp \
		   ShMemory.hpp ShMemory.cpp ShClamping.hpp \
		   ShInstructions.hpp ShInstructions.cpp

incincdir = $(includedir)/sh

incinc_HEADERS = ShAttrib.hpp ShAttribImpl.hpp ShBackend.hpp ShBasicBlock.hpp \
                   ShBlock.hpp ShCtrlGraph.hpp \
                   ShDebug.hpp ShEnvironment.hpp ShError.hpp \
                   ShException.hpp ShLib.hpp \
		   ShLibArith.hpp ShLibArithImpl.hpp \
		   ShLibBoolean.hpp ShLibBooleanImpl.hpp \
		   ShLibClamp.hpp ShLibClampImpl.hpp \
		   ShLibGeometry.hpp ShLibGeometryImpl.hpp \
		   ShLibMatrix.hpp ShLibMatrixImpl.hpp \
		   ShLibMisc.hpp ShLibMiscImpl.hpp \
		   ShLibTrig.hpp ShLibTrigImpl.hpp \
                   ShParser.hpp ShRefCount.hpp ShRefCountImpl.hpp ShProgram.hpp ShStatement.hpp \
                   ShSwizzle.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp \
                   ShUtility.hpp \
		   ShVariable.hpp ShVariableNode.hpp \
		   ShGeneric.hpp ShGenericImpl.hpp \
		   sh.hpp shimpl.hpp \
                   ShMatrix.hpp ShMatrixImpl.hpp \
		   ShArray.hpp ShTable.hpp ShTexture.hpp \
		   ShWrap.hpp \
                   ShBaseTexture.hpp ShBaseTextureImpl.hpp \
                   ShTextureNode.hpp ShImage.hpp ShImage3D.hpp \
		   ShLinearAllocator.hpp \
                   ShLibAttrib.hpp \
                   ShVector.hpp ShVectorImpl.hpp ShLibVector.hpp \
                   ShPoint.hpp ShPointImpl.hpp ShLibPoint.hpp \
                   ShColor.hpp ShColorImpl.hpp ShLibColor.hpp \
		   ShPosition.hpp ShPositionImpl.hpp ShLibPosition.hpp \
                   ShNormal.hpp ShNormalImpl.hpp ShLibNormal.hpp \
                   ShTexCoord.hpp ShTexCoordImpl.hpp ShLibTexCoord.hpp \
                   ShDomTree.hpp ShOptimizer.hpp ShTransformer.hpp ShAlgebra.hpp ShInternals.hpp\
		   ShFixedManipulator.hpp \
		   ShManipulator.hpp ShManipulatorImpl.hpp \
		   ShNibbles.hpp ShNibblesImpl.hpp \
		   ShChannelNode.hpp ShChannel.hpp ShChannelImpl.hpp \
		   ShStream.hpp ShStreamImpl.hpp \
		   ShQuaternion.hpp ShQuaternionImpl.hpp \
		   ShMemory.hpp ShClamping.hpp \
		   ShInstructions.hpp

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
