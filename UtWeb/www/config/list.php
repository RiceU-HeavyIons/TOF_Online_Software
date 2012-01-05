<?php
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/session.php");
  include("../../lib/template.php");
  include("../../lib/utility.php");

  $tsw = new Tsw("Config List");
  $tsw->header();
  $tsw->toolbar('config');
?>

<div id='main'>
<div class="title">Config List</div>

<div><a href="new.php">new set</a></div>

<table style='border: 1px solid black;'>
<tr>
 <th>id</th>
 <th>op.</th>
<!-- <th>created_on</th> -->
 <th>start</th>
 <th>stop</th>
 <th>config_type</th>
 <th>note</th>
 <th># of runs</th>
</tr>
<?php
$tot = RunConfig::count();
$lim = 24;
$off = 0;
$page = isset($rvar_page)? $rvar_page : 1;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
$off = $lim*($page - 1);
if($tot > $lim) { list_pagenate($page, $tot, $lim, ""); }

 $ln = 0;
 foreach(RunConfig::find_all('', 'id desc', "$off, $lim", "") as $rc){
   $ll = $ln % 2;
   echo "<tr  class='row{$ll}'>\n";
   echo "<td> <a href='show.php?id={$rc->id}'>" . $rc->id . "</td>\n";
   echo "<td>" . $rc->user->name . "</td>\n";
#   echo "<td>" . $rc->created_on . "</td>\n"; 
   echo "<td>" . $rc->start . "</td>\n";
   echo "<td>" . $rc->stop . "</td>\n";
   echo "<td>" . $rc->type->name . "</td>\n";
   echo "<td>" . $rc->note . "</td>\n";
   echo "<td style='text-align: right;'>" . Run::count("run_config_id = {$rc->id}") . "</td>\n";
   echo "</tr>\n";
   $ln += 1;
 }
?>
</table>
</div><!-- main -->
<?php
  $tsw->footer();
  mysql_close($con);
?>