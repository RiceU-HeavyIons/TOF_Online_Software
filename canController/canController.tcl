#!/usr/bin/wish -f
###############################################################
# $Id: canController.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $
#
#GUI to configure and get the status of the TDCs via the CANbus
#
################################################################

set thisDir [file dirname [info script]]
auto_mkindex $thisDir *.tcl
lappend auto_path $thisDir

wm title . "Can Controller"

# Set to 1 for debugging
# Not completely implemented
set debug 0

#**********************************
#ENABLE BOARD CHOICE
#**********************************

frame .bc -borderwidth 3 -relief groove
pack .bc -side top -fill x -pady 2


# Make label for the radio buttons
label .bc.boardLabel -text "Choose a board: " -font {arial 10 bold}
pack .bc.boardLabel -side left
set boardChoice 1
radiobutton .bc.1 -variable boardChoice -text "TDIG" \
	-value 1
radiobutton .bc.0 -variable boardChoice -text "Jalapeno" \
	-value 0
pack .bc.1 .bc.0 -side left

# Make actual use the default
.bc.1 select
#**********************************


#******************
#DISPLAY BOX SET-UP
#******************

# Set up status print out box
frame .textFrame
set displayBox [text .textFrame.displayBox -width 65 -height 25 \
	-relief sunken -setgrid true \
	 -yscrollcommand {.textFrame.scroll set}]
scrollbar .textFrame.scroll -command {$displayBox yview}
pack .textFrame.scroll -side right -fill y
pack $displayBox -side bottom -expand true -fill both

# Create Clear button
button .clear -text Clear -command {$displayBox delete 1.0 end}



# Packing of display box and clear button occurs at the 
# 	end to preserve packing order

#bind $displayBox <<Select>> turnDisabledRed

#******************

# fix this later!

# Create button to turn DISABLED red
#button .turnDisabledRed -text "turn red!" -command turnDisabledRed

#proc turnDisabledRed {} {
#	global displayBox

#	set cnt 0
#	set start [$displayBox search -count cnt DISABLED end]
#	$displayBox insert end "Count = $cnt\n"
#	if {$cnt} {
#		$displayBox tag add turnRed $start "$start + $cnt chars"
#		$displayBox tag configure turnRed -foreground red
#	}
#}

# Call status-fetching process
getStatusFrame .

# Call configuration process
configTDCFrame .

# Call set_control process
#setCtrlFrame .
enableChannelFrame .

# Add set threshold frame
setThresholdFrame .

# Add data collection frame
getDataFrame . 


# Pack display box and clear button at the bottom of GUI
pack .textFrame -side top -fill both -expand true -pady {5 0}
pack .clear -pady 5 -side bottom
#pack .turnDisabledRed -pady 5 -side bottom