<?php
# $Id: view.php,v 1.2 2012/01/24 18:23:10 tofp Exp $
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");

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

$id = isset($rvar_id) ? $rvar_id : 0;
$sn = isset($rvar_sn) ? $rvar_sn : '';

$tray = new Tray($id);
if ($tray->id <=0) $tray = Tray::find_by_sn($sn);
if ($tray->id) {
  $sn = $tray->sn;
  $id = $tray->id;
}

include("../../lib/template.php");
$title = "MTD Tray " . $sn;
$tsw = new Tsw($title);
$tsw->header();
tsw_toolbar("mtd_tray");
?>
<div id="main">
<div class="title"><?php echo $title ?></div>
<div>
 [<a href="./view.php?id=<?php echo $id-1; ?>">prev</a>]
 [<a href="./list.php">tray list</a>]
 [<a href="./view.php?id=<?php echo $id+1; ?>">next</a>]
</div>
<?php

#The heart of the page. First determine if the tray has been populated at all. 
#Then find the TCPU serial, drop into a while loop that finds the serial numbers
#for the TDIG and TINO, followed by a nested while loop to find the modules for
#each plugged into each TINO. All locations are based on 'slot number' in the tray.
#Slot numbers run from 1-32 for modules with 1 being at the low eta end and 32
#being at the HV/gas connector end.

