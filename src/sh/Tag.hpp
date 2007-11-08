// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHTAG_HPP
#define SHTAG_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <functional>
#include "DllExport.hpp"
#include "Statement.hpp"
#include "CtrlGraph.hpp"
#include "Program.hpp"
#include "ProgramNode.hpp"

namespace SH {

/** A per-statement tag that transfers over when a statement
 * is transformed into multiple statements or a program fragment. 
 *
 * Useful for tagging a statement and studying what spawns from it 
 * through the various transformations. 
 *
 * Only Info objects that are sublcasses of Tag get propagated   
 */
class
SH_DLLEXPORT
Tag: public Info {
  public:
    typedef std::list<const Tag*> TagList;

    virtual ~Tag() {}
    virtual std::string toHeader() const = 0;
    virtual std::string toString() const = 0; 

    /* Some helpful functions for transfering tags */  
    static void push() { tags.push(TagList()); }
    static void push(const Statement& stmt); 
    static void push(const Tag* t); 
    static void add(const Tag* t);  // adds to top
    static void dup(); // duplicates top node 
    static void pop();

    /* Replaces all tags in top with type T with one copy of the given tag */ 
    template<typename TagType>
    static void replace(const Tag* t) {
      for(TagList::iterator T = tags.top().begin(); T != tags.top().end();) { 
        if(dynamic_cast<const TagType*>(*T) != 0) {
          T = tags.top().erase(T); 
        } else {
          ++T;
        }
      }
      tags.top().push_back(t);
    }

    /* Take top tag and copy to the given item,
     * does nothing if nothing is active */  
    template<typename InputIterator>
    static void cloneToIt(InputIterator start, InputIterator end) {
      std::for_each(start, end, std::ptr_fun(&Tag::cloneTo)); 
    }

    static void cloneTo(Statement& stmt); 
    static void cloneToProgram(ProgramNodePtr p); 

  private:
    typedef std::stack<TagList> TagStack;
    static TagStack tags; 
};

/* Gets a dumper for use with ProgramNodePtr::csvdump */
SH_DLLEXPORT 
StmtCsvDataPtr getTagCsvData(ProgramNodePtr p); 

}

#endif
