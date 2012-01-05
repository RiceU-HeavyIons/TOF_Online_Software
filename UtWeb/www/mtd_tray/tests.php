<?php
# $Id: list.php,v 1.5 2007/05/24 14:06:56 tofp Exp
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/utility.php");
  include("../../lib/session.php");
  include("../../lib/template.php");
$id=$_REQUEST['id'];

$results=mysql_query("select sn from trays where id=$id", $con);
$r=mysql_fetch_object($results);
$ser=$r->sn;

  tsw_header("List of Tests");
  tsw_toolbar('run');



?>
<div id="main">
<?php echo "<div class=\"title\">List of Runs for Tray ", $ser, "</div>";?>
<div class="section">
<table id="run_list">
<tr>
 <th></th>
 <th>ID</th>
 <th>Status</th>
 <th>Operator</th>
 <th>Type</th>
 <th>Config ID</th>
 <th>Start</th>
 <th>Stop</th>
 <th>Avg</th>
 <th>Min</th>
 <th>Max</th>
 <th>MR</th>
</tr>
<?php
/*
function pagenate($page, $total, $line, $opt) {
    echo "<div>page: ";
    $previous=$page-1;
    $next=$page+1;
    if($page != 1){echo "<a href=\"tests.php?page=$previous\">Previous</a>";}
    for($i=1; $i < $total/$line+1; $i++) {
      if($i == $page)
 	     echo " <font style=\"color: red; background-color: white;\">$i</font>";
      else
	echo " <a href=\"tests.php?page=$i\">$i</a>";
     } 
    if($page !=round($total/$line)){echo " <a href=\"tests.php?page=$next\">Next</a>";}
    echo "</div>";
}

$tot = Run::count($con);
$lim = 24;
$off = 0;
$page = isset($rvar_page)? $rvar_page : 0;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
$off = $lim*($page - 1);
if($tot > $lim) { pagenate($page, $tot, $lim, ""); }
*/
$tray = new Tray($id);
$l = 0;
foreach($tray->find_runs() as $run) {
   $l += 1;

   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td>$l.</td>\n";
   echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
   echo "  <td>", $run->status->name, "</td>\n";
   echo "  <td>", $run->user->name, "</td>\n";
   echo "  <td>", $run->type->name, "</td>\n";
   echo "  <td>", $run->config->id, "</td>\n";
   echo "  <td>", $run->start, "</td>\n";
   echo "  <td>", $run->stop, "</td>\n";
   $st = $run->stats_for_tray($tray->id);
   if($ct = $run->mr_count) {
        echo "  <td class='float'>" . sprintf("%5.1f", $st["avg"]) . "</td>\n";
        echo "  <td class='float'>" . sprintf("%5.1f", $st["min"]) . "</td>\n";
        echo "  <td class='float'>" . sprintf("%5.1f", $st["max"]) . "</td>\n";
        echo "  <td class='int'>", $ct, "</td>\n";
   } else {
        echo "  <td></td>\n";
        echo "  <td></td>\n";
        echo "  <td></td>\n";
        echo "  <td></td>\n";
   }
   echo " </tr>\n";
 }
echo "</table>\n";
if($tot > $lim) { pagenate($page, $tot, $lim, ""); }
?>
</div> <!--section -->

</div> <!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
 