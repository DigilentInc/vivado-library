package require xilinx::board 1.0
namespace import ::xilinx::board::*

proc get_pmod_vlnv {} {
	return "digilentinc.com:interface:pmod_rtl:1.0"
}
# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST PROJECT_PARAM.ARCHITECTURE PROJECT_PARAM.BOARD} {
  set c_family ${PROJECT_PARAM.ARCHITECTURE}
  set board ${PROJECT_PARAM.BOARD}
  set Component_Name [ ipgui::add_param  $IPINST  -parent  $IPINST  -name Component_Name ]
  add_board_tab $IPINST
  set C_S_AXI_ACLK_FREQ_HZ_d [ipgui::add_param $IPINST -widget textEdit -name C_S_AXI_ACLK_FREQ_HZ_d]
	set C_S_AXI_ACLK_FREQ_HZ [ipgui::add_param $IPINST -name C_S_AXI_ACLK_FREQ_HZ]
	set_property visible false $C_S_AXI_ACLK_FREQ_HZ
	set_property display_name "AXI CLK Frequency" $C_S_AXI_ACLK_FREQ_HZ_d
}

proc update_PARAM_VALUE.PMOD {IPINST PARAM_VALUE.PMOD PROJECT_PARAM.BOARD} {
	set param_range [get_board_interface_param_range $IPINST -name "PMOD"]
	set_property range $param_range ${PARAM_VALUE.PMOD}
}

proc validate_PARAM_VALUE.PMOD { PARAM_VALUE.PMOD PARAM_VALUE.USE_BOARD_FLOW IPINST PROJECT_PARAM.BOARD} {
	set pmod_vlnv [get_pmod_vlnv]
	set intf [ get_property value ${PARAM_VALUE.PMOD} ]
	set board ${PROJECT_PARAM.BOARD}
	return true
}


proc updateOf_C_S_AXI_ACLK_FREQ_HZ {IPINST} {
	# Procedure called when C_S_AXI_ACLK_FREQ_HZ is updated
        set value_clk_Mhz [get_param_value C_S_AXI_ACLK_FREQ_HZ_d ]
        set val_clk_hz [expr {int($value_clk_Mhz*1000000)} ];
        set_property value $val_clk_hz [ipgui::get_paramspec C_S_AXI_ACLK_FREQ_HZ -of $IPINST ]
	#return true
}

proc C_S_AXI_ACLK_FREQ_HZ_d_updated {IPINST} {
	# Procedure called when PARITY is updated
updateOf_C_S_AXI_ACLK_FREQ_HZ $IPINST
}

proc validate_C_S_AXI_ACLK_FREQ_HZ_d {IPINST} {
	# Procedure called to validate C_S_AXI_ACLK_FREQ_HZ
        set AXI_CLK [get_param_value C_S_AXI_ACLK_FREQ_HZ_d ]
        if {$AXI_CLK < 10 || $AXI_CLK > 300} {
          	set_property errmsg "AXI CLK Frequency should be in 10 to 300 MHz Range" [ipgui::get_paramspec C_S_AXI_ACLK_FREQ_HZ_d -of $IPINST ]
		return false
        } else {
	return true
        }
}

proc updateModel_C_S_AXI_ACLK_FREQ_HZ {IPINST} {
        set value_clk_freq [get_param_value C_S_AXI_ACLK_FREQ_HZ ]
        set_property modelparam_value $value_clk_freq [ipgui::get_modelparamspec -name C_S_AXI_ACLK_FREQ_HZ -of $IPINST ]
        return true
}
