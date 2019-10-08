/********************************************************************
 * This file includes functions to generate module/port names for 
 * Verilog and SPICE netlists 
 *
 * IMPORTANT: keep all the naming functions in this file to be 
 * generic for both Verilog and SPICE generators 
 ********************************************************************/
#include "vtr_assert.h"

#include "sides.h"
#include "fpga_x2p_utils.h"
#include "circuit_library_utils.h"
#include "fpga_x2p_naming.h"

/************************************************
 * Generate the node name for a multiplexing structure 
 * Case 1 : If there is an intermediate buffer followed by,
 *          the node name will be mux_l<node_level>_in_buf
 * Case 1 : If there is NO intermediate buffer followed by,
 *          the node name will be mux_l<node_level>_in
 ***********************************************/
std::string generate_mux_node_name(const size_t& node_level, 
                                   const bool& add_buffer_postfix) {
  /* Generate the basic node_name */
  std::string node_name = "mux_l" + std::to_string(node_level) + "_in";

  /* Add a postfix upon requests */
  if (true == add_buffer_postfix)  {
    /* '1' indicates that the location is needed */
    node_name += "_buf";
  }

  return node_name;
}

/************************************************
 * Generate the module name for a multiplexer in Verilog format
 * Different circuit model requires different names: 
 * 1. LUTs are named as <model_name>_mux
 * 2. MUXes are named as <model_name>_size<num_inputs>
 ***********************************************/
std::string generate_mux_subckt_name(const CircuitLibrary& circuit_lib, 
                                     const CircuitModelId& circuit_model, 
                                     const size_t& mux_size, 
                                     const std::string& postfix) {
  std::string module_name = circuit_lib.model_name(circuit_model); 
  /* Check the model type and give different names */
  if (SPICE_MODEL_MUX == circuit_lib.model_type(circuit_model)) {
    module_name += "_size";
    module_name += std::to_string(mux_size);
  } else {  
    VTR_ASSERT(SPICE_MODEL_LUT == circuit_lib.model_type(circuit_model));
    module_name += "_mux";
  }

  /* Add postfix if it is not empty */
  if (!postfix.empty()) {
    module_name += postfix;
  }

  return module_name;
}

/************************************************
 * Generate the module name of a branch for a
 * multiplexer in Verilog format
 ***********************************************/
std::string generate_mux_branch_subckt_name(const CircuitLibrary& circuit_lib, 
                                            const CircuitModelId& circuit_model, 
                                            const size_t& mux_size, 
                                            const size_t& branch_mux_size, 
                                            const std::string& postfix) {
  /* If the tgate spice model of this MUX is a MUX2 standard cell,
   * the mux_subckt name will be the name of the standard cell
   */
  CircuitModelId subckt_model = circuit_lib.pass_gate_logic_model(circuit_model);
  if (SPICE_MODEL_GATE == circuit_lib.model_type(subckt_model)) {
    VTR_ASSERT (SPICE_MODEL_GATE_MUX2 == circuit_lib.gate_type(subckt_model));
    return circuit_lib.model_name(subckt_model);
  }
  std::string branch_postfix = postfix + "_size" + std::to_string(branch_mux_size);

  return generate_mux_subckt_name(circuit_lib, circuit_model, mux_size, branch_postfix);
}

/************************************************
 * Generate the module name of a local decoder
 * for multiplexer
 ***********************************************/
std::string generate_mux_local_decoder_subckt_name(const size_t& addr_size, 
                                                   const size_t& data_size) {
  std::string subckt_name = "decoder";
  subckt_name += std::to_string(addr_size);
  subckt_name += "to";
  subckt_name += std::to_string(data_size);

  return subckt_name;
} 

/************************************************
 * Generate the module name of a routing track wire
 ***********************************************/
std::string generate_segment_wire_subckt_name(const std::string& wire_model_name, 
                                              const size_t& segment_id) {
  std::string segment_wire_subckt_name = wire_model_name + "_seg" + std::to_string(segment_id);

  return segment_wire_subckt_name;
} 

