#include "PCScheduler.hpp"
#include <list>
#include "GlBackend.hpp"
#include "ShSchedule.hpp"
#include "Utils.hpp"
#include "ShTexCoord.hpp"
#include "ShPosition.hpp"
#include "ShLib.hpp"
#include "ShArray.hpp"
#include "GLXPBufferContext.hpp"
#include "GlTextureStorage.hpp"
#include "ShInstructions.hpp"
#include "ShNibbles.hpp"

namespace {
using namespace SH;
using namespace shgl;

ShProgramNodePtr replace_temps_and_fetches(const ShPass& pass,
                                           std::vector<ShTextureNodePtr>& temp_buffers,
                                           std::map<ShChannelNodePtr, ShTextureNodePtr>& channel_map)
{
  ShProgram result = pass.program->clone();

  // Make a dummy uniform that we'll replace later using the extract operator.
  ShContext::current()->enter(0);
  ShTexCoord2f tc;
  ShContext::current()->exit();

  for (std::list<Mapping>::const_iterator I = pass.inputs.begin(); I != pass.inputs.end(); ++I) {
    const Mapping& mapping = *I;
    SH_DEBUG_ASSERT(mapping.type == MAPPING_TEMP);

    ShTextureNodePtr texnode(temp_buffers[mapping.id]);

    ShProgram fetcher = SH_BEGIN_PROGRAM() {
      // TODO: Handle mapping.offset
      // TOOD: types??
      ShVariable out(new ShVariableNode(SH_OUTPUT, mapping.length, SH_FLOAT));
      ShVariable texVar(texnode);
      ShStatement stmt(out, texVar, SH_OP_TEX, tc);
      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    } SH_END;

    result = result << fetcher;
    
    SH_DEBUG_ASSERT(false);
  }

  // Replace OP_FETCHes
  // TODO: set indexed appropriately
  TexFetcher fetcher(channel_map, tc.node(), true, result.node());

  result.node()->ctrlGraph->dfs(fetcher);

  // Replace the dummy uniform with an input
  result = (result >> tc) << keep<ShTexCoord2f>();
  
  return result.node();
}
                                           
}

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
  virtual void pre_execution(int width, int height);
  virtual void execute_pass(ShPass* pass);
  
  PCChannelMap channel_map;

  std::vector<ShTextureNodePtr> temp_buffers;
  
protected:
  
  void draw_quad(double x1, double y1, double x2, double y2,
                 double d);
  
private:
  GLuint m_query;

  // Vertex program
  ShProgram m_vp;

  // Fragment program, always kills
  ShProgram m_dummy_fp;

  int m_width, m_height;

  ShAttrib2f m_size;

  // Fragment program, kills based on predicate
  ShProgram m_pred_fp;
  ShAttrib1f m_pred_kill;

  // TODO: Should be an Array2D sometimes...
  ShArrayRect<ShAttrib1f> m_pred_texture;
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

  ShTextureDims dims;
  // TODO: This should use a regular PBufferFactory instead.
  FloatExtension ext = GLXPBufferFactory::instance()->get_extension();
  switch (ext) {
  case SH_ARB_NV_FLOAT_BUFFER:
    dims = SH_TEXTURE_RECT;
    break;
  case SH_ARB_ATI_PIXEL_FORMAT_FLOAT:
    dims = SH_TEXTURE_2D;
    break;
  default:
    SH_DEBUG_ERROR("OpenGL PCScheduler: No floating point extension found!");
    return 0;
  }

  for (std::size_t i = 0; i < schedule->num_temps(); i++) {
    ShTextureTraits traits = ShArrayTraits();
    traits.clamping(ShTextureTraits::SH_UNCLAMPED);
    pc_schedule->temp_buffers.push_back(new ShTextureNode(dims, 4, SH_FLOAT, traits, 1, 1, 1, 0));
  }
  
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    ChannelGatherer gatherer(pc_schedule->channel_map, dims);

    I->program->ctrlGraph->dfs(gatherer);
  }
    
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    PCPassPtr p = new PCPass(current_pc);

    ShProgramNodePtr prg = I->program;
    prg = replace_temps_and_fetches(*I, pc_schedule->temp_buffers, pc_schedule->channel_map);

    //split_outputs(prg, p->programs);
    
    current_pc += pc_increment;

    I->backend_data = p;
  }
  
  return pc_schedule;
}

