<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
Session::check_login("../tray/create.php");

function back(){
  echo "<form action='../mtd_tray/list.php' method='post'>";
  echo "<input name='back' type='submit' value='Back'/>";
  echo "</form>";
}

include("../../lib/template.php");

tsw_header("Module Creation Page");
tsw_toolbar();
?>

<div id="main">
<div class='title'>Module Creation Page</div>
<?php
echo "<table><tr><td>", back(),"</td><td>";

echo "<form name='new_mod' action='./update.php' method='post'/>";
echo "<input name='request_type' type='hidden' value='new_module'/>";

echo "<tr><td>Select User<br>";
echo "<select name='user'>";
$result_users=mysql_query("select * from users where email!=''", $con);
while ($users=mysql_fetch_object($result_users)){
    echo "<option>", $users->name, "</option>";
}

echo "<td>Select Vendor<br>";
echo "<select name='vendor'>";
$result_vendors=mysql_query("select * from vendors", $con);
while ($vendors=mysql_fetch_object($result_vendors)){
    echo "<option>", $vendors->name, "</option>";
}


echo "</td><td>Received Date:<br>";
echo "<input name='received_date' type='text' value='YYYY-MM-DD' size='13'/></td>";

echo "<td>Serial Number<br>";
echo "<input name='serial' type='text' size='14'/></td>";

echo "<td>Vendor Serial Number<br>";
echo "<input name='chinese_serial' type='text' value='(int)' size='5'/></td></tr>";


echo "<tr><td colspan='4'>Vendor Module URL<br>";
echo "<input name='chinese_url' type='text' size='80'/></td></tr>";


echo "<tr><td colspan='6'>Notes:<br>";
echo "<textarea name='text' rows='5' cols='65' type='text'/></textarea><br>";

echo "<input name='new_mod' type='submit' value='Insert this information into database'/>";
echo "</form></td></tr></table>";
?>

<p><small>After submitting, you will be redirected to the tray list page. <br></small></p>

<?php tsw_footer(); ?>
<?php mysql_close($con) ?>