<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$type         = $_POST['request_type'];
$tray_0_sn      = $_POST['tray_0_sn'];//MC
$tray_1_sn      = $_POST['tray_1_sn'];//MC
$tray_2_sn      = $_POST['tray_2_sn'];//MC
$tray_4_sn      = $_POST['tray_4_sn'];//MC
$tray_5_sn      = $_POST['tray_5_sn'];//MC
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


  $result2 = mysql_query("select id from users where name= '$user' ", $con);
  $r2 = mysql_fetch_object($result2);
  $user_id = $r2->id;

  $result3 = mysql_query("select id from run_status where name= '$run_status' ", $con);
  $r3 = mysql_fetch_object($result3);
  $run_status_id = $r3->id;

  $data_uri = "file://sagitta/home/data/mtdData/noisetests/" . $data_uri;


	//MC
  function build_query($input_tray_sn,$tray_no_string){
  	global $run_no;
  	global $run_status_id;
  	global $user_id;
  	global $start_date;
  	global $end_date;
  	global $event_no;
  	global $data_uri;
  	global $notes;
	global $con;
	
	if(isset($results_uri)){
		$results_uri = "http://www.rhip.utexas.edu:/~tofp/mtdData/noisetests/" . $results_uri;
		$results_uri .= "{$tray_no_string}.pdf";
	}
	else{
		$results_uri = "http://www.rhip.utexas.edu:/~tofp/mtdData/noisetests/";
		$results_uri .= "{$run_no}/results{$tray_no_string}.pdf";
	}
	
  	if(!isset($input_tray_sn)) return NULL;
	else{
	  	$result1 = mysql_query("select id from trays where sn= '$input_tray_sn' ", $con);
		$r1 = mysql_fetch_object($result1);
		$input_tray_id = $r1->id;
	  
	  	$query = "INSERT INTO mtd_runs (id, tray_id, run_status_id, run_type_id, user_id, start, stop, ";
		$query .= "run_time, events, data_uri, results_uri, note, param1, param2) VALUES (";
		$query .= "'$run_no', '$input_tray_id', '$run_status_id', 4, '$user_id', '$start_date', '$end_date', ";
		$query .= "default, '$event_no', '$data_uri', '$results_uri', '$notes', 6300, 2500 )";
		return $query;
		}
	}
  
	//MC
	$query_tray_0 = build_query($tray_0_sn,"0");
	$query_tray_1 = build_query($tray_1_sn,"1");
	$query_tray_2 = build_query($tray_2_sn,"2");
	$query_tray_4 = build_query($tray_4_sn,"4");
	$query_tray_5 = build_query($tray_5_sn,"5");
		
	//MC
  	if($query_tray_0) $insert_tray_0 = mysql_query($query_tray_0, $con) or die(mysql_error());
  	if($query_tray_1) $insert_tray_1 = mysql_query($query_tray_1, $con) or die(mysql_error());
  	if($query_tray_2) $insert_tray_2 = mysql_query($query_tray_2, $con) or die(mysql_error());
  	if($query_tray_4) $insert_tray_4 = mysql_query($query_tray_4, $con) or die(mysql_error());
  	if($query_tray_5) $insert_tray_5 = mysql_query($query_tray_5, $con) or die(mysql_error());
  
  	header( "Location: ./create_noiserun.php");
  
  
  
  //MC
}



?>
