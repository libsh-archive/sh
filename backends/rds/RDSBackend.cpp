#include "RDSBackend.hpp"
#include "Pass.hpp"
#include "../gl/Arb.hpp"
#include "../gl/GlTextures.hpp"
#include "../gl/GLXPBufferStreams.hpp"
#include <fstream>

#define RDS_DEBUG

void RDSBackend::dump(RDS rds, char* complete, char* partitioned) {
	rds.pdt()->graphvizDump(complete);
	rds.pdt()->printDoms();
	rds.rds();
	rds.print_partitions(partitioned);
}

void RDSBackend::compare(SH::ShProgramNodePtr p) {
  RDS rds(p->clone());
  rds.pdt()->graphvizDump("rds-initial.graph");
#ifdef RDS_DEBUG
  rds.pdt()->printDoms();
#endif
  Timer t;
	rds.rds();
  std::cout << "Elapsed time: " << t.diff() << " ns" << std::endl;
  rds.pdt()->graphvizDump("rds-final.graph");

  RDS rdsh(p->clone());
  rdsh.pdt()->graphvizDump("rdsh-initial.graph");
#ifdef RDS_DEBUG
  rds.pdt()->printDoms();
#endif
  t.start();
  rdsh.rdsh();
  std::cout << "Elapsed time: " << t.diff() << " ns" << std::endl;
  rdsh.pdt()->graphvizDump("rdsh-final.graph");

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
  SH_DEBUG_PRINT("RDS generateCode() call");
#endif

#ifdef RDS_COMPARE
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
  SH_DEBUG_PRINT("RDS execute() call");
#endif
#ifdef RDS_COMPARE
  compare(program->clone());
#endif
  //m_stream->execute(program,dest);
#ifdef RDS_DEMO
  std::cout << "RDS Demo program trace" << std::endl << std::endl;
  program->ctrlGraph->print(std::cout,0);
  std::cout << "RDS Demo program interface" << std::endl << std::endl;
  std::cout << program->describe_interface() << std::endl;
#endif

#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Creating RDS object");
#endif
  RDS rds(program->clone());
  rds.force_fake_limits();
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Running rds");
#endif
  rds.rds();
#ifdef RDS_DEBUG
  SH_DEBUG_PRINT("Scheduling passes");
#endif
  Schedule s(rds.passes(), rds.shared_vars());
 
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
