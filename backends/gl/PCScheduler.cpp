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
#include "ShManipulator.hpp"
#include "GlTextures.hpp"
#include "ShClamping.hpp"

namespace {
using namespace SH;
using namespace shgl;

ShProgramNodePtr replace_temps_and_fetches(const ShPass& pass,
                                           std::vector<ShTextureNodePtr>& temp_buffers,
                                           std::map<ShChannelNodePtr, ShTextureNodePtr>& channel_map)
{
  std::cerr << "Replacing: " << std::endl << pass.program->describe_interface() << std::endl;
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
  }

  // Replace OP_FETCHes
  // TODO: set indexed appropriately
  TexFetcher fetcher(channel_map, tc.node(), true, result.node());

  result.node()->ctrlGraph->dfs(fetcher);

  // Replace the dummy uniform with an input
  result = (result >> tc) << keep<ShTexCoord2f>();

  result.node()->collectVariables();
  
  std::cerr << "Replace result: " << std::endl << result.describe_interface() << std::endl;
  return result.node();
}

void split_outputs(const ShProgramNodePtr& program,
                   std::list<ShProgramNodePtr>& programs)
{
  std::cerr << "Splitting: " << std::endl << program->describe_interface() << std::endl;
  if (program->outputs.size() < 2) {
    programs.push_back(program);
  } else {
    int i = 0;
    for (ShProgramNode::VarList::const_iterator I = program->outputs_begin(); I != program->outputs_end(); ++I, ++i) {
      ShProgram p = shSwizzle(i) << shref_const_cast<ShProgramNode>(program);
      p.node()->target() = program->target();
      programs.push_back(p.node());
    }
  }
  std::cerr << "Split program into " << programs.size() << " programs." << std::endl;
}



void copy_framebuffer_to_texture(PBufferContextPtr context,
                                 const ShTextureNodePtr& texnode)
{
  glFinish();
  std::cerr << "Before copy: "; texnode->memory()->dump(std::cerr) << std::endl;
  PBufferStoragePtr fbs = context->make_storage(texnode->memory().object());
  fbs->dirtyall();
  
  shGlBindTexture(texnode, GL_TEXTURE0_ARB);

  std::cerr << "After copy: "; texnode->memory()->dump(std::cerr) << std::endl;
  texnode->memory()->removeStorage(fbs);
  std::cerr << "After removal: "; texnode->memory()->dump(std::cerr) << std::endl;
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

class PCSchedule : public ShBackendSchedule {
public:
  PCSchedule(ShSchedule* schedule);
  
  virtual ~PCSchedule();
  virtual void pre_execution(int width, int height, const ShStream& stream);
  virtual void execute_pass(ShPass* pass);
  
  PCChannelMap channel_map;

  std::vector<ShTextureNodePtr> temp_buffers;
  
protected:
  
  void draw_quad(double x1, double y1, double x2, double y2,
                 double d);
  
private:

  ShSchedule* m_schedule;
  
  GLuint m_query;

  // Vertex program
  ShProgram m_vp;

  // Fragment program, always kills
  ShProgram m_dummy_fp;

  int m_width, m_height;

  PBufferContextPtr m_context;

  ShAttrib2f m_size;

  // Fragment program, kills based on predicate
  ShProgram m_pred_fp;
  ShAttrib1f m_pred_kill;

  // TODO: Should be an Array2D sometimes...
  ShUnclamped<ShArrayRect<ShAttrib4f> > m_pred_texture;
  ShUnclamped<ShArrayRect<ShAttrib4f> > m_copy_texture;

  ShProgram m_black_fp;
  ShProgram m_copy_fp;
  
  std::vector<ShChannelNodePtr> m_output_channels;
};

typedef ShPointer<PCSchedule> PCSchedulePtr;
typedef ShPointer<const PCSchedule> PCScheduleCPtr;

PCScheduler::PCScheduler()
{
}


ShBackendSchedulePtr PCScheduler::prepare(ShSchedule* schedule)
{
  std::cerr << "Preparing schedule " << schedule << std::endl;
  double pc_increment = 1.0/schedule->num_passes();
  double current_pc = pc_increment; // 0 is special

  PCSchedulePtr pc_schedule = new PCSchedule(schedule);

  ShTextureDims dims;
  // TODO: This should use a regular PBufferFactory instead.
  std::cerr << "Getting FPE..." << std::endl;
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

  
  std::cerr << "Schedule has " << schedule->num_temps() << std::endl;
  for (std::size_t i = 0; i < schedule->num_temps(); i++) {
    std::cerr << "Setting up temporary..." << std::endl;
    ShTextureTraits traits = ShArrayTraits();
    traits.clamping(ShTextureTraits::SH_UNCLAMPED);
    pc_schedule->temp_buffers.push_back(new ShTextureNode(dims, 4, SH_FLOAT, traits, 1, 1, 1, 0));
  }
  
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    SH_DEBUG_ASSERT(!I->program->outputs.empty());
    std::cerr << "Gathering..." << std::endl;
    ChannelGatherer gatherer(pc_schedule->channel_map, dims);

    I->program->ctrlGraph->dfs(gatherer);
  }

  std::cerr << "After gathering, channel_map has size " << pc_schedule->channel_map.size() << std::endl;
  
  for (ShSchedule::PassList::iterator I = schedule->begin(); I != schedule->end();
       ++I) {
    SH_DEBUG_ASSERT(!I->program->outputs.empty());
    PCPassPtr p = new PCPass(current_pc);

    ShProgramNodePtr prg = I->program;
    prg = replace_temps_and_fetches(*I, pc_schedule->temp_buffers, pc_schedule->channel_map);

    prg->target() = "gpu:fragment";
    
    split_outputs(prg, p->programs);
    
    current_pc += pc_increment;

    I->backend_data = p;
  }
  
  std::cerr << "All passes setup." << std::endl;
  return pc_schedule;
}

