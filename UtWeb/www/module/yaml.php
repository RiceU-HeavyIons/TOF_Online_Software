<?php
# $Id: yaml.php,v 1.5 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto.
include("../../lib/connect.php");
include('../../lib/session.php');

header('Content-type: text/plain');
echo 'Generator: "$Id: yaml.php,v 1.5 2008/05/29 19:28:14 tofp Exp $"' . "\n";
echo 'TimeStamp: ' . date("Y-m-d H:i:s", time()) . "\n";
function list_result($con, $id, $type)
{

  $result3 = mysql_query(
			 "SELECT * from module_results where module_id=$id",
			 $con
			 );
  $l = 0;
  while($r = mysql_fetch_array($result3)) {
    $rid = $r["run_id"];
    $result4 = mysql_query("SELECT * from runs where id=$rid", $con);
    $r4 = mysql_fetch_object($result4);
    $cid = $r4->run_config_id;
    $result5 = mysql_query("SELECT * from run_configs where id=$cid", $con);
    $r5 = mysql_fetch_object($result5);

    if($r4->run_type_id == $type){
    	echo "    ${rid}:\n" ;
    	for($p = 1; $p < 7; $p++) {	printf("      - %f\n", $r["pad$p"]); }
  	}
  }
};

if($rvar_id != NULL) {
  $result1 = mysql_query("SELECT * from modules where id=$rvar_id", $con);
} else if($rvar_sn !=NULL) {
  $sn = ucwords($rvar_sn);
  $sn = preg_replace('/\W*(\w+)\W*/', '$1', $sn);
  $result1 = mysql_query("SELECT * from modules where serial='$sn'", $con);
}
$r1 = mysql_fetch_object($result1);
$rid = $r1->id;
?>
Module:
  Id: <?php echo $rid ?>

  ChineseId: <?php echo $r1->chinese_id ?>

  SerialNumber: <?php echo $r1->serial ?>
 
  ReceivedAt: <?php echo $r1->received_on ?>

  Dimension:
<?php
$result2 = mysql_query(
  "SELECT * from module_sizes where module_id=$rid",
  $con);
while($r2 = mysql_fetch_object($result2)) {
  $tag = "    ";
  echo $tag . "$r2->id:\n";
  $tag .= "  - ";
  printf("%s%f\n", $tag, $r2->length1);
  printf("%s%f\n", $tag, $r2->length2);
  printf("%s%f\n", $tag, $r2->width1);
  printf("%s%f\n", $tag, $r2->width2);
  printf("%s%f\n", $tag, $r2->thickness1);
  printf("%s%f\n", $tag, $r2->thickness2);
  printf("%s%f\n", $tag, $r2->jig_passed);
}
?>
  TimingResolution:
<?php list_result($con, $rid, 2); ?>
  NoiseRate:
<?php list_result($con, $rid, 3); ?>
<?php mysql_close($con) ?>
