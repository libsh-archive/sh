#include "RDSBackend.hpp"
#include <fstream>

void RDSBackend::dump(RDS rds, char* complete, char* partitioned) {
  rds.get_pdt()->graphvizDump(complete);
  rds.get_pdt()->printDoms();
	rds.get_partitions();
	rds.print_partitions(partitioned);
}

SH::ShBackendCodePtr RDSBackend::generateCode(const std::string& target,
                                              const SH::ShProgramNodeCPtr& shader)
{
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS generateCode() call, dumping partitions");
#endif
  /*
  SH::ShProgramNodePtr s = shader->clone();
  
  SH::ShContext::current()->enter(s);
  SH::ShTransformer trans(s);
  
  trans.convertInputOutput(); 
  trans.convertTextureLookups();
  */
  RDS rds(shader->clone());
  dump(rds,"c.graph","p.graph");
  return NULL;
}

void RDSBackend::execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest)
{
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS execute() call, dumping partitions");
#endif
  RDS rds(program->clone());
  dump(rds,"c.graph","p.graph");
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
