#include "Arb.hpp"
#include "GlTextures.hpp"
#include "GlBackend.hpp"

namespace shgl {

struct ArbBackend : public GlBackend {
  ArbBackend()
    : GlBackend(new ArbCodeStrategy(), new GlTextures(), 0) // new PBufferStreams())
  {
  }

  std::string name() const { return "arb"; }
};

static ArbBackend* backend = new ArbBackend();

}
