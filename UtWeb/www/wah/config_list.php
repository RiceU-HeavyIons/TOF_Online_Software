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
$page = isset($rvar_page)? $rvar_page : 1;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
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
