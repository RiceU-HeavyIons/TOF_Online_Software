<?php
# $Id: check.php,v 1.11 2008/09/25 19:53:25 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");

set_time_limit(120);

function scrimp($ary, $pfx, $fmt) {

  if (sizeof($ary) == 0) return "";
  $ar1= array();
  $prv = $ary[0];
  foreach($ary as $i) {
    if($prv + 1 == $i) {
      $cur = array_pop($ar1);
      array_push($cur, $i);
      array_push($ar1, $cur);
    }
    else
      array_push($ar1, array($i));

    $prv = $i;
  }

  $ar2 = array();
  foreach($ar1 as $i) {
    $n = count($i);
    if($n == 1)
      array_push($ar2, $pfx . sprintf($fmt, $i[0]));
    else {
      $str = $pfx;
      $str .= sprintf($fmt, $i[0]);
      $str .= "-";
	  $str .= sprintf($fmt, $i[$n-1]);
	  $str .= sprintf(" (%d)", $n);
      array_push($ar2, $str);
    }
  }
  return join(", ", $ar2);
}

function scan_db($first, $last, $pfx, $con) {
  $expect = $last - $first + 1;
  print "<div>Expected: " . $expect . " modules</div>\n";

  $notinsed = array();
  $found   = array();
  $missing = array();
  $notchid = array();
  for($i = $first; $i <= $last; $i += 1) {
    $sn = sprintf($pfx . "%04d", $i);
    $m = Module::find_by_sn($sn, $con);
    if( is_null($m) ) { array_push($missing, $i); }
    else {
  	   if($m->chinese_id <= 0) array_push($notchid, $i);
  	   if($m->tray->id <= 0) array_push($notinsed, $i);
  	   array_push($found, $i);
    }
  }
  print "<div>Found: " . sizeof($found) . " modules: ";
  print scrimp($found, $pfx, "%04d") . "</div>\n";
  print "<div>Missing: " . sizeof($missing) . " modules: ";
  print scrimp($missing, $pfx, "%04d") . "</div>\n";
  print "<div>Not Linked to USTC DB: " . sizeof($notchid) . " modules: ";
  print scrimp($notchid, $pfx, "%04d") . "</div>\n";
  print "<div>Not Linked to Tray: " . sizeof($notinsed) . " modules: ";
  print scrimp($notinsed, $pfx, "%04d") . "</div>\n";
}

tsw_header("Module Check");
tsw_toolbar('module');
?>
<div id="main">
<div class="title">Module Check</div>

<div class="section">
<div class="title">Overall</div>
<?php
 print "<div>Count: " . Module::count("", $con) . "</div>\n";
 $m = Module::first($con);
 $first = intval($m->id);
 print "<div>First Id: " . $m->id . "</div>\n";
 $m = Module::last($con);
 print "<div>Last Id: " . $m->id . "</div>\n";
 $last = intval($m->id);
 $missing = array();
 $installed = 0;
 $notinstalled = 0;
 $notchid = 0;
 for($i=$first; $i <= $last; $i++) {
   $m = new Module($i, $con);
   if($m->id <= 0) array_push($missing, $i);
   if($m->tray->id)
     $installed++;
   else
     $notinstalled++;
   if($m->chinese_id <= 0) $notchid++;
 }
 print "<div>Missing: " . sizeof($missing) . " modules: \n";
 print scrimp($missing, "", "%d") . "</div>\n";
 print "<div>Not Linked to USTC DB: " . $notchid . " modules</div>\n";
 print "<div>Installed: " . $installed . " modules</div>\n";
 print "<div>Not Installed: " . $notinstalled . " modules</div>\n";
 ?>
</div> <!-- section -->

<div class="section">
<div class="title">UM modules</div>
<?php
$vend = "vendor_id = 1";
print "<div>Count: " . Module::count($vend, $con) . "</div>\n";

$m = Module::find_one($vend, "serial asc", $con);
print "<div>First: {$m->serial}</div>\n";
$first = intval( ereg_replace("UM([0-9]+)", "\\1", $m->serial) );

$m = Module::find_one($vend, "serial desc", $con);
print "<div>Last: {$m->serial}</div>\n";
$last = intval( ereg_replace("UM([0-9]+)", "\\1", $m->serial) );
scan_db($first, $last, "UM", $con);
?>
</div> <!-- section -->

<div class="section">
<div class="title">TM modules</div>
<?php
$vend = "vendor_id = 2";
print "<div>Count: " . Module::count($vend, $con) . "</div>\n";

$vend = "vendor_id = 2 AND serial like 'TM060____'";
print "<div>Count (TM060xxxx): " . Module::count($vend, $con) . "</div>\n";

$vend = "vendor_id = 2 AND serial like 'TM____'";
print "<div>Count (TMxxxx): " . Module::count($vend, $con) . "</div>\n";
$m = Module::find_one($vend, "serial asc", $con);
print "<div>First: {$m->serial}</div>\n";
$first = intval( ereg_replace("TM([0-9]+)", "\\1", $m->serial) );

$m = Module::find_one($vend, "serial desc", $con);
print "<div>Last: {$m->serial}</div>\n";
$last = intval( ereg_replace("TM([0-9]+)", "\\1", $m->serial) );

scan_db($first, $last, "TM", $con);
?>
</div> <!-- section -->

<div class="section">
<div class="title">List by Date</div>
<div><table border="1">
	<tr>
		<th>received</th>
		<th>UM</th>
		<th>TM</th>
		<th>total</th>
		<th>int(UM)</th>
		<th>int(TM)</th>
		<th>int(total)</th>
	</tr>
<?php
	$ium = 0;
	$itm = 0;
	$itot = 0;
	$q = mysql_query("select received_on from modules group by received_on");
	while($r = mysql_fetch_object($q)) {
		$um = Module::count("vendor_id = 1 and received_on = '{$r->received_on}'");
		$tm = Module::count("vendor_id = 2 and received_on = '{$r->received_on}'");
		$tot = $um + $tm;

		$ium += $um;
		$itm += $tm;
		$itot += $tot;

		print "<tr>\n";
		print " <td>" . $r->received_on . "</td>\n";
		print " <td class='int'>" . $um . "</td>\n";
		print " <td class='int'>" . $tm . "</td>\n";
		print " <td class='int'>" . $tot . "</td>\n";
		print " <td class='int'>" . $ium . "</td>\n";
		print " <td class='int'>" . $itm . "</td>\n";
		print " <td class='int'>" . $itot . "</td>\n";
		print "</tr>\n";
	}
?>
</div></table>
</div>
<!-- section --></div>

<div class="section">
<div class="title">Chinese Results</div>
<?php
print "<div>Count : "                     . ChineseResult::count("") . "</div>\n";
print "<div>Count (Timing Resolution): "  . ChineseResult::count("result_type = 1") . "</div>\n";
print "<div>Count (Noise Rate): "         . ChineseResult::count("result_type = 2") . "</div>\n";
print "<div>Count (Small Signal Ratio): " . ChineseResult::count("result_type = 3") . "</div>\n";
print "<div>Count (Start Time): "         . ChineseResult::count("result_type = 4") . "</div>\n";
print "<div>Count (Unique SN): " . ChineseResult::count_unique_serial() . "</div>\n";
print "<div>Duplication: "  . ChineseResult::p_duplication() . "</div>\n";
?>
<div>
</div> <!-- section -->
<!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
