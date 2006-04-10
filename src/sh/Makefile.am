INCLUDES = $(LTDLINCL)

lib_LTLIBRARIES = libsh.la

AM_CPPFLAGS=-DENABLE_BINRELOC

libsh_la_LIBADD = $(LIBLTDL)
libsh_la_LDFLAGS = -export-dynamic -no-undefined

incincdir = $(includedir)/sh
incinc_HEADERS  = Attrib.hpp AttribImpl.hpp Backend.hpp BasicBlock.hpp

# General/internals
libsh_la_SOURCES  = BasicBlock.cpp Block.cpp CtrlGraph.cpp
libsh_la_SOURCES += Exception.cpp Error.cpp
libsh_la_SOURCES += Parser.cpp 
libsh_la_SOURCES += Program.cpp Program.hpp
libsh_la_SOURCES += ProgramNode.cpp ProgramNode.hpp
libsh_la_SOURCES += ProgramSet.cpp ProgramSet.hpp
incinc_HEADERS   += ProgramSet.hpp
libsh_la_SOURCES += Record.hpp Record.cpp 
incinc_HEADERS	 += Record.hpp 
libsh_la_SOURCES += Info.hpp InfoImpl.hpp Info.cpp
incinc_HEADERS   += Info.hpp InfoImpl.hpp
libsh_la_SOURCES += Statement.cpp Swizzle.cpp Syntax.cpp Token.cpp
libsh_la_SOURCES += Tokenizer.cpp Utility.cpp Variable.cpp VariableNode.cpp
libsh_la_SOURCES += Operation.hpp Operation.cpp 
libsh_la_SOURCES += Internals.hpp Internals.cpp
libsh_la_SOURCES += BasicBlock.hpp
libsh_la_SOURCES += Block.hpp CtrlGraph.hpp
libsh_la_SOURCES += CfgBlock.hpp CfgBlock.cpp 
incinc_HEADERS 	 += CfgBlock.hpp 
libsh_la_SOURCES += Debug.hpp Error.hpp
libsh_la_SOURCES += Exception.hpp
libsh_la_SOURCES += Parser.hpp RefCount.hpp Statement.hpp
libsh_la_SOURCES += Swizzle.hpp SwizzleImpl.hpp Syntax.hpp Token.hpp Tokenizer.hpp
libsh_la_SOURCES += Utility.hpp Variable.hpp VariableNode.hpp sh.hpp
libsh_la_SOURCES += HashMap.hpp 
libsh_la_SOURCES += Graph.hpp GraphImpl.hpp 
libsh_la_SOURCES += Instructions.hpp Instructions.cpp
libsh_la_SOURCES += Meta.hpp MetaImpl.hpp
libsh_la_SOURCES += MetaForwarder.hpp MetaForwarder.cpp
libsh_la_SOURCES += Context.hpp Context.cpp
libsh_la_SOURCES += Math.hpp 
incinc_HEADERS   += Math.hpp 
libsh_la_SOURCES += DllExport.hpp
incinc_HEADERS   += DllExport.hpp
libsh_la_SOURCES += Pool.hpp Pool.cpp
incinc_HEADERS   += Pool.hpp

# Backend
libsh_la_SOURCES += Backend.cpp Backend.hpp 
libsh_la_SOURCES += LinearAllocator.hpp

# Optimizer and compiler internals
libsh_la_SOURCES += Transformer.hpp BitSet.hpp
libsh_la_SOURCES += Transformer.cpp BitSet.cpp
incinc_HEADERS   += Transformer.hpp BitSet.hpp
libsh_la_SOURCES += TypeConvertTransformer.cpp 
libsh_la_SOURCES += TransformerImpl.hpp 
incinc_HEADERS	 += TransformerImpl.hpp 
libsh_la_SOURCES += Optimizations.cpp Optimizations.hpp
incinc_HEADERS   += Optimizations.hpp
libsh_la_SOURCES += ValueTracking.cpp ConstProp.cpp
libsh_la_SOURCES += Evaluate.cpp Evaluate.hpp
incinc_HEADERS   += Evaluate.hpp
libsh_la_SOURCES += Structural.hpp Structural.cpp
incinc_HEADERS   += Structural.hpp
libsh_la_SOURCES += Section.hpp SectionImpl.hpp Section.cpp 
incinc_HEADERS   += Section.hpp SectionImpl.hpp

