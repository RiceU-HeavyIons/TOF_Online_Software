<?php
# $Id: search.php,v 1.1 2009/08/17 17:11:09 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include('../../lib/connect.php');
include('../../lib/orm.php');
include("../../lib/session.php");
include('../../lib/template.php');

$q = $rvar_q;
$q = mysql_real_escape_string($q);

tsw_header('Module Serch');
tsw_toolbar('module');
?>
<div id='main'>
<div class='title'>Component Search</div>
<form method='GET' action='search.php'>
<div>
 serial:
 <input type='text' name='q' value='<?php echo $q; ?>' />
 <input type='submit' name='b' ' value="search" />
</div>
</form>
<div>
<?php
if($q) {
  $r = Component::find_all("serial={$q}", '', '');
  $c = count($r);
  print "<div style='background: #aae; border-top: 2px solid black;'>{$c} records found</div>\n";
  if($c) {
    print "<table id='module_list'>\n";
    print " <tr><th></th><th>Type</th><th>SN</th><th>Note</th></tr>\n";
    $i = 0;
    foreach($r as $mod) {
      $l = $i%2;
      $i++;
      $note = preg_replace("/($q)/i", "<font class='highlight'>$1</font>", $mod->note);
      $link = $mod->p_link();
      $type = $mod->str_type();
      print " <tr class='row${l}'>\n";
      print "  <td>{$i}.</td>\n";
      print "  <td>{$type}</td>\n";
      print "  <td>{$link}</td>\n";
      print "  <td>{$note}</td>\n";
      print " </tr>\n";
    }
    print "</table>\n";
  }
}
?>
</div>
</div><!-- main -->
<?php
tsw_footer();
?>
