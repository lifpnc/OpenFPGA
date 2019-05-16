#!/bin/bash
# Example of how to run vpr

# Set variables
# For FPGA-Verilog ONLY 
benchmark="test_modes"
verilog_output_dirname="${benchmark}_Verilog"
verilog_output_dirpath="$PWD"
modelsim_ini_file="/uusoc/facility/cad_tools/Mentor/modelsim10.7b/modeltech/modelsim.ini"
# VPR critical inputs
#set arch_xml_file=ARCH/k6_N10_MD_tsmc40nm_chain_TT.xml 
#set arch_xml_file=ARCH/k8_N10_SC_tsmc40nm_chain_TT_stratixIV_lookalike.xml
arch_xml_file="ARCH/.regression_k6_N10_sram_chain_HC.xml"
#set arch_xml_file=ARCH/ed_stdcell.xml
#set arch_xml_file=ARCH/k6_N10_sram_chain_FC_tsmc40.xml
#set arch_xml_file=ARCH/k6_N10_SC_tsmc40nm_chain_TT.xml 
#set arch_xml_file=ARCH/k6_N10_SC_tsmc40nm_chain_TT_yosys.xml 
#set arch_xml_file=ARCH/k6_N10_sram_chain_SC_gf130_2x2.xml
#set verilog_reference=${PWD}/Circuits/alu4_K6_N10_ace.v
#set blif_file=Circuits/shiftReg.blif 
#set act_file=Circuits/shiftReg.act 
blif_file="Circuits/$benchmark.blif" 
act_file="Circuits/$benchmark.act "
verilog_reference="${PWD}/Circuits/$benchmark.v"
#set blif_file=Circuits/frisc.blif 
#set act_file=Circuits/frisc.act 
#set blif_file=Circuits/elliptic.blif 
#set act_file=Circuits/elliptic.act 
vpr_route_chan_width="200"

# Step A: Make sure a clean start 
# Recompile if needed
#make clean
#make -j32
# Remove previous designs
rm -rf $verilog_output_dirpath/$verilog_output_dirname

# Run VPR  
#valgrind 
./vpr $arch_xml_file $blif_file --full_stats --nodisp --activity_file $act_file --fpga_verilog --fpga_verilog_dir $verilog_output_dirpath/$verilog_output_dirname --fpga_x2p_rename_illegal_port --fpga_bitstream_generator --fpga_verilog_print_top_testbench --fpga_verilog_print_input_blif_testbench --fpga_verilog_include_timing --fpga_verilog_include_signal_init --fpga_verilog_print_modelsim_autodeck $modelsim_ini_file --fpga_verilog_print_formal_verification_top_netlist --fpga_verilog_print_autocheck_top_testbench $verilog_reference --fpga_verilog_print_user_defined_template --route_chan_width $vpr_route_chan_width --fpga_verilog_include_icarus_simulator --fpga_verilog_print_sdc_pnr --fpga_verilog_print_report_timing_tcl #--fpga_verilog_print_sdc_analysis  



