#include "UberTextures.hpp"
#include "Arb.hpp"

namespace shgl {

using namespace SH;

struct AtiBackend : public GlBackend {
  AtiBackend()
    : GlBackend(new ArbCodeStrategy(), new UberTextures(), 0) // new PBufferStreams())
  {
  }

  std::string name() const { return "ati"; }
};

static AtiBackend* backend = new AtiBackend();

}
