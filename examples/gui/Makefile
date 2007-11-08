# Change this location to where you installed Sh
# E.g. /usr/local
SH_INSTALL_DIR = ${HOME}/install-sh

SHADERS = diffuse.so phong.so simple.so blend.so

UISH = 	uishConstants.cpp uishBase.cpp uishStyle.cpp uishWidget.cpp 
UISH +=	uishContainer.cpp uishPackLayout.cpp uishGraphLayout.cpp uishFrame.cpp uishFixedFrame.cpp
UISH += uishBorder.cpp uishBox.cpp 
UISH += uishFontManager.cpp uishLabel.cpp
SOURCES = $(UISH) Camera.cpp  ShTrackball.cpp  appwindow.cpp  main.cpp  viewer.cpp scanner.cpp shaderTree.cpp shaderGraph.cpp
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LIBS = `pkg-config --libs gtkmm-2.0 gtkglextmm-1.0` -lglut -lsh -lshutil -lftgl -lfontconfig
LDFLAGS = -L$(SH_INSTALL_DIR)/lib $(LIBS)
INCLUDES = -I/usr/include `pkg-config --cflags gtkmm-2.0 gtkglextmm-1.0` -I$(SH_INSTALL_DIR)/include -I/usr/include/freetype2
CPPFLAGS = $(INCLUDES)
CXXFLAGS = $(CPPFLAGS) -Wall -g
CXX = g++
MAIN = sherr 

all: $(MAIN) $(SHADERS)

depend: $(DEPENDS)

cleandbg:
	rm -f *.vars *.ps *.dot

clean:
	rm -f *.o *.so *.d $(MAIN)

$(MAIN): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

%.so: %.cpp
	$(CXX) -shared -O2 $(CXXFLAGS) -o $@ $< 

%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	set -e; $(CC) -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@
	$(CXX) $(CXXFLAGS) -M $< > $@


include $(DEPENDS)

