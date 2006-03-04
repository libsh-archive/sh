INCLUDES = $(LTDLINCL)

lib_LTLIBRARIES = libsh.la

libsh_la_LIBADD = $(LIBLTDL)
libsh_la_LDFLAGS = -export-dynamic -no-undefined

incincdir = $(includedir)/sh
incinc_HEADERS  = ShAttrib.hpp ShAttribImpl.hpp ShBackend.hpp ShBasicBlock.hpp

# General/internals
libsh_la_SOURCES  = ShBasicBlock.cpp ShBlock.cpp ShCtrlGraph.cpp
libsh_la_SOURCES += ShException.cpp ShError.cpp
libsh_la_SOURCES += ShParser.cpp 
libsh_la_SOURCES += ShProgram.cpp ShProgram.hpp
libsh_la_SOURCES += ShProgramNode.cpp ShProgramNode.hpp
libsh_la_SOURCES += ShProgramSet.cpp ShProgramSet.hpp
incinc_HEADERS   += ShProgramSet.hpp
libsh_la_SOURCES += ShRecord.hpp ShRecord.cpp 
incinc_HEADERS	 += ShRecord.hpp 
libsh_la_SOURCES += ShInfo.hpp ShInfoImpl.hpp ShInfo.cpp
incinc_HEADERS   += ShInfo.hpp ShInfoImpl.hpp
libsh_la_SOURCES += ShStatement.cpp ShSwizzle.cpp ShSyntax.cpp ShToken.cpp
libsh_la_SOURCES += ShTokenizer.cpp ShUtility.cpp ShVariable.cpp ShVariableNode.cpp
libsh_la_SOURCES += ShOperation.hpp ShOperation.cpp 
libsh_la_SOURCES += ShInternals.hpp ShInternals.cpp
libsh_la_SOURCES += ShBasicBlock.hpp
libsh_la_SOURCES += ShBlock.hpp ShCtrlGraph.hpp
libsh_la_SOURCES += ShCfgBlock.hpp ShCfgBlock.cpp 
incinc_HEADERS 	 += ShCfgBlock.hpp 
libsh_la_SOURCES += ShDebug.hpp ShError.hpp
libsh_la_SOURCES += ShException.hpp
libsh_la_SOURCES += ShParser.hpp ShRefCount.hpp ShStatement.hpp
libsh_la_SOURCES += ShSwizzle.hpp ShSwizzleImpl.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp
libsh_la_SOURCES += ShUtility.hpp ShVariable.hpp ShVariableNode.hpp sh.hpp
libsh_la_SOURCES += ShHashMap.hpp 
libsh_la_SOURCES += ShGraph.hpp ShGraphImpl.hpp 
libsh_la_SOURCES += ShInstructions.hpp ShInstructions.cpp
libsh_la_SOURCES += ShMeta.hpp ShMetaImpl.hpp
libsh_la_SOURCES += ShMetaForwarder.hpp ShMetaForwarder.cpp
libsh_la_SOURCES += ShContext.hpp ShContext.cpp
libsh_la_SOURCES += ShMath.hpp 
incinc_HEADERS   += ShMath.hpp 
libsh_la_SOURCES += ShDllExport.hpp
incinc_HEADERS   += ShDllExport.hpp
libsh_la_SOURCES += ShPool.hpp ShPool.cpp
incinc_HEADERS   += ShPool.hpp

# Backend
libsh_la_SOURCES += ShBackend.cpp ShBackend.hpp 
libsh_la_SOURCES += ShLinearAllocator.hpp

