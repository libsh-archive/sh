#ifndef _RDSBACKEND_HPP
#define _RDSBACKEND_HPP

#include "ShProgram.hpp"
#include "ShStream.hpp"
#include "RDS.hpp"

#include <sys/time.h>
#include <time.h>

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

  void RDSBackend::dump(RDS rds, char* complete, char* partitioned);
  void RDSBackend::compare(SH::ShProgramNodePtr p);
};

class Timer {
public:
  Timer() { start(); }

  void start() { gettimeofday(&startval, 0); }

  long diff() {
    timeval endval;
    gettimeofday(&endval, 0);
    return (endval.tv_sec - startval.tv_sec)*1000000
           + (endval.tv_usec - startval.tv_usec);
  }

private:
  timeval startval;
};


#endif
