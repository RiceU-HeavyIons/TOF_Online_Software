<?php
  include("../../lib/connect.php");
  include("../../lib/session.php");
  include("../../lib/orm.php");

  $rc = new RunConfig( $rvar_id );

  if ($rc->type->id == 1 || $rc->type->id == 2) {
    $mod_ids = $rc->module_ids;
    $mods  = array();
    foreach(split("\n", $mod_ids) as $ln) {
      $ln = chop($ln);
      $mid = preg_replace("/- (\d+)/", '${1}', $ln);
      array_push($mods, new Module($mid));
    }
  } else {
    $try_ids = $rc->module_ids;
    $trys  = array();
    foreach(split("\n", $try_ids) as $ln) {
      $ln = chop($ln);
      $tid = preg_replace("/- (\d+)/", '${1}', $ln);
      array_push($trys, new Tray($tid));
    }
  }

  include("../../lib/utility.php");
  include("../../lib/template.php");

  tsw_header("Config " . $rc->id);
  tsw_toolbar('config');
?>

<div id='main'>
<div class="title">Config <?php echo $rc->id?></div>
<?php
    if(Session::loggedin()) {
      echo "<div><a href='./edit.php?id={$rc->id}'>edit</a></div>\n";
    }
?>
<table style='margin: 10px 20px 10px 20px;'>
<?php
  echo "<tr class='row0'><td>id</td><td>"         . $rc->id          . "</td></tr>\n";
  echo "<tr class='row1'><td>user</td><td>"       . $rc->user->name  . "</td></tr>\n";
  
  echo "<tr class='row0'><td>location</td><td>"   . $rc->location->name . "</td></tr>\n";

  echo "<tr class='row1'><td>created_on</td><td>" . $rc->created_on  . "</td></tr>\n";
  echo "<tr class='row0'><td>start</td><td>"      . $rc->start       . "</td></tr>\n";
  echo "<tr class='row1'><td>stop</td><td>"       . $rc->stop        . "</td></tr>\n";

  echo "<tr class='row0'><td>run_config_type</td><td>"  . $rc->run_config_type->name   . "</td></tr>\n";

  echo "<tr class='row1'><td>n_modules</td><td>"  . $rc->n_modules   . "</td></tr>\n";
  echo "<tr class='row0'><td>n_pads</td><td>"    . $rc->n_pads      . "</td></tr>\n";
  echo "<tr class='row1'><td>threshold</td><td>"  . $rc->threshold   . "</td></tr>\n";
  echo "<tr class='row0'><td>hv_plus</td><td>["  . $rc->hv_channel_plus  . "] " . $rc->hv_plus . "</td></tr>\n";
  echo "<tr class='row1'><td>hv_minus</td><td>[" . $rc->hv_channel_minus . "] " . $rc->hv_minus . "</td></tr>\n";

  echo "<tr class='row0'><td>flow_rate1</td><td>[" . $rc->gas_channel_1 . "] " . $rc->flow_rate1  . "</td></tr>\n";
  echo "<tr class='row1'><td>flow_rate2</td><td>[" . $rc->gas_channel_2 . "] " . $rc->flow_rate2  . "</td></tr>\n";
  echo "<tr class='row0'><td>flow_rate3</td><td>[" . $rc->gas_channel_3 . "] " . $rc->flow_rate3  . "</td></tr>\n";
  echo "<tr class='row1'><td>note</td><td>"       . $rc->note        . "</td></tr>\n";

if($rc->type-> id == 1 || $rc->type->id == 2) {
  echo "<tr class='row0'><td>module_ids</td>\n";
} else {
  echo "<tr class='row0'><td>tray(s)</td>\n";
}
  echo "<td>\n";
  echo "<table border=1>\n";

  if($rc->type-> id == 1 || $rc->type->id == 2) {
    for($i = 1; $i <= 32; $i++) {
      $m = $mods[$i];
      if($i % 4 == 1) {
        echo "<tr>\n";
      }
      echo "<td><a href='../module/show.php?id={$m->id}'>{$m->serial}</a></td>\n";
      if($i % 4 == 0) {
        echo "</tr>\n";
      }
    }
  } else {
    echo "<tr>\n";
    for($i = 1; $i <= $rc->n_modules / 32; $i++) {
      $t = $trys[$i];
      echo "<td><a href='../tray/view.php?id={$t->id}'>{$t->sn}</a></td>\n";
    }
    echo "</tr>\n";
  }
?>
</table></td></tr>
</table>

<div class='section'>
<div class='title'>Run List</div>
<table id="run_list">
<tr>
 <th></th>
 <th>ID</th>
 <th>Status</th>
 <th>Operator</th>
 <th>Type</th>
<!-- <th>Config ID</th> -->
 <th>Start</th>
 <th>Stop</th>
 <th>MR</th>
</tr>
<?php
foreach(Run::find_all("run_config_id={$rc->id}", "id desc","") as $run) {
   $l += 1;

   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td>$l.</td>\n";
   echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
   echo "  <td>", $run->status->name, "</td>\n";
   echo "  <td>", $run->user->name, "</td>\n";
   echo "  <td>", $run->type->name, "</td>\n";
#   echo "  <td>", mk_link($run->config->id, "./show.php?id=" . $run->config->id), "</td>\n";
   echo "  <td>", $run->start, "</td>\n";
   echo "  <td>", $run->stop, "</td>\n";
   echo "  <td>", $run->mr_count, "</td>\n";
   echo " </tr>\n";
 }
echo "</table>\n";
?>
</div><!-- section -->

<div class='section'>
<div class='title'>History</div>
<img src="http://www.rhip.utexas.edu/~tofp/var/cf/<?php echo $rc->id ?>/history.gif" />
</div><!-- section -->

<!--
<div class='section'>
<div class='title'>gas information</div>
<table style='margin: 10px 20px 10px 20px;'>
<tr>
  <th>valume (l)</th>
  <th>flow avg. (sccm)</th>
  <th>flow min. (sccm)</th>
  <th>flow max. (sccm)</th>
</tr>
<?php
$sts = $rc->gas_stats(1);
  echo "<tr class='row0'>";
  echo " <td style='text-align: right;'>" . sprintf("%5.2f", $sts["sum"]) . "</td>\n";
  echo " <td style='text-align: right;'>" . sprintf("%5.2f", $sts["avg"]) . "</td>\n";
  echo " <td style='text-align: right;'>" . sprintf("%5.2f", $sts["min"]) . "</td>\n";
  echo " <td style='text-align: right;'>" . sprintf("%5.2f", $sts["max"]) . "</td>\n";
  echo "</tr>\n";
?>
</table>
</div><!-- section -->
-->

</div><!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