/*********************************************************************
 * Generate the port name for the mid-output of a routing track wire
 * Mid-output is the output that is wired to a Connection block multiplexer.
 *      
 *                  |    CLB     |
 *                  +------------+
 *                        ^
 *                        |
 *           +------------------------------+
 *           | Connection block multiplexer |
 *           +------------------------------+
 *                        ^
 *                        |  mid-output         +--------------
 *              +--------------------+          |
 *    input --->| Routing track wire |--------->| Switch Block
 *              +--------------------+  output  |
 *                                              +--------------
 *
 ********************************************************************/
std::string generate_segment_wire_mid_output_name(const std::string& regular_output_name) {
  /* TODO: maybe have a postfix? */
  return std::string("mid_" + regular_output_name);
} 

/*********************************************************************
 * Generate the module name for a memory sub-circuit 
 ********************************************************************/
std::string generate_memory_module_name(const CircuitLibrary& circuit_lib,
                                        const CircuitModelId& circuit_model, 
                                        const CircuitModelId& sram_model, 
                                        const std::string& postfix) {
  return std::string( circuit_lib.model_name(circuit_model) + "_" + circuit_lib.model_name(sram_model) + postfix );
}

/*********************************************************************
 * Generate the netlist name for a unique routing block 
 * It could be 
 * 1. Routing channel
 * 2. Connection block
 * 3. Switch block
 * A unique block id should be given
 *********************************************************************/
std::string generate_routing_block_netlist_name(const std::string& prefix, 
                                                const size_t& block_id,
                                                const std::string& postfix) {
  return std::string( prefix + std::to_string(block_id) + postfix );
}

/*********************************************************************
 * Generate the netlist name for a routing block with a given coordinate
 * It could be 
 * 1. Routing channel
 * 2. Connection block
 * 3. Switch block
 *********************************************************************/
std::string generate_routing_block_netlist_name(const std::string& prefix, 
                                                const vtr::Point<size_t>& coordinate,
                                                const std::string& postfix) {
  return std::string( prefix + std::to_string(coordinate.x()) + std::string("_") + std::to_string(coordinate.y()) + postfix );
}

/*********************************************************************
 * Generate the netlist name for a connection block with a given coordinate
 *********************************************************************/
std::string generate_connection_block_netlist_name(const t_rr_type& cb_type, 
                                                   const vtr::Point<size_t>& coordinate,
                                                   const std::string& postfix) {
  std::string prefix("cb");
  switch (cb_type) {
  case CHANX:
    prefix += std::string("x_");
    break;
  case CHANY:
    prefix += std::string("y_");
    break;
  default:
    vpr_printf(TIO_MESSAGE_ERROR, 
               "(File: %s [LINE%d]) Invalid type of connection block!\n",
               __FILE__, __LINE__);
    exit(1);
  }

  return generate_routing_block_netlist_name(prefix, coordinate, postfix);
}

/*********************************************************************
 * Generate the module name for a unique routing channel
 *********************************************************************/
std::string generate_routing_channel_module_name(const t_rr_type& chan_type, 
                                                 const size_t& block_id) {
  /* Channel must be either CHANX or CHANY */
  VTR_ASSERT( (CHANX == chan_type) || (CHANY == chan_type) );

  /* Create a map between chan_type and module_prefix */
  std::map<t_rr_type, std::string> module_prefix_map;
  /* TODO: use a constexpr string to replace the fixed name? */
  module_prefix_map[CHANX] = std::string("chanx");
  module_prefix_map[CHANY] = std::string("chany");

  return std::string( module_prefix_map[chan_type] + std::string("_") + std::to_string(block_id) + std::string("_") );
}

/*********************************************************************
 * Generate the module name for a routing channel with a given coordinate
 *********************************************************************/
