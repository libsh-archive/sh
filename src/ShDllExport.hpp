#ifndef SHDLLEXPORT_HPP
#define SHDLLEXPORT_HPP

#ifndef SH_DLLEXPORT
# ifdef WIN32
#  define SH_DLLEXPORT __declspec(dllimport)
# else
#  define SH_DLLEXPORT
# endif
#endif

#endif
