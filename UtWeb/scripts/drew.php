<?php
system("cd /tmp; rm a.txt");
system(
 join("; ", array("cd /tmp",
                  "export ROOTSYS=/homes/tofp/c/root",
                  "export LD_LIBRARY_PATH=/homes/tofp/c/root/lib/root:/opt/intel/cc/10.0.023/lib",
                  "/homes/tofp/c/root/bin/root -q -b config_plot.C\\(258,1\\) > a.txt 2>&1")));
header('Content-Length:  30000');
header('Cache-control: private');
header('Content-Type: image/gif');
header('Content-Disposition: inline; filename="/tmp/historyx.gif"');
header('Pragma: public');   // Apache/IE/SSL download fix.
header('Content-Transfer-Encoding:  binary');
#system("rm /tmp/historyx.gif");
readfile("/tmp/historyx.gif");
?>