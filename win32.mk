INCLUDES = -I\dev\libpng\include -I\dev\zlib\include
CPPFLAGS = -DWIN32 -DSH_DEBUG $(INCLUDES)
CXXFLAGS = $(CPPFLAGS) /GR /GX /wd4003
CXX = cl
LIBRARIAN = lib

clean:
	del *.lib *.obj *.d

%.obj: %.cpp
	$(CXX) /Fo$@ /c $< $(CXXFLAGS)

%.d: %.cpp
	makedepend -f- -o.obj $< -- $(CPPFLAGS) > $@ 2>devnull
	-@del devnull
