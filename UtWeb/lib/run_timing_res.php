<?php
# $Id: run_timing_res.php,v 1.6 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
?>
<div class="section">
<div class="title">Summary</div>
<table>
<?php
function link_fig($fig_name, $base) {
	$a = "<a href=\"" . $base . "/fig/" . $fig_name . "\">";
	$s = "<img src=\"". $base . "/fig/thumb/" . $fig_name . "\" />";
	return $a . $s . "</a>";
}

$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
$r2 = mysql_fetch_array($result2);
$figd = $r2["data_uri"];
echo "<tr>\n";
echo " <td>" . link_fig("report.gif", $figd) . "</td>\n";
echo " <td>" . link_fig("resmap.gif", $figd) . "</td>\n";
echo " <td>" . link_fig("hit_event_ratio.gif", $figd) . "</td>\n";
echo " <td>" . link_fig("ch.gif", $figd) . "</td>\n";
echo " <td>" . link_fig("no_corr.gif", $figd) . "</td>\n";
echo " <td>" . link_fig("pid.gif", $figd) . "</td>\n";
echo "</tr>\n";
?>
</table>
</div>

<div class="section">
  <div class="title">dT Histograms (After Correction)</div>
<table>
<?php
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p < $npad + 1; $p++){
	echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
	$figd = $r2["data_uri"];
	$slot = $r2["slot"];
	echo "<tr>\n";
	echo "<th>mod #$slot</th>\n";
	for($p=1; $p < 7; $p++){
		echo "<td><a href=\"${figd}/fig/dT4_m{$slot}_p${p}_rev5.gif\"><img src=\"${figd}/fig/thumb/dT4_m{$slot}_p${p}_rev5.gif\" /></a></td>\n";
}
echo "</tr>\n";
echo "<tr>\n";
echo " <td>", $r2["id"], "</td>\n";
for($p=1; $p < $npad + 1; $p++){
	echo " <td style=\"text-align: right;\">";
	printf("%3.1f", $r2["pad${p}"]);
	echo " ps</td>\n";
}
echo "</tr>\n";
}
?>
</table>
</div>


<div class="section">
<div class="title">Slewing Correction</div>
<table>
<?php
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p < $npad + 1; $p++){
	echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
	$figd = $r2["data_uri"];
	$slot = $r2["slot"];
	echo "<tr>\n";
	echo "<th>mod #$slot</th>\n";
	for($p=1; $p < 7; $p++){
		echo "<td><a href=\"${figd}/fig/dT4vsToT_m{$slot}_p${p}_rev{$slot}.gif\"><img src=\"${figd}/fig/thumb/dT4vsToT_m{$slot}_p${p}_rev{$slot}.gif\" /></a></td>\n";
}
echo "</tr>\n";
}
?>
</table>
</div>

<div class="section">
  <div class="title">dT Histograms (Raw)</div>
<table>
<?php
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
	$figd = $r2["data_uri"];
	$slot = $r2["slot"];
	echo "</tr>\n";
	echo "<tr>\n";
	echo "<td>mod #$slot</td>\n";
	for($p=1; $p < 7; $p++){
		echo "<td><a href=\"${figd}/fig/dT4_m{$slot}_p${p}_rev1.gif\"><img src=\"${figd}/fig/thumb/dT4_m{$slot}_p${p}_rev1.gif\" /></a></td>\n";
}
echo "</tr>\n";
}
?>
</table>
</div>
<?php
  $det = isset($rvar_detail)? $rvar_detail : false; 
  $tot = isset($rvar_tot)? $rvar_tot : false; 
  $pad = isset($rvar_paddle)? $rvar_paddle : false; 
  if($det || $tot) { include('run_tot.php'); }
  if($det || $pad) { include('run_paddle.php'); }
?>
