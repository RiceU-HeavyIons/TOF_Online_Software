# $Id: configTDCFrame.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $
#***********************************
#SET UP OF CONTROLS TO CONFIGURE TDC 
#***********************************
proc configTDCFrame {canControllerFrame} {
	global bitFile

	set thisDir [file dirname [info script]]

	# First make frame, entries and button for getting status
	labelframe .configTDC -text "TDC Configuration" -borderwidth 3 -relief ridge
	pack .configTDC -side top -fill x

	# Make labeled entry for node ID
	label .configTDC.nodeLabel -text "node ID:" 
	pack .configTDC.nodeLabel -side left
	tk_optionMenu .configTDC.nodeID configNodeID 1 2 3 4 5 6 7 8 
	pack .configTDC.nodeID -side left 

	# Make labeled entry for TDC ID
	label .configTDC.tdcLabel -text "TDC ID:" -padx 0
	pack .configTDC.tdcLabel -side left -padx {10 0}
	tk_optionMenu .configTDC.tdcID configTDCID 1 2 3 4
	pack .configTDC.tdcID -side left 

	# Make labeled entry for bit file
	label .configTDC.bitFile -text "Enter Bit File:" 
	pack .configTDC.bitFile -side left -padx {10 0}
	entry .configTDC.fileEntry -width 20 -relief sunken \
		-textvariable bitFile
	set bitFile [file join $thisDir bitConfig.txt] 
	pack .configTDC.fileEntry -side left 

	# Create browse button for finding config file
	button .configTDC.browse -text "Browse" \
		-command configBrowse
	pack .configTDC.browse -side left -padx 10 -pady 10
	bind .configTDC.browse <Return> configBrowse

	# Create Configure TDC button
	button .configTDC.configTDC -text "Configure TDC" \
		-command configTDC -width 14
	pack .configTDC.configTDC -side right -padx {10 5} -pady 10
	bind .configTDC.configTDC <Return> configTDC

	# Process to browse for configuration file
	proc configBrowse {} {
		global bitFile thisDir boardChoice
		# Create file search dialog
		set typelist {
			{"text files" {".txt"}}
			{"all files" {*}}
		}		
		set initDir [file dirname $bitFile]
		set initFile $bitFile
		set bitFile [tk_getOpenFile -filetypes $typelist -initialdir $initDir \
      		-initialfile $initFile -parent .configTDC\
		-title "Choose configuration file"]
	}

	# Configure TDC process
	proc configTDC {} {
		global configTDCID configNodeID displayBox bitFile boardChoice
		set f $bitFile
		if {[file exists $f]} {
			$displayBox insert end "*************Performing Configuration************\n"
			#set config "\n[p_config $configTDCID $configNodeID $f]"
		
			if {$boardChoice} {
     					set dirName [file join [info script] p_configTDIG/Release] 
			} else {
     					set dirName [file join [info script] p_configJalapeno/Release]
			}    
			#set error [catch {open "| $dirName/p_config $configTDCID $configNodeID $f"} fileHandle]
			set fileHandle [open "| $dirName/p_config $configTDCID $configNodeID $f"]
			set linecnt 0
			while {! [eof $fileHandle]} {
			    gets $fileHandle foo
			    incr linecnt
			    #$displayBox insert end "LINE $linecnt : $foo\n"
			    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $foo]
			    update idletasks
			}	
			#$displayBox insert end "Error = $error\n"
			
			#$displayBox insert end $config
			$displayBox insert end "\n****************End Configuration****************\n\n"
			$displayBox see end
		} else {
			error "file $f doesn't exist."
		}
	}
}
#***********************************