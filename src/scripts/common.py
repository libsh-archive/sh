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
// Copyright 2003-2005 Serious Hack Inc.
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
    inprint("#ifndef SH_" + name + "\n#define SH_""" + name + "\n")

def endguard(name):
    inprint("\n#endif // SH_" + name)

def namespace():
    inprint("namespace SH {\n")

def endnamespace():
    inprint("\n} // namespace SH")

def doxygen(s):
    inprint(("/** " + s).replace("\n", "\n * ") + "\n *\n */")
    