# Optimizer and compiler internals
libsh_la_SOURCES += ShTransformer.hpp ShBitSet.hpp
libsh_la_SOURCES += ShTransformer.cpp ShBitSet.cpp
incinc_HEADERS   += ShTransformer.hpp ShBitSet.hpp
libsh_la_SOURCES += ShTypeConvertTransformer.cpp 
libsh_la_SOURCES += ShTransformerImpl.hpp 
incinc_HEADERS	 += ShTransformerImpl.hpp 
libsh_la_SOURCES += ShOptimizations.cpp ShOptimizations.hpp
incinc_HEADERS   += ShOptimizations.hpp
libsh_la_SOURCES += ShValueTracking.cpp ShConstProp.cpp
libsh_la_SOURCES += ShEvaluate.cpp ShEvaluate.hpp
incinc_HEADERS   += ShEvaluate.hpp
libsh_la_SOURCES += ShStructural.hpp ShStructural.cpp
incinc_HEADERS   += ShStructural.hpp
libsh_la_SOURCES += ShSection.hpp ShSectionImpl.hpp ShSection.cpp 
incinc_HEADERS   += ShSection.hpp ShSectionImpl.hpp

# Library functions
libsh_la_SOURCES += ShLib.hpp
libsh_la_SOURCES += ShLibArith.hpp ShLibArithImpl.hpp
libsh_la_SOURCES += ShLibBoolean.hpp ShLibBooleanImpl.hpp
libsh_la_SOURCES += ShLibClamp.hpp ShLibClampImpl.hpp
libsh_la_SOURCES += ShLibGeometry.hpp ShLibGeometryImpl.hpp
libsh_la_SOURCES += ShLibMatrix.hpp ShLibMatrixImpl.hpp
libsh_la_SOURCES += ShLibMisc.hpp ShLibMiscImpl.hpp
libsh_la_SOURCES += ShLibSplines.hpp ShLibSplinesImpl.hpp
libsh_la_SOURCES += ShLibTrig.hpp ShLibTrigImpl.hpp
libsh_la_SOURCES += ShLibDeriv.hpp ShLibDerivImpl.hpp
libsh_la_SOURCES += ShLibNoise.hpp ShLibNoiseImpl.hpp
libsh_la_SOURCES += ShLibWorley.hpp ShLibWorleyImpl.hpp

# Textures and arrays
libsh_la_SOURCES += ShBaseTexture.hpp ShBaseTextureImpl.hpp ShBaseTexture.cpp
libsh_la_SOURCES += ShArray.hpp ShTable.hpp ShTexture.hpp
libsh_la_SOURCES += ShWrap.hpp ShInterp.hpp ShMIPFilter.hpp
libsh_la_SOURCES += ShTextureNode.hpp ShImage.hpp ShImageImpl.hpp ShImage3D.hpp
libsh_la_SOURCES += ShMemory.hpp ShMemory.cpp ShMemoryDep.hpp
libsh_la_SOURCES += ShTextureNode.cpp ShImage3D.cpp
libsh_la_SOURCES += ShTexData.hpp ShTexDataImpl.hpp

# Types
libsh_la_SOURCES += ShVariableType.hpp ShVariableType.cpp
libsh_la_SOURCES += ShTypeInfo.hpp ShTypeInfoImpl.hpp ShTypeInfo.cpp
libsh_la_SOURCES += ShTypeInfoCasts.cpp ShTypeInfoOps.cpp 
libsh_la_SOURCES += ShStorageType.hpp ShStorageTypeImpl.hpp ShStorageType.cpp 
incinc_HEADERS   += ShStorageType.hpp ShStorageTypeImpl.hpp 
libsh_la_SOURCES += ShDataType.hpp ShDataTypeImpl.hpp ShDataType.cpp
incinc_HEADERS   += ShDataType.hpp ShDataTypeImpl.hpp 
libsh_la_SOURCES += ShGeneric.hpp ShGenericImpl.hpp
libsh_la_SOURCES += ShAttrib.hpp ShAttribImpl.hpp
libsh_la_SOURCES += ShLibAttrib.hpp
libsh_la_SOURCES += ShVector.hpp ShLibVector.hpp
libsh_la_SOURCES += ShPoint.hpp ShLibPoint.hpp
libsh_la_SOURCES += ShPosition.hpp ShLibPosition.hpp
libsh_la_SOURCES += ShPlane.hpp ShLibPlane.hpp
libsh_la_SOURCES += ShColor.hpp ShLibColor.hpp
libsh_la_SOURCES += ShNormal.hpp ShLibNormal.hpp
libsh_la_SOURCES += ShTexCoord.hpp ShLibTexCoord.hpp
libsh_la_SOURCES += ShMatrix.hpp ShMatrixImpl.hpp
libsh_la_SOURCES += ShQuaternion.hpp ShQuaternionImpl.hpp
libsh_la_SOURCES += ShVariant.hpp ShVariantImpl.hpp ShVariant.cpp
libsh_la_SOURCES += ShVariantFactory.hpp ShVariantFactoryImpl.hpp 
libsh_la_SOURCES += ShVariantCast.hpp ShVariantCastImpl.hpp 
libsh_la_SOURCES += ShCastManager.hpp ShCastManager.cpp 
libsh_la_SOURCES += ShEval.hpp ShEvalImpl.hpp ShEval.cpp
libsh_la_SOURCES += ShConcreteRegularOpImpl.hpp 
libsh_la_SOURCES += ShConcreteCTypeOpImpl.hpp  ShConcreteCTypeOp.cpp
libsh_la_SOURCES += ShHalf.hpp ShHalfImpl.hpp 
libsh_la_SOURCES += ShFraction.hpp ShFractionImpl.hpp 


