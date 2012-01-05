<?php
# $Id: run_noise_rate.php,v 1.5 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

function link_fig($fig_name, $base) {
	$a = "<a href=\"" . $base . "/fig/" . $fig_name . "\">";
	$s = "<img src=\"". $base . "/fig/thumb/" . $fig_name . "\" />";
	return $a . $s . "</a>";
}
?>
<div class="section">
<div class="title">Summary</div>
<?php
echo "<table>\n";
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p < $npad+1; $p++){
	echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
	$figd = $r2["data_uri"];
	$slot = $r2["slot"];
	echo "<tr>\n";
	echo "<th>mod #$slot</th>\n";
	for($p=1; $p < $npad+1; $p++){
		echo "<td style=\"text-align: right;\">";
		printf("%3.1f", $r2["pad${p}"]);
		echo " Hz</td>\n";
}
echo " <td>", $r2["note"], "</td>\n";
echo "</tr>\n";
}
echo "</table>\n";
?>
<table>
<tr>
 <th>noise rates (l.e.)</th>
 <th>noise rates (t.e.)</th>
 <th>noise rates</th>
 <th>ch correlation (l.e.)</th>
</tr>
<tr>
 <td><?php echo link_fig("first_le_noise_rates.gif", $figd); ?></td>
 <td><?php echo link_fig("first_te_noise_rates.gif", $figd); ?></td>
 <td><?php echo link_fig("ch_noise.gif", $figd); ?></td>
 <td><?php echo link_fig("no_corr.gif", $figd); ?></td>
</tr>
</table>
</div> <!-- section -->

<div class="section">
<div class="title">Leading Edge</div>
<table>
<tr>
 <th>full counts</th>
 <th>full rate</th>
 <th>counts</th>
 <th>rates</th>
<tr>
<?php
echo " <td>", link_fig("le_noise_counts.gif", $figd), "</td>\n";
echo " <td>", link_fig("le_noise_rates.gif", $figd), "</td>\n";
echo " <td>", link_fig("first_le_noise_counts.gif", $figd), "</td>\n";
echo " <td>", link_fig("first_le_noise_rates.gif", $figd), "</td>\n";
?>
</tr>
</table>
</div> <!-- section -->

<div class="section">
<div class="title">Trailing Edge</div>

<table>
<tr>
 <th>full counts</th>
 <th>full rate</th>
 <th>counts</th>
 <th>rates</th>
<tr>
<?php
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
$r2 = mysql_fetch_array($result2);
$figd = $r2["data_uri"];
$slot = $r2["slot"];
echo " <td>", link_fig("te_noise_counts.gif", $figd), "</td>\n";
echo " <td>", link_fig("te_noise_rates.gif", $figd), "</td>\n";
echo " <td>", link_fig("first_te_noise_counts.gif", $figd), "</td>\n";
echo " <td>", link_fig("first_te_noise_rates.gif", $figd), "</td>\n";
?>
</tr>
</table>
</div> <!-- section -->

<div class="section">
<div class="title">Channel Distribution and Correlation</div>
<table>
<tr>
 <th>noise counts</th>
 <th>noise rates</th>
 <th>ch correlation (l.e.)</th>
</tr>
<tr>
 <td><?php echo link_fig("ch_dist.gif", $figd); ?></td>
 <td><?php echo link_fig("ch_noise.gif", $figd); ?></td>
 <td><?php echo link_fig("no_corr.gif", $figd); ?></td>
</tr>
</table>
</div> <!-- section -->