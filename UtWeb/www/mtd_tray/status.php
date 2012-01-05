<?php
# $Id: status.php,v 1.1 2012/01/05 17:29:04 tofp Exp $
include_once("../../lib/orm.php");
include_once("../../lib/connect.php");
include_once("../../lib/session.php");
include_once("../../lib/utility.php");
include_once("../../lib/template.php");
Session::update_ref("../tray/status.php");


function tray_stat($tray) {
  $id = $tray->id;
  if (Session::loggedin()) {
    echo "<form name='stat_drop' method='post' action='./update.php'>\n";
    echo " <input type='hidden' name='id' value='$id' />\n";
    echo " <input type='hidden' name='request_type' value='tray_status' />\n";
    echo " <select name='stat_drop'>";
    TrayStatus::options($tray->tray_status->id);
    echo " </select>\n";
    echo " <input type='submit' value='update' />\n";
    echo "</form>";
  } else {
     echo $tray->tray_status->name;
  }
}
if(Session::loggedin()) {
 tsw_toolbar("tray");
}
tsw_header("Tray Status");
?>
<div id="main">
<table id="tray_list" width="100%" border='1' cellspacing='0'>
<tr>
 <th colspan='4' style='font-size: 120%;'>
  Tray Status <?php echo date("Y-m-d"); ?>
 </th>
 <th colspan='5' >Shipment</th>
</tr>
<tr>
 <th>Tray</th>
 <th>Location</th>
 <th>Status</th>
 <th>Notes</th>
 <th>#1</th>
 <th>#2</th>
 <th>#3</th>
 <th>#4</th>
 <th>#5</th>
 <th>Ideal build week</th>
 <th>Sealed Date</th>

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

$ship_dates = Tray::ship_dates();
$ship_num = array();
$l = 0;
foreach(array_keys($ship_dates) as $d) { $ship_num[$d] = $l++; }

$trays = Tray::find_all("", "(sn+1) asc", "");
$last = sizeof($trays) - 1;
if ($last < 118 + 3) { $last--; }
$last_sn = $trays[$last]->sn;
$dry = split("-", $trays[$last]->start);
$last_dt = parse_sqldate($trays[$last]->start);
$next_mon = strtotime("next Monday", $last_dt);

$l = 1;
foreach ($trays as $tray) {

   if($tray->sn == 118 and $last < 118 + 3) continue;

   $l += 1;

   $id = $tray->id;

   if($tray->ship) {
      $ship_dates[$tray->ship] += 1;
   }

   $ll = $l % 2;
   echo " <tr class='row$ll'>";
   echo "  <td style='text-align: center'>" . elm_a("./view.php?id={$tray->id}", $tray->sn, "") . "</td>\n";
   if($tray->tray_status->id == 3) {
      echo "  <td style='text-align: center;'>BNL</td>\n";
   } else {
      echo "  <td style='text-align: center;'>UT</td>\n";
   }
   echo "  <td style='white-space: nowrap; text-align: center;'>", tray_stat($tray), "</td>\n";
 
   $notes = $tray->notes;
   if($tray->tray_status->id == 3 && $tray->ship && $notes == '') {
      $notes = "Shipped " . $tray->ship;
   }
   if(Session::loggedin()) {
      if($notes == '') $notes = "edit";
      echo "  <td><a href='./notes.php?id={$id}'>{$notes}</a></td>\n";
   } else {
      echo "  <td style='text-align: center;'>" . $notes . "</td>\n";
   }

   for($i = 0; $i < 5; $i++) {
      if ($tray->ship && $ship_num[$tray->ship] == $i) {
         echo "  <td style='text-align: center;'>", $ship_dates[$tray->ship], "</td>\n";
      } else {
        echo "  <td style='text-align: center;'></td>\n";
      }
   }
   echo "  <td></td>\n";
   $sealed = $tray->sealed_date();
   echo "  <td style='text-align: center;'>{$sealed}</td>\n";
   echo " </tr>";
}
$week_in_sec = 7*24*60*60;

$i = 0;
for($sn = $last_sn + 1; $sn <= 120; $sn++) {
   $dt = $next_mon + $week_in_sec * floor($i / 3);
   if($sn != 118) { $i++; }

   $l += 1;
   $ll = $l % 2;

   echo " <tr class='row$ll'>";
   echo "  <td style='text-align: center'>" . $sn . "</td>\n";
   if($sn == 118) {
      echo "  <td style='text-align: center;'>UT</td>\n";
      echo "  <td style='white-space: nowrap; text-align: center;'>Testing</td>\n";
      echo "  <td style='text-align: center;'>Used for Submarine test</td>\n";
   } else {
      echo "  <td style='text-align: center;'></td>\n";
      echo "  <td style='white-space: nowrap; text-align: center;'></td>\n";
      echo "  <td style='text-align: center;'></td>\n";
   }
   echo "  <td style='text-align: center;'></td>\n";
   echo "  <td style='text-align: center;'></td>\n";
   echo "  <td style='text-align: center;'></td>\n";
   echo "  <td style='text-align: center;'></td>\n";
   echo "  <td style='text-align: center;'></td>\n";

   if($sn == 118) {
      echo "  <td style='text-align: center;'></td>\n";
   } else {
      echo "  <td style='text-align: center;'>" . strftime("%Y-%m-%d", $dt). "</td>\n";
   }
   echo "  <td></td>\n";
   echo " </tr>";
}
?>
</table>
</div><!-- main -->
<?php tsw_footer(); ?>
<?php mysql_close($con); ?>