std::string generate_routing_channel_module_name(const t_rr_type& chan_type, 
                                                 const vtr::Point<size_t>& coordinate) {
  /* Channel must be either CHANX or CHANY */
  VTR_ASSERT( (CHANX == chan_type) || (CHANY == chan_type) );

  /* Create a map between chan_type and module_prefix */
  std::map<t_rr_type, std::string> module_prefix_map;
  /* TODO: use a constexpr string to replace the fixed name? */
  module_prefix_map[CHANX] = std::string("chanx");
  module_prefix_map[CHANY] = std::string("chany");

  return std::string( module_prefix_map[chan_type] + std::to_string(coordinate.x()) + std::string("_") + std::to_string(coordinate.y()) + std::string("_") );
}

/*********************************************************************
 * Generate the port name for a routing track with a given coordinate
 * and port direction
 *********************************************************************/
std::string generate_routing_track_port_name(const t_rr_type& chan_type, 
                                             const vtr::Point<size_t>& coordinate,
                                             const size_t& track_id,
                                             const PORTS& port_direction) {
  /* Channel must be either CHANX or CHANY */
  VTR_ASSERT( (CHANX == chan_type) || (CHANY == chan_type) );

  /* Create a map between chan_type and module_prefix */
  std::map<t_rr_type, std::string> module_prefix_map;
  /* TODO: use a constexpr string to replace the fixed name? */
  module_prefix_map[CHANX] = std::string("chanx");
  module_prefix_map[CHANY] = std::string("chany");

  std::string port_name = module_prefix_map[chan_type]; 
  port_name += std::string("_" + std::to_string(coordinate.x()) + std::string("__") + std::to_string(coordinate.y()) + std::string("__"));

  switch (port_direction) {
  case OUT_PORT:
    port_name += std::string("out_"); 
    break;
  case IN_PORT:
    port_name += std::string("in_"); 
    break;
  default:
    vpr_printf(TIO_MESSAGE_ERROR, 
               "(File: %s [LINE%d]) Invalid direction of chan_rr_node!\n",
               __FILE__, __LINE__);
    exit(1);
  }

  /* Add the track id to the port name */
  port_name += std::to_string(track_id) + std::string("_");

  return port_name;
}

/*********************************************************************
 * Generate the middle output port name for a routing track 
 * with a given coordinate
 *********************************************************************/
std::string generate_routing_track_middle_output_port_name(const t_rr_type& chan_type, 
                                                           const vtr::Point<size_t>& coordinate,
                                                           const size_t& track_id) {
  /* Channel must be either CHANX or CHANY */
  VTR_ASSERT( (CHANX == chan_type) || (CHANY == chan_type) );

  /* Create a map between chan_type and module_prefix */
  std::map<t_rr_type, std::string> module_prefix_map;
  /* TODO: use a constexpr string to replace the fixed name? */
  module_prefix_map[CHANX] = std::string("chanx");
  module_prefix_map[CHANY] = std::string("chany");

  std::string port_name = module_prefix_map[chan_type]; 
  port_name += std::string("_" + std::to_string(coordinate.x()) + std::string("__") + std::to_string(coordinate.y()) + std::string("__"));

  port_name += std::string("midout_"); 

  /* Add the track id to the port name */
  port_name += std::to_string(track_id) + std::string("_");

  return port_name;
}

/*********************************************************************
 * Generate the module name for a switch block with a given coordinate
 *********************************************************************/
std::string generate_switch_block_module_name(const vtr::Point<size_t>& coordinate) {
  return std::string( "sb_" + std::to_string(coordinate.x()) + std::string("__") + std::to_string(coordinate.y()) + std::string("_") );
}

/*********************************************************************
 * Generate the module name for a connection block with a given coordinate
 *********************************************************************/
std::string generate_connection_block_module_name(const t_rr_type& cb_type, 
                                                  const vtr::Point<size_t>& coordinate) {
  std::string prefix("cb");
  switch (cb_type) {
  case CHANX:
    prefix += std::string("x_");
    break;
  case CHANY:
    prefix += std::string("y_");
    break;
  default:
    vpr_printf(TIO_MESSAGE_ERROR, 
               "(File: %s [LINE%d]) Invalid type of connection block!\n",
               __FILE__, __LINE__);
    exit(1);
  }

  return std::string( prefix + std::to_string(coordinate.x()) + std::string("__") + std::to_string(coordinate.y()) + std::string("_") );
}

