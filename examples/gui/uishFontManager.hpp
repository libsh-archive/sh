#ifndef UISH_FONTMANAGER_HPP
#define UISH_FONTMANAGER_HPP

#include <map>
#include <string>
#include <FTGL/FTFont.h>
#include <sh/sh.hpp>

/* Doesn't do much right now...just does some setup to allow
 * reuse of fonts.
 *
 * Needs to be run in the same GL context where fonts are 
 * required.
 */
class uishFontManager {
  public:
    static FTFont* getFont(const std::string& family, int size);

  protected:
    typedef std::pair<std::string, int> FontKey;
    typedef std::map<FontKey, FTFont*> FontMap;

    static FontMap m_fonts;
};

#endif
