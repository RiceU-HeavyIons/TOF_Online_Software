<?php
# $Id: create.php,v 1.3 2012/01/05 17:25:10 tofp Exp $
include("../../lib/connect.php");
include("../../lib/session.php");
Session::check_login("../tray/component.php");

#$id = $rvar_id;
function back() {}
include("../../lib/template.php");
tsw_header('Component Information Insertion Page');
tsw_toolbar('component');
?>
<div id="main">
<div class='title'>Component Creation</div>
<form name='new_comp' action='./update.php' method='post'>
<input name='request_type' type='hidden' value='new_component'/>
<table>
 <tr>
  <td>Select Type:<br />
   <select name='type'>
    <option>TCPU</option>
    <option>TDIG</option>
    <option>TINO</option>
    <option>MINO</option>
    <option>MTRIG</option>
   </select>
  </td>
  <td>Arrival Date:<br />
   <input name='arrival_date' type='text' value='YYYY-MM-DD' size='11' onfocus='this.value="";' />
  </td>
 </tr>
 <tr>
  <td colspan='4'>Serial Number(s):<br />
   <input name='serial' type='text' size='35' value='Comma delimited entries'/>
  </td>
 </tr>
 <tr>
  <td colspan='4'>Notes:<br />
   <textarea name='text' rows='5' cols='35' type='text'/></textarea>
  </td>
 </tr>
 <tr></tr>
  <td colspan='4'>
   <input name='new_comp' type='submit' value='Insert this information into database'/>
  </td>
 </tr>
</table>

<p>Once this information is inserted into the database the new component can be selected to be inserted into a new or existing tray by editing the contents of a tray. To edit the contents click 'Show Contents' for a tray in the list and then click 'Edit Contents' on the resulting page. Details and notes about the component can be edited once it has been 'inserted' into a tray. Clicking on the component serial number will allow you to edit the database entry. </p>
<?php tsw_footer(); ?>
<?php mysql_close($con) ?>
