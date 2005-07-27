## Makefile.am -- Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.7 foreign subdir-objects
bin_PROGRAMS = test

# Overall application
test_SOURCES  = main.cpp ShInclude.hpp LuaInclude.hpp
test_SOURCES += LuaSh.cpp LuaSh.hpp LuaBind.hpp
test_SOURCES += LuaShVariable.cpp LuaShVariable.hpp

test_LDADD    = -lsh -lshutil -llua