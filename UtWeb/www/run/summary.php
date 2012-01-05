<?php
# $Id: summary.php,v 1.4 2008/05/29 19:28:15 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header("Run Summary");
tsw_toolbar("run");
?>
<div id="main">
<div class="title">Run Summary</div>

<div class="section">
<div class="title">Month</div>
<div>
<table>
<tr>
 <th>month</th>
 <th>count</th>
 <th>time</th>
</tr>
<?php
$l=0;
$cnt = 0;
$tim = 0;
foreach(Run::month_summary() as $itm) {
  $l++; $l %= 2;
  $cnt += $itm["count"];
  $tim += $itm["time"];
  echo "<tr class='row{$l}'>\n";
  echo " <td>", $itm["month"], "</td>\n";
  echo " <td class='int'>", $itm["count"], "</td>\n";
  echo " <td class='int'>", intval($itm["time"]/3600), " hr</td>\n";
  echo "</tr>\n";
}
echo "<tr>\n";
echo " <td>total</td>\n";
echo " <td class='int'>", $cnt, "</td>\n";
echo " <td class='int'>", intval($tim/3600), " hr</td>\n";
echo "</tr>\n";
?>
</table>
</div>
</div><!-- section -->

<div class="section">
<div class="title">User</div>
<div>
<table>
<tr>
 <th>user</th>
 <th>count</th>
 <th>time</th>
 <th>last</th>
</tr>
<?php
$l=0;
$cnt = 0;
$tim = 0;
foreach(Run::user_summary() as $itm) {
  $l++; $l %= 2;
  $cnt += $itm["count"];
  $tim += $itm["time"];
  echo "<tr class='row{$l}'>\n";
  echo " <td>", $itm["user"]->name, "</td>\n";
  echo " <td class='int'>", $itm["count"], "</td>\n";
  echo " <td class='int'>", intval($itm["time"]/3600), " hr</td>\n";
  echo " <td class='int'>", $itm["last"], "</td>\n";
  echo "</tr>\n";
}
echo "<tr>\n";
echo " <td>total</td>\n";
echo " <td class='int'>", $cnt, "</td>\n";
echo " <td class='int'>", intval($tim/3600), " hr</td>\n";
echo "</tr>\n";
?>
</table>
</div>
</div><!-- section -->

</div><!-- main -->
<?php
tsw_footer();
?>
