<?php
# $Id: show.php,v 1.28 2008/07/29 20:45:10 tofp Exp $
# Copyright (C) 2007, 2008, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");
include("../../lib/utility.php");

$run = new Run($rvar_id, $con);
$config = $run->config;
$nmod = $config->n_modules;
$npad = $config->n_pads;

if($run->status->name == 'Finished') { # finished
  $stop = $run->stop;
  $rtime = $run->str_run_time();
} else if($run->status->name == 'Running') {
  $stop = "...";
  $start = $run->start;
  $rtime = sprintf("%5.2f hr", (time() - strtotime($start))/3600);
}

tsw_header("Run " . $run->id);
tsw_toolbar('run');
?>
<div id="main">
<div class="title">Run <?php echo $run->id?></div>
<?php
    if(Session::loggedin()) {
      echo "<div><a href='./edit.php?id={$run->id}'>edit</a></div>\n";
    }
?>
<div>
 [<a href="./show.php?id=<?php echo $rvar_id;?>&detail=0">simple</a>]
 [<a href="./show.php?id=<?php echo $rvar_id;?>&detail=1">detail</a>]
</div>
<table style="border: solid 1px #999">
 <tr class="row0">
  <th>id</th>
  <td><?php echo $run->id; ?></td>
 </tr>
  <tr class="row1">
   <th>Status</th>
   <td><?php echo $run->status->name; ?></td>
 </tr>
 <tr class="row0">
   <th>Type</th>
   <td><?php echo $run->type->name; ?></td>
 </tr>
 <tr class="row1">
   <th>Start</th>
   <td><?php echo $run->start; ?></td>
 </tr>
 <tr class="row0">
   <th>Stop</th>
   <td><?php echo $stop; ?></td>
 </tr>
 <tr class="row1">
  <th>Run time</th>
   <td><?php echo $rtime; ?></td>
 </tr>
 <tr class="row0">
  <th>Events</th>
   <td><?php echo $run->events; ?></td>
 </tr>
  <tr class="row1">
  <th>Note</th>
   <td><?php echo $run->note; ?></td>
 </tr>
 <tr class='row0'>
  <th>Config</th>
  <td>
    <a href="../config/show.php?id=<?php echo $config->id ?>"><?php echo $config->id?></a>
    -- <?php echo $config->note; ?>
  </td>
 </tr>
 <tr class='row1'>
  <th>HV+</th>
   <td><?php echo $config->hv_plus; ?> V</td>
 </tr>
 <tr class="row0">
  <th>HV-</th>
   <td><?php echo $config->hv_minus; ?> V</td>
 </tr>
 <tr class="row1">
  <th>Threshold</th>
   <td><?php echo $config->threshold; ?> mV</td>
 </tr>
 <tr class="row0">
  <th>Freon</th>
   <td><?php echo $config->flow_rate1; ?> sccm</td>
 </tr>
 <tr class="row1">
  <th>Isobutane</th>
   <td><?php echo $config->flow_rate2; ?> sccm</td>
 </tr>
<?php
if($run->type->id < 4)
for($i=0; $i < $nmod; $i++) {
  $j = $i + 1;
  $row = $j % 2;
  $m = $config->modules[$j];
  echo "<tr class=\"row$row\">\n";
  echo " <th>module $j</th>\n";
  echo " <td>" . mk_link($m->serial, "../module/show.php?id=" . $m->id) . "</td>\n";
  echo "</tr>\n";
}
?>
</table>
<?php
if ($run->status->id == 2) {
 echo "<div class='section'>\n";
 echo "<div class='title'>Snapshot</div>\n";
 echo "<div><a href='/~tofp/var/r/{$run->id}/report.pdf'>full report</a></div>\n";
 echo " <a href='/~tofp/var/r/{$run->id}/summary.gif'>\n";
 echo "  <img src='/~tofp/var/r/{$run->id}/summary.gif' alt='summary'/>\n";
 echo " </a>\n";
 echo "</div><!-- section -->\n";
} else if ($run->type->id == 2) {
   include("../../lib/run_timing_res.php");
} else if ($run->type->id == 3) {
   include("../../lib/run_noise_rate.php");
} else if ($run->type->id == 4) {
   include("../../lib/run_tray_noise_rate.php");
} else if ($run->type->id == 5) {
  include("../../lib/run_tray_noise_rate.php");
} else if ($run->type->id == 6) {
  include("../../lib/run_tray_timing_res.php");
} else if ($run->type->id == 7) {
  include("../../lib/run_tray_timing_res.php");
}

?>
</div> <!-- id=main -->
<?php 
  tsw_footer();
  mysql_close($con);
?>

