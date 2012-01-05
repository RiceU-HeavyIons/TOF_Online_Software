<?php
# $Id: run_tot.php,v 1.3 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
?>
<div class="section">
<div class="title">ToT Histograms</div>
<table>
<?php # input: $run
$rev = "";
if($run->id >= 10000126) {
  $rev = "_rev5";
}
$npad = $run->config->n_pads;
$nmod = $run->config->n_modules;

echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p <= $npad; $p++){
	echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
  $figd = $r2["data_uri"];
  $slot = $r2["slot"];
  echo "<tr>\n";
  echo "<td>mod #$slot</td>\n";
  for($p=1; $p <= $npad; $p++){
    echo "<td><a href=\"${figd}/fig/ToT_m{$slot}_p${p}$rev.gif\"><img src=\"${figd}/fig/thumb/ToT_m{$slot}_p${p}$rev.gif\" /></a></td>\n";
  }
  echo "</tr>\n";
}

if($run->id >= 10000126) {
echo "</table>\n";
echo "</div>\n";
echo "<div class=\"section\">\n";
echo "<div class=\"title\">ToT Histograms (Raw)</div>\n";
echo "<table>\n";
$rev = "_rev0";
echo "<tr>\n";
echo "<th></th>\n";
for($p=1; $p <= $npad; $p++){
	echo " <th> pad #${p}</th>\n";
}
echo "</tr>\n";
$result2 = mysql_query("SELECT * from module_results where run_id=$rvar_id", $con);
while($r2 = mysql_fetch_array($result2)) {
  $figd = $r2["data_uri"];
  $slot = $r2["slot"];
  echo "<tr>\n";
  echo "<td>mod #$slot</td>\n";
  for($p=1; $p <= $npad; $p++){
    echo "<td><a href=\"${figd}/fig/ToT_m{$slot}_p${p}$rev.gif\"><img src=\"${figd}/fig/thumb/ToT_m{$slot}_p${p}$rev.gif\" /></a></td>\n";
  }
  echo "</tr>\n";
}
}
?>
</table>
</div>