/*********************************************************************
 * Generate the port name for a Grid
 * TODO: add more comments about why we need different names for 
 * top and non-top netlists
 *********************************************************************/
std::string generate_grid_port_name(const vtr::Point<size_t>& coordinate,
                                    const size_t& height, 
                                    const e_side& side, 
                                    const size_t& pin_id,
                                    const bool& for_top_netlist) {
  if (true == for_top_netlist) {
    std::string port_name = std::string("grid_");
    port_name += std::to_string(coordinate.x());
    port_name += std::string("__");
    port_name += std::to_string(coordinate.y());
    port_name += std::string("__pin_");
    port_name += std::to_string(height);
    port_name += std::string("__");
    port_name += std::to_string(size_t(side));
    port_name += std::string("__");
    port_name += std::to_string(pin_id);
    port_name += std::string("_");
    return port_name;
  } 
  /* For non-top netlist */
  VTR_ASSERT( false == for_top_netlist );
  Side side_manager(side);
  std::string port_name = std::string(side_manager.to_string());
  port_name += std::string("_height_");
  port_name += std::to_string(height);
  port_name += std::string("__pin_");
  port_name += std::to_string(pin_id);
  port_name += std::string("_");
  return port_name;
}


/*********************************************************************
 * Generate the port name for a reserved sram port, i.e., BLB/WL port
 * When port_type is BLB, a string denoting to the reserved BLB port is generated
 * When port_type is WL, a string denoting to the reserved WL port is generated
 *
 * DO NOT put any SRAM organization check codes HERE!!!
 * Even though the reserved BLB/WL ports are used by RRAM-based FPGA only, 
 * try to keep this function does simple job. 
 * Check codes should be added outside, when print the ports to files!!!  
 *********************************************************************/
std::string generate_reserved_sram_port_name(const e_spice_model_port_type& port_type) {
  VTR_ASSERT( (port_type == SPICE_MODEL_PORT_BLB) || (port_type == SPICE_MODEL_PORT_WL) ); 

  if (SPICE_MODEL_PORT_BLB == port_type) {
    return std::string("reserved_blb");
  }
  return std::string("reserved_wl");
}

/*********************************************************************
 * Generate the port name for a sram port, used for formal verification
 * The port name is named after the cell name of SRAM in circuit library
 *********************************************************************/
std::string generate_formal_verification_sram_port_name(const CircuitLibrary& circuit_lib,
                                                        const CircuitModelId& sram_model) {
  std::string port_name = circuit_lib.model_name(sram_model) + std::string("_out_fm");

  return port_name;
}

/*********************************************************************
 * Generate the head port name of a configuration chain
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_configuration_chain_head_name() {
  return std::string("ccff_head");
}

/*********************************************************************
 * Generate the tail port name of a configuration chain
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_configuration_chain_tail_name() {
  return std::string("ccff_tail");
}

/*********************************************************************
 * Generate the memory output port name of a configuration chain
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_configuration_chain_data_out_name() {
  return std::string("mem_out");
}

/*********************************************************************
 * Generate the inverted memory output port name of a configuration chain
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_configuration_chain_inverted_data_out_name() {
  return std::string("mem_outb");
}

/*********************************************************************
 * Generate the addr port (input) for a local decoder of a multiplexer
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_mux_local_decoder_addr_port_name() {
  return std::string("addr");
}

/*********************************************************************
 * Generate the data port (output) for a local decoder of a multiplexer
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_mux_local_decoder_data_port_name() {
  return std::string("data");
}

/*********************************************************************
 * Generate the inverted data port (output) for a local decoder of a multiplexer
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_mux_local_decoder_data_inv_port_name() {
  return std::string("data_inv");
}

/*********************************************************************
 * Generate the port name of a local configuration bus
 * TODO: This could be replaced as a constexpr string
 *********************************************************************/
std::string generate_local_config_bus_port_name() {
  return std::string("config_bus");
}

/*********************************************************************
 * Generate the port name for a regular sram port which appears in the
 * port list of a module
 * The port name is named after the cell name of SRAM in circuit library
 *********************************************************************/
