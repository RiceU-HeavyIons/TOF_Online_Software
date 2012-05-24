<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$type         = $_POST['request_type'];
$tray_sn      = $_POST['tray_sn'];
$user         = $_POST['user'];
$run_status   = $_POST['run_status'];
$run_no       = $_POST['run_no'];
$event_no     = $_POST['event_no'];
$start_date   = $_POST['start_date'];
$end_date     = $_POST['end_date'];
$data_uri     = $_POST['data_uri'];
$results_uri  = $_POST['results_uri'];
$notes        = $_POST['text'];

Session::check_login("../mtd_tray/new_run.php");

if ($type == 'new_run') {
  if($start_date=='' || $start_date=='YYYY-MM-DD HH:MM:SS'){
    $start_date=date(" Y-m-d H:i:s");
  }
  if($end_date=='' || $end_date=='YYYY-MM-DD HH:MM:SS'){
    $end_date=date(" Y-m-d H:i:s");
  }
  $result1 = mysql_query("select id from trays where sn= '$tray_sn' ", $con);
  $r1 = mysql_fetch_object($result1);
  $tray_id = $r1->id;

  $result2 = mysql_query("select id from users where name= '$user' ", $con);
  $r2 = mysql_fetch_object($result2);
  $user_id = $r2->id;

  $result3 = mysql_query("select id from run_status where name= '$run_status' ", $con);
  $r3 = mysql_fetch_object($result3);
  $run_status_id = $r3->id;


  $sql = "INSERT INTO mtd_runs (id, tray_id, run_status_id, run_type_id, user_id, start, stop, ";
  $sql .= "run_time, events, data_uri, results_uri, note, param1, param2) VALUES (";
  $sql .= "'$run_no', '$tray_id', '$run_status_id', 4, '$user_id', '$start_date', '$end_date', ";
  $sql .= "default, '$event_no', '$data_uri', '$results_uri', '$notes', 6300, 2500 )";

  $insert = mysql_query($sql, $con) or die(mysql_error()); 
  header( "Location: ./create_noiserun.php");
}



?>
