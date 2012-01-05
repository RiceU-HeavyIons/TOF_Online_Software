<?php
# $Id: index.php,v 1.39 2008/08/29 02:38:33 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header_upload("System");
tsw_toolbar('system');
?>
<div id="main">
<div class="title">System</div>

<div class="section">
<div class="title">Current Run</div>

<table id="run_list">
<tr>
 <th></th>
 <th>ID</th>
 <th>Status</th>
 <th>Operator</th>
 <th>Type</th>
 <th>Config ID</th>
 <th>Start</th>
 <th>Run Time</th>
</tr>
<?php
$tm = time();
$l = 0;
foreach(Run::find_all("run_status_id=2", "id desc") as $run) {
   $l += 1;
   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td>$l.</td>\n";
   echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
   echo "  <td>", $run->status->name, "</td>\n";
   echo "  <td>", $run->user->name, "</td>\n";
   echo "  <td>", $run->type->name, "</td>\n";
   echo "  <td>", mk_link($run->config->id, "../config/show.php?id=" . $run->config->id), "</td>\n";
   echo "  <td>", $run->start, "</td>\n";
   printf("  <td>%5.2f hr</td>\n", ($tm - strtotime($run->start))/3600);
   echo " </tr>\n";
 }
?>
</table>
</div><!-- section -->

<div class="section">
<div class="title">Tray Stand 1</div>
<div class="section">
<div class="title">HV Status</div>
<div>
<table id='gas_list'>
 <tr>
  <th>Time</th>
  <th>VMON[0]</th>
  <th>IMON[0]</th>
  <th>STATUS[0]</th>
  <th>RAMP[0]</th>
  <th>VMON[4]</th>
  <th>IMON[4]</th>
  <th>STATUS[4]</th>
  <th>RAMP[4]</th>
 </tr>
<?php
for($j=0; $j < 6; $j++) {
 echo "<tr>\n";
  foreach(array(0, 4) as $i) {
    $q = mysql_query("SELECT * from hv_readings where channel = $i order by datetime desc limit $j, 1", $con);
    $r1 = mysql_fetch_object($q);
    if($i == 0) echo "<td>$r1->datetime</td>\n";
    echo "<td class=\"val\">$r1->vmon</td>\n";
    echo "<td class=\"val\">$r1->imon</td>\n";
    echo "<td class=\"val\">$r1->status</td>\n";
    echo "<td class=\"val\">$r1->ramp</td>\n";
  }
  echo "</tr>\n";
}
?>
</table>
</div>
</div><!-- section -->

<div class="section">
<div class="title">Gas Status</div>
<table id='gas_list'>
  <tr>
    <th>Timestamp</th>
    <th>Freon 1</th>
    <th></th>
    <th></th>
<!--    <th>Isobutane</th> -->
<!--    <th>SF6</th> -->
    <th>Freon 2</th>
  </tr>
<?php
for($j=0; $j < 6; $j++) {
	echo "<tr>\n";
	for($i = 1; $i <= 4; $i++) {
		$result1 = mysql_query("SELECT * from gas_readings where gas_type_id = $i order by datetime desc limit $j, 1", $con);
		$r1 = mysql_fetch_object($result1);
		if($i == 1) echo "<td>$r1->datetime</td>\n";
		printf("<td class='val'>%6.2f sccm</td>\n", $r1->value);
	}
	echo "</tr>\n";
}
?>
</table>
</div><!-- section -->
</div><!-- section -->

<div class="section">
<div class="title">Tray Stand 2</div>
<div class="section">
<div class="title">HV Status</div>
<table id='gas_list'>
 <tr>
  <th>Time</th>
  <th>VMON[2]</th>
  <th>IMON[2]</th>
  <th>STATUS[2]</th>
  <th>RAMP[2]</th>
  <th>VMON[6]</th>
  <th>IMON[6]</th>
  <th>STATUS[6]</th>
  <th>RAMP[6]</th>
 </tr>
<?php
for($j=0; $j < 6; $j++) {
 echo "<tr>\n";
  foreach(array(2, 6) as $i) {
    $q = mysql_query("SELECT * from hv_readings where channel = $i order by datetime desc limit $j, 1", $con);
    $r1 = mysql_fetch_object($q);
    if($i == 2) echo "<td>$r1->datetime</td>\n";
    echo "<td class=\"val\">$r1->vmon</td>\n";
    echo "<td class=\"val\">$r1->imon</td>\n";
    echo "<td class=\"val\">$r1->status</td>\n";
    echo "<td class=\"val\">$r1->ramp</td>\n";
  }
  echo "</tr>\n";
}
?>
</table>
</div><!-- section -->
</div><!-- section -->

<div class="section">
<div class="title">Submarine</div>
<div class="section">
<div class="title">HV Status</div>
<table id='gas_list'>
 <tr>
  <th>Time</th>
  <th>VMON[1]</th>
  <th>IMON[1]</th>
  <th>STATUS[1]</th>
  <th>RAMP[1]</th>
  <th>VMON[5]</th>
  <th>IMON[5]</th>
  <th>STATUS[5]</th>
  <th>RAMP[5]</th>
 </tr>
<?php
for($j=0; $j < 6; $j++) {
 echo "<tr>\n";
  foreach(array(1, 5) as $i) {
    $q = mysql_query("SELECT * from hv_readings where channel = $i order by datetime desc limit $j, 1", $con);
    $r1 = mysql_fetch_object($q);
    if($i == 1) echo "<td>$r1->datetime</td>\n";
    echo "<td class=\"val\">$r1->vmon</td>\n";
    echo "<td class=\"val\">$r1->imon</td>\n";
    echo "<td class=\"val\">$r1->status</td>\n";
    echo "<td class=\"val\">$r1->ramp</td>\n";
  }
  echo "</tr>\n";
}
?>
</table>
</div><!-- section -->
</div><!-- section -->

</div><!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