std::string generate_sram_port_name(const CircuitLibrary& circuit_lib,
                                    const CircuitModelId& sram_model,
                                    const e_sram_orgz& sram_orgz_type,
                                    const e_spice_model_port_type& port_type) {
  std::string port_name = circuit_lib.model_name(sram_model) + std::string("_");

  switch (sram_orgz_type) {
  case SPICE_SRAM_STANDALONE: {
    /* Two types of ports are available:  
     * (1) Regular output of a SRAM, enabled by port type of INPUT
     * (2) Inverted output of a SRAM, enabled by port type of OUTPUT
     */
    if (SPICE_MODEL_PORT_INPUT == port_type) {
      port_name += std::string("out"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_OUTPUT == port_type );
      port_name += std::string("outb"); 
    }
    break;
  }
  case SPICE_SRAM_SCAN_CHAIN:
    /* Two types of ports are available:  
     * (1) Head of a chain of Configuration-chain Flip-Flops (CCFFs), enabled by port type of INPUT
     * (2) Tail of a chian of Configuration-chain Flip-flops (CCFFs), enabled by port type of OUTPUT
     *           +------+    +------+    +------+
     *  Head --->| CCFF |--->| CCFF |--->| CCFF |---> Tail
     *           +------+    +------+    +------+
     */
    if (SPICE_MODEL_PORT_INPUT == port_type) {
      port_name += std::string("ccff_head"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_OUTPUT == port_type );
      port_name += std::string("ccff_tail"); 
    }
    break;
  case SPICE_SRAM_MEMORY_BANK:
    /* Four types of ports are available:  
     * (1) Bit Lines (BLs) of a SRAM cell, enabled by port type of BL
     * (2) Word Lines (WLs) of a SRAM cell, enabled by port type of WL
     * (3) Inverted Bit Lines (BLBs) of a SRAM cell, enabled by port type of BLB
     * (4) Inverted Word Lines (WLBs) of a SRAM cell, enabled by port type of WLB
     *
     *           BL BLB WL WLB    BL BLB WL WLB    BL BLB WL WLB
     *          [0] [0] [0] [0]  [1] [1] [1] [1]  [i] [i] [i] [i]
     *            ^  ^  ^  ^       ^  ^  ^  ^       ^  ^  ^  ^
     *            |  |  |  |       |  |  |  |       |  |  |  |
     *           +----------+     +----------+     +----------+
     *           |   SRAM   |     |   SRAM   | ... |   SRAM   |         
     *           +----------+     +----------+     +----------+
     */
    if (SPICE_MODEL_PORT_BL == port_type) {
      port_name += std::string("bl"); 
    } else if (SPICE_MODEL_PORT_WL == port_type) {
      port_name += std::string("wl"); 
    } else if (SPICE_MODEL_PORT_BLB == port_type) {
      port_name += std::string("blb"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_WLB == port_type );
      port_name += std::string("wlb"); 
    }
    break;
  default:
    vpr_printf(TIO_MESSAGE_ERROR,
               "(File:%s,[LINE%d])Invalid type of SRAM organization !\n",
               __FILE__, __LINE__);
    exit(1);
  }

  return port_name;
}

/*********************************************************************
 * Generate the port name for a regular sram port which is an internal
 * wire of a module
 * The port name is named after the cell name of SRAM in circuit library
 *********************************************************************/