PCSchedule::PCSchedule()
  : m_width(0), m_height(0)
{
  glGenOcclusionQueriesNV(1, &m_query);

  // Ensure we're at the global scope
  ShContext::current()->enter(0);
  
  m_vp = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShOutputTexCoord2f tc;

    tc = pos(0,1);
    pos(0,1) = pos(0,1) * (rcp(m_size) * 0.5) - 0.5;
  } SH_END;

  m_dummy_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    discard(ShConstAttrib1f(1.0));
  } SH_END;

  m_pred_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputTexCoord2f tc;

    ShAttrib1f predicate = m_pred_texture[tc];
    
    discard(cond(m_pred_kill, predicate, 1.0 - (predicate > 0)));
  } SH_END;
  
  ShContext::current()->exit();
}

PCSchedule::~PCSchedule()
{
  glDeleteOcclusionQueriesNV(1, &m_query);
}

void PCSchedule::pre_execution(int width, int height)
{
  if (width == m_width && height == m_height) return;
  
  SH_DEBUG_ASSERT(width > 0);
  SH_DEBUG_ASSERT(height > 0);
  
  SH_DEBUG_ASSERT(!(width & (width - 1)));
  SH_DEBUG_ASSERT(!(height & (height - 1)));

  m_width = width;
  m_height = height;

  m_size = ShAttrib2f(m_width, m_height);

  for (std::size_t i = 0; i < temp_buffers.size(); i++) {
    temp_buffers[i]->setTexSize(m_width, m_height);
    temp_buffers[i]->memory(new GlTextureMemory(temp_buffers[i]));
  }
  m_pred_texture.size(m_width, m_height);
  m_pred_texture.memory(new GlTextureMemory(m_pred_texture.node()));
}

void PCSchedule::draw_quad(double x1, double y1, double x2, double y2,
                           double d)
{
  glBegin(GL_QUADS); {
    glVertex3f(x1, y1, d);
    glVertex3f(x1, y2, d);
    glVertex3f(x2, y2, d);
    glVertex3f(x2, y1, d);
  } glEnd();
}

void PCSchedule::execute_pass(ShPass* pass)
{
  if (pass->count) return;

  glPushAttrib(GL_ENABLE_BIT);
  
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);
  
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
    draw_quad(0.0, 0.0, m_width, m_height, pc_pass->pc);
    // either use RTT or CTT to store result in buffer
  }

  if (pass->predicate_pass) {
    PCPassPtr pred_pc_pass = shref_dynamic_cast<PCPass>(pass->predicate_pass->backend_data);
    SH_DEBUG_ASSERT(pred_pc_pass);
    PCPassPtr default_pc_pass = shref_dynamic_cast<PCPass>(pass->default_pass->backend_data);
    SH_DEBUG_ASSERT(default_pc_pass);

    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, stencil_mask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // Set up predication texture.
    m_pred_texture.memory(temp_buffers[pass->predicate.id]->memory());
    m_pred_texture.size(m_width, m_height);

    shBind(m_pred_fp);
    
    if (!bad_occlusion_query) {
      glBeginOcclusionQueryNV(m_query);
    }
    
    // draw a quad at depth = pc_pred, killing if false
    m_pred_kill = 0.0;
    draw_quad(0.0, 0.0, m_width, m_height, pred_pc_pass->pc);

    if (!bad_occlusion_query) {
      glEndOcclusionQueryNV();
    }

    m_pred_kill = 1.0;
    // draw a quad at depth = pc_default, killing if true
    draw_quad(0.0, 0.0, m_width, m_height, default_pc_pass->pc);

    // Turn off stencil
    glStencilFunc(GL_ALWAYS, 1, stencil_mask);
    if (bad_occlusion_query) {
      glDepthFunc(GL_EQUAL);
      glDepthMask(GL_FALSE);
      shBind(m_dummy_fp);
      
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

  glPopAttrib();
}

ScheduleStrategy* PCScheduler::create()
{
  return new PCScheduler();
}

}
