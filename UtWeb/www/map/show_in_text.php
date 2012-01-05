<?php
# $Id: show_in_text.php,v 1.2 2009/07/20 22:27:42 tofp Exp $
# Copyright (C) 2007, 2008, Kohei Kajimoto. All rights reserved.
header("Content-type: text/plain");

include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/utility.php");

$map = new Map($rvar_id, $con);

?>
# Date: <?php echo $map->created_at, "\n"; ?>
# User: <?php echo $map->user->name, "\n"; ?>
# Name: <?php echo $map->name, "\n"; ?>
# Note: <?php echo $map->note, "\n"; ?>
#
#    Pos  TraySN    TCPU   TDIG1   TDIG2   TDIG3   TDIG4   TDIG5   TDIG6   TDIG7   TDIG8
<?php
foreach (MapTray::find_all("map_id={$map->id}", "position", "0, 100") as $mt) {
   $tray = new Tray($mt->tray_id);
   $tcpu = new Component($mt->tcpu_id);
   $td = Array();
   for($i = 0; $i < 8; $i++) {
     array_push($td, new Component($mt->tdig_id[$i]));
   }

   $ll = $l % 2;
   printf("%8d", $mt->position);
   printf("%8d", $tray->sn);
   printf("%8d", $tcpu->sn);
   for($i=0; $i < 8; $i++) {
      printf("%8d", $td[$i]->sn);
   }
#   print " # ", $mt->note;
   print("\n");
 }

?>

