<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");
include("../../lib/utility.php");

tsw_header('Component Summary');
tsw_toolbar('component');
?>
<div id='main'>
<div class='title'>Component Summary</div>
<table>
 <tr>
  <th>kind</th>
  <th></th>
  <th>stock</th>
  <th>installed</th>
  <th>sub total</th>
  <th>total</th>
 </tr>
 <tr class='row0'>
  <td>TCPU</td>
  <td></td>
  <td><?php echo Component::count("component_type_id=1 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=1 and tray_id > 0"); ?></td>
  <td></td>
  <td><?php echo Component::count("component_type_id=1"); ?></td>
 </tr>
 <tr class='row1'>
  <td></td>
  <td>TCPU-C</td>
  <td><?php echo Component::count("component_type_id=1 and serial >= 18 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=1 and serial >= 18 and tray_id > 0"); ?></td>
  <td><?php echo Component::count("component_type_id=1 and serial >= 18"); ?></td>
  <td></td>
 </tr>
 <tr class='row0'>
  <td></td>
  <td>TCPU-B</td>
  <td><?php echo Component::count("component_type_id=1 and serial < 18 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=1 and serial < 18 and tray_id > 0"); ?></td>
  <td><?php echo Component::count("component_type_id=1 and serial < 18"); ?></td>
  <td></td>
 </tr>
 <tr class='row1'>
  <td>TDIG</td>
  <td></td>
  <td><?php echo Component::count("component_type_id=2 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and tray_id > 0"); ?></td>
  <td></td>
  <td><?php echo Component::count("component_type_id=2"); ?></td>
 </tr>
 <tr class='row0'>
  <td></td>
  <td>REV-D</td>
  <td><?php echo Component::count("component_type_id=2 and serial < 21 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial < 21 and tray_id > 0"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial < 21"); ?></td>
  <td></td>
 </tr>
 <tr class='row1'>
  <td></td>
  <td>REV-E</td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 21 and serial < 38 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 21 and serial < 38 and tray_id > 0"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 21 and serial < 38"); ?></td>
  <td></td>
 </tr>
 <tr class='row0'>
  <td></td>
  <td>REV-F</td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 38 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 38 and tray_id > 0"); ?></td>
  <td><?php echo Component::count("component_type_id=2 and serial >= 38"); ?></td>
  <td></td>
 </tr>
 <tr class='row1'>
  <td>TINO</td>
  <td></td>
  <td><?php echo Component::count("component_type_id=3 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=3 and tray_id > 0"); ?></td>
  <td></td>
  <td><?php echo Component::count("component_type_id=3"); ?></td>
 </tr>
 
 <tr class='row0'>
  <td>MINO</td>
  <td></td>
  <td><?php echo Component::count("component_type_id=7 and tray_id is null"); ?></td>
  <td><?php echo Component::count("component_type_id=7 and tray_id > 0"); ?></td>
  <td></td>
  <td><?php echo Component::count("component_type_id=7"); ?></td>
 </tr>
 
 <tr class='row1'>
  <td>all</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td><?php echo Component::count(""); ?></td>
 </tr>
</table>
</div><!-- main -->
<?php
tsw_footer();
?>
