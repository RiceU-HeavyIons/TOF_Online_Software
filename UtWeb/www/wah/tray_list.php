<?php
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  header("Content-type: text/plain");

function comp_tray($a, $b) {
  return ((int)$a->sn) < ((int)$b->sn);
}

function comp_slot($a, $b) {
    return $a->slot < $b->slot;
}

$trs = Tray::find_all("id >= 1", 'id', '');
# uksort($trs, "comp_tray");
foreach($trs as $t) {
  $ar = $t->tdig;
  uksort($ar, "comp_slot");
  $chip_sn = $t->tcpu->chip_sn;
  if ($chip_sn == '') { continue; }
  $tray_sn = $t->sn;
  if ('A' <= $tray_sn) { $tray_sn = 123 + ord($tray_sn) - ord('@'); }
  echo $t->id . " " . $tray_sn . " 0x" . $chip_sn . "\n";
}
?>
