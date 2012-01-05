<?php
# $Id: show.php,v 1.47 2012/01/05 17:25:42 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/utility.php");
  include("../../lib/session.php");
  include("../../lib/template.php");

function list_result($id, $type)
{
  echo "<table>\n";
  echo " <tr class=\"rowt\">\n";
#  echo "  <th class=\"key\">Analyzed</th>\n";
  echo "  <th class=\"key\">Run</th>\n";
  echo "  <th class=\"key\">Run Start</th>\n";
  echo "  <th class=\"key\">Run Stop</th>\n";
  echo "  <th class=\"key\">Slot</th>\n";
  echo "  <th class=\"key\">HV+ (V)</th>\n";
  echo "  <th class=\"key\">HV- (V)</th>\n";
  echo "  <th class=\"key\">Threshold<br/>(mV)</th>\n";
  echo "  <th class=\"key\">Freon<br/>(SCCM)</th>\n";
  echo "  <th class=\"key\">Isobutane<br/>(SCCM)</th>\n";
  echo "  <th class=\"key\">Pad 1</th>\n";
  echo "  <th class=\"key\">Pad 2</th>\n";
  echo "  <th class=\"key\">Pad 3</th>\n";
  echo "  <th class=\"key\">Pad 4</th>\n";
  echo "  <th class=\"key\">Pad 5</th>\n";
  echo "  <th class=\"key\">Pad 6</th>\n";
  echo "  <th class=\"key\">Note</th>\n";
  echo " </tr>\n";

  $l = 0;
  foreach(ModuleResult::find_all("module_id=$id", "", "") as $mr) {
    $run = $mr->run;
    $conf = $run->config;

    if($run->type->id == $type){
      echo " <tr class=\"row$l\">\n";
#     echo "  <td>" . $r["analyzed_on"] . "</td>\n" ;
      echo "  <td class='int'> " . mk_link($run->id, "../run/show.php?id=" . $run->id) . "</td>\n" ;
      echo "  <td class='timestamp'>" . $run->start . "</td>\n" ;
      echo "  <td class='timestamp'>" . $run->stop . "</td>\n" ;
      echo "  <td>" . $mr->slot . "</td>\n" ;
      echo "  <td class=\"float\">" . $conf->hv_plus . "</td>\n" ;
      echo "  <td class=\"float\">" . $conf->hv_minus . "</td>\n" ;
      echo "  <td class=\"float\">" . $conf->threshold . "</td>\n" ;
      echo "  <td class=\"float\">" . $conf->flow_rate1 . "</td>\n" ;
      echo "  <td class=\"float\">" . $conf->flow_rate2 . "</td>\n" ;
      for($p = 1; $p <= $conf->n_pads; $p++) {
        echo "  <td class=\"float\">";
        printf("%3.1f", $mr->pad["$p"]);
        echo "</td>\n";
      }
      echo "  <td>" . $mr->note . "</td>\n" ;
#      echo "  <td><a href='../results/run/ts$rid/'>histograms</a></td>\n"; 
      echo " </tr>\n";
      $l = ($l+=1)%2;
    }
  }
  echo "</table>\n";
};


if($rvar_id != NULL) {
  $mid = intval(preg_replace('/\W*(\d+)\W*/', '$1', $rvar_id));
  $mod = new Module($mid);
} else if($rvar_sn !=NULL) {
  $sn = ucwords($rvar_sn);
  $sn = preg_replace('/\W*(\w+)\W*/', '$1', $sn);
  $mod = Module::find_by_sn($sn);
}
$mid = $mod->id;
?>
<?php
  tsw_header($mod->serial);
  tsw_toolbar('module');
?>
<!--
  <link rel="prev" href="show.php?id=<?php if(isset($rid)) {echo $rid-1;}?>" />
  <link rel="next" href="show.php?id=<?php if(isset($rid)) {echo $rid+1;}?>" />
-->
<div id="main">
<div class="title"><?php echo $mod->serial?></div>
<a href="show.php?id=<?php echo $mid-1?>" accesskey="p">&lt;prev</a>
| <a href="list.php?page=<?php echo intval(1+$mid/32); ?>">module list</a>
| <a href="show.php?id=<?php echo $mid+1?>" accesskey="n">next&gt;</a>
<div>[<a href="yaml.php?id=<?php echo $mid ?>">yaml</a>]</div>
<div>Received: <?php echo $mod->received_on ?></div>
<div>Vendor ID: <?php echo mk_link($mod->chinese_id, $mod->chinese_link) ?></div>
<div>Tray: <?php echo $mod->tray->p_link(); ?></div>
<div>Note: <?php echo $mod->note ?></div>
<div>HV Test: <?php echo $mod->hvtest->result ?></div>

<div class="section">
<div class="title">Dimensions</div>
<table>
 <tr class="rowt">
  <th class="key">Length 1 (mm)</th>
  <th class="key">Length 2 (mm)</th>
  <th class="key">Width 1 (mm)</th>
  <th class="key">Width 2 (mm)</th>
  <th class="key">Thickness 1 (mm)</th>
  <th class="key">Thickness 2 (mm)</th>
  <th class="key">Jig (0=failed, 1=passed)</th>
  <th class="key">Note</th>
 </tr>

<?php
$result2 = mysql_query(
  "SELECT * from module_sizes where module_id=$mid",
  $con
);

$l = 0;
while($r2 = mysql_fetch_object($result2)) {
  echo " <tr class=\"row$l\">\n";
  echo "  <td class=\"size\">" . $r2->length1 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->length2 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->width1 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->width2 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->thickness1 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->thickness2 . "</td>\n" ;
  echo "  <td class=\"size\">" . $r2->jig_passed . "</td>\n" ;
  echo "  <td>" . $r2->note . "</td>\n" ;
  echo " </tr>\n";}
$l+=1;
?>

</table>
<a href="javascript:window.open('http:../modules/figs/rpc_meas_diagram.jpg','_blank','width=810,height=510,resizable=1,scrollbars=1');undefined;">diagram</a>
| <a href="size.php">list</a>
</div> <!-- section -->

<div class="section">
<div class="title">Large Stand Noise Test (Hz)</div>
<?php list_result($mid, 5); ?>
</div> <!-- section -->

<div class="section">
<div class="title">Large Stand Timing Resolution Test (ps)</div>
<?php list_result($mid, 6); ?>
</div> <!-- section -->

<div class="section">
 <div class="title">Tray Noise Rates (Hz)</div>
 <?php list_result($mid, 4); ?>
</div> <!-- section -->

<div class="section">
 <div class="title">Tray Timing Resolutions (ps)</div>
 <?php list_result($mid, 7); ?>
</div> <!-- section -->

<div class="section">
 <div class="title">Small Stand Noise Rates (Hz)</div>
 <?php list_result($mid, 3); ?>
</div> <!-- section -->

<div class="section">
 <div class="title">Small Stand Timing Resolutions (ps)</div>
 <?php list_result($mid, 2); ?>
</div> <!-- section -->

</div> <!-- id=main -->
<?php
  tsw_footer();
  mysql_close($con);
 ?>
