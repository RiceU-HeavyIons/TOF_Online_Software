<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
Session::check_login("../tray/create.php");

function back(){
  echo "<form action='list.php' method='post'>";
  echo "<input name='back' type='submit' value='Back'/>";
  echo "</form>";
}

include("../../lib/template.php");
tsw_header("Tray Creation Page");
tsw_toolbar();
?>
<div id="main">
<div class='title'>Tray Creation</div>
<?php
  echo "<table><tr><td>", back(),"</td><td>";
  echo "<form name='new_tray' action='./update.php' method='post'/>";
  echo "<input name='request_type' type='hidden' value='new_tray'/>";
  echo "<tr><td>Select User<br>";
  echo "<select name='user'>";
  $result_users=mysql_query("select * from users where email!=''", $con);
  while ($users=mysql_fetch_object($result_users)){
    echo "<option>", $users->name, "</option>";
  }
  echo "<td>Select Status:<br>";
  echo "<select name='stat_drop'>";
  TrayStatus::options(1);
#  echo "<option selected>construction</option>";
#  echo "<option>testing</option>";
#  echo "<option>shipping</option>";
#  echo "<option>repairing</option>";
#  echo "<option>storage</option>";
  echo "</td><td>Start Date:<br>";
  echo "<input name='start_date' type='text' value='YYYY-MM-DD' size='11'/></td></tr>";
  echo "<tr><td colspan='4'>Notes:<br>";
  echo "<textarea name='text' rows='5' cols='35' type='text'/></textarea><br>";
  echo "<input name='new_tray' type='submit' value='Insert this information into database'/>";
  echo "</form></td></tr></table>";
?>

<p><small>After submitting, you will be redirected to the tray list page. <br></small></p>

<?php tsw_footer(); ?>
<?php mysql_close($con) ?>