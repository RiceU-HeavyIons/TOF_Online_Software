<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
Session::check_login("../mtd_tray/create_noiserun.php");

function back(){
  echo "<form action='list.php' method='post'>";
  echo "<input name='back' type='submit' value='Back'/>";
  echo "</form>";
}

include("../../lib/template.php");
tsw_header("MTD Noise Run Creation Page");
tsw_toolbar();
?>

<div id="main">
<div class='title'>MTD Noise Run</div>

<?php
  echo "<table><tr><td>", back(),"</td><td>";
  echo "<form name='new_noiserun' action='./new_run.php' method='post'/>";
  echo "<input name='request_type' type='hidden' value='new_run'/>";

  echo "<tr><td>Tray Serial:<br>";
  echo "<input name='tray_sn' type='text' size='8'/></td></tr>";

  echo "<tr><td>Select User<br>";
  echo "<select name='user'>";
  $result_users=mysql_query("select * from users where email!=''", $con);
  while ($users=mysql_fetch_object($result_users)){
    echo "<option>", $users->name, "</option>";
  }
  echo "<td>Select Status:<br>";
  echo "<select name='run_status'>";
  $result_status=mysql_query("select * from run_status", $con);
  while ($r_status=mysql_fetch_object($result_status)){
    echo "<option>", $r_status->name, "</option>";
  }
  echo "</td><td>Run Number:<br>";
  echo "<input name='run_no' type='text' size='11'/></td></tr>";

  echo "<tr><td>Run Start:<br>";
  echo "<input name='start_date' type='text' value='YYYY-MM-DD HH:MM:SS' size='21'/></td>";
  echo "<td>Run End:<br>";
  echo "<input name='end_date' type='text' value='YYYY-MM-DD HH:MM:SS' size='21'/></td>";
  echo "<td>Events:<br>";
  echo "<input name='event_no' type='text' size='11'/></td></tr>";

  echo "<tr><td colspan='5'>Data Location:<br>";
  echo "<input name='data_uri' type='text' size='80'/></td></tr>";

  echo "<tr><td colspan='5'>Results PDF Location:<br>";
  echo "<input name='results_uri' type='text' size='80'/></td></tr>";

  echo "<tr><td colspan='4'>Notes:<br>";
  echo "<textarea name='text' rows='5' cols='35' type='text'/></textarea><br>";

  echo "<input name='new_run' type='submit' value='Insert this information into database'/>";
  echo "</form></td></tr></table>";
?>

<?php tsw_footer(); ?>
<?php mysql_close($con) ?>