# Library functions
libsh_la_SOURCES += Lib.hpp
libsh_la_SOURCES += LibArith.hpp LibArithImpl.hpp
libsh_la_SOURCES += LibBoolean.hpp LibBooleanImpl.hpp
libsh_la_SOURCES += LibClamp.hpp LibClampImpl.hpp
libsh_la_SOURCES += LibGeometry.hpp LibGeometryImpl.hpp
libsh_la_SOURCES += LibMatrix.hpp LibMatrixImpl.hpp
libsh_la_SOURCES += LibMisc.hpp LibMiscImpl.hpp
libsh_la_SOURCES += LibSplines.hpp LibSplinesImpl.hpp
libsh_la_SOURCES += LibTrig.hpp LibTrigImpl.hpp
libsh_la_SOURCES += LibDeriv.hpp LibDerivImpl.hpp
libsh_la_SOURCES += LibNoise.hpp LibNoiseImpl.hpp
libsh_la_SOURCES += LibWorley.hpp LibWorleyImpl.hpp

# Textures and arrays
libsh_la_SOURCES += BaseTexture.hpp BaseTextureImpl.hpp BaseTexture.cpp
libsh_la_SOURCES += Array.hpp Table.hpp Texture.hpp
libsh_la_SOURCES += Wrap.hpp Interp.hpp MIPFilter.hpp
libsh_la_SOURCES += TextureNode.hpp Image.hpp ImageImpl.hpp Image3D.hpp
libsh_la_SOURCES += Memory.hpp Memory.cpp MemoryDep.hpp
libsh_la_SOURCES += TextureNode.cpp Image3D.cpp
libsh_la_SOURCES += TexData.hpp TexDataImpl.hpp

# Types
libsh_la_SOURCES += VariableType.hpp VariableType.cpp
libsh_la_SOURCES += TypeInfo.hpp TypeInfoImpl.hpp TypeInfo.cpp
libsh_la_SOURCES += TypeInfoCasts.cpp TypeInfoOps.cpp 
libsh_la_SOURCES += StorageType.hpp StorageTypeImpl.hpp StorageType.cpp 
incinc_HEADERS   += StorageType.hpp StorageTypeImpl.hpp 
libsh_la_SOURCES += DataType.hpp DataTypeImpl.hpp DataType.cpp
incinc_HEADERS   += DataType.hpp DataTypeImpl.hpp 
libsh_la_SOURCES += Generic.hpp GenericImpl.hpp
libsh_la_SOURCES += Attrib.hpp AttribImpl.hpp
libsh_la_SOURCES += LibAttrib.hpp
libsh_la_SOURCES += Vector.hpp LibVector.hpp
libsh_la_SOURCES += Point.hpp LibPoint.hpp
libsh_la_SOURCES += Position.hpp LibPosition.hpp
libsh_la_SOURCES += Plane.hpp LibPlane.hpp
libsh_la_SOURCES += Color.hpp LibColor.hpp
libsh_la_SOURCES += Normal.hpp LibNormal.hpp
libsh_la_SOURCES += TexCoord.hpp LibTexCoord.hpp
libsh_la_SOURCES += Matrix.hpp MatrixImpl.hpp
libsh_la_SOURCES += Quaternion.hpp QuaternionImpl.hpp
libsh_la_SOURCES += Variant.hpp VariantImpl.hpp Variant.cpp
libsh_la_SOURCES += VariantFactory.hpp VariantFactoryImpl.hpp 
libsh_la_SOURCES += VariantCast.hpp VariantCastImpl.hpp 
libsh_la_SOURCES += CastManager.hpp CastManager.cpp 
libsh_la_SOURCES += Eval.hpp EvalImpl.hpp Eval.cpp
libsh_la_SOURCES += ConcreteRegularOpImpl.hpp 
libsh_la_SOURCES += ConcreteCTypeOpImpl.hpp  ConcreteCTypeOp.cpp
libsh_la_SOURCES += Half.hpp HalfImpl.hpp 
libsh_la_SOURCES += Fraction.hpp FractionImpl.hpp 


# Shader Algebra
libsh_la_SOURCES += Algebra.hpp Algebra.cpp
libsh_la_SOURCES += Manipulator.hpp ManipulatorImpl.hpp Manipulator.cpp
libsh_la_SOURCES += Nibbles.hpp NibblesImpl.hpp
libsh_la_SOURCES += FixedManipulator.hpp FixedManipulator.cpp

# Streams
libsh_la_SOURCES += ChannelNode.hpp ChannelNode.cpp
libsh_la_SOURCES += Channel.hpp ChannelImpl.hpp
libsh_la_SOURCES += Stream.hpp StreamImpl.hpp Stream.cpp

# Palettes
libsh_la_SOURCES += Palette.hpp PaletteImpl.hpp
libsh_la_SOURCES += PaletteNode.hpp PaletteNode.cpp
incinc_HEADERS   += Palette.hpp PaletteImpl.hpp PaletteNode.hpp

