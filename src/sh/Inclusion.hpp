// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef SHINCLUSION_HPP
#define SHINCLUSION_HPP

#include <map>
#include "Info.hpp"
#include "Program.hpp"
#include "Algebra.hpp"

/** @file Inclusion.hpp
 * Turns programs involving only float/double types into the corresponding interval program using only
 * interval float/double types.
 *
 * The conversion on operations is mostly trivial (replace one value type with another), but for textures,
 * at the time of conversion, 
 *
 * @todo range if a texture has the "texture: static" meta flag defined, then the 
 * lookups on that texture will be replaced by lookups into an interval texture.  Otherwise,
 * the lookups will be replaced by a user-declared range on the texture values (using the lowBound and highBound 
 * meta information)
 *
 * The original and result programs will both have StmtIndex objects attached
 * to each statement.  Matching pairs of statements from the original and
 * converted program will have the same index.
 *
 * Also, constants are not converted (assume they don't change, hence do not require an interval value),
 * and uniforms 
 */

namespace SH {

/* This info struct attaches unique indices to objects */
struct StmtIndex: public Info 
{
  StmtIndex();
  Info* clone() const;

  long long index() const;
  bool operator<(const StmtIndex& other) const;
  static void reset() { cur_index = 0; }

  private:
    unsigned long long m_index;
    static unsigned long long cur_index;
};

/** Adds statement indices to a program for statements that do not already have
 * an index 
 *
 * Reset starts from 0 */
void add_stmt_indices(Program a, bool reset=false);
void clear_stmt_indices(Program a);

typedef std::map<StmtIndex, Statement*> IndexStmtMap;
IndexStmtMap gather_indices(Program a);

/** Makes a clone of the control graph in a and turns all regular float computation to interval computation */
SH_DLLEXPORT
Program inclusion(Program a, bool doMarking=true);

/** Makes a clone of the control graph in a and turns all regular flot
 * computations to affine computations.
 *
 * @return A Program that is a copy of the original program, but has all 
 *         non-uniforms turned into affine types. 
 *         The inputs and outputs are converted to intervals, and the resulting
 *
 * Both the original program and result program will have StmtIndexInfo
 * objects attached to each statement so that it is possible to match 
 * up a statement in the original program with one in the affine program. 
 *
 * The inputs and outputs are still intervals, but internally everything is done
 * with affine arithmetic.
 */
SH_DLLEXPORT
Program affine_inclusion(Program a, bool doMarking=true);

/** Similar to affine_inclusion, except that the outputs are affine and
 * have a center + tuples for error symbols 
 *
 * @return A Program that is a copy of the original program, but has all 
 *         non-uniforms turned into affine types. 
 *
 * Maybe just a stream conversion function that converts a stream (with one
 * channel for the center and several channels for errsyms) into a single
 * channel of affine values.
 */
SH_DLLEXPORT
Program affine_inclusion_syms(Program a, bool doMarking=true);  

/** Alters the given program so any affine inputs become interval inputs */
SH_DLLEXPORT 
void affine_to_interval_inputs(Program a);

/** Alters the given program so any affine outputs become interval outputs */
SH_DLLEXPORT 
void affine_to_interval_outputs(Program a);

}

#endif
