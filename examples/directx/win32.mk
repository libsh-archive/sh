INSTALLDIR = \home\mjchase\dxsamples\demos
SH_INSTALLDIR = \dev\install

MSSDK_DIR = \dev\mssdk
DXSDK_DIR = \dxsdk

# MS platform SDK
INCLUDES += -I$(MSSDK_DIR)\include
# DirectX SDK
INCLUDES += -I$(DXSDK_DIR)\include
# Sh
INCLUDES += -I$(SH_INSTALLDIR)\include

CPPFLAGS = -DWIN32 $(INCLUDES) -DNOMINMAX -D_USE_MATH_DEFINES

RELEASE_CPPFLAGS = $(CPPFLAGS)
DEBUG_CPPFLAGS   = $(CPPFLAGS) /D SH_DEBUG /D _DEBUG

CXXFLAGS = /GR /GX /wd4003
RELEASE_CXXFLAGS = $(CXXFLAGS) $(RELEASE_CPPFLAGS)  /MD
DEBUG_CXXFLAGS = $(CXXFLAGS) $(DEBUG_CPPFLAGS) /MDd /ZI /Od /RTC1

LDFLAGS = 
DEBUG_LDFLAGS = $(LDFLAGS) /Zi
RELEASE_LDFLAGS = $(LDFLAGS)

CXX = cl

clean:
	del *.lib *.obj *.d $(CLEANFILES)

%.r.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(RELEASE_CXXFLAGS)

%.d.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(DEBUG_CXXFLAGS)

%.r.d: %.cpp
	makedepend -f- -o.r.obj $< -- $(RELEASE_CPPFLAGS) > $@ 2>devnull
	-@del devnull

%.d.d: %.cpp
	makedepend -f- -o.d.obj $< -- $(RELEASE_CPPFLAGS) > $@ 2>devnull
	-@del devnull
