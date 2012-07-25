<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$type         = $_POST['request_type'];
$ship_date    = $_POST['date'];
$notes        = $_POST['text'];
$user         = $_POST['user'];
$start_date   = $_POST['start_date'];
$slot         = $_POST['slot'];
$comp_type    = $_POST['type'];
$arrival_date = $_POST['arrival_date'];
$serial       = $_POST['serial'];
$data_name    = $_POST['data_name'];
$comp_id      = $_POST['comp_id'];
$chip_sn      = $_POST['chip_sn'];

Session::check_login("../component/edit.php?id=$comp_id");

if ($type == 'edit_component'){
    if($arrival_date==''|| $arrival_date=='Unknown'){$arrival_date=date(" Y-m-d ");}
    if($comp_type == 'TCPU')     {$comp_type='1';}
    elseif($comp_type == 'TDIG') {$comp_type='2';}
    elseif($comp_type == 'TTRIG'){$comp_type='5';}
#    else                         {$comp_type='3';}
    
    elseif($comp_type =='TINO') {$comp_type='3';}
    else {$comp_type='6';}


#    $date = date(" Y-m-d ");
#    $full_notes=addslashes($notes.$date);
     $full_notes=addslashes($notes);
    $insert = mysql_query("update components set component_type_id='$comp_type', received_on='$arrival_date', notes=\"$full_notes\", chip_sn='$chip_sn' where id='$comp_id'", $con) or die(mysql_error());
    header( "Location: ../component/edit.php?id=$comp_id");

}

if ($type == 'new_component') {
  if($arrival_date=='' || $arrival_date=='YYYY-MM-DD'){$arrival_date=date(" Y-m-d ");}
  if($comp_type == 'TCPU')     {$comp_type='1';}
  elseif($comp_type == 'TDIG') {$comp_type='2';}
  elseif($comp_type == 'TINO') {$comp_type='3';}
  elseif($comp_type == 'MINO') {$comp_type='7';}
  elseif($comp_type == 'MTRIG') {$comp_type='8';}
  if($notes!=''){
#    $date = date(" Y-m-d ");
#    $full_notes=addslashes($date.$notes);
    $full_notes=addslashes($notes);
  }
  $comparray = explode(",",$serial);
  foreach($comparray as $compserial){
    $compserial=trim($compserial);
    if(strchr($compserial, '-')==false) {
        $insert = mysql_query("insert into components values ('default', '$compserial', '$comp_type', default, default, '$arrival_date', \"$full_notes\", '$chi_sn')", $con) or die(mysql_error());
    } elseif(strchr($compserial, '-') !== false) {
      $serialblock = explode('-', $compserial, 2);

      $serialstart = $serialblock[0];
      $serialstart = trim($serialstart);
      $serialstart = (integer) $serialstart;

      $serialstop = $serialblock[1];
      $serialstop = trim($serialstop);
      $serialstop = (integer) $serialstop;

      if($serialstop<$serialstart){
          $temp=$serialstop;
          $serialstop=$serialstart;
          $serialstart=$temp;
      }

      for($insert_serial = $serialstart; $insert_serial <= $serialstop; $insert_serial++){
        $insert = mysql_query("insert into components values ('default', '$insert_serial', '$comp_type', default, default, '$arrival_date', '$full_notes', '$chi_sn')", $con) or die(mysql_error());
      }
    }
    header( "Location: ../component/create.php");
  }
}
?>






