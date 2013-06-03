<?php
# $Id: show.php,v 1.4 2012/01/05 17:25:10 tofp Exp $
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");

$id = $rvar_id;
function back() {}
tsw_header('Component Information Page');
tsw_toolbar('component');

$c = Component::find_one("id=$id");
$comp_type = $c->component_type_id;
$date = $c->date;
if($date==""){$date='Unknown';}

?>
<div id="main">
<div class='title'>Component  Information: ID = <?php echo $c->id ?> </div>
<form name='=edit_comp' action='' method='post'>
<input name='request_type' type='hidden' value='edit_component'>
<input name='comp_id' type='hidden' value=<?php echo $id ?>>
<table>
 <tr>
  <td>Select Type:<br />
   <select name='type'>
<?php 
if($comp_type==1){echo "<option selected>TCPU</option>";}
else{echo "<option>TCPU</option>";}
if($comp_type==5){echo "<option selected>TTRIG</option>";}
else{echo "<option>TTRIG</option>";}
if($comp_type==2){echo "<option selected>TDIG</option>";}
else{echo "<option>TDIG</option>";}
if($comp_type==3){echo "<option selected>TINO</option>";}
else{echo "<option>TINO</option>";}
if($comp_type==7){echo "<option selected>MINO</option>";}
else{echo "<option>MINO</option>";}
if($comp_type==8){echo "<option selected>MTRIG</option>";}
else{echo "<option>MTRIG</option>";}
?>
   </select>
  </td>
  <td>Arrival Date:<br />
   <input name='arrival_date' type='text' size='15' value=<?php echo $date ?>>
  </td>
 </tr>

 <tr>
  <td>Serial Number:<br />
   <input name='serial' type='text' size='10' value=<?php echo $c->serial ?>>
  </td>
 </tr>

 <tr>
  <td colspan='4'>Chip SN:<br />
   <input name='chip_sn' type='text' size='20' value=<?php echo $c->chip_sn ?>>
  </td>
 </tr>

 <tr>
  <td>Tray ID:<br />
   <a href='../tray/view.php?id=<?php echo $c->tray_id ?>'> <?php echo $c->tray_id ?> </a>
  </td>
  <td>Slot:<br />
   <?php echo $c->slot ?>
  </td>
 </tr>

 <tr>
  <td colspan='4'>Notes:<br />
   <textarea name='text' rows='5' cols='35'><?php echo $c->note ?></textarea>
  </td>

 </tr>
</table>

<?php tsw_footer(); ?>
<?php mysql_close($con) ?>