std::string generate_sram_local_port_name(const CircuitLibrary& circuit_lib,
                                          const CircuitModelId& sram_model,
                                          const e_sram_orgz& sram_orgz_type,
                                          const e_spice_model_port_type& port_type) {
  std::string port_name = circuit_lib.model_name(sram_model) + std::string("_");

  switch (sram_orgz_type) {
  case SPICE_SRAM_STANDALONE: {
    /* Two types of ports are available:  
     * (1) Regular output of a SRAM, enabled by port type of INPUT
     * (2) Inverted output of a SRAM, enabled by port type of OUTPUT
     */
    if (SPICE_MODEL_PORT_INPUT == port_type) {
      port_name += std::string("out_local_bus"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_OUTPUT == port_type );
      port_name += std::string("outb_local_bus"); 
    }
    break;
  }
  case SPICE_SRAM_SCAN_CHAIN:
    /* Three types of ports are available:  
     * (1) Input of Configuration-chain Flip-Flops (CCFFs), enabled by port type of INPUT
     * (2) Output of a chian of Configuration-chain Flip-flops (CCFFs), enabled by port type of OUTPUT
     * (2) Inverted output of a chian of Configuration-chain Flip-flops (CCFFs), enabled by port type of INOUT
     *           +------+    +------+    +------+
     *  Head --->| CCFF |--->| CCFF |--->| CCFF |---> Tail
     *           +------+    +------+    +------+
     */
    if (SPICE_MODEL_PORT_INPUT == port_type) {
      port_name += std::string("ccff_in_local_bus"); 
    } else if ( SPICE_MODEL_PORT_OUTPUT == port_type ) {
      port_name += std::string("ccff_out_local_bus"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_INOUT == port_type );
      port_name += std::string("ccff_outb_local_bus"); 
    }
    break;
  case SPICE_SRAM_MEMORY_BANK: {
    /* Two types of ports are available:  
     * (1) Regular output of a SRAM, enabled by port type of INPUT
     * (2) Inverted output of a SRAM, enabled by port type of OUTPUT
     */
    if (SPICE_MODEL_PORT_INPUT == port_type) {
      port_name += std::string("out_local_bus"); 
    } else {
      VTR_ASSERT( SPICE_MODEL_PORT_OUTPUT == port_type );
      port_name += std::string("outb_local_bus"); 
    }
    break;
  }
  default:
    vpr_printf(TIO_MESSAGE_ERROR,
               "(File:%s,[LINE%d])Invalid type of SRAM organization !\n",
               __FILE__, __LINE__);
    exit(1);
  }

  return port_name;
}

/*********************************************************************
 * Generate the port name for the input bus of a routing multiplexer
 * This is very useful in Verilog code generation where the inputs of 
 * a routing multiplexer may come from different ports. 
 * On the other side, the datapath input of a routing multiplexer
 * is defined as a bus port. 
 * Therefore, to interface, a bus port is required, and this function
 * give a name to the bus port
 * To keep the bus port name unique to each multiplexer we will instance,
 * a mux_instance_id should be provided by user
 *********************************************************************/
std::string generate_mux_input_bus_port_name(const CircuitLibrary& circuit_lib,
                                             const CircuitModelId& mux_model,
                                             const size_t& mux_size, 
                                             const size_t& mux_instance_id) {
  std::string postfix = std::string("_") + std::to_string(mux_instance_id) + std::string("_inbus");
  return generate_mux_subckt_name(circuit_lib, mux_model, mux_size, postfix);
}

/*********************************************************************
 * Generate the name of a bus port which is wired to the configuration
 * ports of a routing multiplexer
 * This port is supposed to be used locally inside a Verilog/SPICE module 
 *********************************************************************/
std::string generate_mux_config_bus_port_name(const CircuitLibrary& circuit_lib,
                                              const CircuitModelId& mux_model,
                                              const size_t& mux_size, 
                                              const size_t& bus_id,
                                              const bool& inverted) {
  std::string postfix = std::string("_configbus") + std::to_string(bus_id);
  /* Add a bar to the end of the name for inverted bus ports */
  if (true == inverted) {
     postfix += std::string("_b");  
  }

  return generate_mux_subckt_name(circuit_lib, mux_model, mux_size, postfix);
}

/*********************************************************************
 * Generate the port name for a SRAM port of a circuit
 * This name is used for local wires that connecting SRAM ports
 * of a circuit model inside a Verilog/SPICE module
 * Note that the SRAM ports share the same naming
 * convention regardless of their configuration style
 *********************************************************************/
