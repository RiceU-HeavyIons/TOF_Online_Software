<?php
# $Id: run_paddle.php,v 1.3 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
?>
<div class="section">
<div class="title">Paddle</div>
<table>
<?php # function fig_magnifier_link($a, $b) { return link_fig($a, $b);} ?>
<tr>
 <th>ToT</th>
<?php
	for($i=0; $i < 3; $i++){
		echo " <td>" . fig_magnifier_link("SxToT_s{$i}_rev8.gif", $figd) . "</td>\n";
	}
?>
</tr>
<tr>
 <th>dT</th>
<?php
	for($i=0; $i < 3; $i++) {
		echo " <td>" . fig_magnifier_link("SxdT3_s${i}_rev8.gif", $figd) . "</td>\n";
	}
	echo "</tr>\n";
	echo "<tr>\n";
	echo " <th>dT vs ToT</th>\n";
	for($i=0; $i < 3; $i++) {
		echo " <td>" . fig_magnifier_link("SxdT3vsToT_s${i}_rev8.gif", $figd) . "</td>\n";
	}
	echo "</tr>\n";
	echo "</table>\n";
	echo "</div>\n";
	
	echo "<div class=\"section\">\n";
	echo "<div class=\"title\">Paddle (Raw)</div>\n";
	echo "<table>\n";

	echo "<tr>\n";
	echo " <th>ToT</th>\n";
	for($i=0; $i < 3; $i++){
		echo " <td>" . fig_magnifier_link("SxToT_s{$i}_rev0.gif", $figd) . "</td>\n";
	}
	echo "</tr>\n";
	echo "<tr>\n";
	echo " <th>dT</th>\n";
	for($i=0; $i < 3; $i++) {
		echo " <td>" . fig_magnifier_link("SxdT3_s${i}_rev0.gif", $figd) . "</td>\n";
	}
	echo "</tr>\n";
	echo "<tr>\n";
	echo " <th>dT vs ToT</th>\n";
	for($i=0; $i < 3; $i++) {
		echo " <td>" . fig_magnifier_link("SxdT3vsToT_s${i}_rev0.gif", $figd) . "</td>\n";
	}
	echo "</tr>\n";
?>
</table>
</div> 

<div class="section">
<div class="title">SdT Histograms</div>
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
  echo "<th>mod #$slot</th>\n";
  for($p=1; $p <= $npad; $p++) {
	echo "<td>" . link_fig("SdT_m{$slot}_p{$p}_rev8.gif", $figd) . "</td>\n";
  }
echo "</tr>\n";
}
?>
</table>
</div> <!-- section -->

<div class="section">
<div class="title">MRPC-avg(PMT) Histograms</div>
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
  echo "<th>mod #$slot</th>\n";
  for($p=1; $p <= $npad; $p++) {
	echo "<td>" . link_fig("RawSdT_m{$slot}_p{$p}_rev0.gif", $figd) . "</td>\n";
  }
echo "</tr>\n";
}
?>
</table>
</div> <!-- section -->
