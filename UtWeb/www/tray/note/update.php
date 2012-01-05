<?php
include("../../lib/connect.php");
include("../../lib/session.php");

# print_r($_POST);

$tray_id      = $_POST['id'];
$action       = $_POST['action'];
$note         = $_POST['note'];
$note_type    = $_POST['note_type'];
$user_id      = $_POST['user_id'];

Session::check_login("../tray/notes.php?id=$tray_id");

if ($action == 'insert'){
  $date = date("Y-m-d");
  $red_start  = '<font color=#FF0000><b>';
  $red_end    = '<font color=#000000></b>';
  $blue_start = '<font color=#0000FF><b>';
  $blue_end   = '<font color=#000000></b>';

  if (strlen($note) > 0) {
    if($note_type=='1'){ $notes = $red_start.$notes.$red_end; }
    if($note_type=='2'){ $notes = $blue_start.$notes.$blue_end; }

    $stm = "INSERT INTO tray_notes (";
    $stm .= join(",", array('user_id', 'created_on', 'tray_id', 'note'));
    $stm .= ") VALUES (";
    $stm .= join(",", array($user_id, "'$date'", $tray_id, "'$note'"));
    $stm .= ")";
    $insert = mysql_query($stm) or die (mysql_error());
    header( "Location: ./notes.php?id=$tray_id");
  } else {
     header( "Location: ./notes.php");
  }
} else if ($action == 'update_sticky') {
   $sticky = $_POST['sticky'];
   $tray_id = $_POST['tray_id'];
   $stm = "UPDATE trays set notes='$sticky' where id=$tray_id";
   $update = mysql_query($stm) or die (mysql_error());
   header( "Location: ./notes.php?id=$tray_id");
}
?>
