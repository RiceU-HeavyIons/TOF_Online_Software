<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$type           = $_POST['request_type'];
$user           = $_POST['user'];
$vendor         = $_POST['vendor'];
$received_date  = $_POST['received_date'];
$notes          = $_POST['text'];
$chinese_url    = $_POST['chinese_url'];
$serial         = $_POST['serial'];
$chinese_serial = $_POST['chinese_serial'];

Session::check_login("../module/create.php");


if ($type == 'new_module') {


  if($received_date=='' || $received_date=='YYYY-MM-DD') {
    $arrival_date=date(" Y-m-d ");
  }

  $result1 = mysql_query("select id from users where name= '$user' ", $con);
  $r1 = mysql_fetch_object($result1);
  $user_id = $r1->id;

  $result2 = mysql_query("select id from vendors where name= '$vendor' ", $con);
  $r2 = mysql_fetch_object($result2);
  $vendor_id = $r2->id;



  if($notes!=''){
    $full_notes=addslashes($notes);
  }
  else {
    $full_notes = $notes;
  }

  $insert = mysql_query("insert into modules (serial, received_on, user_id, vendor_id, chinese_id, chinese_link, note) values ('$serial', '$received_date', '$user_id', '$vendor_id', '$chinese_serial', '$chinese_url', '$notes')", $con) or die(mysql_error()); 
  header( "Location: ../module/create.php");
}
?>