PCSchedule::PCSchedule(ShSchedule* schedule)
  : m_schedule(schedule), m_width(0), m_height(0)
{
  // Ensure we're at the global scope
  ShContext::current()->enter(0);
  
  m_vp = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShOutputTexCoord2f tc;

    // TODO: change depending on rect vs 2d
    tc = pos(0,1);
    pos(0,1) = 2.0 * pos(0,1) * (rcp(m_size)) - 1.0;
  } SH_END;

  m_dummy_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    discard(ShConstAttrib1f(1.0));
  } SH_END;

  m_black_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    ShOutputAttrib4f out = ShConstAttrib4f(0.9, 0.8, 0.7, 0.6);
  } SH_END;

  m_pred_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputTexCoord2f tc;

    ShAttrib1f predicate = m_pred_texture[tc](0);
    
    discard(cond(m_pred_kill, predicate, 1.0 - (predicate > 0)));
  } SH_END;

  m_copy_fp = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputTexCoord2f tc;
    ShOutputAttrib4f out;

    out = m_copy_texture[tc];
  } SH_END;
  
  ShContext::current()->exit();
}

PCSchedule::~PCSchedule()
{
  SH_GL_CHECK_ERROR(glDeleteOcclusionQueriesNV(1, &m_query));
}

void PCSchedule::pre_execution(int width, int height, const ShStream& outputs)
{
  std::cerr << "Pre-execution " << width << "x" << height << " with " << outputs.size() << " outputs." << std::endl;
  m_output_channels.clear();
  for (ShStream::NodeList::const_iterator I = outputs.begin(); I != outputs.end(); ++I) {
    m_output_channels.push_back(*I);
  }
  
  std::cerr << "Setup channels." << std::endl;
  if (width == m_width && height == m_height) return;
  
  std::cerr << "Setting up viewport size." << std::endl;
  SH_DEBUG_ASSERT(width > 0);
  SH_DEBUG_ASSERT(height > 0);
  
  SH_DEBUG_ASSERT(!(width & (width - 1)));
  SH_DEBUG_ASSERT(!(height & (height - 1)));

  m_width = width;
  m_height = height;

  m_size = ShAttrib2f(m_width, m_height);

  std::cerr << "Setting up context" << std::endl;
  m_context = GLXPBufferFactory::instance()->get_context(m_width, m_height, this);

  std::cerr << "Activating." << std::endl;
  PBufferHandlePtr old_context = m_context->activate();

  SH_GL_CHECK_ERROR(glEnable(GL_VERTEX_PROGRAM_ARB));
  SH_GL_CHECK_ERROR(glEnable(GL_FRAGMENT_PROGRAM_ARB));
  SH_GL_CHECK_ERROR(glClearColor(0.3, 0.4, 0.5, 0.6));
  SH_GL_CHECK_ERROR(glDisable(GL_CULL_FACE));
  SH_GL_CHECK_ERROR(glGenOcclusionQueriesNV(1, &m_query));

  for (std::size_t i = 0; i < temp_buffers.size(); i++) {
    std::cerr << "Allocate temporary buffer " << i << std::endl;
    temp_buffers[i]->setTexSize(m_width, m_height);
    temp_buffers[i]->memory(new GlTextureMemory(temp_buffers[i]));
  }
  std::cerr << "Allocate predicate texture" << std::endl;
  m_pred_texture.size(m_width, m_height);
  m_pred_texture.memory(new GlTextureMemory(m_pred_texture.node()));
  m_copy_texture.size(m_width, m_height);
  m_copy_texture.memory(new GlTextureMemory(m_pred_texture.node()));

  std::cerr << "Initializing depth buffer." << std::endl;

  SH_GL_CHECK_ERROR(glClear(GL_DEPTH_BUFFER_BIT));

  shBind(m_vp);
  shBind(m_black_fp);
  SH_GL_CHECK_ERROR(glEnable(GL_STENCIL_TEST));
  SH_GL_CHECK_ERROR(glEnable(GL_DEPTH_TEST));

  SH_GL_CHECK_ERROR(glDepthFunc(GL_ALWAYS));
  SH_GL_CHECK_ERROR(glDepthMask(GL_TRUE));
  

  // Initialize depth buffer
  draw_quad(0.0, 0.0, m_width, m_height, 1.0/m_schedule->num_passes());

  
  std::cerr << "Restoring context." << std::endl;
  if (old_context) old_context->restore();

}

