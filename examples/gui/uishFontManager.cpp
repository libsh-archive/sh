#include <fontconfig/fontconfig.h>
#include <FTGL/FTGLPolygonFont.h>
//#include <FTGL/FTGLTextureFont.h>
#include <FTGL/FTGLBitmapFont.h>


#include "uishFontManager.hpp"

uishFontManager::FontMap uishFontManager::m_fonts;

FTFont* uishFontManager::getFont(const std::string& family, int size) {
  std::cerr << "get font" << std::endl;
  FontKey key(family, size);
  if(m_fonts.find(key) == m_fonts.end()) {
    FcInit();

    FcPattern* pattern;
    double point_size = size;
    pattern = FcPatternBuild (NULL,
                           FC_FAMILY, FcTypeString, family.c_str(),
                           FC_SIZE, FcTypeDouble, point_size,
                           FC_SLANT, FcTypeInteger, 0,
                           NULL);
    FcPattern* matched;
    FcResult result;
    matched = FcFontMatch (0, pattern, &result);

    FcChar8* filename;
    if (FcPatternGetString (matched, FC_FILE, 0, &filename) != FcResultMatch) {
      std::cerr << "unmatched filename" << std::endl;
    }

    int id;
    if (FcPatternGetInteger (matched, FC_INDEX, 0, &id) != FcResultMatch) {
      std::cerr << "unmatched id" << std::endl;
    }

    std::cerr << "Font filename: " << filename << std::endl;

    FTFont* newFont = new FTGLPolygonFont((char*)filename);
    //FTFont* newFont = new FTGLBitmapFont((char*)filename);
    if(newFont->Error()) {
      std::cerr << "Could not open font " << (char*)filename 
        << " error = " << newFont->Error() << std::endl;
    }
    if(!newFont->FaceSize((int)size)) {
      std::cerr << "Could not set font size " << (char*)filename << std::endl; 
    }

    FcPatternDestroy (pattern);
    FcPatternDestroy (matched);

    m_fonts[key] = newFont; 
  }
  return m_fonts[key];
}
