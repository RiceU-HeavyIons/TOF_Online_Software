<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$tray_id      = $_POST['id'];
$type         = $_POST['request_type'];

$new_status   = $_POST['stat_drop'];
$ship_date    = $_POST['date'];
$notes        = $_POST['text'];
$note_type    = $_POST['texttype'];
$old_notes    = $_POST['old_notes'];
$user         = $_POST['user'];
$start_date   = $_POST['start_date'];
$tcpu         = $_POST['tcpu_select'];
$ttrig        = $_POST['ttrig_select'];
$tdig         = $_POST['tdig_select'];
$mino         = $_POST['mino_select'];
$module       = $_POST['module_select'];
$position     = $_POST['position'];
$backleg      = $_POST['backleg'];
$slot         = $_POST['slot'];
$comp_type    = $_POST['type'];
$arrival_date = $_POST['arrival_date'];
$serial       = $_POST['serial'];
$data_name    = $_POST['data_name'];
$value        = $_POST['new_value'];
$comp_id      = $_POST['comp_id'];

Session::check_login("../mtd_tray/inner.php?id=$tray_id");

if ($type == 'new_col'){
  $datetime = date(" Y-m-d H:i:s ");
  $data_name=addslashes($data_name);
  $insert = mysql_query("insert into tray_info values(default,'$tray_id','$data_name','$value', '$datetime')", $con) or die(mysql_error());
  header( "Location: ./info.php?id=$tray_id");
}

if ($type == 'status'){
  $insert = mysql_query("update trays set status='$new_status' where id='$tray_id' ", $con) or die (mysql_error());
  header( 'Location: ./list.php');
}

if ($type == 'tray_status'){
  $insert = mysql_query("update trays set tray_status_id='$new_status' where id='$tray_id' ", $con) or die (mysql_error());
  header( 'Location: ./list.php');
}

if ($type == 'cancel_shipment'){
  $insert = mysql_query("update trays set ship=default where id='$tray_id'", $con) or die (mysql_error());
#    $insert = mysql_query("update trays set status='storage' where id='$tray_id'", $con) or die (mysql_error());
  header( 'Location: ./list.php');
}
  
if ($type == 'new_ship_date'){
  $insert = mysql_query("update trays set ship='$ship_date' where id='$tray_id'", $con) or die (mysql_error());
#    $insert = mysql_query("update trays set status='shipped' where id='$tray_id'", $con) or die (mysql_error());
  header( 'Location: ./list.php');
}

if ($type == 'append_text'){
  $date = date(" Y-m-d ");
  $result1 = mysql_query("select notes from trays where id ='$tray_id'", $con);
  $old_note = mysql_fetch_object($result1);
  $return = '<br>';
  $red_start = '<font color=#FF0000><b>';
  $red_end = '<font color=#000000></b>';
  $blue_start = '<font color=#0000FF><b>';
  $blue_end = '<font color=#000000></b>';
  if(strlen($notes)!=0){
    if($note_type=='1'){$notes=$red_start.$notes.$red_end;}
    if($note_type=='2'){$notes=$blue_start.$notes.$blue_end;}
    if(strlen($old_note->notes) == 0){$full_notes=addslashes($date.$notes.$return);}
    else{$full_notes=addslashes($date.$notes.$return.$old_note->notes);}
    $insert = mysql_query("update trays set notes='$full_notes' where id='$tray_id'", $con) or die (mysql_error());
    header( "Location: ./notes.php");}
  else{header( "Location: ./notes.php");}
}

if ($type == 'new_tray') {
  if($start_date=='' || $start_date=='YYYY-MM-DD'){$start_date=date(" Y-m-d ");}
  $result1 = mysql_query("select count(*) as count from trays where sn > 199", $con);
  $r1 = mysql_fetch_object($result1);
  $result2 = mysql_query("select id from users where name= '$user' ", $con);
  $r2 = mysql_fetch_object($result2);
  $sn = $serial;
  if($sn=='') $sn = $r1->count + 201;
#$id = $r1->count+1;
  $user_id = $r2->id;
  $insert = mysql_query("insert into trays (user_id, tcpu_id, start, ship, sn, tray_status_id, notes) values ('$user_id', default, '$start_date', default, '$sn', '$new_status', '$notes')", $con) or die(mysql_error()); 
  header( "Location: ./inner.php?sn=$sn ");
}


