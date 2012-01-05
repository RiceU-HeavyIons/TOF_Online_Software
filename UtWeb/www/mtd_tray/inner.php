<?php
include("../../lib/connect.php");
include("../../lib/session.php");

$sn = $rvar_sn;
$id = $rvar_id;

function update() {
  echo "<a href='list.php'>No data found for this tray!</a>";
}
function back() {
  echo "<form action='list.php' method='post'>\n";
  echo "<input name='back' type='submit' value='Back'/>\n";
  echo "</form>\n";
}
function edit($id){
  echo "<form action='edit.php' method='post'>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='edit' type='submit' value='Edit Contents'/>";
  echo "</form>";
}


if($sn !=""){
  $result1=mysql_query("select * from trays where sn='$sn'", $con);
  $r1=mysql_fetch_object($result1);
  $id=$r1->id;
  $count1=mysql_query("select count(*) as count from trays where sn='$sn'", $con);
  $c1=mysql_fetch_object($count1);
  $count=$c1->count;
}
else{$count1=mysql_query("select count(*) as count from trays where id='$id'", $con);
 $c1=mysql_fetch_object($count1);
 $count=$c1->count;
 $result1=mysql_query("select * from trays where id='$id'", $con);
 $r1=mysql_fetch_object($result1);
 $sn=$r1->sn;
}

if($id) {
    Session::update_ref("../mtd_tray/inner.php?id=$id");
} elseif($sn) {
    Session::update_ref("../mtd_tray/inner.php?sn=$sn");
}
include("../../lib/template.php");
tsw_header("MTD Tray ", $sn, " Contents");
tsw_toolbar("mtd_tray");
?>
<div id="main">
<div class="title"><?php echo "MTD Tray ", $sn, " Contents" ?></div>

<table id="module_content">
<?php

#The heart of the page. First determine if the tray has been populated at all. 
#Then find the TCPU serial, drop into a while loop that finds the serial numbers
#for the TDIG and TINO, followed by a nested while loop to find the modules for
#each plugged into each TINO. All locations are based on 'slot number' in the tray.
#Slot numbers run from 1-32 for modules with 1 being at the low eta end and 32
#being at the HV/gas connector end.

if($count!=0){

  #Find out if anything has been put in a tray
  $newresult1=mysql_query("select count(*) as count from components where tray_id=$id", $con);
  $nr1=mysql_fetch_object($newresult1);
  $num_component=$nr1->count;
  $module_check=mysql_query("select count(*) as count from modules where tray_id=$id", $con);
  $mc1=mysql_fetch_object($module_check);
  $mc1_count=$mc1->count;
  $l=0;
  if($num_component==0){
    echo back();
    if($mc1->count!=0){
      srand(time());
      $random = (rand()%3);
      if($random == '0'){ echo "<tr><th>Into which boards are the modules I've found plugged?</th></tr>\n"; }
      elseif($random == '1'){  echo "<tr><th>", $mc1->count, " modules found, but 0 boards. What's the deal?</th></tr>\n"; }
      elseif($random == '2'){  echo "<tr><th>I've found modules in the database but no boards. Whose responsible?</th></tr>\n"; }
      echo "<tr><td>",edit($id), "</td></tr>\n";}
    else{
      srand(time());
      $random = (rand()%3);
      if($random == '0'){ echo "<tr><th>It looks like this tray is empty to me.</th></tr>\n"; }
      elseif($random == '1'){  echo "<tr><th>I can't seem to find anything in this tray.</th></tr>\n"; }
      elseif($random == '2'){  echo "<tr><th>Are you sure there should be something there?</th></tr>\n"; }
      echo "<tr><td>",edit($id), "</td></tr>\n";}
  }
  else {
    echo back();
    if(Session::loggedin()){
      echo edit($id);
    } else {
      Session::login_button();
    }
    echo "<tr><th>TCPU</th>";
    echo "<th>TDIG</th>";
    echo "<th>MINO</th>";
    echo "<th>MRPC</th>";
    
    
# TCPU
    $newresult2=mysql_query("select * from components where tray_id=$id and component_type_id=1", $con);
    $nr2=mysql_fetch_object($newresult2); 
    if($nr2 != '') {
      echo "<tr><td class='tcpu' bgcolor=#ddddff><a href=\"../component/edit.php?id=",$nr2->id,"\">",$nr2->serial, "</td>\n";
    }
    else {echo "<tr><td class='tcpu' bgcolor=#ddddff>Nothing here </td>";}
    
# TDIG
    $l=$l+1;
    $ll=$l%2;
    $newresult3=mysql_query("select * from components where tray_id=$id and component_type_id=2", $con);
    $nr3=mysql_fetch_object($newresult3);

    if($nr3->serial != '') {
      echo "<tr class='row$ll'><td></td><td class='tdig'><a href=\"../component/edit.php?id=",$nr3->id,"\">",$nr3->serial,"</td></tr>\n";
    }
    else {
      echo "<tr class='row$ll'><td></td><td class='tdig'>Nothing here </td></tr>\n";
    }

    $l=$l+1;
    $ll=$l%2;
    
# MINO
    $newresult4=mysql_query("select * from components where tray_id=$id and component_type_id=7", $con);
    $nr4=mysql_fetch_object($newresult4);


    if($nr4->serial != '') {
      echo "<tr class='row$ll'><td></td><td>Board #$i &rarr</td><td class='tino'><a href=\"../component/edit.php?id=", $nr4->id, "\">", $nr4->serial, "</td></tr>\n";
    }
    else { echo "<tr class='row$ll'><td></td><td></td><td>Nothing here </td></tr>\n";}
    $l=$l+1;
    $ll=$l%2;

    $newresult5=mysql_query("select * from modules where tray_id=$id", $con);
    $nr5=mysql_fetch_object($newresult5);
    if($nr5->serial != '') {
      echo "<td class='module'><a href=\"../module/show.php?id=", $nr5->id, "\">", $nr5->serial, "</td>\n";}
    else{ echo "<td> Nothing here</td>\n";}
    echo "</tr>\n";
  }
}
else{
  echo "That tray doesn't exist... yet.<br>";
  back();
}
?>
</div> <!-- main -->
<?php tsw_footer(); ?>
