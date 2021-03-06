import sys, string

curindent = 0
lastindent = 0

def inprint(x):
    global curindent
    instr = " " * curindent
    sys.stdout.write(instr + string.replace(x, "\n", "\n" + instr).rstrip(' ') + "\n")

def indent():
    global curindent
    curindent = curindent + 2

def deindent():
    global curindent
    curindent = curindent - 2
    if curindent < 0:
        curindent = 0

# TODO: Use a stack
def noindent():
    global curindent, lastindent
    lastindent = curindent
    curindent = 0

def reindent():
    global curindent, lastindent
    curindent = lastindent
    
def copyright():
    inprint("""// Sh: A GPU metaprogramming language.
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
""")

def warning():
    inprint("""// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from """ + sys.argv[0] + """.
// If you wish to change it, edit that file instead.
//
// ---
//""")

def header():
    warning()
    copyright()

def guard(name):
    inprint("#ifndef " + name + "\n#define """ + name + "\n")

def endguard(name):
    inprint("\n#endif // " + name)

def namespace():
    inprint("namespace SH {\n")

def endnamespace():
    inprint("\n} // namespace SH")

def doxygen(s):
    inprint(("/** " + s).replace("\n", "\n * ") + "\n *\n */")
    
