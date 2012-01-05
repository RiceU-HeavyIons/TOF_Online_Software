<?php
include("../../lib/connect.php");
include("../../lib/session.php");
Session::check_login("../tray/edit.php");

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

function ttrigform($r_ttrig, $id){
  echo "<form name='ttrig' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_ttrig'/>";
  echo "<select name='ttrig_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_trig=mysql_fetch_object($r_ttrig)){
    if($r_trig->tray_id == $id){
      echo "<option selected>*", $r_trig->serial, "</option>\n";}
    elseif($r_trig->tray_id ==''){
      echo "<option>", $r_trig->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select><input type='submit' value=''/></form>\n";
}

function tdigform($r_tdig, $id, $slot){
  echo "<form name='tdig' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_tdig'/>";
  echo "<input name='slot' type='hidden' value='$slot'/>";
  echo "<select name='tdig_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_dig=mysql_fetch_object($r_tdig)){
    if($r_dig->tray_id==$id and $r_dig->slot==$slot){
      echo "<option selected>*", $r_dig->serial, "</option>\n";}
    elseif($r_dig->tray_id==''){
      echo "<option>", $r_dig->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select><input type='submit' value=''/></form>\n";
}

function tinoform($r_tino, $id, $slot){
  echo "<form name='tino' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_tino'/>";
  echo "<input name='slot' type='hidden' value='$slot'/>";
  echo "<select name='tino_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_ino=mysql_fetch_object($r_tino)){
    if($r_ino->tray_id==$id and $r_ino->slot==$slot){
      echo "<option selected>*", $r_ino->serial, "</option>\n";}
    elseif($r_ino->tray_id==''){
      echo "<option>", $r_ino->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select><input type='submit' value=''/></form>\n";
}

function moduleform($r_module, $id, $slot){
  echo "<form name='module' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='modify_module'/>";
  echo "<input name='slot' type='hidden' value='$slot'/>";
  echo "<select name='module_select'>\n";
  echo "<option selected> Pick one </option>\n";
  while($r_m=mysql_fetch_object($r_module)){
    if($r_m->tray_id==$id and $r_m->slot==$slot){
      echo "<option selected>*", $r_m->serial, "</option>\n";}
    elseif($r_m->tray_id==''){
      echo "<option>", $r_m->serial, "</option>\n";}
  }
  echo "<option>----------</option>\n";
  echo "<option>Remove</option>\n";
  echo "</select><input type='submit' value=''/></form>\n";
}

$serial1=mysql_query("select sn from trays where id=$id", $con);
$s1=mysql_fetch_object($serial1);

include("../../lib/template.php");
tsw_header("Edit Tray ", $s1->sn, " Contents");
tsw_toolbar("tray");
?>
<div id="main">
<div class="title"><?php echo "Edit Tray ", $s1->sn, " Contents" ?></div>

<table id="module_list">
<?php
  $newresult1=mysql_query("select count(*) as count from components where tray_id=$id", $con);
  $nr1=mysql_fetch_object($newresult1);
  $num_component=$nr1->count;
  $l=0;
  echo back();
  echo "<tr><th>TCPU</th>";
//  echo "<th>TTRIG</th>";
  echo "<th>TDIG</th>";
  echo "<th>TINO</th>";
  echo "<th>MRPC 1</th>";
  echo "<th>MRPC 2</th>";
  echo "<th>MRPC 3</th>";
  echo "<th>MRPC 4</th></tr>";
  $newresult2=mysql_query("select * from components where (tray_id=$id and component_type_id=1) or (tray_id is null and component_type_id=1) order by serial", $con);
  echo "<tr><td class='tcpu' bgcolor=#ddddff>",tcpuform($newresult2, $id), "</td>\n";
//  $newresult5=mysql_query("select * from components where (tray_id=$id and component_type_id=5) or (tray_id is null and component_type_id=5) order by serial", $con);
//  echo "<td class='tcpu' bgcolor=#ddddff>",ttrigform($newresult5, $id), "</td>\n";

  echo "<td></td><td></td><td>High &eta</td><td></td><td></td><td>Low &eta</td>";
  $i=8;
  while($i>0){

    $l=$l+1;
    $ll=$l%2;
    $newresult3=mysql_query("select * from components where (tray_id=$id and component_type_id=2 and slot=$i) or (component_type_id='2' and tray_id is null and slot is null) order by serial", $con);
    $newresult4=mysql_query("select * from components where (tray_id=$id and component_type_id=3 and slot=$i) or (component_type_id='3' and tray_id is null and slot is null) order by serial", $con);
    echo "<tr class='row$ll'><td></td><td class='tdig'>", tdigform($newresult3, $id, $i),"</td></tr>\n";
    $l=$l+1;
    $ll=$l%2;
    echo "<tr class='row$ll'><td></td><td>Board #$i &rarr </td><td class='tino'>", tinoform($newresult4, $id, $i), "</td></tr>\n";
    $l=$l+1;
    $ll=$l%2;
    $j=$i*4;
    $k=($i-1)*4;
    echo "<tr class='row$ll'><td>Lower &eta &darr</td><td></td><td></td>";
    while($j>$k){
#      $newresult5=mysql_query("select distinct m.serial, m.tray_id, m.slot from module_results module_results inner join modules m on module_results.module_id = m.id where (m.tray_id=$id and m.slot=$j) or (m.tray_id is null)", $con);
      $newresult5=mysql_query("select distinct serial, tray_id, slot from modules order by serial", $con);
      echo "<td class='module'>", moduleform($newresult5, $id, $j), "</td>\n";
      $j=$j-1;
    }
    echo "</tr>\n";
    $i=$i-1;
  }
?>
</table>
</div><!-- main -->
<?php tsw_footer(); ?>
<?php mysql_close($con) ?>
