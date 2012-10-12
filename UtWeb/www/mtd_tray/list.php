<?php
# $Id: list.php,v 1.2 2012/01/24 18:22:22 tofp Exp $
include("../../lib/orm.php");
include("../../lib/connect.php");
include("../../lib/session.php");
$thissession = new Session("../");
$thissession->update_ref("../mtd_tray/list.php");
#Session::update_ref("../mtd_tray/list.php");

function open_form($id){
  echo "<form name='frm_prop_search' method='post' action='http://www.rhip.utexas.edu/~tofp/xoops/modules/dms/search_prop.php'>";
  echo "<input type='hidden' name='slct_srch_property_name' value='3' />";
  echo "<input type='hidden' name='slct_srch_doc_owner' value='0' />";
  echo "<input type='hidden' name='slct_srch_create_date' value='0' />";
  printf("<input type='hidden' name='txt_property_name' value='[tray%03d]' />", $id);
  echo "<input type='hidden' name='hdn_reload_flag' value='TRUE' />";
  echo "\n";
}
function body_form() {
  echo "<input type='submit' name='btn_search' value='Find Documents' />";
}
 
function close_form(){
  echo "</form>\n";
}

function docs($id){
  echo "<form action='./documents.php' method='post'>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='commit' type='submit' value='Show'/></form>";
}

function wiki(){
  echo "<small><a href=\"http://www.rhip.utexas.edu/~tofp\" onMouseOver=\"ShowPopup(this);\" onMouseOut=\"HidePopup();\">To the Wiki!</a></small>";
}

function show($id){
  echo "<form name='show' action='inner.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='commit' type='submit' value='Show contents' /></form>";
}

function shipdate($id, $name){
  if($_SESSION['known_user']=='1'){
    echo "<form name='ship_date' action='ship.php' method='post'/>";
    echo "<input name='id' type='hidden' value='$id'/>";
    echo "<input name='commit' type='submit' value='$name'/></form>";}
  else{ echo $name;}

}

function notes($id, $sub_notes){
    echo "<form action='notes.php' method='post'>";
    echo "<input name='id' type='hidden' value='$id'/>";
    echo "<input name='notes' type='submit' value='$sub_notes'/>";
    echo "</form>";
}

function tray_stat($tray) {
  $id = $tray->id;
  if (Session::loggedin()) {
    echo "<form name='stat_drop' method='post' action='./update.php'>\n";
    echo "<input type='hidden' name='id' value='$id'>\n";
    echo "<input type='hidden' name='request_type' value='tray_status'>\n";
    echo "<select name='stat_drop'>";
    TrayStatus::options($tray->tray_status->id);
    echo "</select>\n";
    echo "<input type='submit' value='update'>\n";
    echo "</form>";
  } else {
     echo $tray->tray_status->name;
  }
}

function find_tray(){
  echo "<form  action='view.php' method='post'>";
  echo "Tray SN: <input name='sn' type='text' value='' size='3'/>";
  echo "<input name='commit' type='submit' value='Show'/>";
  echo "</form>";
}

function new_tray(){
  echo "<form action='create.php' method='post'>";
  echo "<input name='new' type='submit' value='Create New MTD Tray'/>";
  echo "</form>";
}

function new_run(){
  echo "<form action='create_noiserun.php' method='post'>";
  echo "<input name='new' type='submit' value='Run'/>";
  echo "</form>";
}

function new_component(){
  echo "<form action='../component/create.php' method='post'>";
  echo "<input name='new' type='submit' value='Create New Board'/>";
  echo "</form>";
}

function new_module(){
  echo "<form action='../module/create.php' method='post'>";
  echo "<input name='new' type='submit' value='Create New Module'/>";
  echo "</form>";
}

include("../../lib/template.php");
tsw_header("MTD Tray List");
tsw_toolbar("mtd_tray");
?>
<div id="main">
<div class="title">MTD Tray List</div>
<?php

$finished  = Tray::count("tray_status_id=3 and sn>199");
$workingon = Tray::count("tray_status_id!=3 and tray_status_id!=5 and sn>199");
$instorage = Tray::count("tray_status_id=5 and sn>199");
$tot = Tray::count("sn>199");
$lim = 20;
$off = 0;
if (isset( $_GET['page']) && !empty( $_GET['page']) )
  $page = $_GET['page'];
else
   $page = 1;
  
#$page = NULL;
#if($page == NULL) $page = 1;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
$off = $lim*($page - 1);
?>

