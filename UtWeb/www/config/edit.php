<?php
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/session.php");
  include("../../lib/template.php");

  $id = $rvar_id;
  $rc = new RunConfig( $id );

  $session = new Session("..");
  $session->check_login("../config/edit.php?id=$id");

  $q = mysql_query("SELECT serial, id from modules order by serial");
  $mod_list = array();
  $mod_dict = array();
  while($r = mysql_fetch_array($q) ) {
    array_push($mod_list, "'" . $r[0] . "'");
    array_push($mod_dict, "'" . $r[0] . "': " . $r[1]);
  }

  $q = mysql_query("SELECT sn, id from trays order by cast(sn as signed)");
  $try_list = array();
  $try_dict = array();
  while($r = mysql_fetch_array($q) ) {
    array_push($try_list, "'" . $r[0] . "'");
    array_push($try_dict, "'" . $r[0] . "': " . $r[1]);
  }

$hds = "<script language='javascript'>\n";
$hds .= "var mod_list = [ " . join($mod_list, ", ") . "];\n";
$hds .= "var mod_dict = { " . join($mod_dict, ", ") . "};\n";
$hds .= "var try_list = [ " . join($try_list, ", ") . "];\n";
$hds .= "var try_dict = { " . join($try_dict, ", ") . "};\n";
$hds .= "</script>\n";
$hds .= "<script src='../scripts/config_editor.js' type='text/javascript'></script>\n";

$tsw = new Tsw("run_config");
$tsw->set_opt($hds);
$tsw->set_body_opt(" onload='init_edit()'");
$tsw->header();
$tsw->toolbar("config");
?>

<div id='main'>
<div class='title'>RunConfig Editor</div>
<div style="margin: 20px;">
<form
  name='myform'
  action='save.php'
  method='post'
  onsubmit='return verify();'>

<input type='reset' value='Reset'>
<input type='submit' value='Update'>
<table>
 <tr>
  <td>id</td>
  <td><input
    name='id'
    type='text'
    value='<?php echo $rc->id ?>'
    readonly />
  </td>
 </tr>
 
 <tr>
  <td>user</td>
  <td><select name='user_id'>
<?php
  foreach(User::find_all() as $u) {
    if ($rc->user->id == $u->id)
      echo "    <option value='{$u->id}' selected>{$u->name}</option>\n";
    else
      echo "    <option value='{$u->id}'>{$u->name}</option>\n";
  }
?>
  </select></td>
 </tr>

 <tr>
  <td>location</td>
  <td><select name='location_id'>
  <?php Location::options($rc->location->id); ?>
  </select></td>
 </tr>

 <tr>
  <td>created_on</td>
  <td><input
    id='datime1'
    name='created_on'
    type='text'
    value='<?php echo $rc->created_on; ?>'
  />
  </td>
 </tr>
 <tr>
  <td>start</td>
  <td><input
    id='datime2'
    name='start'
    type='text'
    value='<?php echo $rc->start; ?>' />
  </td>
 </tr>
 <tr>
  <td>stop</td>
  <td><input
    id='datime3'
    name='stop'
    type='text'
    value='<?php echo $rc->stop; ?>' />
     <input type='button' value='now' onclick="input_now('datime3');" />
   </td>
 </tr>

 <tr>
  <td>run_config_type</td>
  <td>
   <select name='run_config_type_id' id='config_type'>
 <?php
  foreach(RunConfigType::find_all() as $t) {
    if ($rc->run_config_type->id == $t->id)
      echo "    <option value='{$t->id}' selected>{$t->name}</option>\n";
    else
      echo "    <option value='{$t->id}'>{$t->name}</option>\n";
  }
?>
   </select>
  </td>
 </tr>


 <tr>
  <td>n_modules</td>
  <td><input
    name='n_modules'
    id='n_modules'
    type='text'
    value='<?php echo $rc->n_modules ?>' />
  </td>
 </tr>

 <tr>
  <td>n_pads</td>
  <td><input
    name='n_pads'
    type='text'
    value='<?php echo $rc->n_pads ?>'
    readonly />
  </td>
 </tr>

 <tr>
  <td>threshold</td>
  <td><input
    name='threshold'
    type='text'
    value='<?php echo $rc->threshold; ?>'>
  </td>
 </tr>

 <tr>
  <td>impedance</td>
  <td><input
    name='impedance'
    type='text'
    value='<?php echo $rc->impedance; ?>'
    readonly />
  </td>
 </tr>
 
 <tr>
  <td>hv_plus</td>
  <td>
   ch <input
     name='hv_channel_plus'
     id='hv_channel_plus'
     type='text'
     value='<?echo $rc->hv_channel_plus ?>' size='3'/>
   set <input
     name='hv_plus'
     type='text'
     value='<?php echo $rc->hv_plus; ?>' size='5'/>
  </td>
 </tr>

 <tr>
  <td>hv_minus</td>
  <td>
   ch <input
     name='hv_channel_minus'
     id='hv_channel_minus'
     type='text'
     value='<?echo $rc->hv_channel_minus ?>' size='3'/>
   set <input
     name='hv_minus'
     type='text'
     value='<?php echo $rc->hv_minus; ?>' size='5'/>
  </td>
 </tr>

 <tr>
  <td>flow_rates</td>
  <td>
   ch <input
     name='gas_channel_1'
     id='gas_channel_1'
     type='text'
     value='<?echo $rc->gas_channel_1 ?>' size='3'/>
   set <input
    name='flow_rate1'
    id='flow_rate1'
    type='text'
    value='<?php echo $rc->flow_rate1; ?>' size='5'>
 </tr>
 <tr>
  <td></td>
  <td>
   ch <input
     name='gas_channel_2'
     id='gas_channel_2'
     type='text'
     value='<?echo $rc->gas_channel_2 ?>' size='3'/>
   set <input
    name='flow_rate2'
    id='flow_rate2'
    type='text'
    value='<?php echo $rc->flow_rate2; ?>' size='5'
    readonly />
 </tr>
 <tr>
  <td></td>
  <td>
   ch <input
     name='gas_channel_3'
     id='gas_channel_3'
     type='text'
     value='<?echo $rc->gas_channel_3 ?>' size='3'/>
   set <input
    name='flow_rate3'
    id='flow_rate3'
    type='text'
    value='<?php echo $rc->flow_rate3; ?>' size='5'
    readonly />
  </td>

 <tr>
  <td>note</td>
  <td><input
    name='note'
    type='text'
    value='<?php echo $rc->note; ?>'
    size='80'>
  </td>
 </tr>
 

<tr><td>module_ids</td><td>
<input type='checkbox' id='auto' checked>auto fill
<table border='1'>
<tr><th></th><th>c1</th><th>c2</th><th>c3</th><th>c4</th></tr>
<?php
$ids = $rc->ids_array();
for($i = 1; $i <= 32; $i++) {
  if ($rc->run_config_type->id <= 2) {
    $m = new Module($ids[$i-1]);
  } else {
    $m = new Tray($ids[$i-1]);
  }

  if ($i % 4 == 1) {
    echo "<tr>\n";
    echo "<th>r" .  round($i/4+1, 0) . "</th>\n";
  }
  echo " <td style='text-align: right;'>\n";
  echo "   $i <input type='text' name='$i' id='tx$i' size=10 value='{$m->serial}'>\n";
  echo " </td>\n";
  if ($i % 4 == 0) { echo "</tr>\n"; }
}
?>
</table>

<input type='hidden' name='module_ids' id='module_ids'>
<input type='hidden' name='type' value='update'>
</td>
</tr>
</table>

</form>
</div>
</div><!--main-->
<?php $tsw->footer(); ?>