if ($tray->id > 0) {

  echo "<div>\n";
  echo "<table>\n";
  echo "<tr><th>Id</th>\n";
  echo " <td>", $tray->id, "</td></tr>\n";

##dylan -- this is the line I'm editing 
  echo "<tr><th>Backleg</th>\n";
  $backleg_D = mysql_query("SELECT backleg FROM tray_backlegs where tray_id = $tray->id", $con);
  if($blD2 = mysql_fetch_object($backleg_D)) {
    $bl = $blD2->backleg;
    echo " <td>", $bl , "</td></tr>\n";
  }
  else {echo " </tr>";}
##end damage

  $slotr = mysql_query("SELECT slot FROM components where tray_id = $tray->id AND component_type_id=2", $con);
  $slot = mysql_fetch_object($slotr);
  echo "<tr><th>Slot</th>\n";
  echo " <td>", $slot->slot, "</td></tr>\n";

  echo "<tr><th>S/N</th>\n";
  echo " <td>", mk_link($tray->sn, "./view.php?sn=" . $tray->sn), "</td></tr>\n";
  echo "<tr><th>Technician</th>\n";
  echo " <td>", $tray->user->name_first, " ", $tray->user->name_last, "</td></tr>\n";
  echo "<tr><th>Start Date</th>\n";
  echo " <td>", $tray->start, "</td></tr>\n";
  echo "<tr><th>Status\n";
  echo " <td>", $tray->tray_status->name, "</td></tr>\n";
  echo "<tr><th>Ship Date\n";
  echo " <td>", $tray->ship, "</td></tr>\n";
  echo "</table>\n";
  echo "</div>\n";

# -----------------------------------------------------------------------------
  echo "<div class='section'>\n";
  echo "<div class='title'>Notes</div>\n";
  echo "<div>\n";
  $notes = TrayNote::find_all("tray_id={$tray->id}", "created_on desc", "");
  if(sizeof($notes)) {
    echo "<table>";
    $l = 1;
    foreach($notes as $tn) {
      $l++; $ll =$l%2;
      $user = new User($tn->user_id);
      echo "<tr class='row{$ll}'>\n";
      echo " <td style='white-space: nowrap;'	>" . $tn->created_on . "</td>\n";
      echo " <td>", $user->name, "</td>\n";
      echo " <td>" . $tn->note . "</td>\n";
      echo "</tr>\n";
    }
    echo "</table>";
  }
  echo "</div>";
  echo "</div> <!-- section -->\n";
# -----------------------------------------------------------------------------
  echo "<div class='section'>\n";
  echo "<div class='title'>Tray General Information</div>\n";
  echo "<div>\n";
  if(count($tray->info)) {
    echo "<table id='info'>\n";
    echo "<tr>\n";
    echo " <th>Data Type</th><th>Value</th><th>Date Inserted</th>\n";
    echo "</tr>\n";
    $l=1;
    foreach($tray->info as $itm) {
      $l+=1;
      $ll=$l%2;
      echo "<tr class='row$ll'>\n";
      echo "  <td>", $itm->data_name, "</td>\n";
      echo "  <td>", $itm->data, "</td>\n";
      echo "  <td>", $itm->date, "</td>\n";
      echo "</tr>\n";
    }
    echo "</table>\n";
  }
  echo "</div>";
  echo "</div> <!-- section -->\n";

# -----------------------------------------------------------------------------
  echo "<div class='section'>\n";
  echo "<div class='title'>Documents</div>\n";
  echo "<div>\n";
  if(count($tray->doc)) {
    $result=mysql_query("select * from tray_info where tray_id=$id order by date DESC", $con);
    echo "<table id='doc'>\n";
    echo "<tr>\n";
    echo " <th></th>\n";
    echo " <th>File Name</th>\n";
    echo " <th>File Description</th>\n";
    echo " <th>Timestamp</th>\n";
    echo "</tr>\n";

    $l=1;
    foreach($tray->doc as $itm) {
      $l+=1;
      $ll=$l%2;
      echo "<tr class='row$ll'>\n";
      echo "  <td style='text-align: right;'>", $l-1, ".</td>\n";
      echo "  <td>", mk_link($itm->fname, "./file.php?id=" . $itm->id), "</td>\n";
      echo "  <td>", $itm->name, "</td>\n";
      echo "  <td>", $itm->date, "</td>\n";
      echo "</tr>\n";
    }
    echo "</table>\n";
  } else
    echo "<div>No document found.</div>\n";
  echo "</div>";
  echo "</div> <!-- section -->\n";
  

# -----------------------------------------------------------------------------
# Noise Runs:
  $runsr=mysql_query("SELECT * FROM mtd_runs WHERE tray_id = '$tray->id' AND run_type_id = 4 ", $con);
  if (mysql_num_rows($runsr) > 0) {
    echo "<div class='section'>\n";
    echo "<div class='title'>Noise Runs</div>\n";
    $l = 0;
    echo "<table>\n";
    echo "<tr>\n";
    echo " <th></th>\n";
    echo " <th>ID</th>\n";
    echo " <th>Start</th>\n";
    echo " <th>Stop</th>\n";
    echo " <th>Events</th>\n";
    echo " <th>Results</th>\n";
    echo " <th>Notes</th>\n";
    echo "</tr>\n";

    while ($rs = mysql_fetch_object($runsr)) {
        $l += 1;
        $ll = $l % 2;
        echo " <tr class='row$ll'>\n";
        echo "  <td>$l.</td>\n";
        echo "  <td>", $rs->id, "</td>\n";
        #echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
        echo "  <td>", $rs->start, "</td>\n";
        echo "  <td>", $rs->stop, "</td>\n";
        echo "  <td>", $rs->events, "</td>\n";
        echo "  <td>", mk_link("results.pdf", $rs->results_uri), "</td>\n";
        echo "  <td>", $rs->note, "</td>\n";
        #$st = $run->stats_for_tray($tray->id);
        #echo "  <td class='float'>" . sprintf("%5.1f", $st["avg"]) . "</td>\n";
        #echo "  <td class='float'>" . sprintf("%5.1f", $st["min"]) . "</td>\n";
        #echo "  <td class='float'>" . sprintf("%5.1f", $st["max"]) . "</td>\n";
#        echo "  <td class='int'>", $ct, "</td>\n";
        echo " </tr>\n";
    }
    echo "</table>\n";
    echo "</div> <!-- section -->\n";
  }

# -----------------------------------------------------------------------------
# Time Resolution runs:
  if ($rs = $tray->find_runs("run_type_id = 7")) {
    echo "<div class='section'>\n";
    echo "<div class='title'>Timing Resolution Test (ps)</div>\n";
    $l = 0;
    echo "<table>\n";
    echo "<tr>\n";
    echo " <th></th>\n";
    echo " <th>ID</th>\n";
#    echo " <th>Status</th>\n";
#    echo " <th>Operator</th>\n";
#   echo " <th>Type</th>\n";
    echo " <th>Config</th>\n";
    echo " <th>Setup</th>\n";
    echo " <th>Start</th>\n";
    echo " <th>Stop</th>\n";
    echo " <th>Avg</th>\n";
    echo " <th>Min</th>\n";
    echo " <th>Max</th>\n";
#    echo " <th>MR</th>\n";
    echo "</tr>\n";

    foreach($rs as $run) {
      if($ct = $run->mr_count) {
        $l += 1;
        $ll = $l % 2;
        echo " <tr class='row$ll'>\n";
        echo "  <td>$l.</td>\n";
        echo "  <td>", mk_link($run->id, "../run/show.php?id=" . $run->id), "</td>\n";
#        echo "  <td>", $run->status->name, "</td>\n";
#        echo "  <td>", $run->user->name, "</td>\n";
#       echo "  <td>", $run->type->name, "</td>\n";
        echo "  <td>", mk_link($run->config->id, "../config/show.php?id=" . $run->config->id), "</td>\n";
        echo "  <td>", $run->config->type->name, "</td>\n";
        echo "  <td>", $run->start, "</td>\n";
        echo "  <td>", $run->stop, "</td>\n";
        $st = $run->stats_for_tray($tray->id);
        echo "  <td class='float'>" . sprintf("%5.1f", $st["avg"]) . "</td>\n";
        echo "  <td class='float'>" . sprintf("%5.1f", $st["min"]) . "</td>\n";
        echo "  <td class='float'>" . sprintf("%5.1f", $st["max"]) . "</td>\n";
#        echo "  <td class='int'>", $ct, "</td>\n";
        echo " </tr>\n";
      }
    }
    echo "</table>\n";
    echo "</div> <!-- section -->\n";
  }

# -----------------------------------------------------------------------------
  echo "<div class='section'>\n";
  echo "<div class='title'>Contents</div>\n";
  echo "<div><table id='module_content'>\n";

#Find out if anything has been put in a tray
$num_component = Component::count("tray_id=${id}");
$num_module = Module::count("tray_id=${id}");
$l=0;
if($num_component == 0){
  srand(time());
  if($num_module !=0){
    srand(time());
    switch(rand()%3) {
    case 0:
      echo "<tr><th>No boards found in the database for this tray</th></tr>\n";
      break;
    case 1:
      echo "<tr><th>", $num_module, " modules found, but 0 boards.</th></tr>\n";
      break;
    case 2:
      echo "<tr><th>I've found modules in the database but no boards.</th></tr>\n";
      break;
    }
    echo "<tr><td>",edit($id), "</td></tr>\n";
  } else {
    switch(rand()%2) {
    case 0:
      echo "<tr><th>This tray might be empty.</th></tr>\n";
      break;
    case 1:
      echo "<tr><th>Nothing found for this tray.</th></tr>\n";
      break;
    }
    if(Session::loggedin()) {
      echo "<tr><td>",edit($id), "</td></tr>\n";
    }
  }
} else{
  if(Session::loggedin()) {
    echo "<tr><td>",edit($id), "</td></tr>\n";
  }
  echo "<tr><th>TCPU</th>";
  echo "<th>MTRG</th>";
  echo "<th>TDIG</th>";
  echo "<th>MINO</th>";
  echo "<th>MRPC</th></tr>";

  if(Session::loggedin()) {
    $action="../component/edit.php";
  } else {
    $action="../component/show.php";
  }
  echo "<tr>\n";

# TCPU:
  if($tray->tcpu->id){
   echo " <td class='tcpu' bgcolor=#ddddff>\n";
   echo "   <a href=\"{$action}?id=", $tray->tcpu->id, "\">", $tray->tcpu->sn, "</a>\n";
   echo " </td>\n";
  } else {
   echo "  <td class='tcpu' bgcolor=#ddddff>Nothing here</td>\n";
  }

#TTRG:
  if($tray->ttrig->id){
   echo " <td class='tcpu' bgcolor=#ddddff>\n";
   echo "   <a href=\"{$action}?id=", $tray->ttrig->id, "\">", $tray->ttrig->sn, "</a>\n";
   echo " </td>\n";
  } else {
   echo "  <td class='tcpu' bgcolor=#ddddff>Nothing here</td>\n";
  }


  echo "</tr>\n";

  $l=$l+1;
  $ll=$l%2;

  if($tray->tdig) {
    $i=5;
    while (!($tdig=$tray->tdig[$i])) {$i=$i-1;}
    echo "<tr class='row$ll'>\n";
    echo " <td></td><td></td>\n";
    echo " <td class='tdig'><a href=\"{$action}?id=",$tdig->id,"\">",$tdig->sn,"</td>\n";
    echo "</tr>\n";
  } else {
    echo "<tr class='row$ll'><td></td><td class='tdig'>Nothing here </td></tr>\n";
  }

  $l=$l+1;
  $ll=$l%2;
  echo "<tr class='row$ll'>\n";
  if($tray->mino){
    $i=5;
    while (!($mino=$tray->mino[$i])) {$i=$i-1;}
    echo " <td></td><td></td>\n";
    echo " <td></td>\n";
    echo " <td class='tino'><a href=\"{$action}?id=", $mino->id, "\">", $mino->sn, "</td>\n";
    echo "</tr>\n";
  } else {
    echo "<td></td><td></td><td></td><td>Nothing here </td>";
  }
  echo "</tr>\n";

  $l=$l+1;
  $ll=$l%2;
  echo "<tr class='row$ll'><td></td><td></td><td></td><td></td>";
  if($tray->module) {
    $i=5;
    while (!($mod=$tray->module[$i])) {$i=$i-1;}
    echo "  <td class='module'><a href=\"../module/show.php?id=", $mod["id"], "\">", $mod["serial"], "</td>\n";
  } else {
    echo "<td> Nothing here</td>\n";
  }
  echo "</tr>\n";
}
echo "</table></div>\n";
echo "</div> <!-- section -->\n";

} else {
  echo "<div>That tray doesn't exist... yet.</div>\n";
}
?>
</div> <!-- main -->
<?php tsw_footer(); ?>
