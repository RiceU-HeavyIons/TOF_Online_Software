<?php
# $Id: ship.php,v 1.1 2012/01/05 17:29:04 tofp Exp $
include("../../lib/connect.php");
include("../../lib/session.php");

$id=$_GET['id']; 
$serial1=mysql_query("select sn from trays where id=$id", $con);
$s1=mysql_fetch_object($serial1);

Session::check_login("../tray/ship.php?id=$id");


function cancel($id){
  echo "<form name='cancel' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='request_type' type='hidden' value='cancel_shipment'/>";
  echo "<input name='cancel' type='submit' value='Cancel shipment date' /></form>\n";
}

function newshipdate($id){
  echo "<form name='new_ship_date' action='update.php' method='post'/>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='date' type='text' value='YYYY-MM-DD' size='11' onfocus='this.value=\"\";' />";
  echo "<input name='request_type' type='hidden', value='new_ship_date'/>";
  echo "<input name='new_date' type='submit' value='Create new shipment date' /></form>\n";
}

include("../../lib/template.php");
tsw_header("Tray {$s1->sn} Shipment Update");
tsw_toolbar('tray');
?>
<div id="main">
<div class="title">Update Tray <?php echo $s1->sn ?> Shipment Information </div>
<?php
$result1=mysql_query("select ship from trays where id=$id", $con);
$r1=mysql_fetch_object($result1);

  if($r1->ship == ''){
    echo "<table><tr><td>";
    echo "A shipment date has not been set for this tray.</td></tr></table><br>\n";}
  else{
    echo "<table>";
    echo "<tr><td>The current ship date is set to: ", $r1->ship, "</td></tr>\n";
    echo "<tr><td>To cancel the shipment and set tray status to 'In Storage': </td></tr>\n";
    echo "<tr><td>", cancel($id), "</td></tr>\n";
    echo "</table>";}
  echo "<table><tr><td>To set the shipment date for this tray input the date in the form YYYY-MM-DD.</td></tr>";
  echo "<tr><td>", newshipdate($id), "</td></tr>\n";
?>
</table>
<?php tsw_footer(); ?>

