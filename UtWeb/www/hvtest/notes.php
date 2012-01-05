<?php
# $Id: notes.php,v 1.4 2007/08/07 10:41:11 tofp Exp $
$id=$_POST['id'];
$id=$_GET['id'];
include("../../lib/connect.php");
include("../../lib/session.php");
Session::check_login("../hvtest/notes.php");

include("../../lib/template.php");
tsw_header("HV Test Results Input Page");
tsw_toolbar('hvtest');
function mod_entry(){
  echo "<form name='add_new_notes' action='update.php' method='post'/>";
  echo "<center><table><tr><td colspan=3>Enter module serial numbers excluding 0's</td></tr>";
  echo "<tr><td colspan='2'>Tsinghua modules</td><td colspan='2'>USTC modules</td></tr>";
  echo "<tr><td colspan='2'><textarea name='tm' rows='25' cols='50' /></textarea></td>";
  echo "<td colspan='2'><textarea name='um' rows='25' cols='50'/></textarea></td></tr>";
  echo "<tr><td><font color='green'><b>Pass</b><input name='pftm' type='radio' value='1' checked /></td>";
  echo "<td><font color='red'><b>Fail</b><input name='pftm' type='radio' value='0' /></td>";
  echo "<td><font color='green'><b>Pass</b><input name='pfum' type='radio' value='1'checked /></td>";
  echo "<td><font color='red'><b>Fail</b><input name='pfum' type='radio' value='0' /></td></tr></table>";
  echo "<input name='request_type' type='hidden', value='hvupdate'/><br>";
  echo "<input name='new_text' type='submit' value='Enter this into the database' /></form>\n";
}

?>
<div id="main">
<div class="title">HV Test Results Input Page</div>
<?php
  $all_count=mysql_query("select count(*) as count from hvtest", $con);
  $all_c=mysql_fetch_object($all_count);
  $count=$all_c->count;
  $bad_count=mysql_query("select count(*) as count from hvtest where pass='0'", $con);
  $bad_c=mysql_fetch_object($bad_count);
  $bad=$bad_c->count;
  $good=$count-$bad;

  echo "A total of ",$count," modules have been tested. Of those ", $bad," failed the test.<br>";
  echo "<a href=\"./list.php\">List of all modules tested</a><br>";
  echo "<a href=\"./datelist.php\">List of dates</a><br>";
  
  echo "<br>", mod_entry(), "<br></center>";
  Session::logout_button();
?>
<?php tsw_footer(); ?>
