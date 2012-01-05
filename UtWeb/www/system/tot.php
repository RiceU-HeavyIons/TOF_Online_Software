<?php
# $Id: tot.php,v 1.11 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header_upload('System - ToT');
tsw_toolbar('system');
?>
<div id="main">
<div class="title">UT TOF/System</div>

<div class="section">
<div class="title">Current Run</div>
<table>
<?php
$run = Run::last($con);
$rid = $run->id;
$start = $run->start;
$rvar_id = $run->id;

$config = $run->config;
$nmod = $config->n_modules;
$npad = $config->n_pads;

$cid = $run->run_config_id;
$tid = $run->run_type_id;
$sid = $run->run_status_id;
$rvar_id = $run->id;

$type = $run->type;
$status = $run->status;

if($status->id == 3) { # finished
	$stop = $run->stop;
	$rtime = (strtotime($stop) - strtotime($start))/3600;
} else if($status->id == 2) {
	$stop = "...";
	$rtime = (time() - strtotime($start))/3600;
}
printf("<tr><th>Id</th><td><a href=\"../run/show.php?id=%d\">%d</a></td></tr>\n", $rid, $rid);
print "<tr><th>Type</th><td>$type->name</td></tr>\n";
print "<tr><th>Status</th><td>$status->name</td></tr>\n";
print "<tr><th>Start</th><td>$start</td></tr>\n";
print "<tr><th>Stop</th><td>$stop</td></tr>\n";
printf("<tr><th>Run Time</th><td>%5.2f hr</td></tr>\n", $rtime);
?>
</table>
</div>

<?php
$figd = "http://www.rhip.utexas.edu/~tofp/var/r/$rid";

if ($run->type->id == 2) {
	echo "<div class=\"section\">\n";
	echo "<div class=\"title\">Summary</div>\n";
	echo "<table>\n";

	echo "<tr>\n";
	echo " <td>" . fig_magnifier_link("report.gif", $figd) . "</td>\n";
	echo " <td>" . fig_magnifier_link("resmap.gif", $figd) . "</td>\n";
	echo " <td>" . fig_magnifier_link("hit_event_ratio.gif", $figd) . "</td>\n";
	echo " <td>" . fig_magnifier_link("ch.gif", $figd) . "</td>\n";
	echo " <td>" . fig_magnifier_link("pid.gif", $figd) . "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
	echo "</div>\n";
?>

<div class="section">
<div class="title">ToT Histograms</div>
<table>
<?php
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p <= $npad; $p++) {
  echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
for($slot=1; $slot <= $nmod; $slot++) {
  echo "<tr>\n";
  echo "<td>mod #$slot</td>\n";
  for($p=1; $p <= $npad; $p++) {
	echo "<td>" . fig_magnifier_link("ToT_m{$slot}_p{$p}_rev1.gif", $figd) . "</td>\n";
}
echo "</tr>\n";
}
?>
</table>
</div>


<div class="section">
<div class="title">ToT Histograms (Raw)</div>
<table>
<?php
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p <= $npad; $p++) {
  echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
for($slot=1; $slot <= $nmod; $slot++) {
  echo "<tr>\n";
  echo "<td>mod #$slot</td>\n";
  for($p=1; $p <= $npad; $p++) {
	echo "<td>" . fig_magnifier_link("ToT_m{$slot}_p{$p}_rev0.gif", $figd) . "</td>\n";
}
echo "</tr>\n";
}
?>
</table>
</div>

<?php } else if ($run->type->id == 3) {
#	include("../../lib/run_noise_rate.php");
}
?>
</div> <!-- main -->
<?php
tsw_footer();
mysql_close($con);
?>
