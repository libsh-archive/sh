#ifndef PCSCHEDULER_HPP
#define PCSCHEDULER_HPP

#include "GlBackend.hpp"

namespace shgl {

struct PCScheduler : public ScheduleStrategy {
  PCScheduler();

  virtual SH::ShBackendSchedulePtr prepare(SH::ShSchedule* schedule);

  virtual ScheduleStrategy* create();
  
};

}
#endif
