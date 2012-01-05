<?php
# $Id: status.php,v 1.3 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<!-- $Id: status.php,v 1.3 2007/08/21 11:18:11 tofp Exp $ -->
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<?php
import_request_variables("gP", "rvar_");
$con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
mysql_select_db("TOF", $con);
$title = "Gas Status";
?>

<title><?php echo $title ?></title>
<link rel="stylesheet" type="text/css" href="../../style/default.css" />
<style type="text/css">
td.val { background: #eee; }
</style>
</head>
<body>
<div id="main">
<?php include("../../lib/toolbar.php"); toolbar('system'); ?>
<div class="title"><?php echo $title ?></div>

<table>
 <tr>
  <th>Time</th>
  <th>Freon</th>
  <th>Isobutane</th>
  <th>SF6</th>
 </tr>
<?php
 for($j=0; $j < 15; $j++) {
  print "<tr>\n";
  for($i = 1; $i <= 3; $i++) {
    $result1 = mysql_query("SELECT * from gas_readings where gas_type_id = $i order by datetime desc limit $j, 1", $con);
    $r1 = mysql_fetch_object($result1);
    if($i == 1) print "<td>$r1->datetime</td>\n";
    print "<td class=\"val\">$r1->value sccm</td>\n";
  }
  print "</tr>\n";
 }
?>
</table>
</div> <!-- id=main -->
</body>
</html>
<?php mysql_close($con) ?>