# These should really be moved up top
incinc_HEADERS += Block.hpp CtrlGraph.hpp
incinc_HEADERS += Debug.hpp Error.hpp
incinc_HEADERS += Exception.hpp Lib.hpp
incinc_HEADERS += LibArith.hpp LibArithImpl.hpp
incinc_HEADERS += LibBoolean.hpp LibBooleanImpl.hpp
incinc_HEADERS += LibClamp.hpp LibClampImpl.hpp
incinc_HEADERS += LibGeometry.hpp LibGeometryImpl.hpp
incinc_HEADERS += LibMatrix.hpp LibMatrixImpl.hpp
incinc_HEADERS += LibMisc.hpp LibMiscImpl.hpp
incinc_HEADERS += LibSplines.hpp LibSplinesImpl.hpp
incinc_HEADERS += LibTrig.hpp LibTrigImpl.hpp
incinc_HEADERS += LibDeriv.hpp LibDerivImpl.hpp
incinc_HEADERS += LibNoise.hpp LibNoiseImpl.hpp
incinc_HEADERS += LibWorley.hpp LibWorleyImpl.hpp
incinc_HEADERS += Parser.hpp RefCount.hpp RefCountImpl.hpp 
incinc_HEADERS += Program.hpp ProgramNode.hpp Statement.hpp 
incinc_HEADERS += Swizzle.hpp SwizzleImpl.hpp Syntax.hpp Token.hpp Tokenizer.hpp
incinc_HEADERS += Utility.hpp
incinc_HEADERS += HashMap.hpp
incinc_HEADERS += Graph.hpp GraphImpl.hpp 
incinc_HEADERS += Variable.hpp VariableNode.hpp
incinc_HEADERS += Operation.hpp VariableType.hpp 
incinc_HEADERS += Generic.hpp GenericImpl.hpp
incinc_HEADERS += sh.hpp
incinc_HEADERS += Matrix.hpp MatrixImpl.hpp
incinc_HEADERS += Array.hpp Table.hpp Texture.hpp
incinc_HEADERS += Wrap.hpp
incinc_HEADERS += BaseTexture.hpp BaseTextureImpl.hpp
incinc_HEADERS += TextureNode.hpp Image.hpp ImageImpl.hpp Image3D.hpp
incinc_HEADERS += TexData.hpp TexDataImpl.hpp
incinc_HEADERS += LinearAllocator.hpp
incinc_HEADERS += LibAttrib.hpp
incinc_HEADERS += Vector.hpp LibVector.hpp
incinc_HEADERS += Point.hpp LibPoint.hpp
incinc_HEADERS += Color.hpp LibColor.hpp
incinc_HEADERS += Plane.hpp LibPlane.hpp
incinc_HEADERS += Position.hpp LibPosition.hpp
incinc_HEADERS += Normal.hpp LibNormal.hpp
incinc_HEADERS += TexCoord.hpp LibTexCoord.hpp
incinc_HEADERS += Algebra.hpp Internals.hpp
incinc_HEADERS += FixedManipulator.hpp
incinc_HEADERS += Manipulator.hpp ManipulatorImpl.hpp
incinc_HEADERS += Nibbles.hpp NibblesImpl.hpp
incinc_HEADERS += ChannelNode.hpp Channel.hpp ChannelImpl.hpp
incinc_HEADERS += Stream.hpp StreamImpl.hpp
incinc_HEADERS += Quaternion.hpp QuaternionImpl.hpp
incinc_HEADERS += TypeInfo.hpp TypeInfoImpl.hpp 
incinc_HEADERS += Variant.hpp VariantImpl.hpp Eval.hpp EvalImpl.hpp
incinc_HEADERS += VariantFactory.hpp VariantFactoryImpl.hpp 
incinc_HEADERS += VariantCast.hpp VariantCastImpl.hpp 
incinc_HEADERS += CastManager.hpp 
incinc_HEADERS += ConcreteRegularOpImpl.hpp ConcreteCTypeOpImpl.hpp 
incinc_HEADERS += Half.hpp HalfImpl.hpp 
incinc_HEADERS += Fraction.hpp FractionImpl.hpp 
incinc_HEADERS += Memory.hpp Interp.hpp MemoryDep.hpp MIPFilter.hpp
incinc_HEADERS += Instructions.hpp
incinc_HEADERS += Meta.hpp MetaImpl.hpp MetaForwarder.hpp 
incinc_HEADERS += Context.hpp 

# BinReloc
libsh_la_SOURCES += binreloc.h binreloc.c
incinc_HEADERS   += binreloc.h

# Generated by python programs in scripts/
GENERATED  = Attrib.hpp AttribImpl.hpp \
	     Vector.hpp \
	     Point.hpp \
	     Color.hpp \
	     Position.hpp \
	     Normal.hpp \
	     TexCoord.hpp \
	     Plane.hpp

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
