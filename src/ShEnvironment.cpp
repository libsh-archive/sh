#include "ShEnvironment.hpp"

namespace SH {

ShShader ShEnvironment::shader = 0;
bool ShEnvironment::insideShader = false;
ShShader ShEnvironment::boundShader[shShaderKinds] = {0, 0};
ShBackendPtr ShEnvironment::backend = 0;

}
