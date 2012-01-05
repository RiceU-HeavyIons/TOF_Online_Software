<?php
# $Id: update.php,v 1.5 2007/10/11 21:05:20 drew Exp $
include('../../lib/connect.php');
include('../../lib/session.php');
Session::check_login('../hvtest/update.php');

$type      = $_POST['request_type'];

$serial    = $_POST['serial'];
$data_name = $_POST['data_name'];
$umlist    = $_POST['um'];
$tmlist    = $_POST['tm'];
$pfum      = $_POST['pfum'];
$pftm      = $_POST['pftm'];
$mod_id    = $_POST['modid'];

switch($type) {

case 'hvupdate':
  $date = date(" Y-m-d ");    
  if($tmlist !='') {
    if(eregi('[a-z]', $tmlist)){header("Location: ./no.php");}
    $tmarray = explode(",",$tmlist);
    if($pftm=='1') {
      foreach($tmarray as $tmodule){
        $find=mysql_query("select id from modules where substring(serial,3,7)=$tmodule and vendor_id=2", $con) or die(mysql_error());
        $f=mysql_fetch_object($find);
	$mid=$f->id;
	$count=mysql_query("select count(*) as count from hvtest where id=$mid", $con) or die(mysql_error());
	$c=mysql_fetch_object($count);
	$dup=$c->count;
	if($dup=='0'){
	  $insert = mysql_query("insert into hvtest values ('$mid', '$pftm', '$date')" , $con) or die(mysql_error());
	}
      }
    } else {
      foreach($tmarray as $tmodule) {
	$find=mysql_query("select id from modules where substring(serial,3,7)=$tmodule and vendor_id=2", $con) or die(mysql_error());
	$f=mysql_fetch_object($find);
	$mid=$f->id;
	$count=mysql_query("select count(*) as count from hvtest where id=$mid", $con) or die(mysql_error());
	$c=mysql_fetch_object($count);
	$dup=$c->count;
	if($dup=='0'){
	$insert = mysql_query("insert into hvtest values ('$mid', '$pftm', '$date')" , $con) or die(mysql_error());
	}
      }
    }
  }
  
  if($umlist !=''){
    if(eregi('[a-z]', $umlist)){header("Location: ./no.php");}
    $umarray = explode(",",$umlist);
    if($pfum=='0') {
      foreach($umarray as $umodule) {
	$find=mysql_query("select id from modules where substring(serial,3,7)=$umodule and vendor_id=1", $con) or die(mysql_error());
        $f=mysql_fetch_object($find);
        $mid=$f->id;
	$count=mysql_query("select count(*) as count from hvtest where id=$mid", $con) or die(mysql_error());
	$c=mysql_fetch_object($count);
	$dup=$c->count;
	if($dup=='0'){
	  $insert = mysql_query("insert into hvtest values ('$mid', '$pfum', '$date')" , $con) or die(mysql_error());
	}
      }
    } else {
      foreach($umarray as $umodule){
        $find=mysql_query("select id from modules where substring(serial,3,7)=$umodule and vendor_id=1", $con) or die(mysql_error());
        $f=mysql_fetch_object($find);
        $mid=$f->id;
	$count=mysql_query("select count(*) as count from hvtest where id=$mid", $con) or die(mysql_error());
	$c=mysql_fetch_object($count);
	$dup=$c->count;
	if($dup=='0'){
	  $insert = mysql_query("insert into hvtest values ('$mid', '$pfum', '$date')" , $con) or die(mysql_error());
	}
      }
    }
  }
  header("Location: ./notes.php");
  break;
  

case 'pass_fail_switch':
  // $get_mod_id=mysql_query("select * from modules where serial=$serial", $con) or die(mysql_error());
  // $mod_id=mysql_fetch_object($get_mod_id);
  // $mid=$mod_id->id;
  $check_pass_value=mysql_query("select * from hvtest where id=$mod_id", $con) or die(mysql_error());
  $check=mysql_fetch_object($check_pass_value);
  $pass=$check->pass;
  // $change=mysql_query("update hvtest set pass='0' where id='148'", $con);
  if($pass=='1') {
    $change=mysql_query("update hvtest set pass='0' where id=$mod_id", $con) or die(mysql_error());
  } elseif($pass=='0') {
    $change=mysql_query("update hvtest set pass='1' where id=$mod_id", $con) or die(mysql_error());
  }
  header("Location: ./list.php");
  break;


case 'drop_value':
  $drop=mysql_query("delete from hvtest where id=$mod_id", $con) or die(mysql_error());
  header("Location: ./list.php");
  break;
}
?>
