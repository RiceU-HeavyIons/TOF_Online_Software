# $Id: reset.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $

	# Reset process that resets the inputted TDC at the inputted node by
	# bringing the global reset bit in the control word low, high, and 
	# then low again.  See section 11.4 and 17.6 in HPTDC handbook, 
	# Version 2.2 for reference.
	proc reset {TDCID nodeID resolution} {
		global displayBox
		$displayBox insert end "******Reset Procedure******\n"
		$displayBox insert end [disableAllChan $TDCID $nodeID $resolution]
		$displayBox insert end [globalReset $TDCID $nodeID $resolution]
		$displayBox insert end [disableAllChan $TDCID $nodeID $resolution]
		$displayBox insert end "******End Reset Procedure******\n"
		$displayBox see 1.0
		$displayBox see end
	}