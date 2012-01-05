<?php
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  include("../../lib/session.php");
  include("../../lib/template.php");

  $id = $rvar_id;
  $r = new Run( $id );
  Session::check_login("../run/edit.php?id=$id");
  tsw_header("Run Edit");
  tsw_toolbar('run');
?>
<script language='javascript'>
function verify() {}
</script>
<div id='main'>
<div class='title'>Run <?php echo $r->id; ?></div>
<form
  name='run'
  action='save.php'
  method='post'
  onsubmit='return verify();'>

<table>
 <tr>
  <td>id</td>
  <td><input
    name='id'
    type='text'
    value='<?php echo $r->id; ?>'
    readonly />
  </td>
 </tr>

 <tr>
  <td>status</td>
  <td><input
    name='run_status_id'
    type='text'
    value='<?php echo $r->status->id; ?>' />
  </td>
 </tr>

 <tr>
  <td>type</td>
  <td><input
    name='run_type_id'
    type='text'
    value='<?php echo $r->type->id; ?>'
    readonly />
  </td>
 </tr>
 
 <tr>
  <td>config</td>
  <td><input
    name='run_config_id'
    type='text'
    value='<?php echo $r->config->id; ?>'
    readonly />
  </td>
 </tr>

 <tr>
  <td>user</td>
  <td><select name='user_id'>
   <?php User::options($r->user->id); ?>
  </select></td>
 </tr>

 <tr>
  <td>start</td>
  <td><input
    name='start'
    type='text'
    value='<?php echo $r->start; ?>' />
  </td>
 </tr>
 <tr>
  <td>stop</td>
  <td><input
    name='stop'
    type='text'
    value='<?php echo $r->stop; ?>' />
  </td>
 </tr>
 <tr>

 <tr>
  <td>run_time</td>
  <td><input
    name='run_time'
    type='text'
    value='<?php echo $r->run_time; ?>' />
  </td>
 </tr>
 <tr>

 <tr>
  <td>events</td>
  <td><input
    name='events'
    type='text'
    value='<?php echo $r->events; ?>' />
  </td>
 </tr>
 <tr>
 
  <td>data_uri</td>
  <td><input
    name='data_uri'
    type='text'
    size='50'
    value='<?php echo $r->data_uri; ?>' />
  </td>
 <tr>
  <td>note</td>
  <td>
   <textarea
    name='note'
    rows='5'
    cols='50'
    type='text'><?php echo $r->note; ?></textarea>
  </td>
 </tr>
</table>
<input type='hidden' name='type' value='update'>
<input type='reset' value='Reset'>
<input type='submit' value='Update'>
</form>
</div><!-- main -->
<?
  tsw_footer();
  mysql_close($con);
?>
