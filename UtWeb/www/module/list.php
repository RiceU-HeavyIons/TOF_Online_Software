<?php
# $Id: list.php,v 1.52 2008/06/05 17:26:35 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto, Andrew Oldag. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header("Module List");
tsw_toolbar('module');
?>
<div id="main">
<div class="title">Module List</div>
<div><a href="#" onclick="var s = getElementById('option').style; if(s.display == 'none') {s.display = 'inline'} else {s.display = 'none'}; return false;">option</a></div>
<div id="option" style="display: none;">
<table>
<tr>
 <form  action="show.php" method="get">
 <td>Serial Number:</td>
 <td><input name="sn" type="text" /></td>
 <td><input name="commit" type="submit" value="find" /></td>
 </form>
</tr>
<tr>
 <form method='GET' action='search.php'>
 <td>Keyword:</td>
 <td><input type='text' name='q' value='' /></td>
 <td><input type='submit' name='b' value="search" /></td>
 </form>
</tr>
</table>
<?php
function tostr(&$ar) {
	$s = '';
	foreach ($ar as $k => $v) {
		$s .= "&{$k}={$v}";
	}
	return $s;
}

$opt = array();
$lim_tr = isset($rvar_tr)? $rvar_tr : 0;
$lim_nr = isset($rvar_nr)? $rvar_nr : 0;
if($lim_tr <= 0) $lim_tr = 120.00;
if($lim_nr <= 0) $lim_nr = 50.00;
if($lim_tr) { $opt["tr"] = $lim_tr; }
if($lim_nr) { $opt["nr"] = $lim_nr; }
$srt = '';
$ky = isset($rvar_k)? $rvar_k : "id";
if($ky == 'tray') {
	$opt["k"] = 'tray';
	$srt = 'tray_id';
} elseif($ky == 'sn') {
	$opt["k"] = 'sn';
	$srt = 'serial';
} else {
	$opt["k"] = 'id';
	$srt = 'id';
}
$di = isset($rvar_d)? $rvar_d : "id";
if($di == 'desc') {
	$opt["d"] = 'desc';
	$srt = $srt . ' desc';
} elseif($di== 'asc') {
	$opt["d"] = 'asc';
	$srt = $srt . ' asc';
} else {
	$opt["d"] = null;
}
threshold_form($lim_tr, $lim_nr, 'list.php');
?>
</div><!-- option -->
<?php

$tot=Module::count("");
$lim = 32;
$off = 0;
$page = isset($rvar_page)? $rvar_page : 0;
if($page <= 0) $page = 1;
if($page > $tot/$lim) $page = ceil($tot/$lim);
$off = $lim*($page - 1);
$opt_org = $opt;
if($tot > $lim) { pagenate("list.php", $page, $tot, $lim, tostr($opt_org)); }
?>
<table id="module_list">
<tr>
<!-- <th>id</th> -->
 <th></th>
 <th>ID
   <a href='list.php?<?php $opt["k"]="id"; $opt["d"]="asc"; echo "page=$page" . tostr($opt); ?>'>&and;</a>
   <a href='list.php?<?php $opt["k"]="id"; $opt["d"]="desc"; echo "page=$page" . tostr($opt); ?>'>&or;</a>
 </th>
 <th>S/N
  <a href='list.php?<?php $opt["k"]="sn"; $opt["d"]="asc"; echo "page=$page" . tostr($opt); ?>'>&and;</a>
  <a href='list.php?<?php $opt["k"]="sn"; $opt["d"]="desc"; echo "page=$page" . tostr($opt); ?>'>&or;</a>
 </th>
 <th>ID(USTC)</th>
 <th>Tray
  <a href='list.php?<?php $opt["k"]="tray"; $opt["d"]="asc"; echo "page=$page" . tostr($opt); ?>'>&and;</a>
  <a href='list.php?<?php $opt["k"]="tray"; $opt["d"]="desc"; echo "page=$page" . tostr($opt); ?>'>&or;</a>
 </th>
 <th>Received On</th>
<!--   <th>Received By</th> -->
 <th>max tr.</th>
 <th>max nr.</th>
 <th>max tr.(chn)</th>
 <th>max nr.(chn)</th>
 <th>size</th>
 <th>hvtest</th>
 <th>Note</th>
</tr>

<?php
$l = 0;
foreach(Module::find_all("", $srt, "$off, $lim") as $mod) {
   $l += 1;
   $ll = $l % 2;
   echo " <tr class='row$ll'>\n";
   echo "  <td class='int'>$l.</td>\n";
   echo "  <td class='int'>", $mod->id, "</td>\n";
   echo "  <td>", $mod->p_link(), "</td>\n";
   echo "  <td class='int'>", mk_link($mod->chinese_id, $mod->chinese_link), "</td>\n";
   echo "   <td class='int'>" . $mod->tray->p_link() . "</td>\n";
   echo "  <td>", $mod->received_on, "</td>\n";
#   echo "  <td>", $mod->user->name, "</td>\n";
   if( ($tr = $mod->max_tr()) == 0.0) {
     echo "    <td></td>\n";
   } else if( $tr < $lim_tr) {
     printf("  <td style='text-align: right;'>%5.1f ps</td>\n", $tr);
   } else {
     printf("  <td style='text-align: right; color: red;'>%5.1f ps</td>\n", $tr);
   }
   if( ($nr = $mod->max_nr()) == 0.0) {
   	 echo "    <td></td>\n";
   } else if( $nr < $lim_nr) {
     printf("  <td style='text-align: right;'>%5.1f Hz</td>\n", $nr);
   } else {
     printf("  <td style='text-align: right; color: red;'>%5.1f Hz</td>\n", $nr);
   }
   echo "  <td style='text-align: right;'>" . sprintf("%d ps", $mod->max_chinese_tr()) . "</td>\n";
   echo "  <td style='text-align: right;'>" . sprintf("%5.1f Hz", $mod->max_chinese_nr()) . "</td>\n";
   echo "  <td class='int'>" . $mod->p_size_tested() . "\n";
   echo "  <td>" . $mod->p_hvtest() . "</td>\n";
   echo "  <td>" . $mod->p_note() . "</td>\n";
#   echo "  <td><a href='show.php?id=", $r1->id, "'>show</td>\n";
  echo " </tr>\n";
 }
echo "</table>\n";
if($tot > $lim) { pagenate("list.php", $page, $tot, $lim, tostr($opt_org)); }
?>
</div> <!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