<table>
<?php
if ( $workingon == 1){
  echo "<tr><td>We are working on ", $workingon, " tray currently.</td>";}
else{
  echo "<tr><td>We are working on ", $workingon, " trays currently.</td>";}
if ( $instorage == 1){ 
  echo "<td>", $instorage," tray is pending shipment.</td></tr>";}
else{
  echo "<td>", $instorage," trays are pending shipment.</td></tr>";}
?>
<tr><td colspan='2'><?php echo $finished ?> of 120 planned trays are built, tested and shipped.</td></tr>
<tr><td>
<?php find_tray() ?></td><td>
<?php
if(Session::loggedin()){
  new_tray();
  echo "</td><td>";
  new_module();
  echo "</td><td>";
  new_component();
  echo "</td><td>";
  new_run();
  echo "</td><td>";
  #Session::logout_button();
  $thissession->logout_button();
}
else{
  echo "</td><td>";
  #Session::login_button();
  $thissession->login_button();
}
?>
</td></tr>
</table>

<table id="tray_list">
<tr>
 <th>SN</th>
 <th>Technician</th>
 <th>Start Date</th>
 <th>Status</th>
 <th>Ship Date</th>
 <th>Contents</th>
 <th>Documents</th>
 <th>Notes</th>
 <th>Small Notes</th>
 <th>Tests</th>

</tr>

<?php
function tgl_link($lnk, $txt, $sw) {
  $ret = "";
  if ($sw) { $ret .= "<a href=\"$lnk\">"; }
  $ret .= $txt;
  if ($sw) { $ret .= "</a>"; }
  return $ret;
}

function elm_a($adr, $txt, $atr) {
  $ret = "<a href=\"$adr\"";
  if ($atr) $ret .= " $atr";
  $ret .= ">$txt</a>";
  return $ret;
}

$l = 1;
foreach (Tray::find_all("sn>199", "sn desc", "$off, $lim") as $tray) {
   $l += 1;

   $id = $tray->id;
   $user = $tray->user;

   $compcount = Component::count("tray_id = $id");
   $modcount = Module::count("tray_id = $id");

   $compcount=(($compcount + $modcount)/3)*100;
   if ($compcount > 100) $compcount = 100;

   $ll = $l % 2;
   echo " <tr class='row$ll'>";
   echo "  <td>" . elm_a("./view.php?id={$tray->id}", $tray->sn, "") . "</td>\n";
   echo "  <td>" . elm_a("mailto:{$user->email}", $user->name_last, "") . "</td>\n";
   echo "  <td style='white-space: nowrap;'>{$tray->start}</td>\n";
   echo "  <td style='white-space: nowrap;'>", tray_stat($tray), "</td>\n";
   $txt = $tray->ship ? $tray->ship : "Not Set";
   echo "  <td>" . tgl_link("./ship.php?id={$id}", $txt, Session::loggedin()) . "</td>\n";
   echo "  <td style='white-space: nowrap;'>" . elm_a("./inner.php?sn={$tray->sn}", sprintf("%3.0f%% Filled", $compcount), "") . "</td>\n";
   echo "  <td>" . elm_a("./documents.php?id={$id}", "Documents", "") . "</td>\n";

   // Notes column
   $tn = TrayNote::last("tray_id={$tray->id}");
   if($tn) $notes = $tn->note;
   else $notes='';
   if(strlen($notes) == 0) {
     if(Session::loggedin()) {
       $notes='Create a note';
       echo "  <td><a href=\"./notes.php?id=",$id, "\">",$notes,"</a></td>";
     } else {
       echo "  <td></td>";
     }
   } else {
      if(strlen($notes) >= 40) { 
         $notes = substr($notes, 0, 40) . '...';
      }
      echo "  <td><a href=\"./notes.php?id={$id}\">", 
         $tn->created_on, " ", $notes,"</a></td>\n";
   }

   echo "  <td><a href=\"./info.php?id={$id}\">", sizeof($tray->info),"</a></td>\n";
   echo "  <td><a href=\"./tests.php?id={$id}\">", sizeof($tray->find_runs()), "</a></td>\n";

   echo " </tr>";
 }

?>
</table>

<?php
  if($tot > $lim) {
    echo "<div>Page:";
    for($i=1; $i < $tot/$lim + 1; $i++) {
      if($i == $page)
         echo " <font style=\"color: red;\">$i</font>";
      else
         echo " <a href=\"list.php?page=$i\">$i</a>";
     }
  }
?>
</div><!-- main -->
<?php tsw_footer(); ?>
<?php mysql_close($con); ?>
