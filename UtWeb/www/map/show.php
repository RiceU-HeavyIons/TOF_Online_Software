<?php
# $Id: show.php,v 1.2 2012/01/05 17:25:31 tofp Exp $
# Copyright (C) 2007, 2008, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");
include("../../lib/utility.php");

$map = new Map($rvar_id, $con);
  tsw_header("Map List");
  tsw_toolbar('map');
?>
<div id="main">
<div class="title">Map List</div>
<div>[<a href="show_in_text.php?id=<?echo $map->id;?>">Text Format</a>]</div>
<table style="border: solid 1px #999">
 <tr class="row0">
  <th>id</th>
  <td><?php echo $map->id; ?></td>
 </tr>
 <tr class="row1">
   <th>Date</th>
   <td><?php echo $map->created_at; ?></td>
 </tr>
 <tr class="row0">
   <th>User</th>
   <td><?php echo $map->user->name; ?></td>
 </tr>
 <tr class="row1">
   <th>Name</th>
   <td><?php echo $map->name; ?></td>
 </tr>
 <tr class="row0">
   <th>Note</th>
   <td><?php echo $map->note; ?></td>
 </tr>
</table>
<table id="maptray_list">
<tr>
 <th>Position</th>
 <th>Tray SN</th>
 <th>TCPU</th>
 <th>TDIG 1</th>
 <th>TDIG 2</th>
 <th>TDIG 3</th>
 <th>TDIG 4</th>
 <th>TDIG 5</th>
 <th>TDIG 6</th>
 <th>TDIG 7</th>
 <th>TDIG 8</th>
 <th>Note</th>
</tr>

<?php
$l = 1;
foreach (MapTray::find_all("map_id={$map->id}", "position", "0, 120") as $mt) {
   $l += 1;
   $tray = new Tray($mt->tray_id);
   $tcpu = new Component($mt->tcpu_id);
   $td = Array();
   for($i = 0; $i < 8; $i++) {
     array_push($td, new Component($mt->tdig_id[$i]));
   }

   $ll = $l % 2;
   echo " <tr class='row$ll'>";
   echo "  <td style='text-align: right;'>", $mt->position, "</td>\n";
   echo "  <td style='text-align: center;'>", $tray->p_link(), "</td>\n";
   echo "  <td style='text-align: center;'>", $tcpu->p_link(), "</td>\n";
   for($i=0; $i < 8; $i++) {
     echo "  <td style='text-align: center;'>", $td[$i]->p_link(), "</td>\n";
   }
   echo "  <td style='text-align: left;'>", $mt->note, "</td>\n";
   echo " </tr>";
 }

?>
</table>
</div> <!-- id=main -->
<?php 
  tsw_footer(false);
  mysql_close($con);
?>

