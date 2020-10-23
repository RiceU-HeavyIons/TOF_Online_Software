#!/usr/bin/perl -w

#DEBUG    normal
#NOTICE   green
#WARNING  magenta
#ERROR    cyan
#OPERATOR red
#CRITICAL reverse red

while (<>) {
    if($_ =~ /tof|TOF/) {
	if ($_ =~ /DEBUG/) {
	    print "[0m",$_,"[0m";
	}
	if ($_ =~ /NOTICE/) {
	    print "[0m[32m",$_,"[0m";
	}
	if ($_ =~ /WARNING/) {
	    print "[0m[35m",$_,"[0m";
	}
	if ($_ =~ /ERROR/) {
	    print "[0m[36m",$_,"[0m";
	}
	if ($_ =~ /OPERATOR/) {
	    print "[1m[31m",$_,"[0m";
	}
	if ($_ =~ /CRITICAL/) {
	    print "[7m[31m",$_,"[0m";
	}
    }
}
