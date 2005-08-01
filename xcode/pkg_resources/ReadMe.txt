The Mac OS X version of Sh has currently only been tested on Panther.

Currently the cc and ARB backends are known to work.

Floating point textures should work, but are untested. PBufferStreams
(i.e. stream programs in the ARB backend) have not been implemented on
OSX yet.

A few examples are included in the package. These will be installed in
Applications/Sh Examples.

Sh itself is installed as a framework. You should be able to use it
simply by using

  #include <sh/sh.hpp>

and linking with "-framework Sh".

For more information about Sh see our homepage at <http://libsh.org/>.