std::string generate_local_sram_port_name(const std::string& port_prefix, 
                                          const size_t& instance_id,
                                          const e_spice_model_port_type& port_type) {
  std::string port_name = port_prefix + std::string("_") + std::to_string(instance_id) + std::string("_");

  if (SPICE_MODEL_PORT_INPUT == port_type) {
    port_name += std::string("out"); 
  } else {
    VTR_ASSERT( SPICE_MODEL_PORT_OUTPUT == port_type );
    port_name += std::string("outb"); 
  }

  return port_name;
}

/*********************************************************************
 * Generate the port name for a SRAM port of a routing multiplexer
 * This name is used for local wires that connecting SRAM ports
 * of routing multiplexers inside a Verilog/SPICE module
 * Note that the SRAM ports of routing multiplexers share the same naming
 * convention regardless of their configuration style
 **********************************************************************/
std::string generate_mux_sram_port_name(const CircuitLibrary& circuit_lib,
                                        const CircuitModelId& mux_model,
                                        const size_t& mux_size, 
                                        const size_t& mux_instance_id,
                                        const e_spice_model_port_type& port_type) {
  std::string prefix = generate_mux_subckt_name(circuit_lib, mux_model, mux_size, std::string());
  return generate_local_sram_port_name(prefix, mux_instance_id, port_type);
}

/*********************************************************************
 * Generate the netlist name of a grid block
 **********************************************************************/
std::string generate_grid_block_netlist_name(const std::string& block_name,
                                             const bool& is_block_io,
                                             const e_side& io_side,
                                             const std::string& postfix) {
  /* Add the name of physical block */
  std::string module_name(block_name);

  if (true == is_block_io) {
    Side side_manager(io_side);
    module_name += std::string("_");
    module_name += std::string(side_manager.to_string());
  }

  module_name += postfix;

  return module_name;
}

/*********************************************************************
 * Generate the module name of a grid block
 **********************************************************************/
std::string generate_grid_block_module_name(const std::string& prefix,
                                            const std::string& block_name,
                                            const bool& is_block_io,
                                            const e_side& io_side) {
  std::string module_name(prefix);

  module_name += generate_grid_block_netlist_name(block_name, is_block_io, io_side, std::string());

  return module_name;
}

/*********************************************************************
 * Generate the module name of a physical block
 * To ensure a unique name for each physical block inside the graph of complex blocks
 * (pb_graph_nodes), this function trace backward to the top-level node
 * in the graph and add the name of these parents 
 * The final name will be in the following format:
 * <top_physical_block_name>_<mode_name>_<parent_physical_block_name> ... <current_physical_block_name>
 *
 * TODO: to make sure the length of this name does not exceed the size of
 * chars in a line of a file!!! 
 **********************************************************************/
std::string generate_physical_block_module_name(const std::string& prefix,
                                                t_pb_type* physical_pb_type) {
  std::string module_name(physical_pb_type->name);

  t_pb_type* parent_pb_type = physical_pb_type;

  /* Backward trace until we meet the top-level pb_type */
  while (1) {
    /* If there is no parent mode, this is a top-level pb_type, quit the loop here */
    t_mode* parent_mode = parent_pb_type->parent_mode;
    if (NULL == parent_mode) {
      break;
    }
    
    /* Add the mode name to the module name */
    module_name = std::string("mode[") + std::string(parent_mode->name) + std::string("]_") + module_name;

    /* Backtrace to the upper level */
    parent_pb_type = parent_mode->parent_pb_type;

    /* If there is no parent pb_type, this is a top-level pb_type, quit the loop here */
    if (NULL == parent_pb_type) {
      break;
    }

    /* Add the current pb_type name to the module name */
    module_name = std::string(parent_pb_type->name) + std::string("_") + module_name;
  }

  /* Exception for top-level pb_type: add an virtual mode name (same name as the pb_type) 
   * This is to follow the naming convention as non top-level pb_types
   * In addition, the name can be really unique, being different than the grid blocks
   */
  if (NULL == physical_pb_type->parent_mode) {
    module_name += std::string("_mode[") + std::string(physical_pb_type->name) + std::string("]");
  }

  /* Add the prefix */
  module_name = prefix + module_name;

  return module_name;
}
