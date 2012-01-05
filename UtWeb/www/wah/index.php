<?php
# $Id: list.php,v 1.5 2007/05/24 14:06:56 tofp Exp
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/utility.php");
  include("../../lib/session.php");
  include("../../lib/template.php");

  tsw_header("Test at WAH");
  tsw_toolbar("wah");
?>
<div id="main">
<div class="title">Test at WAH</div>

<div class="section">
<div class="title">File Upload</div>
<div>Please select tarball (2009mmdd_HHMMSS.tgz) to upload.</div>
<div>
 <form action="upload_file.php" method="post" enctype="multipart/form-data">
 <label for="file">Filename:</label>
 <input type="file" name="file" id="file" size="40"/>
 <br />
 <input type="submit" name="submit" value="submit" />
 </form>
</div>
</div><!-- section -->

<div class="section">
<div class="title">Results</div>

<table class="run_list">
<tr>
 <th></th>
 <th>ID</th>
<!-- <th>Status</th> -->
 <th>Operator</th>
 <th>Run Type</th>
 <th>Config ID</th>
 <th>Start</th>
 <th>Stop</th>
 <th>MR</th>
</tr>
<?php

$tot = Run::count_with_config("rc.location_id=4");
$lim = 20;
$off = 0;
$page = 1;
$off = $lim*($page - 1);
if($tot > $lim) { pagenate("run_list.php", $page, $tot, $lim, ""); }

$l = 0;
foreach(Run::find_with_config("rc.location_id = 4", "id desc", "$off, $lim", $con) as $run) {
   $l += 1;

   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td>$l.</td>\n";
   echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
#   echo "  <td>", $run->status->name, "</td>\n";
   echo "  <td>", $run->user->name, "</td>\n";
   echo "  <td>", $run->type->name, "</td>\n";
   $txt = $run->config->id . " - " . $run->config->note;
   echo "  <td>", mk_link($txt, "../config/show.php?id=" . $run->config->id), "</td>\n";
   echo "  <td>", $run->start, "</td>\n";
   echo "  <td>", $run->stop, "</td>\n";
   echo "  <td>", $run->mr_count, "</td>\n";
   echo " </tr>\n";
 }
echo "</table>\n";
?>
</div><!-- section -->

<div class='section'>
<div class="title">Config List</div>

<table class="run_list">
<tr>
 <th></th>
 <th>ID</th>
 <th>Operator</th>
<!-- <th>created_on</th> -->
 <th>Config Type</th>
 <th>Note</th>
 <th>Start</th>
 <th>Stop</th>
 <th># of Runs</th>
</tr>
<?php
$tot = RunConfig::count("location_id=4");
$lim = 20;
$off = 0;
$page = 1;
$off = $lim*($page - 1);
if($tot > $lim) { pagenate("config_list.php", $page, $tot, $lim, ""); }

 $ln = 1;
 foreach(RunConfig::find_all('location_id=4', 'id desc', "$off, $lim", "") as $rc){
   $ll = $ln % 2;
   echo "<tr  class='row{$ll}'>\n";
   echo "<td>$ln.</td>\n";
   echo "<td> <a href='../config/show.php?id={$rc->id}'>" . $rc->id . "</td>\n";
   echo "<td>" . $rc->user->name . "</td>\n";
#   echo "<td>" . $rc->created_on . "</td>\n"; 
   echo "<td>" . $rc->type->name . "</td>\n";
   echo "<td>" . $rc->note . "</td>\n";
   echo "<td>" . $rc->start . "</td>\n";
   echo "<td>" . $rc->stop . "</td>\n";
   echo "<td style='text-align: right;'>" . Run::count("run_config_id = {$rc->id}") . "</td>\n";
   echo "</tr>\n";
   $ln += 1;
 }
?>
</table>
</div><!-- section -->
</div><!-- main -->
<?php
  $tsw->footer();
  mysql_close($con);
?>
