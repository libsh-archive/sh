#include "RDSBackend.hpp"
#include <fstream>

void RDSBackend::dump(RDS rds, char* complete, char* partitioned) {
  rds.get_pdt()->graphvizDump(complete);
  rds.get_pdt()->printDoms();
	rds.rds();
	rds.print_partitions(partitioned);
}

void RDSBackend::compare(SH::ShProgramNodePtr p) {
  RDS rds(p->clone());
  rds.get_pdt()->graphvizDump("rds-initial.graph");
  rds.get_pdt()->printDoms();
  Timer t;
	rds.rds();
  std::cout << "Elapsed time: " << t.diff() << std::endl;
  rds.get_pdt()->graphvizDump("rds-final.graph");

  RDS rdsh(p->clone());
  rdsh.get_pdt()->graphvizDump("rdsh-initial.graph");
  rdsh.get_pdt()->printDoms();
  t.start();
	rdsh.rdsh();
  std::cout << "Elapsed time: " << t.diff() << std::endl;
  rdsh.get_pdt()->graphvizDump("rdsh-final.graph");

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
  compare(shader->clone());
  return NULL;
}

void RDSBackend::execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest)
{
#ifdef SH_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS execute() call, dumping partitions");
#endif
  compare(program->clone());
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
