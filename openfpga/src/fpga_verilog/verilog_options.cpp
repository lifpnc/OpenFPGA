/******************************************************************************
 * Memember functions for data structure FabricVerilogOption
 ******************************************************************************/
#include "vtr_assert.h"

#include "verilog_options.h"

/* begin namespace openfpga */
namespace openfpga {

/**************************************************
 * Public Accessors 
 *************************************************/
std::string FabricVerilogOption::output_directory() const {
  return output_directory_;
}

bool FabricVerilogOption::support_icarus_simulator() const {
  return support_icarus_simulator_;
}

bool FabricVerilogOption::include_timing() const {
  return include_timing_;
}

bool FabricVerilogOption::include_signal_init() const {
  return include_signal_init_;
}

bool FabricVerilogOption::explicit_port_mapping() const {
  return explicit_port_mapping_;
}

bool FabricVerilogOption::compress_routing() const {
  return compress_routing_;
}

bool FabricVerilogOption::verbose_output() const {
  return verbose_output_;
}

/******************************************************************************
 * Private Mutators
 ******************************************************************************/
void FabricVerilogOption::set_output_directory(const std::string& output_dir) {
  output_directory_ = output_dir;
}

void FabricVerilogOption::set_support_icarus_simulator(const bool& enabled) {
  support_icarus_simulator_ = enabled;
}

void FabricVerilogOption::set_include_timing(const bool& enabled) {
  include_timing_ = enabled;
}

void FabricVerilogOption::set_include_signal_init(const bool& enabled) {
  include_signal_init_ = enabled;
}

void FabricVerilogOption::set_explicit_port_mapping(const bool& enabled) {
  explicit_port_mapping_ = enabled;
}

void FabricVerilogOption::set_compress_routing(const bool& enabled) {
  compress_routing_ = enabled;
}

void FabricVerilogOption::set_verbose_output(const bool& enabled) {
  verbose_output_ = enabled;
}

} /* end namespace openfpga */
