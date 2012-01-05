<?php
# $Id: run_tray_noise_rate.php,v 1.9 2008/05/16 03:22:55 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

function link_fig($fig_name, $base) {
   $a = "<a href=\"" . $base . "/fig/" . $fig_name . "\">";
   $s = "<img src=\"". $base . "/fig/thumb/" . $fig_name . "\" />";
   return $a . $s . "</a>";
}

function tricolor($val, $thr1, $thr2) {
  if ($val < $thr1) {
    $clr = 'black';
  } elseif ($val < $thr2) {
    $clr = 'blue';
  } else {
    $clr = 'red';
  }
  return "<font color='$clr'>" . sprintf("%4.1f", $val) . "</font>";
}
?>
<div class="section">
<div class="title">Tray Noise Rates (Hz)</div>
<?php
echo "<table>\n";
echo "<tr>\n";
echo "<th>slot #</th>\n";
echo "<th>module</th>\n";
echo "<th>tray (slot)</th>\n";
for($p=1; $p < $npad+1; $p++){
   echo " <th> pad #${p}</th>\n";
}
echo "<th>avg</th>\n";
echo "<th>min</th>\n";
echo "<th>max</th>\n";
echo "<th>note</th>\n";
echo "</tr>\n";
$l = 0;
foreach(ModuleResult::find_all("run_id=" . $rvar_id, "slot", "") as $mr) {
   $l += 1;
   $ll = $l % 2;
   $figd = $mr->data_uri;
   $md = $mr->module;
   echo " <tr class='row$ll'>\n";
   echo "  <th>" . $mr->slot . "</th>\n";
   if($md->id > 0) {
     echo "  <td>" . $md->p_link(). "</td>\n";
     echo "  <td>" . $md->tray->p_link() . " (" . $md->slot . ")</td>\n";
   } else {
     echo "  <td></td>\n";
     echo "  <td></td>\n";
   }
   for($i=1; $i <= $mr->npad; $i++) {
      echo "  <td style=''>" . tricolor($mr->pad[$i], 25, 50) . "</td>\n";
   }
   echo "  <td style=''>" . tricolor($mr->avg, 25, 50) . "</td>\n";
   echo "  <td style=''>" . tricolor($mr->min, 25, 50) . "</td>\n";
   echo "  <td style=''>" . tricolor($mr->max, 25, 50) . "</td>\n";
   echo "  <td>" . $mr->note . "</td>\n";
   echo " </tr>\n";
}
echo "</table>\n";
?>
</div> <!-- section -->

<div class="section">
<div class="title">Figure</div>
 <div><a href="<?php echo $figd ?>/report.pdf">full report</a></div>
 <div><img src="<?php echo $figd ?>/summary.gif" /></div>
</div> <!-- section -->