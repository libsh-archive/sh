#ifndef _RDSBACKEND_HPP
#define _RDSBACKEND_HPP

#include "ShProgram.hpp"
#include "ShStream.hpp"

class RDSBackend : public SH::ShBackend {
public:

  RDSBackend(void);
  ~RDSBackend(void);
  std::string name(void) const;
  
  /*
   * These will temporarily just output the RDSed versions of their programs
   */
  virtual SH::ShBackendCodePtr generateCode(const std::string& target,
                                            const SH::ShProgramNodeCPtr& shader);

  virtual void execute(const SH::ShProgramNodeCPtr& program, SH::ShStream& dest);

};

#endif
