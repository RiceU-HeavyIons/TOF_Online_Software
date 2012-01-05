<?php
  include_once("../../lib/connect.php");
  include_once("../../lib/orm.php");
  include_once("../../lib/session.php");
  include_once("../../lib/template.php");


  $session = new Session("..");
  $session->check_login('../config/new.php');

  $q = mysql_query("SELECT serial, id from modules where tray_id is null order by serial");
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
$tsw->set_body_opt(" onload='init_new()'");
$tsw->header();
$tsw->toolbar("config");
?>

<div id='main'>
<div class='title'>New RunConfig</div>
<div style="margin: 20px;">
<form
  name='myform'
  action='save.php'
  method='post'
  onsubmit='return verify();'>
<input type='reset' value='Reset'>
<input type='submit' value='Create'>
<table>
 <tr>
  <td>user</td>
  <td>
   <select name='user_id'>
    <?php User::options(108); ?>
   </select>
  </td>
 </tr>

 <tr>
  <td>location</td>
  <td><select name='location_id'>
  <?php Location::options(1); ?>
  </select></td>
 </tr>

 <?php $tm = strftime("%Y-%m-%d %T", time()); ?>
 <tr>
  <td>created_on</td>
  <td><input
    id='datime1'
    name='created_on'
    type='text'
    value='<?php echo $tm; ?>'
  />
  </td>
 </tr>

 <tr>
  <td>start</td>
  <td><input
    id='datime2'
    name='start'
    type='text'
    value='<?php echo $tm; ?>' />
  </td>
 </tr>

 <?php $tm = strftime("%Y-%m-%d %T", time() + 3600); ?>
 <tr>
  <td>stop</td>
  <td><input
    id='datime3'
    name='stop'
    id='stop'
    type='text'
    value='<?php echo $tm; ?>' />
    <input type='button' value='now' onclick="input_now('datime3');" />
  </td>
 </tr>

 <td>run_config_type</td>
  <td>
   <select name='run_config_type_id' id='config_type'>
    <?php RunConfigType::options(2); ?>
   </select>
  </td>
 </tr>


 <tr>
  <td>n_modules</td>
  <td><input
    name='n_modules'
    id='n_modules'
    type='text'
    value='32' />
  </td>
 </tr>

 <tr>
  <td>n_pads</td>
  <td><input
    name='n_pads'
    type='text'
    value='6'
    readonly />
  </td>
 </tr>

 <tr>
  <td>threshold</td>
  <td><input
    name='threshold'
    type='text'
    value='1500'>
  </td>
 </tr>

 <tr>
  <td>impedance</td>
  <td><input
    name='impedance'
    type='text'
    value='0'
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
     value='5' size='3'/>
   set <input
     name='hv_plus'
     type='text'
     value='7000' size='5'/>
  </td>
 </tr>

 <tr>
  <td>hv_minus</td>
  <td>
   ch <input
     name='hv_channel_minus'
     id='hv_channel_minus'
     type='text'
     value='1' size='3'/>
   set <input
     name='hv_minus'
     type='text'
     value='7000' size='5'/>
  </td>
 </tr>

 <tr>
  <td>flow_rates</td>
  <td>
   ch <input
    name='gas_channel_1'
    id='gas_channel_1'
    type='text'
    size='3'
    value='0'>
   set <input
    name='flow_rate1'
    id='flow_rate1'
    type='text'
    size='5'
    value='40.0'>
  </td>
 </tr>
 <tr><td></td>
  <td>
   ch <input
    name='gas_channel_2'
    id='gas_channel_2'
    type='text'
    size='3'
    value='0'
    readonly />
   set <input
    name='flow_rate2'
    id='flow_rate2'
    type='text'
    size='5'
    value='0.0'
    readonly />
   </td>
 </tr>
 <tr><td></td>
  <td>
   ch <input
    name='gas_channel_3'
    id='gas_channel_3'
    type='text'
    size='3'
    value='0'
    readonly />
   set <input
    name='flow_rate3'
    id='flow_rate3'
    type='text'
    size='5'
    value='0.0'
    readonly />
  </td>
 </tr>

 <tr>
  <td>note</td>
  <td><input
    name='note'
    type='text'
    value=''
    size='80'>
  </td>
 </tr>
 

<tr><td>module_ids</td><td>
<input type='checkbox' id='auto' checked>auto fill
<table border=1>
<tr><th></th><th>c1</th><th>c2</th><th>c3</th><th>c4</th></tr>
<?php
for($i = 1; $i <= 32; $i++) {
  if ($i % 4 == 1) {
    echo "<tr>\n";
    echo "<th>r" .  round($i/4+1, 0) . "</th>\n";
  }
  echo " <td style='text-align: right;'>\n";
  echo "   $i <input type='text' name='$i' id='tx$i' size=10>\n";
  echo " </td>\n";
  if ($i % 4 == 0) { echo "</tr>\n"; }
}
?>
</table>
<input type='hidden' name='module_ids' id='module_ids'>
<input type='hidden' name='type' value='create'>
</td>
</tr>
</table>

</form>
</div>
</div><!-- main -->
</body>
</html>