if ($type == 'modify_tcpu'){
  $tcpu=trim($tcpu, "*");
  if($tcpu=='Remove') {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='1' and tray_id=$tray_id", $con) or die(mysql_error());
  }
  elseif($tcpu=='----------' || $tcpu=='Pick one') {
    header( "Location: ./edit.php?id=$tray_id");
  }
  else {
    $update = mysql_query("update components set tray_id=$tray_id, slot=default where serial='$tcpu' and component_type_id='1'", $con) or die(mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

if ($type == 'modify_ttrig') {
  $ttrig=trim($ttrig, "*");
  if($ttrig=='Remove') {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='5' and tray_id=$tray_id", $con) or die(mysql_error());
  }
  elseif($tcpu=='----------' || $tcpu=='Pick one') {
    header( "Location: ./edit.php?id=$tray_id");
  }
  else {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='5' and tray_id=$tray_id", $con) or die(mysql_error());
      $update = mysql_query("update components set tray_id=$tray_id, slot=default where serial='$ttrig' and component_type_id='5'", $con) or die(mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

if ($type == 'modify_tdig') {
  $tdig=trim($tdig, "*");
  if($tdig=='Remove') {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='2' and tray_id=$tray_id", $con) or die(mysql_error());
  }
  elseif($tdig=='----------' || $tdig=='Pick one') {
    header( "Location: ./edit.php?id=$tray_id");
  }
  else {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='2' and tray_id=$tray_id", $con) or die(mysql_error());
    $update = mysql_query("update components set tray_id=$tray_id, slot=default where serial='$tdig' and component_type_id='2'", $con) or die(mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}
  
if ($type == 'modify_mino'){
  $mino=trim($mino, "*");
  if($mino=='Remove') {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='7' and tray_id=$tray_id", $con) or die(mysql_error());
  }
  elseif($mino=='----------' || $mino=='Pick one') {
    header( "Location: ./edit.php?id=$tray_id");
  }
  else {
    $edit = mysql_query("update components set tray_id=default, slot=default where component_type_id='7' and tray_id=$tray_id", $con) or die(mysql_error());
    $update = mysql_query("update components set tray_id=$tray_id, slot=default where serial='$mino' and component_type_id='7'", $con) or die(mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

if ($type == 'modify_module') {
  $module=trim($module, "*");
  if($module=='Remove') {
    $edit = mysql_query("update modules set tray_id=default, slot=default where tray_id=$tray_id", $con) or die(mysql_error());}
  elseif($module=='----------' || $module=='Pick one') {
    header( "Location: ./edit.php?id=$tray_id");
  }
  else {
    $edit = mysql_query("update modules set tray_id=default, slot=default where tray_id=$tray_id", $con) or die(mysql_error());
    $update = mysql_query("update modules set tray_id=$tray_id, slot=default where serial='$module'", $con) or die(mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

if ($type == 'modify_backleg') {
  $countr=mysql_query("select count(*) as count from tray_backlegs where tray_id='$tray_id'", $con);
  $c1=mysql_fetch_object($countr);
  $count=$c1->count;
  if($count == 0) {
    $insert = mysql_query("insert into tray_backlegs values('$tray_id','$backleg')", $con) or die(mysql_error());
  }
  else {
    $insert = mysql_query("update tray_backlegs set backleg='$backleg' where tray_id='$tray_id' ", $con) or die (mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

if ($type == 'modify_slot') {
  $tdigr=mysql_query("select * from components where tray_id=$tray_id and component_type_id=2", $con);
  if($c1=mysql_fetch_object($tdigr)) {
    $comp_id=$c1->id;
    $insert = mysql_query("update components set slot='$slot' where id='$comp_id' ", $con) or die (mysql_error());
  }
  header( "Location: ./edit.php?id=$tray_id");
}

?>
