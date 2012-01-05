<?php
# $Id: edit.php,v 1.6 2008/04/21 18:57:01 drew Exp $
$ser=$_POST['ser'];
$ser=$_GET['ser'];
include("../../lib/connect.php");
include("../../lib/session.php");
Session::check_login("../hvtest/edit.php?ser={$ser}");

function failed($ser, $mid){
  echo "<form name='switch_pass_fail_value' action='update.php' method='post'/>";
  echo "<input name='serial' type='hidden', value='$ser'/>";
  echo "<input name='modid' type='hidden', value='$mid'/>";
  echo "<input name='request_type' type='hidden', value='pass_fail_switch'/>";
  echo "<input name='new_text' type='submit' value='This module actually failed' /></form>\n";
}

function passed($ser, $mid){
  echo "<form name='switch_pass_fail_value' action='update.php' method='post'/>";
  echo "<input name='serial' type='hidden', value='$ser'/>";
  echo "<input name='modid' type='hidden', value='$mid'/>";
  echo "<input name='request_type' type='hidden', value='pass_fail_switch'/>";
  echo "<input name='new_text' type='submit' value='This module actually passed' /></form>\n";
}

function nottested($ser, $mid){
  echo "<form name='drop_value' action='update.php' method='post'/>";
  echo "<input name='serial' type='hidden', value='$ser'/>";
  echo "<input name='modid' type='hidden', value='$mid'/>";
  echo "<input name='request_type' type='hidden', value='drop_value'/>";
  echo "<input name='new_text' type='submit' value='This module has not been tested' /></form>\n";
}

include("../../lib/template.php");
tsw_header("HV Test Results Module Editing Page");
tsw_toolbar('hvtest');
?>
<div id="main">
<div class="title">HV Test Results Module Editing Page</div>
<div><a href="./notes.php">Back to the module entry page</a></div>
<div><a href="./datelist.php">Back to the list of dates</a></div>
<br />
<?php
  $module=mysql_query("select m.serial as serial, h.pass as pass, m.id as id, h.date as date from modules m inner join hvtest h on m.id=h.id where m.serial='$ser'", $con);
  $m=mysql_fetch_object($module);
  $pass=$m->pass;
  $mid=$m->id;
  $date=$m->date;
  echo "<a href=\"./dlist.php?d=$date\">Other modules tested on this date</a><br><br>";
  if ($pass=='1'){
    echo $ser, " Passed<br><br>";
    echo "If this module was mistakenly entered as 'passed', click here:";
    echo failed($ser, $mid);
  } else {
    echo $ser, " Failed<br><br>";
    echo "If this module was mistakenly entered as 'failed', click here:";
    echo passed($ser, $mid);
  }
 
  echo "If this module was mistakenly entered, click here:";
  echo nottested($ser, $mid);
  Session::logout_button();
  //  echo "<a href=\"./list.php\">Let's see 'em all!</a>";
?>
<?php tsw_footer(); ?>
