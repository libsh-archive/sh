#include "Arb.hpp"
#include "GlTextures.hpp"
#include "GlBackend.hpp"
#include "PBufferStreams.hpp"

namespace shgl {

struct ArbBackend : public GlBackend {
  ArbBackend()
    : GlBackend(new ArbCodeStrategy(),
                new GlTextures(),
                new PBufferStreams())
  {
  }

  std::string name() const { return "arb"; }
};

static ArbBackend* backend = new ArbBackend();

}
