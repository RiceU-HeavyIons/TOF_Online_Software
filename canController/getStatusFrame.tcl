# $Id: getStatusFrame.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $
#************************************
#SET UP OF CONTROLS TO GET TDC STATUS 
#************************************

proc getStatusFrame {canControllerFrame} {

	# First make frame for getting status
	labelframe .gs -text "Status Checking" -borderwidth 3 -relief ridge
	pack .gs -side top -fill x

	# Make labeled menu for node ID
	label .gs.nodeLabel -text "node ID:" 
	pack .gs.nodeLabel -side left
	tk_optionMenu .gs.nodeID gsNodeID 1 2 3 4 5 6 7 8
	pack .gs.nodeID -side left 

	# Make labeled menu for TDC ID
	label .gs.tdcLabel -text "TDC ID:" 
	pack .gs.tdcLabel -side left -padx {10 0}
	# Add values below as more TDCs exist on board (only two exist now)
	tk_optionMenu .gs.tdcNum gsTDCNum 1 2 3 4
	pack .gs.tdcNum -side left

	# Create Get TDC Status button
	button .gs.get_statusTDC -text "Get TDC Status" \
		-command getStatus -width 14
	pack .gs.get_statusTDC -side right -padx {10 5} -pady 10
	bind .gs.get_statusTDC <Return> getStatus

	# Get TDC status process
	proc getStatus {} {
		global gsTDCNum gsNodeID displayBox debug
		$displayBox insert end "*************Status Report************\n"
		
		
		#set stat "[get_status $gsTDCNum $gsNodeID $displayBox $boardChoice]"
		
		if {$debug} {
     				set dirName [file join [info script] get_status/Debug] 
		} else {
     				set dirName [file join [info script] get_status/Release]
		}    
		set error [catch {open "| $dirName/get_status $gsTDCNum $gsNodeID"} cmd]
		#set error [catch {open "| C:////Liz////RichardsCode////cooked////Debug////cooked.exe -f"} cmd]
		set linecnt 0
		while {! [eof $cmd]} {
		    gets $cmd foo
		    incr linecnt
		    #puts stdout "LINE $linecnt : $foo"
		    #$displayBox insert end "LINE $linecnt : $foo\n"
		    $displayBox insert end [format "LINE %3d : %s\n"  $linecnt $foo]
		    update idletasks
		}	
		$displayBox insert end "Error = $error\n"
		#$displayBox insert end "$stat\n" 
		$displayBox insert end "***********End Status Report**********\n\n"
		$displayBox see 1.0
		$displayBox see end 
	}
}
#************************************
