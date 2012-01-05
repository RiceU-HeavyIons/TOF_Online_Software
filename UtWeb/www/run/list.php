<?php
# $Id: list.php,v 1.5 2007/05/24 14:06:56 tofp Exp
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/utility.php");
  include("../../lib/session.php");
  include("../../lib/template.php");

  tsw_header("Run List");
  tsw_toolbar('run');
?>
<div id="main">
<div class="title">Run List</div>

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
 <th>MR</th>
</tr>
<?php

$tot = Run::count();
$lim = 24;
$off = 0;
$page = isset($rvar_page)? $rvar_page : 0;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
$off = $lim*($page - 1);
if($tot > $lim) { pagenate("list.php", $page, $tot, $lim, ""); }

$l = 0;
foreach(Run::find_all("", "id desc", "$off, $lim", $con) as $run) {
   $l += 1;

   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td>$l.</td>\n";
   echo "  <td>", mk_link($run->id, "./show.php?id=" . $run->id), "</td>\n";
   echo "  <td>", $run->status->name, "</td>\n";
   echo "  <td>", $run->user->name, "</td>\n";
   echo "  <td>", $run->type->name, "</td>\n";
   echo "  <td>", mk_link($run->config->id, "../config/show.php?id=" . $run->config->id), "</td>\n";
   echo "  <td>", $run->start, "</td>\n";
   echo "  <td>", $run->stop, "</td>\n";
   echo "  <td>", $run->mr_count, "</td>\n";
   echo " </tr>\n";
 }
echo "</table>\n";
if($tot > $lim) { pagenate("list.php", $page, $tot, $lim, ""); }
?>
</div> <!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
 