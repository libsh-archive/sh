#include "PCScheduler.hpp"
#include <list>
#include "GlBackend.hpp"
#include "ShSchedule.hpp"
#include "Utils.hpp"

namespace shgl {

using namespace SH;

struct PCPass : public ShVoid {
  PCPass(double pc) : pc(pc)
  {
  }
  
  double pc;

  std::list<ShProgramNodePtr> programs;
};

typedef ShPointer<PCPass> PCPassPtr;
typedef ShPointer<const PCPass> PCPassCPtr;

typedef std::map<ShChannelNodePtr, ShTextureNodePtr> PCChannelMap;

struct PCSchedule : public ShVoid {
  PCSchedule();
  
  virtual ~PCSchedule();
  virtual void execute_pass(ShPass* pass);
  
  PCChannelMap channel_map;

private:
  GLuint m_query;
};

typedef ShPointer<PCSchedule> PCSchedulePtr;
typedef ShPointer<const PCSchedule> PCScheduleCPtr;

PCScheduler::PCScheduler()
{
}

ShVoidPtr PCScheduler::prepare(ShSchedule* schedule)
{
  double pc_increment = 1.0/schedule->num_passes();
  double current_pc = pc_increment; // 0 is special

  PCSchedulePtr pc_schedule = new PCSchedule();

  // TODO determine dims.
  
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    ChannelGatherer gatherer(pc_schedule->channel_map, dims);

    I->program->ctrlGraph->dfs(gatherer);
  }
    
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    PCPassPtr p = new PCPass(current_pc);

    ShProgramNodePtr prg = replace_fetches(I->program, pc_schedule->channel_map);
    // Need to do temp buffers here too.

    split_outputs(prg, p->programs);
    
    current_pc += pc_increment;

    I->backend_data = p;
  }
  
  return pc_schedule;
}

PCSchedule::PCSchedule()
{
  glGenOcclusionQueriesNV(1, &m_query);
}

PCSchedule::~PCSchedule()
{
  glDeleteOcclusionQueriesNV(1, &m_query);
}

void PCSchedule::execute_pass(ShPass* pass)
{
  if (pass->count) return;
  
  PCPassPtr pc_pass = shref_dynamic_cast<PCPass>(pass->backend_data);
  SH_DEBUG_ASSERT(pc_pass);

  // Bind vertex program
  shBind(m_vp);

  GLint stencil_mask;
  glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencil_mask);
  
  glDepthFunc(GL_EQUAL);
  glDepthMask(GL_FALSE);

  // TODO: set this to true on ATI.
  bool bad_occlusion_query = false;
  
  for (std::list<ShProgramNodePtr>::iterator I = pc_pass->programs.begin();
       I != pc_pass->programs.end(); ++I) {
    std::list<ShProgramNodePtr>::iterator J = I; ++J;

    if (J == pc_pass->programs.end() && pass->predicate_pass) {
      glClear(GL_STENCIL_BUFFER_BIT);
      glStencilFunc(GL_ALWAYS, 1, stencil_mask);
      glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    }
    ShProgram prg(*I);
    shBind(prg);

    // draw a quad at depth = pc
    // either use RTT or CTT to store result in buffer
  }

  if (pass->predicate_pass) {
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, stencil_mask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    // Some pseudocode
    texture_node.memory(predicate_memory);
    texture_node.size(current_width, current_height);

    if (!bad_occlusion_query) {
      glBeginOcclusionQueryNV(m_query);
    }
    // draw a quad at depth = pc_pred, killing if false

    if (!bad_occlusion_query) {
      glEndOcclusionQueryNV();
    }
    
    // draw a quad at depth = pc_default, killing if true

    // Turn off stencil
    glStencilFunc(GL_ALWAYS, 1, stencil_mask);
    if (bad_occlusion_query) {
      glDepthFunc(GL_EQUAL);
      glDepthMask(GL_FALSE);
      shBind(dummy);
      
      glBeginOcclusionQueryNV(m_query);
      // draw a quad at depth = pc_pred
      glEndOcclusionQueryNV();
    }

    GLuint pred_count;
    glGetOcclusionQueryuivNV(m_query, GL_PIXEL_COUNT_NV, &pred_count);

    pass->predicate_pass->count += pred_count;
    pass->default_pass->count += pass->count - pred_count;
  } else if (pass->default_pass) {
    pass->default_pass->count += pass->count;
  }
  pass->count = 0;
}

ScheduleStrategy* PCScheduler::create()
{
  return new PCScheduler();
}

}
