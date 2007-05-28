#ifndef COLOR_FINDER_HPP
#define COLOR_FINDER_HPP

#include <sh/sh.hpp>
#include <map>

/** Colors snagged from the ColorBrewer web page
 *
 * Copyright (c) 2002 Cynthia Brewer, Mark Harrower, and The Pennsylvania State University. All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions as source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. The end-user documentation included with the redistribution, if any, must include the following acknowledgment:
 * This product includes color specifications and designs developed by Cynthia Brewer (http://colorbrewer.org/).
 * 
 * 3. Alternately, this acknowledgment may appear in the software itself, if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The name "ColorBrewer" must not be used to endorse or promote products derived from this software without prior written permission. For written permission, please 
 * contact Cynthia Brewer at cbrewer@psu.edu.
 * 
 * 5. Products derived from this software may not be called "ColorBrewer", nor may "ColorBrewer" appear in their name, without prior written permission of Cynthia Brewer.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CYNTHIA BREWER, MARK HARROWER, OR THE 
 * PENNSYLVANIA STATE UNIVERSITY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY 
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

class ColorFinder {
  public:
    enum Scheme {
      Accent, // Qualitative (8)
       Blues,   // Sequential (9)
       BrBG, // Diverging (11)
       BuGn, // Sequential (9)
       BuPu, // Sequential (9)
       Dark2, // Qualitative (8)
       GnBu, // Sequential (9)
       Greens, // Sequential  (9)
       Greys, // Sequential (9)
       Oranges, // Sequential (9)
       OrRd, // Sequential (9)
       Paired, // Qualitative (12)
       Pastel1, // Qualitative (9)
       Pastel2, // Qualitative (8)
       PiYG, // Diverging (11)
       PRGn, // Diverging (11)
       PuBu, // Sequential (9)
       PuBuGn, // Sequential (9)
       PuOr, // Diverging (11)
       PuRd, // Sequential (9)
       Purples, // Sequential (9)
       RdBu, // Diverging (11)
       RdGy, // Diverging (11)
       RdPu, // Sequential (9)
       Reds, // Sequential (9)
       RdYlBu, // Diverging (11)
       RdYlGn, // Diverging (11)
       Set1, // Qualitative (9)
       Set2, // Qualitative (8)
       Set3, // Qualitative (12)
       Spectral, // Diverging (11)
       YlGn, // Sequential (9)
       YlGnBu, // Sequential (9)
       YlOrBr, // Sequential (9)
       YlOrRd, // Sequential (9)
    };
    /* Fetches the i'th color from the version of the scheme with n colors in total.
     * Non-integer i are bilinearly interpolated for 0 <= i <= n - 1 */   
    static SH::Color3f color(Scheme scheme, int n, const SH::Generic<1, float>& i);
    static SH::Color3f color(Scheme scheme, int n, float i) {
      return color(scheme, n, SH::Attrib1f(i));
    }

    /* Fetches the (n - i - 1)'th color from the version of the scheme with n colors in total.
     * Non-integer i are bilinearly interpolated for 0 <= i <= n - 1 */   
    static SH::Color3f reverseColor(Scheme scheme, int n, const SH::Generic<1, float>& i) {
      return color(scheme, n, n - 1 - i); 
    }
    static SH::Color3f reverseColor(Scheme scheme, int n, float i) {
      return reverseColor(scheme, n, SH::Attrib1f(i));
    }


  private:
  typedef SH::Palette<SH::Color3f> ColorPalette;
  /* @todo range
   * Palettes aren't designed to copy construct or assign nicely, so we'll allow a small leak hear
   * for fixing later */
  typedef std::vector<ColorPalette*> PaletteVec; 
  typedef std::map<Scheme, PaletteVec> SchemeMap; 
  static SchemeMap schemes;
  static SchemeMap buildSchemes(); 
  
  /** returns the array index to lookup for a particular number of colors */
  static const int rawSchemes[][3]; 
};


#endif
