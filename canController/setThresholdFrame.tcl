######################################################################
# $Id: setThresholdFrame.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $
#
#Adds discriminator threshold-settin' components to canController GUI
#
######################################################################

#package require Iwidgets 4.0

# Parameters:
# canControllerFrame - the toplevel in canController.tcl

proc setThresholdFrame {canControllerFrame} {
	
	# define global variables for use in subfunctions
	global debug threshFrame threshNodeID boardChoice\
		displayBox voltage

	set thisDir [file dirname [info script]]

	set threshFrame [labelframe $canControllerFrame.thresh -borderwidth 3 \
		 -text "Threshold Setting" -relief ridge]
	pack $threshFrame -side top -fill x
	
	# Set debug to 0 if you don't want debug statements
	set debug 0

	# Make labeled menu for node ID
	label $threshFrame.nodeIDLabel -text "node ID:" 
	pack $threshFrame.nodeIDLabel -side left 
	tk_optionMenu $threshFrame.nodeID threshNodeID 1 2 3 4
	pack $threshFrame.nodeID -side left 	
	
      # Make a labeled entry for threshold voltage
      label $threshFrame.voltageLabel -text "Enter threshold\nvoltage in mV:"
      pack $threshFrame.voltageLabel -side left -padx {10 0}
      set voltage 30
      entry $threshFrame.voltageEntry -width 20 -relief sunken \
      	  -textvariable voltage
      pack $threshFrame.voltageEntry -side left	

	# Make set threshold button 
	button $threshFrame.threshBut -text "Set Threshold" -command setThresh -width 14
	pack $threshFrame.threshBut -side right -padx {10 5} -pady 10
	bind $threshFrame.threshBut <Return> setThresh
	
	# Set threshold process--happens when threshBut gets pushed
	proc setThresh {} {
		global threshFrame threshNodeID displayBox debug boardChoice\
			voltage

		$displayBox insert end \
				"*********Set Threshold Procedure********\n"
		set thresh "[setThreshold $threshNodeID $voltage]"
		$displayBox insert end $thresh 
		$displayBox insert end \
				"\n******** End Set Threshold Procedure ********\n\n"
		$displayBox see 1.0
		$displayBox see end 
	}
}