void PCSchedule::draw_quad(double x1, double y1, double x2, double y2,
                           double d)
{
  glBegin(GL_QUADS); {
    glVertex3f(x1, y1, d);
    glVertex3f(x2, y1, d);
    glVertex3f(x2, y2, d);
    glVertex3f(x1, y2, d);
  } SH_GL_CHECK_ERROR(glEnd());
}

void PCSchedule::execute_pass(ShPass* pass)
{
  std::cerr << "Attempting pass execution [" << pass << "]" << std::endl;
  if (!pass->count) return;
  std::cerr << "Performing execution." << std::endl;

  // Assume CTT for now, so just one context
  PBufferHandlePtr old_context = m_context->activate();
  
  std::cerr << "glEnable calls." << std::endl;
  SH_GL_CHECK_ERROR(glPushAttrib(GL_ENABLE_BIT));
  
  //SH_GL_CHECK_ERROR(glDisable(GL_STENCIL_TEST));
  // SH_GL_CHECK_ERROR(glDisable(GL_DEPTH_TEST));
  
  std::cerr << "Retrieve backend data." << std::endl;
  PCPassPtr pc_pass = shref_dynamic_cast<PCPass>(pass->backend_data);
  SH_DEBUG_ASSERT(pc_pass);

  // Bind vertex program
  shBind(m_vp);

  GLint stencil_mask;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_STENCIL_VALUE_MASK, &stencil_mask));
  
  SH_GL_CHECK_ERROR(glDepthMask(GL_FALSE));

  // TODO: set this to true on ATI.
  bool bad_occlusion_query = false;
  
  SH_GL_CHECK_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
  SH_GL_CHECK_ERROR(glStencilFunc(GL_ALWAYS, 1, stencil_mask));
  SH_GL_CHECK_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));

  // TODO: Should really store the mappings in pc_pass, since we might
  // have multiple outputs with ATI_draw_buffers
  std::list<Mapping>::const_iterator O = pass->outputs.begin();
  for (std::list<ShProgramNodePtr>::iterator I = pc_pass->programs.begin();
       I != pc_pass->programs.end(); ++I, ++O) {
    std::cerr << "Executing program" << std::endl;
    std::list<ShProgramNodePtr>::iterator J = I; ++J;

    m_copy_texture.size(m_width, m_height);
    if (O->type == MAPPING_TEMP) {
      std::cerr << "Setting up temporary " << O->id << std::endl;
      m_copy_texture.memory(temp_buffers[O->id]->memory());
      m_copy_texture.memory()->dump(std::cerr) << std::endl;
      
    } else if (O->type == MAPPING_OUTPUT) {
      std::cerr << "Setting up output" << std::endl;
      // TODO: perhaps handle non-stream outputs?
      m_copy_texture.memory(m_output_channels[O->id]->memory());
      m_copy_texture.node()->set_count(m_output_channels[O->id]->count());
      m_copy_texture.memory()->dump(std::cerr) << std::endl;
    } else {
      SH_DEBUG_ASSERT(false);
    }

    std::cerr << "Running init" << std::endl;
    SH_GL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));
    shBind(m_copy_fp);

    SH_GL_CHECK_ERROR(glDepthFunc(GL_ALWAYS));
    draw_quad(0.0, 0.0, m_width, m_height, pc_pass->pc);
    
    if (J == pc_pass->programs.end() && pass->predicate_pass) {
      std::cerr << "Setting up stencil" << std::endl;
      SH_GL_CHECK_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
      SH_GL_CHECK_ERROR(glStencilFunc(GL_ALWAYS, 1, stencil_mask));
      SH_GL_CHECK_ERROR(glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP));
    }
    std::cerr << "Running pass" << std::endl;
    ShProgram prg(*I);
    shBind(prg);

    SH_GL_CHECK_ERROR(glDepthFunc(GL_EQUAL));
    //SH_GL_CHECK_ERROR(glDepthFunc(GL_ALWAYS));
    // draw a quad at depth = pc
    draw_quad(0.0, 0.0, m_width, m_height, pc_pass->pc);
    // TODO either use RTT or CTT to store result in buffer

    std::cerr << "CTT" << std::endl;
    copy_framebuffer_to_texture(m_context, m_copy_texture.node());
  }

  if (pass->predicate_pass) {
    std::cerr << "Predication" << std::endl;
    PCPassPtr pred_pc_pass = shref_dynamic_cast<PCPass>(pass->predicate_pass->backend_data);
    SH_DEBUG_ASSERT(pred_pc_pass);
    PCPassPtr default_pc_pass = shref_dynamic_cast<PCPass>(pass->default_pass->backend_data);
    SH_DEBUG_ASSERT(default_pc_pass);

    SH_GL_CHECK_ERROR(glDepthFunc(GL_ALWAYS));
    SH_GL_CHECK_ERROR(glDepthMask(GL_TRUE));
    SH_GL_CHECK_ERROR(glStencilFunc(GL_EQUAL, 1, stencil_mask));
    SH_GL_CHECK_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));

    // Set up predication texture.
    m_pred_kill = 0.0;
    m_pred_texture.memory(temp_buffers[pass->predicate.id]->memory());
    m_pred_texture.size(m_width, m_height);

    shBind(m_pred_fp);
    
    if (!bad_occlusion_query) {
      SH_GL_CHECK_ERROR(glBeginOcclusionQueryNV(m_query));
    }
    
    // draw a quad at depth = pc_pred, killing if false
    m_pred_kill = 0.0;
    draw_quad(0.0, 0.0, m_width, m_height, pred_pc_pass->pc);

    if (!bad_occlusion_query) {
      SH_GL_CHECK_ERROR(glEndOcclusionQueryNV());
    }

    m_pred_kill = 1.0;
    // draw a quad at depth = pc_default, killing if true
    draw_quad(0.0, 0.0, m_width, m_height, default_pc_pass->pc);

    // Turn off stencil
    SH_GL_CHECK_ERROR(glStencilFunc(GL_ALWAYS, 1, stencil_mask));
    if (bad_occlusion_query) {
      SH_GL_CHECK_ERROR(glDepthFunc(GL_EQUAL));
      SH_GL_CHECK_ERROR(glDepthMask(GL_FALSE));
      shBind(m_dummy_fp);
      
      SH_GL_CHECK_ERROR(glBeginOcclusionQueryNV(m_query));
      // draw a quad at depth = pc_pred
      draw_quad(0.0, 0.0, m_width, m_height, pred_pc_pass->pc);
      SH_GL_CHECK_ERROR(glEndOcclusionQueryNV());
    }

    GLuint pred_count;
    SH_GL_CHECK_ERROR(glGetOcclusionQueryuivNV(m_query, GL_PIXEL_COUNT_NV, &pred_count));

    pass->predicate_pass->count += pred_count;
    pass->default_pass->count += pass->count - pred_count;

    std::cerr << "from " << pass->count << " there were " << pred_count << " true, "
              << (pass->count - pred_count) << " false predicates." << std::endl;
  } else if (pass->default_pass) {
    pass->default_pass->count += pass->count;
  }
  pass->count = 0;
  m_schedule->remove_eligible(pass);
  if (pass->predicate_pass && pass->predicate_pass->count > 0) {
    SH_DEBUG_PRINT("Add predicate pass");
    m_schedule->add_eligible(pass->predicate_pass);
  }
  if (pass->default_pass && pass->default_pass->count > 0) {
    SH_DEBUG_PRINT("Add default pass");
    m_schedule->add_eligible(pass->default_pass);
  }

  SH_GL_CHECK_ERROR(glPopAttrib());

  if (old_context) old_context->restore();
}

ScheduleStrategy* PCScheduler::create()
{
  return new PCScheduler();
}

}
