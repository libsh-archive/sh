// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Bryan Chan 
//          
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLA_RENDERGLOBAL_HPP
#define SHLA_RENDERGLOBAL_HPP

#include "ShProgram.hpp"
#include "ShFramebuffer.hpp"
#include "ShTexture.hpp"
#include "ShAttrib.hpp"

namespace Shla {

using namespace SH;

/** \file ShlaRenderGlobal.hpp
 * This static class sets up framebuffer, texture, and uberbuffers 
 * with the given dimensions and element type 
 */


/** \brief A storage for frequently used variables for rendering.
 * This sets up a static store of 2D textures, uber buffers, and framebuffer
 * of dimension M * N using element type T.
 * 
 * This way, sub classes that ultimately render to buffers of the same size
 * can re-use the same framebuffer and temporary texture/uber buffer objects.
 */
template<typename T, int M, int N> 
class ShlaRenderGlobal {
  public:
    typedef ShColor<T::typesize, SH_VAR_OUTPUT, typename T::ValueType> OutputColorType;
    typedef ShAttrib<T::typesize, SH_VAR_TEMP, typename T::ValueType> TempType;

    static ShFramebufferPtr renderbuf;
    static ShTexture2D<T> op1, op2;  // M*N 2D textures for attaching to vectors

    static ShTexture2D<T> accum;
    static ShUberbufferPtr accumRead, accumWrite; // N*N 2D uber buffers for storing intermediate results


    /** Clears the render buffer and switches active render buffer to renderbuf */
    static void useRenderbuf();  

    /** draws a quad covering the rectangle with diagonal (0,0) to (w, h)
     * with texture coordinates (0,0) to (w/M, h/N).
     *
     * The z parameter changes the depth of the quad.
     */
    static void drawQuad(int w, int h, double z ); 

    /** draws a quad covering the entire M*N rectangle
     * at z = 0.0;
     */
    static void drawQuad();

    /** detaches renderbuf, op1, op2, and accum from any attached uber buffers */
    static void detachAll();

    /** binds the default vertex shader and the given fragment shader */
    static void bindDefault( ShProgram fsh );

    /** binds the reduction vertex shader and given fragment shader */
    static void bindReduce( ShProgram fsh );

    /** binds a vertex shader & fragment shader that outputs a given 2D texture */
    static void bindOutput( ShTexture2D<T>& tex );

    // helper functions for doing a render to texture loop with at least 2 iterations
    //
    // Before first iteration, use accumInit( initial state );
    // Before subsequent iterations, except the last, call accumLoop();
    // Before the last iteration, call accumLastLoop( your_output_ubuf );
    // Use accumDetach() after loop is done to detach m_framebuffer & accum
    //
    // Then in each loop, you can use accum as a texture holding the output from
    // the previous render loop.
    //

    /** \brief Initializes the loop, starting with given initial input.
     * Binds framebuffer to given output (accumWrite by default)
     */
    static void accumInit( ShUberbufferPtr init );

    /** \brief Swaps accumWrite from last iteration to accumRead
     * and attaches it to accum to read from in the next iteration.
     * Output goes to accumWrite by default, but can be changed to another
     * given uber buffer.
     */
    static void accumLoop(); 

    static void accumLastLoop( ShUberbufferPtr out );

    static void accumDetach();

    /** \brief initializes the zero uber buffer
     * Currently this is only used for matrix multiplication (in case 
     * the matrix has no diagonals, the multiplicatoin must return 0.
     * Instead of reinitializing a 0 vector each time (which is really slow
     * with current ubuf implementation), keep this zero here
     *
     */
    static ShUberbufferPtr getZeroMem();

    /** \brief Prints the floating point contents of the uber buffer
     * (must have dimensions M x N and hold elements of type T)
     *
     * TODO remove this (it's for temporary debugging purposes)
     */
    static void printData( ShUberbufferPtr ub, int c = T::typesize ); 
    
  private:
    // TODO this may not be necessary...
    static ShUberbufferPtr zero;
};



}

#include "ShlaRenderGlobalImpl.hpp"

#endif
