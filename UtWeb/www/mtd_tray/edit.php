<?php
include("../../lib/connect.php");
include("../../lib/session.php");
Session::check_login("../mtd_tray/edit.php");

$id = $rvar_id;
function title() {
	printf ("Tray %03d", $id);
}
function update() {
  echo "<a href='list.php'>No data found for this tray!</a>";
}
function back() {
  echo "<form action='list.php' method='post'>";
  echo "<input name='back' type='submit' value='Finished'/></form>";
}

function edit($id){
  echo "<form action='edit.php' method='post'>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='edit' type='submit' value='Edit Contents'/>";
  echo "</form>";
}
function tcpuform($r_tcpu, $id){
  echo "<form name='tcpu' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_tcpu'/>";
  echo "<select name='tcpu_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_cpu=mysql_fetch_object($r_tcpu)){
    if($r_cpu->tray_id == $id){
      echo "<option selected>*", $r_cpu->serial, "</option>\n";}
    elseif($r_cpu->tray_id ==''){
      echo "<option>", $r_cpu->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select><input type='submit' value=''/></form>\n";
}

function tdigform($r_tdig, $id) {
  echo "<form name='tdig' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_tdig'/>";
  echo "<select name='tdig_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_dig = mysql_fetch_object($r_tdig)){
    if($r_dig->tray_id==$id) {
      echo "<option selected>*", $r_dig->serial, "</option>\n";
    }
    elseif($r_dig->tray_id=='') {
      echo "<option>", $r_dig->serial, "</option>\n";
    }
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select>\n";
  echo "<input type='submit' value=''/></form>\n";
}

function minoform($r_mino, $id){
  echo "<form name='mino' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_mino'/>";
  echo "<select name='mino_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_ino=mysql_fetch_object($r_mino)){
    if($r_ino->tray_id==$id) {
      echo "<option selected>*", $r_ino->serial, "</option>\n";
    }
    elseif($r_ino->tray_id=='') {
      echo "<option>", $r_ino->serial, "</option>\n";
    }
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select>\n";
  echo "<input type='submit' value=''/></form>\n";
}

function moduleform($r_module, $id) {
  $slot = 1;
  echo "<form name='module' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_module'/>";
  echo "<select name='module_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_m=mysql_fetch_object($r_module)) {
    if($r_m->tray_id==$id) {
      echo "<option selected>*", $r_m->serial, "</option>\n";
    }
    elseif($r_m->tray_id==''){
      echo "<option>", $r_m->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select>\n";
  echo "<input type='submit' value=''/></form>\n";
}

$serial1=mysql_query("select sn from trays where id=$id", $con);
$s1=mysql_fetch_object($serial1);

include("../../lib/template.php");
tsw_header("Edit MTD Tray ", $s1->sn, " Contents");
tsw_toolbar("mtd_tray");
?>
<div id="main">
<div class="title"><?php echo "Edit MTD Tray ", $s1->sn, " Contents" ?></div>

<table id="module_list">
<?php
$newresult1=mysql_query("select count(*) as count from components where tray_id=$id", $con);
$nr1=mysql_fetch_object($newresult1);
$num_component=$nr1->count;

$l=0;
echo back();
echo "<br><br>\n";

$backleg_D = mysql_query("SELECT backleg FROM tray_backlegs where tray_id=$id", $con);
if($blD2 = mysql_fetch_object($backleg_D)) {
  $bl = $blD2->backleg;
}

echo "<form name='backleg' action='update.php' method='post'/>";
echo "<input name='id' type='hidden' value='$id'/>";
echo "<input name='request_type' type='hidden' value='modify_backleg'/>";
echo "Backleg:\n";
echo "<input name='backleg' type='text' size='5' value='$bl'/>";
echo "<input type='submit' value=''/></form>\n";

echo "<br><br>\n";

$slotr = mysql_query("SELECT slot FROM components where tray_id=$id AND component_type_id=2", $con);
if($slotf = mysql_fetch_object($slotr)) {
  $slot = $slotf->slot;
}
else {
  $slot = 0;
}


echo "<form name='slot' action='update.php' method='post'/>";
echo "<input name='id' type='hidden' value='$id'/>";
echo "<input name='request_type' type='hidden' value='modify_slot'/>";
echo "Slot:\n";
echo "<input name='slot' type='text' size='2' value='$slot'/>";
echo "<input type='submit' value=''/></form>\n";

echo "<br><br>\n";


echo "<tr><th>TCPU</th>";
echo "<th>TDIG</th>";
echo "<th>MINO</th>";
echo "<th>MRPC</th></tr>";

# TCPU:
$newresult2=mysql_query("select * from components where (tray_id=$id and component_type_id=1) or (tray_id is null and component_type_id=1) order by serial", $con);
echo "<tr><td class='tcpu' bgcolor=#ddddff>",tcpuform($newresult2, $id), "</td>\n";

$l=$l+1;
$ll=$l%2;

# TDIG:
$newresult3=mysql_query("select * from components where (tray_id=$id and component_type_id=2) or (component_type_id='2' and tray_id is null) order by serial", $con);
echo "<tr class='row$ll'><td></td><td class='tdig'>", tdigform($newresult3, $id),"</td></tr>\n";

# MINO:
$newresult4=mysql_query("select * from components where (tray_id=$id and component_type_id=7) or (component_type_id='7' and tray_id is null) order by serial", $con);

$l=$l+1;
$ll=$l%2;
echo "<tr class='row$ll'><td></td><td></td><td class='tino'>", minoform($newresult4, $id), "</td></tr>\n";

# MODULE
$newresult5=mysql_query("select distinct serial, tray_id, slot from modules order by serial", $con);

$l=$l+1;
$ll=$l%2;
echo "<tr class='row$ll'><td></td><td></td><td></td><td class='module'>", moduleform($newresult5, $id), "</td>\n";

echo "</tr>\n";
?>
</table>
</div><!-- main -->
<?php tsw_footer(); ?>
<?php mysql_close($con) ?>
