# $Id: setThreshold.tcl,v 1.1 2004-08-20 20:26:40 jschamba Exp $

# Calls setThreshold.exe to set the discriminator threshold of all TDCs at the inputted node
proc setThreshold {nodeID voltage} {
	global displayBox
	
	set dirName [file join [info script] setThreshold/Release] 
	set errorNum [catch {exec $dirName/setThreshold $nodeID $voltage} error]
	$displayBox insert end "$error\nExit code: $errorNum"	
}