# Shader Algebra
libsh_la_SOURCES += ShAlgebra.hpp ShAlgebra.cpp
libsh_la_SOURCES += ShManipulator.hpp ShManipulatorImpl.hpp ShManipulator.cpp
libsh_la_SOURCES += ShNibbles.hpp ShNibblesImpl.hpp
libsh_la_SOURCES += ShFixedManipulator.hpp ShFixedManipulator.cpp

# Streams
libsh_la_SOURCES += ShChannelNode.hpp ShChannelNode.cpp
libsh_la_SOURCES += ShChannel.hpp ShChannelImpl.hpp
libsh_la_SOURCES += ShStream.hpp ShStreamImpl.hpp ShStream.cpp

# Palettes
libsh_la_SOURCES += ShPalette.hpp ShPaletteImpl.hpp
libsh_la_SOURCES += ShPaletteNode.hpp ShPaletteNode.cpp
incinc_HEADERS   += ShPalette.hpp ShPaletteImpl.hpp ShPaletteNode.hpp

# These should really be moved up top
incinc_HEADERS += ShBlock.hpp ShCtrlGraph.hpp
incinc_HEADERS += ShDebug.hpp ShError.hpp
incinc_HEADERS += ShException.hpp ShLib.hpp
incinc_HEADERS += ShLibArith.hpp ShLibArithImpl.hpp
incinc_HEADERS += ShLibBoolean.hpp ShLibBooleanImpl.hpp
incinc_HEADERS += ShLibClamp.hpp ShLibClampImpl.hpp
incinc_HEADERS += ShLibGeometry.hpp ShLibGeometryImpl.hpp
incinc_HEADERS += ShLibMatrix.hpp ShLibMatrixImpl.hpp
incinc_HEADERS += ShLibMisc.hpp ShLibMiscImpl.hpp
incinc_HEADERS += ShLibSplines.hpp ShLibSplinesImpl.hpp
incinc_HEADERS += ShLibTrig.hpp ShLibTrigImpl.hpp
incinc_HEADERS += ShLibDeriv.hpp ShLibDerivImpl.hpp
incinc_HEADERS += ShLibNoise.hpp ShLibNoiseImpl.hpp
incinc_HEADERS += ShLibWorley.hpp ShLibWorleyImpl.hpp
incinc_HEADERS += ShParser.hpp ShRefCount.hpp ShRefCountImpl.hpp 
incinc_HEADERS += ShProgram.hpp ShProgramNode.hpp ShStatement.hpp 
incinc_HEADERS += ShSwizzle.hpp ShSwizzleImpl.hpp ShSyntax.hpp ShToken.hpp ShTokenizer.hpp
incinc_HEADERS += ShUtility.hpp
incinc_HEADERS += ShHashMap.hpp
incinc_HEADERS += ShGraph.hpp ShGraphImpl.hpp 
incinc_HEADERS += ShVariable.hpp ShVariableNode.hpp
incinc_HEADERS += ShOperation.hpp ShVariableType.hpp 
incinc_HEADERS += ShGeneric.hpp ShGenericImpl.hpp
incinc_HEADERS += sh.hpp
incinc_HEADERS += ShMatrix.hpp ShMatrixImpl.hpp
incinc_HEADERS += ShArray.hpp ShTable.hpp ShTexture.hpp
incinc_HEADERS += ShWrap.hpp
incinc_HEADERS += ShBaseTexture.hpp ShBaseTextureImpl.hpp
incinc_HEADERS += ShTextureNode.hpp ShImage.hpp ShImageImpl.hpp ShImage3D.hpp
incinc_HEADERS += ShTexData.hpp ShTexDataImpl.hpp
incinc_HEADERS += ShLinearAllocator.hpp
incinc_HEADERS += ShLibAttrib.hpp
incinc_HEADERS += ShVector.hpp ShLibVector.hpp
incinc_HEADERS += ShPoint.hpp ShLibPoint.hpp
incinc_HEADERS += ShColor.hpp ShLibColor.hpp
incinc_HEADERS += ShPlane.hpp ShLibPlane.hpp
incinc_HEADERS += ShPosition.hpp ShLibPosition.hpp
incinc_HEADERS += ShNormal.hpp ShLibNormal.hpp
incinc_HEADERS += ShTexCoord.hpp ShLibTexCoord.hpp
incinc_HEADERS += ShAlgebra.hpp ShInternals.hpp
incinc_HEADERS += ShFixedManipulator.hpp
incinc_HEADERS += ShManipulator.hpp ShManipulatorImpl.hpp
incinc_HEADERS += ShNibbles.hpp ShNibblesImpl.hpp
incinc_HEADERS += ShChannelNode.hpp ShChannel.hpp ShChannelImpl.hpp
incinc_HEADERS += ShStream.hpp ShStreamImpl.hpp
incinc_HEADERS += ShQuaternion.hpp ShQuaternionImpl.hpp
incinc_HEADERS += ShTypeInfo.hpp ShTypeInfoImpl.hpp 
incinc_HEADERS += ShVariant.hpp ShVariantImpl.hpp ShEval.hpp ShEvalImpl.hpp
incinc_HEADERS += ShVariantFactory.hpp ShVariantFactoryImpl.hpp 
incinc_HEADERS += ShVariantCast.hpp ShVariantCastImpl.hpp 
incinc_HEADERS += ShCastManager.hpp 
incinc_HEADERS += ShConcreteRegularOpImpl.hpp ShConcreteCTypeOpImpl.hpp 
incinc_HEADERS += ShHalf.hpp ShHalfImpl.hpp 
incinc_HEADERS += ShFraction.hpp ShFractionImpl.hpp 
incinc_HEADERS += ShMemory.hpp ShInterp.hpp ShMemoryDep.hpp ShMIPFilter.hpp
incinc_HEADERS += ShInstructions.hpp
incinc_HEADERS += ShMeta.hpp ShMetaImpl.hpp ShMetaForwarder.hpp 
incinc_HEADERS += ShContext.hpp 

# Generated by python programs in scripts/
GENERATED  = ShAttrib.hpp ShAttribImpl.hpp \
	     ShVector.hpp \
	     ShPoint.hpp \
	     ShColor.hpp \
	     ShPosition.hpp \
	     ShNormal.hpp \
	     ShTexCoord.hpp \
	     ShPlane.hpp

%.hpp: scripts/%.hpp.py scripts/common.py scripts/semantic.py
	@rm -f $@
	python $< > $@
	@chmod -w $@

%.cpp: scripts/%.cpp.py scripts/common.py scripts/semantic.py
	@rm -f $@
	python $< > $@
	@chmod -w $@

%.o: $(GENERATED)
%.Plo: $(GENERATED)

all-generated: $(GENERATED)

clean-generated:
	@rm -f $(GENERATED)
