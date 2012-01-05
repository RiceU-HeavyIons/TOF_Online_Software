<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");
import_request_variables("gP", "rvar_");

$tray=new Tray($rvar_id);
$sn=$tray->sn;
mysql_close($con);

$con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
mysql_select_db("toftstand", $con);

tsw_header("Documents for Tray " . $sn);
tsw_toolbar("tray");
?>
<div id="main">
<div class="title">Documents for Tray <?php echo $sn ?> </div>
<table><tr><td>
<form action='list.php' method='post'>
 <input name='back' type='submit' value='Back' />
</form></td>
<td>
<?php
$s = sprintf("'%%[tray%03s]%%'", $sn);
$c = mysql_query("SELECT count(*) as count from xoops_dms_objects where obj_name like $s", $con);
$count = mysql_fetch_object($c);
echo "<a href='http://www.rhip.utexas.edu/~tofp/xoops/modules/dms/'>Upload a document to this list here.</a>(Login required)";
echo "<br><small>Be sure that the uploaded file name begins with '[tray###]'.<br> Otherwise it will not be found with the query.</small><br>";
if ($count->count != 0){
echo "</td></tr></table><table><tr>";
echo "<th>File Name</th>";
echo "<th>File Description</th>";
 echo "<th>Timestamp</th><tr>";}
else {echo "<br>No documents found.";}
?>
<?php
$s = sprintf("'%%[tray%03s]%%'", $sn);
$q = mysql_query("SELECT * from xoops_dms_objects where obj_name like $s", $con);

$row = 0;
while($r = mysql_fetch_object($q))
{
	$row = ($row+1)%2;
	$oid = $r->obj_id;
	$orn = $r->obj_name;
	$subq = mysql_query("SELECT * from xoops_dms_object_versions where obj_id=$oid order by major_version desc, minor_version desc, sub_minor_version desc limit 1", $con);
	$subr = mysql_fetch_object($subq);

	echo " <tr class='row$row'>\n";
	echo "  <td><a href='./file.php?id=".$r->obj_id."'>".$subr->file_name."</td></a>\n";
	echo "  <td>".$r->obj_name."</td>\n";
	echo "  <td>".date("Y-m-d H:i:s T", $subr->time_stamp)."</td>\n";
	echo " </tr>\n";
}
?>
</table>
</div><!-- main -->
<?php tsw_footer() ?>
