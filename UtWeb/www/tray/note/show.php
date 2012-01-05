<?php
$id=$_POST['id'];
$id=$_GET['id'];
include("../../../lib/connect.php");
include("../../../lib/orm.php");
include("../../../lib/session.php");
include("../../../lib/template.php");

Session::update_ref("../tray/note/show.php?id=$id");

function newtext($id){
  echo "<form name='insert' action='update.php' method='post'/>\n";
  echo "<input name='action' type='hidden' value='insert'>\n";
  echo "<input name='id' type='hidden' value='$id'>\n";

  echo "<table>\n";
  echo " <tr>\n";
  echo "  <th>User: </th>\n";
  echo "  <td><select name='user_id'>\n";
  User::options(107);
  echo "  </select></td>\n";
  echo " </tr>\n";

  echo " <tr>\n";
  echo "  <th>Purpose:</th>\n";
  echo "  <td>\n";
  echo "   General<input name='note_type' type='radio' value='0' checked>\n";
  echo "   <font color='red'><b>Problem</b><input name='note_type' type='radio' value='1'></font>\n";
  echo "   <font color='blue'><b>Solution</b><input name='note_type' type='radio' value='2'>\n";
  echo "  </td>\n";
  echo " </tr>\n";
  echo " <tr>\n";
  echo "  <th>Note:</th>\n";
  echo "  <td><textarea name='note' rows='10' cols='75'></textarea></td>";
  echo " </tr>\n";
  echo " <tr><td colspan='2' style='text-align: center;'>\n";
  echo "  <input name='new_text' type='submit' value='Append this text to note'>\n";
  echo " </td></tr>\n";
  echo "</table>\n";

  echo "</form>\n";
}

$tray = new Tray($id);
$serial1=mysql_query("select sn from trays where id=$id", $con);
$s1=mysql_fetch_object($serial1);

tsw_header("Full text of notes for tray {$tray->sn}");
tsw_toolbar('tray');
?>
<div id="main">
<div class="title"> Tray <?php echo $tray->sn ?> Notes </div>

<div style='margin: 5px 25px 10px 25px;'>
<?php
  if(!Session::loggedin()) {
     echo "Please login to edit notes about this tray.\n";
     Session::login_button();
  }
?>
</div>

<div class='section'>
<div class='title'>Sticky</div>
<div style='margin: 5px 25px 10px 25px;'>
<?php
if(Session::loggedin()) {
   echo "<form action='update.php' method='post'>";
   echo "<input type='hidden' name='action' value='update_sticky' />\n";
   echo "<input type='hidden' name='tray_id' value='{$tray->id}' />\n";
   echo "<table>\n";
   echo " <tr><td>\n";
   echo "  <textarea name='sticky' rows='5' cols='75'>{$tray->notes}</textarea>\n";
   echo " </td></tr>\n";
   echo " <tr><td style='text-align: center;'>\n";
   echo "  <input type='submit' value='update sticky' />\n";
   echo " </td></tr>\n";
   echo "</table>\n";
   echo "</form>\n";
} else {
   echo $tray->notes;
}
?>
</div></div><!-- section -->

<div class='section'>
<div class='title'>Notes</div>
<div style='margin: 5px 25px 10px 25px;'>
<?php
$notes = TrayNote::find_all("tray_id=$id", "created_on desc", "");
if(sizeof($notes) > 0) {
  echo "<table id='notes'>\n";
  echo " <tr>\n";
  foreach($notes as $tn) {
    echo " <tr>\n";
    echo "  <td style='white-space: nowrap;'>", $tn->created_on, "</td>\n";
    $user = new User($tn->user_id);
    echo "  <td>", $user->name, "</td>\n";
    echo "  <td>", $tn->note, "</td>\n";
    echo " </tr>\n";
  }
  echo "</table>";
}
if(Session::loggedin()){ echo newtext($id); }
?>
</div></div><!--  section -->

</div><!-- main -->
<?php tsw_footer(); ?>
