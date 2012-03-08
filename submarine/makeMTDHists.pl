#!/usr/bin/env perl

$rootf=@ARGV[0];
$inst=@ARGV[1] || "USTC";
$sensor=@ARGV[2] || "001";
$hv=@ARGV[3] || 7300;
$threshold=@ARGV[4] || 2500;

#print "Enter something: ";
#my $input = <STDIN>;
#$input =~ s/[\n\r\f\t]//g; # remove whitespace and line ends
#print "You entered: '$input'\n";

print "Institute [USTC/THU]: ";
$inst = <STDIN>;
$inst =~ s/[\n\r\f\t]//g; # remove whitespace and line ends

print "MRPC #: ";
$sensor = <STDIN>;
$sensor =~ s/[\n\r\f\t]//g; # remove whitespace and line ends

print "High Voltage: ";
$hv = <STDIN>;
$hv =~ s/[\n\r\f\t]//g; # remove whitespace and line ends

print "Threshold: ";
$threshold = <STDIN>;
$threshold =~ s/[\n\r\f\t]//g; # remove whitespace and line ends

print "No of events: ";
$n_events = <STDIN>;
$n_events =~ s/[\n\r\f\t]//g; # remove whitespace and line ends


$command="root -l  -b 'makeMTDHists.C(\"$rootf\", \"$inst\", \"$sensor\", \"$hv\", \"$threshold\", \"$n_events\")'";

system($command);

