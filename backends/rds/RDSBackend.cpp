#include "RDSBackend.hpp"
#include "Pass.hpp"
#include "../gl/Arb.hpp"
#include "../gl/GlTextures.hpp"
#include "../gl/GLXPBufferStreams.hpp"
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
#ifdef RDS_DEBUG
  rds.get_pdt()->printDoms();
#endif
  Timer t;
	rds.rds();
  std::cout << "Elapsed time: " << t.diff() << " ns" << std::endl;
  rds.get_pdt()->graphvizDump("rds-final.graph");

  RDS rdsh(p->clone());
  rdsh.get_pdt()->graphvizDump("rdsh-initial.graph");
#ifdef RDS_DEBUG
  rds.get_pdt()->printDoms();
#endif
  t.start();
	rdsh.rdsh();
  std::cout << "Elapsed time: " << t.diff() << " ns" << std::endl;
  rdsh.get_pdt()->graphvizDump("rdsh-final.graph");

}

void RDSBackend::time_rds(SH::ShProgramNodeCPtr p) {
  RDS *rds = new RDS(p->clone());
  rds->force_graph_limits();
  Timer t;
	rds->rds();
  std::cout << "RDS took: " << t.diff() << " ns" << std::endl;
  //rds->print_partition();
  delete rds;
}

SH::ShBackendCodePtr RDSBackend::generateCode(const std::string& target,
                                              const SH::ShProgramNodeCPtr& shader)
{
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS generateCode() call, dumping partitions");
  compare(shader->clone());
#endif
  /*
  SH::ShProgramNodePtr s = shader->clone();
  
  SH::ShContext::current()->enter(s);
  SH::ShTransformer trans(s);
  
  trans.convertInputOutput(); 
  trans.convertTextureLookups();
  */
#ifdef RDS_DEMO
  time_rds(shader->clone());
#endif
  return m_code->generate(target,shader,m_texture);
}

void RDSBackend::execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest)
{
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
  SH_DEBUG_PRINT("RDS execute() call, dumping partitions");
  compare(program->clone());
#endif
  //m_stream->execute(program,dest);
#ifdef RDS_DEMO
  std::cout << "RDS Demo program trace" << std::endl << std::endl;
  program->ctrlGraph->print(std::cout,0);
  std::cout << "RDS Demo program interface" << std::endl << std::endl;
  std::cout << program->describe_interface() << std::endl;
#endif

  RDS rds(program->clone());
  rds.force_fake_limits();
  rds.rds();
  Schedule s(rds.m_passes, rds.m_shared_vars);
  
  for (std::vector<Pass*>::iterator I = s.get_passes()->begin(); I != s.get_passes()->end(); ++I) {
    std::cout << "Executing pass " << (*I)->id << std::endl;
    std::cout << (*I)->get_prog()->describe_interface() << std::endl;
    (*I)->get_bb()->print(std::cout,2);
    m_stream->execute((*I)->get_prog(),dest);
  }
}

RDSBackend::RDSBackend(void) {
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
  m_code = new shgl::ArbCodeStrategy();
  m_texture = new shgl::GlTextures();
  m_stream =  new shgl::GLXPBufferStreams();
}

RDSBackend::~RDSBackend(void) {
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
}

std::string RDSBackend::name(void) const {
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT( __FUNCTION__ );
#endif
  return "rds";
}

static RDSBackend* backend = new RDSBackend();
