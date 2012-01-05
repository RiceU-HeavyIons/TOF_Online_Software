<?php
# $Id: bad.php,v 1.6 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Andrew Oldag. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");
tsw_header("List of Bad Modules");
tsw_toolbar('module');
?>
<div id="main">
<div class="title">Bad Modules</div>
<div class="section">
<div class="title">Timing Resolution or Noise Rate Failure</div>
<div>
<?php
$tr_limit = isset($rvar_tr)? $rvar_tr : 0;
$nr_limit = isset($rvar_nr)? $rvar_nr : 0;
if($tr_limit <= 0) $tr_limit = 120.00;
if($nr_limit <= 0) $nr_limit = 50.00;
threshold_form($tr_limit, $nr_limit, 'bad.php');
?>
<table>
<tr>
  <th></th>
  <th>S/N</th>
  <th>Tray</th>
  <th>T.R.</th>
  <th>N.R.</th>
</tr>
<?php
$cnt = 1;
foreach(Module::find_by_threshold($tr_limit, $nr_limit) as $mod) {
	$tr = $mod->max_tr();
	$nr = $mod->max_nr();
	if($tray = $mod->tray) $tray = $tray->p_link();
    echo "<tr class='row", $cnt%2, "'>\n";
    echo "  <td>", $cnt++, ".</td>\n";
    echo "  <td>", $mod->p_link(),"</td>\n";
    echo "  <td style='text-align: right;'>", $tray , "</td>\n";
    if ($tr > $tr_limit)
    	printf("  <td style='text-align: right; color: red;'>%5.1f ps</td>\n", $tr);
    else
    	echo "  <td></td>\n";
    if ($nr > $nr_limit)
    	printf("  <td style='text-align: right; color: red;'>%5.1f Hz</td>\n", $nr);
    else
    	echo "  <td></td>\n";
    echo "</tr>\n";
}
?>
</table></div>
</div> <!-- section -->
<div class="section">
<div class="title">HV Test Failure</div>
<div><table>
<?php
$cnt = 1;
$list=mysql_query("select m.serial as serial, m.id as id, h.pass as pass from modules m inner join hvtest h on m.id=h.id where h.pass=0 order by m.serial asc");
while($l=mysql_fetch_object($list)){
  $id=$l->id;
  $ser=$l->serial;
  $pass=$l->pass;
  echo "<tr>\n";
  echo "  <td>", $cnt++, "</td>\n";
  echo "  <td>","<a href=\"../module/show.php?id=",$id,"\">",$ser,"</a></td><td><font color='red'> Fail</td>\n";
  echo "</tr>\n";
}
?>
</table></div>
</div><!-- section -->

<div class="section">
<div class="title">Dimensional Failure</div>
<div><table>
<?php
$cnt = 1;
$list=mysql_query("select m.serial as serial,m.id as id, s.jig_passed as pass, s.note as note from modules m inner join module_sizes s on s.id=m.id where s.jig_passed='0'");
while($l=mysql_fetch_object($list)){
  $id=$l->id;
  $ser=$l->serial;
  $p=$l->pass;
  $n=$l->note;
  echo "<tr>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>","<a href=\"../module/show.php?id=",$id,"\">",$ser,"</a></td>\n";
  echo "  <td>",$n,"</td>\n";
  echo "</tr>\n";
}
?>
</table></div>
</div><!-- section -->

<div class="section">
<div class="title">Continuity Failure</div>
<div>
<table>
<?php
$cnt = 1;
$list=mysql_query("select m.serial as serial, m.id as id,  s.continuity as cont, s.note as note from modules m inner join module_sizes s on s.id=m.id where s.continuity='0'");
while($l=mysql_fetch_object($list)){
  $id=$l->id;
  $ser=$l->serial;
  $c=$l->cont;
  $n=$l->note;
  echo "<tr>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>","<a href=\"../module/show.php?id=",$id,"\">",$ser,"</a></td>\n";
  echo "  <td>",$n,"</td>\n";
  echo "</tr>\n";
}
?>
</table>
</div>
</div><!-- section -->

<div class="section">
<div class="title">Modules with Notes Attached</div>
<div>
<table>
<?php
$cnt = 1;
$list=mysql_query("select * from modules where note!='' order by serial asc");
while ($l=mysql_fetch_object($list)){
  $id=$l->id;
  $ser=$l->serial;
  $n=$l->note;
  echo "<tr>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>","<a href=\"../module/show.php?id=",$id,"\">",$ser,"</a></td>\n";
  echo "  <td>",$n,"</td>\n";
  echo "</tr>\n";
}
?>
</table>
</div>
</div><!-- section -->

<?php tsw_footer(); ?>
