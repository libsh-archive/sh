#include "RDSBackend.hpp"
#include "RDS.hpp"

SH::ShBackendCodePtr RDSBackend::generateCode(const std::string& target,
                                              const SH::ShProgramNodeCPtr& shader)
{
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS generateCode() call, dumping partitions");
#endif
  RDS *m_rds = new RDS(shader->clone());
  return NULL;
}

void RDSBackend::execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest)
{
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS execute() call, dumping partitions");
#endif
  RDS *m_rds = new RDS(program->clone());
}

RDSBackend::RDSBackend(void) {
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
}

RDSBackend::~RDSBackend(void) {
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
}

std::string RDSBackend::name(void) const {
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
  return "rds";
}

static RDSBackend* backend = new RDSBackend();
