# $Id: enableChannelFrame.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $
################################################################
#
#Adds set_control components to canController GUI
#
################################################################

#package require Iwidgets 4.0

# Parameters:
# canControllerFrame - the toplevel in canController.tcl

proc enableChannelFrame {canControllerFrame} {

	#**********************************
	# DISPLAY SET-UP
	#**********************************
	
	# define global variables for use in subfunctions
	global chanMenu channelRes debug ctrlFrame ctrlEdgeType ctrlNodeID \
		boardChoice displayBox chanSelFrame resolution
		
	# Globalize channel variables and initialize enabled	
	foreach node {1} {
		set varArrayName ""
		append varArrayName node $node LeadingEdgeChan
		global $varArrayName
		for {set i 0} {$i < 24} {incr i} {
			set varName ""
			append varName $varArrayName ($i)
			set $varName 1
		}
				
		# for TDC 4 (trailing edge channels
		set varArrayName ""
		append varArrayName node $node TrailingEdgeChan
		global $varArrayName
		for {set i 0} {$i < 24} {incr i} {
			set varName ""
			append varName $varArrayName ($i)
			set $varName 1
		}
	}
		
	set ctrlFrame [labelframe $canControllerFrame.ctrl -borderwidth 3 \
		-relief ridge -text "Channel Enable"]
	pack $ctrlFrame -side top -fill x
	
	# Set debug to 0 if you don't want debug statements
	set debug 0

	# Make labeled menu for node ID
	label $ctrlFrame.nodeLabel -text "node ID:" 
	grid $ctrlFrame.nodeLabel -column 0 -row 0 -sticky w  -padx {10 0}
	tk_optionMenu $ctrlFrame.nodeID ctrlNodeID 1 2 3 4 5 6 7 8
	grid $ctrlFrame.nodeID -column 1 -row 0 -sticky w -padx {0 5}
	
	# Make labeled menu for edge type
	label $ctrlFrame.edgeTypeLabel -text "edge type:" 
	grid $ctrlFrame.edgeTypeLabel -column 0 -row 1 -sticky w  -padx {10 0}
	set edgeTypeMenu [tk_optionMenu $ctrlFrame.edgeType ctrlEdgeType leading trailing both]
	grid $ctrlFrame.edgeType -column 1 -row 1 -sticky w -padx {0 5}
	
	set ctrlEdgeType leading
	
	# Make frame for channel selection 
	set chanSelFrame [labelframe $ctrlFrame.chanSelFrame -text "Channel Select"]
	grid $chanSelFrame -column 2 -row 0 -rowspan 2 -pady {3 5} -padx 5
	
	# Add channel checkboxes
	for {set i 0} {$i < 3} {incr i} {
		for {set j 0} {$j < 8} {incr j} {
			set k [expr [expr $i * 8] + $j]
			checkbutton $chanSelFrame.$k -text $k -variable channel($k) 
			grid $chanSelFrame.$k -row $i -column $j -sticky w
		}
	}  

	# Create Channel Enable button
	button $ctrlFrame.chan_enableBut -text "Enable\nSelected\nChannels" \
		-command {
			if {$ctrlEdgeType == "leading"} {
				set tdcList {1 2 3}
			} elseif {$ctrlEdgeType == "trailing"} {
				set tdcList 4
			} else {
				set tdcList {1 2 3 4}
			}
			foreach tdc $tdcList {
				enableChan channel $tdc
			}
		}
	grid $ctrlFrame.chan_enableBut -column 3 -row 0 -rowspan 2 -padx 5
	bind $ctrlFrame.chan_enableBut <Return> {
			if {$ctrlEdgeType == "leading"} {
				set tdcList {1 2 3}
			} elseif {$ctrlEdgeType == "trailing"} {
				set tdcList 4
			} else {
				set tdcList {1 2 3 4}
			}
			foreach tdc $tdcList { 
				enableChan channel $tdc
			}
		}
	
	# Make frame for enable/disable all buttons
	labelframe $ctrlFrame.doAll -text "Affect all channels of\nselected edge type(s) "
	grid $ctrlFrame.doAll -column 4 -row 0 -rowspan 2 -padx 5 -pady {0 5}
	
	# Make enable all button
	button $ctrlFrame.doAll.enableAllBut -text "Enable All" -width 10\
		-command enableAllButProc					
	grid $ctrlFrame.doAll.enableAllBut -column 0 -row 0 -pady {3 0}
	bind $ctrlFrame.doAll.enableAllBut <Return> enableAllButProc
															
	# Make disable all button
	button $ctrlFrame.doAll.disableAllBut -text "Disable All" -width 10\
		-command disableAllButProc
					
	grid $ctrlFrame.doAll.disableAllBut -column 0 -row 1 -pady 5
	bind $ctrlFrame.doAll.disableAllBut <Return> disableAllButProc

	# Create Reset button
	button $ctrlFrame.resetBut -text "Reset TDC(s)" \
		-command reset -width 14
	grid $ctrlFrame.resetBut -column 5 -row 0 -padx 5
	bind $ctrlFrame.resetBut <Return> reset
	
	# Create show channel status button
	button $ctrlFrame.chanStatBut -text "See status of channels\nat selected node"\
		-command {chanStatFrame $ctrlNodeID}
	grid $ctrlFrame.chanStatBut -column 5 -row 1 -padx 5
	
	#**********************************
	
	#**********************************
	# Process User Input
	#**********************************
	
	# Make pop-up for displaying channel status of all TDCs at selected node
	proc chanStatFrame {nodeID} {
		
		set chanStatFrame [toplevel .fNode$nodeID]
		wm title $chanStatFrame "Node $nodeID Channel Status"
		# for leading edge
		set varArrayName ""
		append varArrayName node$nodeID LeadingEdgeChan
		global $varArrayName
		set f [labelframe $chanStatFrame.leadingFrame -text "Leading edge channels"]
		grid $f -column 0 -row 0
		for {set i 0} {$i < 3} {incr i} {
				for {set j 0} {$j < 8} {incr j} {
					set k [expr [expr $i * 8] + $j]
					set varName ""
					append varName $varArrayName ($k)
					checkbutton $f.$k -text $k -variable $varName -state disabled -disabledforeground black
					grid $f.$k -row $i -column $j -sticky w
				}
		} 
		
		# for trailing edge
		set varArrayName ""
		append varArrayName node$nodeID TrailingEdgeChan
		global $varArrayName
		set f [labelframe $chanStatFrame.trailingFrame -text "Trailing edge channels"]
		grid $f -column 0 -row 4
		for {set i 0} {$i < 3} {incr i} {
				for {set j 0} {$j < 8} {incr j} {
					set k [expr [expr $i * 8] + $j]
					set varName ""
					append varName $varArrayName ($k)
					checkbutton $f.$k -text $k -variable $varName -state disabled -disabledforeground black
					grid $f.$k -row $i -column $j -sticky w
				}
		} 
	}

	# Enable All button proc
	proc enableAllButProc {} {
		global ctrlEdgeType ctrlNodeID channel 
		
		if {($ctrlEdgeType == "leading") || ($ctrlEdgeType == "both")} {
			foreach tdc {1 2 3} {
				enableAllChan $tdc $ctrlNodeID 1
			}
		} 
		if {($ctrlEdgeType == "trailing") || ($ctrlEdgeType == "both")} {
			enableAllChan 4 $ctrlNodeID 0
		}
	}
	
	# Disable All button proc
	proc disableAllButProc {} {
		global ctrlEdgeType ctrlNodeID channel 
		
		if {($ctrlEdgeType == "leading") || ($ctrlEdgeType == "both")} {
			foreach tdc {1 2 3} {
				disableAllChan $tdc $ctrlNodeID 1
			}
		} 
		if {($ctrlEdgeType == "trailing") || ($ctrlEdgeType == "both")} {
			disableAllChan 4 $ctrlNodeID 0
		}
	}

	# Reset button process that resets the inputted TDC at the inputted node by
	# bringing the global reset bit in the control word low, high, and 
	# then low again.  In the end, all channels are enabled, much like an MCU reset.
	# See section 11.4 and 17.6 in HPTDC handbook, 
	# Version 2.2 for reference.
	proc reset {} {
		global ctrlEdgeType ctrlNodeID displayBox
		
		$displayBox insert end "******Reset Procedure******\n"
		# Call enableChan
		if {($ctrlEdgeType == "leading") || ($ctrlEdgeType == "both")} {
			foreach tdc {1 2 3} {
				disableAllChan $tdc $ctrlNodeID 1
				globalReset $tdc $ctrlNodeID 1
				enableAllChan $tdc $ctrlNodeID 1
			}
		} 
		if {($ctrlEdgeType == "trailing") || ($ctrlEdgeType == "both")} {
				disableAllChan 4 $ctrlNodeID 1
				globalReset 4 $ctrlNodeID 1
				enableAllChan 4 $ctrlNodeID 1
		}
		$displayBox insert end "******End Reset Procedure******\n"
		$displayBox see end
	}
	
	# Enables all channels
	proc enableAllChan {TDCID nodeID resolution} {
		global displayBox ctrlNodeID 
		
     	set dirName [file join [info script] set_controlTDIG/Release] 
		$displayBox insert end "*********Enable All Channels Procedure**********\n"
		$displayBox insert end "\nEnabling all channels in TDC $TDCID at node $nodeID...\n"

		set error [catch {open "| $dirName/set_control $TDCID $nodeID enableAll.txt $resolution"} cmdPipeLineChan]
		$displayBox see end
		set linecnt 0
		while {! [eof $cmdPipeLineChan]} {
		    gets $cmdPipeLineChan output
		    incr linecnt
		    #$displayBox insert end "LINE $linecnt : $output\n"
		    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $output]
			update idletasks
			$displayBox see end
		}	
		$displayBox insert end "Error = $error\n" 
		# If the process went successfully, update the channel status frame
		if {!$error} {
			set varArrayName ""
			if {$TDCID == 4} {
				append varArrayName node $ctrlNodeID TrailingEdgeChan
			} else {
				append varArrayName node $ctrlNodeID LeadingEdgeChan
			}
			global $varArrayName
			for {set i 0} {$i < 24} {incr i} {
				set varName ""
				append varName $varArrayName ($i)
				set $varName 1
			}
		}
		$displayBox insert end "*********End Enable All Channels Procedure**********\n"
		$displayBox see end
	}	
	
	# Disables all channels
	proc disableAllChan {TDCID nodeID resolution} {
		global displayBox ctrlNodeID 
				
		set dirName [file join [info script] set_controlTDIG/Release] 
		$displayBox insert end "*********Disable All Channels Procedure**********\n"
		$displayBox insert end "\nDisabling all channels in TDC $TDCID at node $nodeID...\n"
		
		set error [catch {open "| $dirName/set_control $TDCID $nodeID disableAll.txt $resolution"} cmdPipeLineChan]
		$displayBox see end
		set linecnt 0
		while {! [eof $cmdPipeLineChan]} {
		    gets $cmdPipeLineChan output
		    incr linecnt
		    #$displayBox insert end "LINE $linecnt : $output\n"
		    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $output]
		    update idletasks
		    $displayBox see end
		}	
		$displayBox insert end "Error = $error\n" 
		# If the process went successfully, update the channel status frame
		if {!$error} {
			set varArrayName ""
			if {$TDCID == 4} {
				append varArrayName node $ctrlNodeID TrailingEdgeChan
			} else {
				append varArrayName node $ctrlNodeID LeadingEdgeChan
			}
			global $varArrayName
			for {set i 0} {$i < 24} {incr i} {
				set varName ""
				append varName $varArrayName ($i)
				set $varName 0
			}
		}
		$displayBox insert end "*********End Enable All Channels Procedure**********\n"
		$displayBox see end
	}	
	
	# Sets the global reset bit high in the control word
	# NOTE: this process does not update the channel status frame, 
	#	because it is never called without enable all following
	proc globalReset {TDCID nodeID resolution} {
		global displayBox

		set dirName [file join [info script] set_controlTDIG/Release] 
		$displayBox insert end "\nResetting TDC $TDCID at node $nodeID...\n"
		
		set error [catch {open "| $dirName/set_control $TDCID $nodeID globalReset.txt $resolution"} cmdPipeLineChan]
		$displayBox see end
		set linecnt 0
		while {! [eof $cmdPipeLineChan]} {
		    gets $cmdPipeLineChan output
		    incr linecnt
		    #$displayBox insert end "LINE $linecnt : $output\n"
		    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $output]
		    update idletasks
		    $displayBox see end
		}	
		$displayBox insert end "Error = $error\n" 
	}
	
	# Enable Selected Channels button process
	proc enableChan {channelArrayName TDC} {
		global ctrlFrame ctrlNodeID displayBox debug $channelArrayName 
		
		if {$TDC == 4} {set resolution 0} else {set resolution 1}
		
		# Put control word bits into an array
		# Initialize all bits to 0
		for {set i 0} {$i <= 39} {incr i} {
			set ctrlWord($i) 0
		}
		
		# Globalize channel status variables
		set varArrayName ""
		if {$TDC == 4} {
			append varArrayName node $ctrlNodeID TrailingEdgeChan
		} else {
			append varArrayName node $ctrlNodeID LeadingEdgeChan
		}
		global $varArrayName
		for {set i 0} {$i < 24} {incr i} {
			set varName ""
			append varName $varArrayName ($i)
		}

		# ctrlWord[3:0] = enable_pattern[3:0] = 0100
		set ctrlWord(2) 1

		# ctrlWord[4] = 0

		# ctrlWord[36:5] = enable_channel[31:0] 
		# check user input to see which channels to enable

		if {$resolution} {
			# Calculate channel offset (TDC 1->chanOffset=0, TDC 2->8, TDC 3->16)
			set chanOffset [expr [expr $TDC - 1] * 8]
			for {set i 0} {$i<8} {incr i} {
				set j [expr $i + $chanOffset]
				set varArrayName ""
				append varArrayName node $ctrlNodeID LeadingEdgeChan
				set varName ""
				if {[set [set channelArrayName]($j)]} {
					# change enable_channel index to ctrlWord index
					# and set all four channel bits high
					set ctrlIndexLow [expr [expr $i * 4] + 5]
					set ctrlIndexHigh [expr $ctrlIndexLow + 4]
					for {set k $ctrlIndexLow} {$k < $ctrlIndexHigh} {incr k} {
						set ctrlWord($k) 1
					}
					# Set the corresponding channel status variable high
					append varName $varArrayName ($j)
					$displayBox insert end "Setting channel stat variable $varName high!\n"
					$displayBox see end
					set $varName 1
				} else {
					# If channel isn't enabled, set the corresponding channel status variable low
					append varName $varArrayName ($j)
					set $varName 0
				}
			}
		} else { # if TDC 4
			for {set i 0} {$i<24} {incr i} {
				set varArrayName ""
				append varArrayName node $ctrlNodeID TrailingEdgeChan
				set varName ""
				if {[set [set channelArrayName]($i)]} {
					# change enable_channel index to ctrlWord index
					set j [expr $i + 5]
					set ctrlWord($j) 1
					
					# Set the corresponding channel status variable high
					append varName $varArrayName ($i)
					set $varName 1
				} else {
					# If channel isn't enabled, set the corresponding channel status variable low
					append varName $varArrayName ($i)
					set $varName 0
				}
				
			}
		}

		# Calculate parity bit and put in ctrlWord[39]
		set parity 0
		for {set i 0} {$i <= 39} {incr i} { 
			if {$ctrlWord($i)} {
				incr parity
			}
		}
		set ctrlWord(39) [expr $parity % 2]

		
		if {$debug} {		
			for {set i 0} {$i <= 39} {incr i} { 
				$displayBox insert end "$i $ctrlWord($i)\n"
				$displayBox see end
			}
		}

		# Control word is calculated, now print to a file
		# If file exists, it gets truncated.  If it doesn't exist, it gets created.
		set fileName ctrlWord.txt
		set fileID [open $fileName w]

		set j 0
		for {set i 39} {$i >= 0} {incr i -1} {
			# Check for end of byte, if not, no newline
			if {[expr [expr $j + 1] % 8]} {
				puts -nonewline $fileID $ctrlWord($i)
			  #if so, put a newline
			} else {
				puts $fileID $ctrlWord($i)
			}
			incr j
		}
		close $fileID
		

		$displayBox insert end \
				"*********Enable Channels Procedure********\n"
				
     	set dirName [file join [info script] set_controlTDIG/Release] 
		set error [catch {open "| $dirName/set_control $TDC $ctrlNodeID $fileName $resolution"} cmdPipeLineChan]
		$displayBox see end
		set linecnt 0
		while {! [eof $cmdPipeLineChan]} {
		    gets $cmdPipeLineChan output
		    incr linecnt
		    #$displayBox insert end "LINE $linecnt : $output\n"
		    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $output]
		    update idletasks
		    $displayBox see end
		}	
		$displayBox insert end "Error = $error\n" 
					
		$displayBox insert end \
				"***********End Channel Enable**********\n\n"
		$displayBox see 1.0
		$displayBox see end 
	}
	
	#**********************************
}
