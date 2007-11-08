#ifndef PROGRAMSTATS_HPP
#define PROGRAMSTATS_HPP

#include <sh/sh.hpp>
#include <string>
#include <sstream>

struct ProgramStats {
  std::string prefix;
  int temps, scalar_temps;
  int instr_count, scalar_instr_count;
  int live, scalar_live;
  float avg_live, avg_scalar_live;

  float inbind, outbind, render;

  ProgramStats(const std::string& prefix=""): prefix(prefix), temps(-1), scalar_temps(-1), instr_count(-1), scalar_instr_count(-1),
    live(-1), scalar_live(-1), avg_live(-1), avg_scalar_live(-1),
    inbind(-1), outbind(-1), render(-1) {}

  /* update program stats - only do this right after compiling */
  void update(const std::string& backend) {
      instr_count = SH::Context::current()->get_stat((backend + "_instr_count").c_str());
      scalar_instr_count = SH::Context::current()->get_stat((backend + "_scalar_instr_count").c_str());
      temps = SH::Context::current()->get_stat((backend + "_num_temps").c_str());
      scalar_temps = SH::Context::current()->get_stat((backend + "_num_scalar_temps").c_str());
      live = SH::Context::current()->get_stat((backend + "_num_live").c_str());
      scalar_live = SH::Context::current()->get_stat((backend + "_num_scalar_live").c_str());
      avg_live = SH::Context::current()->get_stat((backend + "_avg_num_live").c_str());
      avg_scalar_live = SH::Context::current()->get_stat((backend + "_avg_num_scalar_live").c_str());
  }

  /* update timings - do this after each stream program run */
  void update_times() {
      inbind = SH::Context::current()->get_stat("fbo_binding"); 
      outbind = SH::Context::current()->get_stat("fbo_texbind_output"); 
      render = SH::Context::current()->get_stat("fbo_render"); 
  }

  std::string header() {
    std::ostringstream sout;
    sout << "," << prefix << "instr_count," << prefix << "scalar_instr_count," << prefix << "temps," << prefix << "scalar_temps," << prefix << "live," << prefix << "scalar_live," << prefix 
         << "avg_live," << prefix << "avg_scalar_live,"
         << prefix << "_inbind,"
         << prefix << "_outbind,"
         << prefix << "_render";
    return sout.str();
  }

  std::string csv() {
    std::ostringstream sout;
    sout << "," <<instr_count<< "," <<scalar_instr_count<< "," <<temps<< "," <<scalar_temps<< "," 
         <<live<< "," <<scalar_live<< "," <<avg_live<< "," <<avg_scalar_live << ","
         << inbind << "," << outbind << "," << render; 
    return sout.str();
  }
};

#endif
