#include "shutil.hpp"
#include <string>

using namespace std;

namespace ShUtil {

string normalize_path(const string& path)
{
  string ret(path);
  const unsigned size = ret.size();
  for (unsigned i=0; i < size; i++) {
#ifdef WIN32
    if ('/' == ret[i]) ret[i] = '\\';
#else
    if ('\\' == ret[i]) ret[i] = '/';
#endif
  }
  return ret;
}

}
