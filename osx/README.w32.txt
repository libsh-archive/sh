On Windows, you can build Sh with

 - the Visual C++ Toolkit, available at
   http://msdn.microsoft.com/visualc/vctoolkit2003/
 - GNU make, available at
   ftp://ftp.gnu.org/gnu/make/
   or binaries at http://www.mingw.org/download.shtml
 - The MS Platform SDK.
 - makedepend, available from
   http://unxutils.sourceforge.net/
 - libpng and zlib (static versions).

Edit the file win32.mk and change the settings there to match your
system. Then 
  make -f Makefile.win32 
should compile, and 
  make -f Makefile.win32 install
install Sh.

Sorry that these instructions aren't very detailed right now. I will
update them later.

 -- Stefanus Du Toit, July 28, 2004
