<?php echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<!-- $Id$ -->
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<?php
include("../../lib/connect.php");
include("../../lib/session.php");
include("../../lib/template.php");
?>
<div id="main">
<div class="title"><?php echo "Tray-Slot-TDIG List" ?></div>

<?php
$result=mysql_query("select t.sn as  tsn, c.slot as cslot, c.serial as cserial from trays t join components c on t.id = c.tray_id where t.tray_status_id = 3 and c.component_type_id =2 order by t.id, c.slot", $con);
echo "sn &nbsp slot &nbsp &nbsp  serial<br>\n";
while ($r_tray=mysql_fetch_object($result)){
  echo $r_tray->tsn, "&nbsp &nbsp &nbsp ",$r_tray->cslot,  "&nbsp &nbsp &nbsp", $r_tray->cserial, "<br>\n";

}
mysql_close($con)
?>
