# $Id: getDataFrame.tcl,v 1.2 2004-08-23 22:22:44 jschamba Exp $
################################################################
#
#Adds TDC data-fetchin' components to canController GUI
#
################################################################

#package require Iwidgets 4.0

# Parameters:
# canControllerFrame - the toplevel in canController.tcl
# connected - a boolean set by the user (1 if connected to 
# 	      the CANbus, 2 if in demo mode)

proc getDataFrame {canControllerFrame} {
	
	# define global variables for use in subfunctions
	global debug dataFrame dataNodeID dataTDCID dataFile boardChoice numDataWords\
		displayBox 

	set thisDir [file dirname [info script]]

	set dataFrame [labelframe $canControllerFrame.data -borderwidth 3 \
		 -text "Data Collection" -relief ridge]
	pack $dataFrame -side top -fill x
	
	# Set debug to 0 if you don't want debug statements
	set debug 0
	
	# Make check box for collecting whatever's coming
	global collectAllBool
	checkbutton $dataFrame.collectAll -text "Collect whatever's\ncoming" -command collectAll\
		-variable collectAllBool
	grid $dataFrame.collectAll -column 0 -row 0
	
	# Make -OR- label
	label $dataFrame.or -text "OR specify..." 
	grid $dataFrame.or -column 0 -row 1
	
	# ---------------
	# Data Type Frame
	# ---------------
	
	# Make frame for data type
	global dataTypeFrame
	set dataTypeFrame [labelframe $dataFrame.dataTypeFrame -text "Data type"]
    grid $dataTypeFrame -column 0 -row 2 -padx 5 -pady {0 5}
    
    # Add data type radio buttons
    set i 0
    global dataTypeChoice
    foreach dataType {"uncorrelated" "synchronous" "cable-delay"} {
		radiobutton $dataTypeFrame.$dataType -variable dataTypeChoice -text $dataType \
			-value $dataType 	
		grid $dataTypeFrame.$dataType -column 0 -row $i -sticky w
		incr i
	}
	
	# Make uncorrelated the default
	$dataTypeFrame.uncorrelated select
	
	$dataTypeFrame.synchronous configure -command {
		global dataChanSelFrame
		set cableLength2 0.0
		
		grid $cableDelayFrame -column 6 -row 0 -rowspan 3 -padx 5 -pady {0 5}
		for {set i 0} {$i < 24} {incr i} {
			grid remove $dataChanSelFrame.$i
		}
		grid remove $dataChanSelFrame.chanBLabel $dataChanSelFrame.chanBMenu
		
		#---
		grid $dataChanSelFrame.chanALabel -column 0 -row 6 -sticky e
		grid $dataChanSelFrame.chanAMenu -column 1 -row 6 -padx {0 5} -sticky w
	
		$dataChanSelFrame.chanALabel configure -text "Channel:" 
		
		$cableDelayFrame configure -text "Calculate cable delay"
		grid remove $cableDelayFrame.cable1Label $cableDelayFrame.cable1Entry\
			$cableDelayFrame.cable2Label $cableDelayFrame.cable2Entry
		grid $cableDelayFrame.synchCableLabel -column 0 -row 0 -sticky e
		grid $cableDelayFrame.synchCableEntry -column 1 -row 0 -sticky w -padx {0 10} -pady 5
		setDataFileList
	}
		
	$dataTypeFrame.cable-delay configure -command {
		global dataChanSelFrame
		grid $cableDelayFrame -column 6 -row 0 -rowspan 3 -padx 5 -pady {0 5}
		
		# Change cable delay frame text
		$cableDelayFrame configure -text "Calculate differential cable delay"
		
		# Remove the synch cable label that says 
		# "Enter length of cable\nconnected from TDIG board\nto interface board (m):"
		grid remove $cableDelayFrame.synchCableLabel $cableDelayFrame.synchCableEntry
		
		# Remove channel checkboxes
		for {set i 0} {$i < 24} {incr i} {
			grid remove $dataChanSelFrame.$i
		}
		
		# Add channel B stuff
		grid $dataChanSelFrame.chanBLabel -column 0 -row 7 -sticky e
		grid $dataChanSelFrame.chanBMenu -column 1 -row 7 -padx {0 5} -sticky w
		
		# Grid channel A and change label
		grid $dataChanSelFrame.chanALabel -column 0 -row 6 -sticky e
		grid $dataChanSelFrame.chanAMenu -column 1 -row 6 -padx {0 5} -sticky w
		$dataChanSelFrame.chanALabel configure -text "Channel A:"
		
		grid $cableDelayFrame.cable1Label -column 0 -row 0 -sticky e
		grid $cableDelayFrame.cable1Entry -column 1 -row 0 -sticky w -padx {0 10} -pady 5
    
    	grid $cableDelayFrame.cable2Label -column 0 -row 1 -sticky e
		grid $cableDelayFrame.cable2Entry -column 1 -row 1 -sticky w -padx {0 10} -pady 5
		setDataFileList
	}
	
	$dataTypeFrame.uncorrelated configure -command {
		global dataChanSelFrame
		grid remove $cableDelayFrame $dataChanSelFrame.chanALabel $dataChanSelFrame.chanAMenu \
			$dataChanSelFrame.chanBLabel $dataChanSelFrame.chanBMenu
		for {set i 0} {$i < 24} {incr i} {
			grid $dataChanSelFrame.$i
		}
		setDataFileList
	}
	
	# ---------------
	
	# Create labeled entry for board serial number
	label $dataFrame.boardSerialNumLabel -text "Enter board\nserial number:"
    grid $dataFrame.boardSerialNumLabel -column 2 -row 0 -padx {5 0}
    global boardSerialNum
    set boardSerialNum **changeme**
    entry $dataFrame.boardSerialNumEntry -width 12 -relief sunken \
		-textvariable boardSerialNum 
		#-foreground red 
    grid $dataFrame.boardSerialNumEntry -column 3 -row 0 -padx {0 5}
    #$dataFrame.boardSerialNumEntry configure -validate focusout -validatecommand {
	#		$dataFrame.boardSerialNumEntry configure -foreground black
	#		return 1
	#		}
	
	# Make labeled menu for node ID
	label $dataFrame.nodeIDLabel -text "node ID:" 
	grid $dataFrame.nodeIDLabel -column 2 -row 1 -sticky e
	set nodeIDMenu [tk_optionMenu $dataFrame.nodeID dataNodeID 1 2 3 4]
	grid $dataFrame.nodeID -column 3 -row 1 -padx {0 5} -sticky w
	
	# Configure the serial num entry to validate now, so that setDataFileList doesn't
	# get called when focus leaves the entry to make nodeIDLabel
	$dataFrame.boardSerialNumEntry configure -validate focusout -validatecommand setDataFileList

	
	# ---------------
	# Edge Type Frame
	# ---------------
	
	# Make frame for leading/trialing edge
	global edgeTypeFrame
	set edgeTypeFrame [labelframe $dataFrame.edgeTypeFrame -text "Pulse edge type"]
    grid $edgeTypeFrame -column 4 -row 0 -rowspan 2 -padx 5 -pady {0 5}
    
    # Add leading/trailing edge radio buttons
    set i 0
    global edgeTypeChoice
    foreach edgeType {"leading" "trailing"} {
		radiobutton $edgeTypeFrame.$edgeType -variable edgeTypeChoice -text $edgeType \
			-value $edgeType -command setDataFileList
		grid $edgeTypeFrame.$edgeType -column 0 -row $i -sticky w
		incr i
	}
	
	# Make leading edge the default
	$edgeTypeFrame.leading select
	# ---------------
	
	# ---------------
	# TAMP Frame
	# ---------------
	
	# Make frame for TAMP connection on/off
	global tampFrame
	set tampFrame [labelframe $dataFrame.tampFrame -text "got TAMP?"]
    grid $tampFrame -column 4 -row 2 -padx 5 -pady {0 5}
    
    # Add TAMP radio buttons
    global TAMP
	radiobutton $tampFrame.connected -variable TAMP -text connected \
		-value TAMP -command setDataFileList
	grid $tampFrame.connected -column 0 -row 0 -sticky w
	radiobutton $tampFrame.unconnected -variable TAMP -text "not connected" \
		-value notTAMP -command setDataFileList
	grid $tampFrame.unconnected -column 0 -row 1 -sticky w
	
	# Make connected the default
	$tampFrame.connected select
	# ---------------
	
	# ------------------
	# cable delay Frame
	# ------------------
	
	# Make frame for calculating cable delay
	# NOTE:  Only gets gridded when synch or cable-delay data type are selected
	global cableDelayFrame
	set cableDelayFrame [labelframe $dataFrame.cableDelayFrame -text "Calculate cable delay"]
    
    # Add cableDelay widgets
    global cableLength1 cableLength2 varDelay cableDelay
    
    set cableLength1 0.0
    set cableLength2 0.0
    set varDelay 0.0
    
    proc calculateCableDelay {} {
		global cableLength1 cableLength2 varDelay cableDelay 
		# cableDelay = (diff in cable lengths)*(7/10)*1/(c = 0.3 nm/s) + varDelay
		set cableDelay [expr abs([expr $cableLength1 - $cableLength2]) * .7 * [expr 1 / 0.3] + $varDelay]
		return 1
	}
	
    
    label $cableDelayFrame.cable1Label -text "Enter length of cable\nconnected to channel A (m):"
    grid $cableDelayFrame.cable1Label -column 0 -row 0 -sticky e
    entry $cableDelayFrame.cable1Entry -width 8 -textvariable cableLength1 -validate focusout -validatecommand calculateCableDelay
    grid $cableDelayFrame.cable1Entry -column 1 -row 0 -sticky w -padx {0 10} -pady 5
    bind $cableDelayFrame.cable1Entry <Return> calculateCableDelay
    
    # This is the labeled entry for synch data cable length--doesn't get gridded until synch data type is chosen
    # NOTE: the textvariable for the entry is the same as for the cable1Entry so that the calculateCableDelay proc still works!
    label $cableDelayFrame.synchCableLabel -text "Enter length of cable\nconnected from TDIG board\nto interface board (m):"
    entry $cableDelayFrame.synchCableEntry -width 8 -textvariable cableLength1 -validate focusout -validatecommand calculateCableDelay
    bind $cableDelayFrame.synchCableEntry <Return> calculateCableDelay
    
    
    label $cableDelayFrame.cable2Label -text "Enter length of cable\nconnected to channel B (m):"
    grid $cableDelayFrame.cable2Label -column 0 -row 1  -sticky e
    entry $cableDelayFrame.cable2Entry -width 8 -textvariable cableLength2 -validate focusout -validatecommand calculateCableDelay
    grid $cableDelayFrame.cable2Entry -column 1 -row 1 -sticky w -padx {0 10} -pady 5
     bind $cableDelayFrame.cable2Entry <Return> calculateCableDelay
    
    label $cableDelayFrame.varDelayLabel -text "Enter variable delay (ns):"
    grid $cableDelayFrame.varDelayLabel -column 0 -row 2 -sticky e
    entry $cableDelayFrame.varDelayEntry -width 8 -textvariable varDelay -validate focusout -validatecommand calculateCableDelay
    grid $cableDelayFrame.varDelayEntry -column 1 -row 2 -sticky w -padx {0 10} -pady 5
     bind $cableDelayFrame.varDelayEntry <Return> calculateCableDelay
   
    label $cableDelayFrame.calcDelayLabel -text "Calculated cable delay (ns):"
    grid $cableDelayFrame.calcDelayLabel -column 0 -row 3 -sticky e
    entry $cableDelayFrame.calcDelayEntry -width 8 -textvariable cableDelay -state readonly
		# -disabledforeground SystemButtonText 
    grid $cableDelayFrame.calcDelayEntry -column 1 -row 3 -sticky w -padx {0 10} -pady 5
    
    # Configure the entries to validate now, so that setDataFileList doesn't
	# get called when focus leaves the entry to make the next widget
	#$cableDelayFrame.cable1Entry configure -validate focusout -validatecommand setDataFileList
	#$cableDelayFrame.synchCableEntry configure -validate focusout -validatecommand setDataFileList
	#$cableDelayFrame.cable2Entry configure -validate focusout -validatecommand setDataFileList
	#$cableDelayFrame.varDelayEntry configure -validate focusout -validatecommand setDataFileList
	$cableDelayFrame.calcDelayEntry configure -validate key -validatecommand setDataFileList
	# ---------------
	
	
	# Make frame for channel selection 
	global dataChanSelFrame
	set dataChanSelFrame [labelframe $dataFrame.chanSelFrame -text "Channel Select"]
	grid $dataChanSelFrame -column 7 -row 0 -rowspan 3 -pady {3 5} -padx 5
	
	# Add channel checkboxes
	global dataChannel
	for {set i 0} {$i < 6} {incr i} {
		for {set j 0} {$j < 4} {incr j} {
			set k [expr [expr $i * 4] + $j]
			checkbutton $dataChanSelFrame.$k -text $k -variable dataChannel($k)\
				-command setDataFileList 
			grid $dataChanSelFrame.$k -row $i -column $j -sticky w
		}
	}  
	
	# Make labeled channel option menus for synch and cable-delay
	# Note: these don't get gridded unless either cable-delay or synch data is selected.
	global chanA
	label $dataChanSelFrame.chanALabel -text "Channel A:" 
	set chanAMenu [tk_optionMenu $dataChanSelFrame.chanAMenu chanA 0]
	$chanAMenu entryconfigure 0 -command setDataFileList
	for {set i 1} {$i < 24} {incr i} {
		$chanAMenu add radiobutton -label $i -variable chanA -command setDataFileList
	}
	
	# Make labeled channel option menus for synch and cable-delay
	global chanB
	label $dataChanSelFrame.chanBLabel -text "Channel B:" 
	set chanBMenu [tk_optionMenu $dataChanSelFrame.chanBMenu chanB 0]
	for {set i 1} {$i < 24} {incr i} {
		$chanBMenu add radiobutton -label $i -variable chanB -command setDataFileList
	}
	
    # Make a labeled entry for number of data words
    label $dataFrame.numWordsLabel -text "Enter # of Data\nWords to Collect:" 
    grid $dataFrame.numWordsLabel -column 8 -row 0 -padx {5 0}
    set numDataWords 4000
    entry $dataFrame.numWordsEntry -width 20 -relief sunken \
      	-textvariable numDataWords 
    grid $dataFrame.numWordsEntry -column 9 -row 0 -padx {0 5}

    # Make labeled entry for data file
    label $dataFrame.dataFile -text "Enter Filename \nfor Data Collection:" \
		-state disabled
    grid $dataFrame.dataFile -column 8 -row 1 -padx {5 0}
    #set dataFile [file join $thisDir dataTest.raw] 
    entry $dataFrame.fileEntry -width 20 -relief sunken \
    -textvariable dataFile -state disabled
    grid $dataFrame.fileEntry -column 9 -row 1 -padx {0 5}

    # Create browse button for finding an existing data file 
    button $dataFrame.browse -text "Browse" \
		-command dataBrowse -state disabled
    grid $dataFrame.browse -column 10 -row 1 -padx 5
    bind $dataFrame.browse <Return> dataBrowse
    
    # ------------------
	# file name Frame
	# ------------------
    
    # Create frame for listing filenames
    global fileNameFrame
    set fileNameFrame [labelframe $dataFrame.fileNameFrame -text "Data filename(s)"]    
    grid $fileNameFrame -column 8 -row 2 -columnspan 3 -padx 5 -pady {3 5}
    
    # ------------------
    
	# Make get data button 
	button $dataFrame.dataBut -text "Collect\nData" -command getTDCData 
	grid $dataFrame.dataBut -column 12 -row 0 -rowspan 3 -padx 5
	bind $dataFrame.dataBut <Return> getTDCData
	#**********************************
	
	#**********************************
	# Process User Input
	#**********************************
	
	# Re-evaluate data file names and re-display them
	proc setDataFileList {} {
		global thisDir boardSerialNum edgeTypeChoice TAMP dataTypeChoice chanA chanB \
			cableDelay fileNameFrame dataChannel collectAllBool dataFileList displayBox \
			channelList dataFile
		set dataFileList {}
		set channelList {}
		if {!$collectAllBool} {
			if {$dataTypeChoice == "uncorrelated"} {
				for {set i 0} {$i < 24} {incr i} {
					if {$dataChannel($i)} {
						lappend channelList $i
						lappend dataFileList \
							[file join $thisDir data $boardSerialNum $dataTypeChoice $edgeTypeChoice $TAMP Ch$i [clock format [clock seconds] -format %Y-%m-%d_%Hh%Mm].raw]
					}
				}
			} elseif {$dataTypeChoice == "synchronous"} {
				lappend dataFileList \
					[file join $thisDir data $boardSerialNum $dataTypeChoice $edgeTypeChoice $TAMP Ch$chanA  [expr round($cableDelay)]ns_[clock format [clock seconds] -format %Y-%m-%d_%Hh%Mm].raw]
			} elseif {$dataTypeChoice == "cable-delay"} {
				lappend dataFileList \
					[file join $thisDir data $boardSerialNum $dataTypeChoice $edgeTypeChoice $TAMP Ch$chanA-Ch$chanB [expr round($cableDelay)]ns_[clock format [clock seconds] -format %Y-%m-%d_%Hh%Mm].raw]
			}
		} elseif {$collectAllBool} {
			if {$dataFile == ""} {
				set dataFile \
					[file join $thisDir data otherData [clock format [clock seconds] -format %Y-%m-%d_%Hh%Mm].raw]
			}
			lappend dataFileList $dataFile
		}
		# Add fileNames as labels
		set i 0
		foreach fileName $dataFileList {
			if {[winfo exists $fileNameFrame.$i]} {
				$fileNameFrame.$i configure -text $fileName
				grid $fileNameFrame.$i -column 0 -row $i
			} else {
				label $fileNameFrame.$i -text $fileName -wraplength 250
				grid $fileNameFrame.$i -column 0 -row $i
			}
			incr i
		}
		# Remove any labels with files not in the current dataFileList
		while {[winfo exists $fileNameFrame.$i]} {
			grid remove $fileNameFrame.$i
			incr i
		}
		return 1	
	}
	
	proc collectAll {} {
		global dataFrame collectAllBool dataTypeFrame displayBox edgeTypeFrame thisDir \
			boardSerialNum edgeTypeChoice TAMP dataChan testParam dataChanSelFrame \
			tampFrame cableDelayFrame
		
		# Disable channels as necessary
		if {$collectAllBool} {	
			set state disabled
			set oppositeState normal
			#set foregroundText SystemDisabledText
		} else {
			set state normal
			set oppositeState disabled
			#set foregroundText SystemButtonText
		}
		
		#$dataChanSelFrame configure -foreground $foregroundText
		foreach child [winfo children $dataChanSelFrame] {
			$child configure -state $state
		}
		#$tampFrame  configure -foreground $foregroundText
		foreach child [winfo children $tampFrame] {
			$child configure -state $state
		}

		$dataFrame.boardSerialNumLabel configure -state $state
		$dataFrame.boardSerialNumEntry configure -state $state
		$dataFrame.browse configure -state $oppositeState
		$dataFrame.dataFile configure -state $oppositeState
		$dataFrame.fileEntry configure -state $oppositeState
		$dataFrame.nodeIDLabel configure -state $state
		$dataFrame.nodeID configure -state $state
		foreach child [winfo children $dataTypeFrame] {
			$child configure -state $state
		}
		#$dataTypeFrame configure -foreground $foregroundText
		#$edgeTypeFrame configure -foreground $foregroundText
		foreach child [winfo children $edgeTypeFrame] {
			$child configure -state $state
		}
		#$cableDelayFrame configure -foreground $foregroundText
		foreach child [winfo children $cableDelayFrame] {
			$child configure -state $state
		}
		#$cableDelayFrame.calcDelayEntry configure -state readonly -foreground $foregroundText
		$cableDelayFrame.calcDelayEntry configure -state readonly
		setDataFileList
	}

    # Process to browse for a data file
    proc dataBrowse {} {
		global dataFile boardChoice dataFrame
		# Create file search dialog
		set typelist {
      		{"text files" {".raw"}}
      		{"all files" {*}}
		}
		#if {$boardChoice} {
		#	set initDir [file join [info script] getDataTDIG/Release/]
		#} else {
		#	set initDir [file join [info script] getDataJalapeno/Release/]
		#}
		set initDir [file dirname $dataFile]
		set initFile $dataFile
		#[file join $initDir dataTest.raw]
		set dataFile [tk_getSaveFile -filetypes $typelist -initialdir $initDir \
      		-initialfile $initFile -parent $dataFrame\
      		-title "Choose file for data collection"]
    }	
	
	# Get data process--happens when dataBut gets pushed
	proc getTDCData {} {
		global dataFrame dataTDCID dataNodeID displayBox connected \
			debug boardChoice numDataWords edgeTypeChoice dataTypeChoice \
			boardSerialNum collectAllBool dataFileList channelList boardChoice\
			dataFile
			
		setDataFileList	
				
		if {(!$collectAllBool) && ($boardSerialNum == "**changeme**")} {
			set button [tk_messageBox -icon error -type ok\
					-title "No board serial number..."\
					-message "You must enter the board serial number to collect data."\
					-parent $dataFrame]
		} else {
		
			# Enable only the selected channels, disable all others
			#if {$dataTypeChoice == "uncorrelated"} {
			#	if {$edgeTypeChoice == "leading"} {
			#		foreach tdc {1} {
			#			enableChan dataChannel $tdc 
			#		}
			#	} else {enableChan dataChannel 4}
			#}
			
			$displayBox insert end \
					"*********Get Data Procedure********\n"
			# If the file exists and it's non-empty...	
			
			# Grab first filename in dataFileList
			set dataFile1 [lindex $dataFileList 0]
			
			if {$debug} {
				$displayBox insert end "$dataFile1\n"
				$displayBox see end
			}
			if {[file exists $dataFile1] && [file size $dataFile1]} {
					set button [tk_messageBox -icon question -type okcancel\
					-title "File Exists..."\
					-message "$dataFile1 already exists! Do you want to \
					append data to this file?"\
					-parent $dataFrame]
				if {$button == "ok"} {
				
					
					if {$debug} {
     					set dirName [file join [info script] getDataTDIG/Debug] 
					} else {
     					set dirName [file join [info script] getDataTDIG/Release]
					}    
					set error [catch {open "| $dirName/getData $dataNodeID $numDataWords $dataFile1 $channelList"} cmdPipeLineChan]
					set linecnt 0
					while {! [eof $cmdPipeLineChan]} {
						gets $cmdPipeLineChan output
						incr linecnt
						$displayBox insert end "LINE $linecnt : $output\n"
						update idletasks
						$displayBox see end 
					}	
					$displayBox insert end "Error = $error\n" 

				}
			# If the file doesn't exist, go ahead with data-fetching
			# (a new file will get created)
			} else {
				if {(!$collectAllBool) && ($dataTypeChoice == "uncorrelated")} {
					# Make all the channel directories
					foreach fileItem $dataFileList {
						file mkdir [file dirname $fileItem]
					}
										
     				set dirName [file join [info script] getUncorrelatedData/Release] 
					set dataFileDirName [file dirname [file dirname $dataFile1]]
					set dataFileName [file tail $dataFile1] 
					if {$debug} {
						$displayBox insert end "dataFileDirName = $dataFileDirName\ndataFileName = $dataFileName\n"
					}
					set error [catch {open "| $dirName/getUncorrelatedData $dataNodeID $numDataWords $dataFileDirName $dataFileName $channelList"} cmdPipeLineChan]
					$displayBox see end
					set linecnt 0
					while {! [eof $cmdPipeLineChan]} {
						gets $cmdPipeLineChan output
						incr linecnt
						$displayBox insert end "LINE $linecnt : $output\n"
						update idletasks
						$displayBox see end 
					}	
					$displayBox insert end "Error = $error\n" 
				} else {
				
					file mkdir [file dirname $dataFile1]
					
					#--
					
					if {$debug} {
     					set dirName [file join [info script] getDataTDIG/Debug] 
					} else {
     					set dirName [file join [info script] getDataTDIG/Release]
					}    
					set error [catch {open "| $dirName/getData $dataNodeID $numDataWords $dataFile1 $channelList"} cmdPipeLineChan]
					$displayBox see end
					set linecnt 0
					while {! [eof $cmdPipeLineChan]} {
						gets $cmdPipeLineChan output
						incr linecnt
						$displayBox insert end "LINE $linecnt : $output\n"
						update idletasks
						$displayBox see end
					}	
					$displayBox insert end "Error = $error\n" 
				}
			}
		}
			$displayBox insert end \
					"\n******** End Get Data Procedure ********\n\n"
			$displayBox see 1.0
			$displayBox see end 
		}
	}