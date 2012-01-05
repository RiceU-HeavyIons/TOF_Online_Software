<?php
include("../../lib/connect.php");
include("../../lib/session.php");

function newcolumn($id){
  echo "<form name='new_col' action='update.php' method='post'>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='data_name' type='text' value='ex. Weight (lbs)' onfocus='page.grabby($id);' />";
  echo "<input name='new_value' type='text' value='ex. 75' onfocus='this.value=\"\";' />";
  echo "<input name='request_type' type='hidden', value='new_col'/>";
  echo "<input name='new_col' type='submit' value='Insert into database' />\n";
  echo "</form>\n";
}

function grabby($id){
  $result999=mysql_query("select count(*) as count from trays_info where tray_id=$id", $con);
  $r999=mysql_fetch_object($result999);
  echo "\n\n",$r999->count,"\n\n";


}

$id=$_REQUEST['id'];
$serial1=mysql_query("select sn from trays where id=$id", $con);
$s1=mysql_fetch_object($serial1);

Session::update_ref("../tray/info.php?id=$id");
include("../../lib/template.php");
tsw_header("Tray ${$s1->sn} General Information Page");
tsw_toolbar('tray');
?>

<div id="main">
<div class="title"> Tray <?php echo $s1->sn ?> Short Notes Page</div>
<?php
if(!Session::loggedin()){
  echo "Please login to add short notes.\n";
  Session::login_button();
}

?>
<table id='page'><tr><td>
<?php
$count1=mysql_query("select count(*) as count from tray_info where tray_id=$id", $con);
$c1=mysql_fetch_object($count1);

if($c1->count != '0') {
  $result=mysql_query("select * from tray_info where tray_id=$id order by date DESC", $con);
  echo "<table id='info'><tr>";
  echo "<th>Data Type</th><th>Value</th><th>Date Inserted</th>";
  echo "</tr>\n";
  $l=1;
  while($r1 = mysql_fetch_object($result)){
    $l+=1;
    $ll=$l%2;
    echo "<tr class='row$ll'><td>", $r1->data_name, "</td>";
    echo "<td>", $r1->data, "</td>";
    echo "<td>", $r1->date, "</td></tr>";}
  echo "</table>";}

echo "</td><td>";

if(Session::loggedin()){
  echo "<table id='edit'><tr><td>To insert information into the database enter a description and a value.<br />For comments or long notes, <a href=\"./notes.php?id=",$id, "\">add text here</a>.</td></tr>";
  echo "<tr><td>", newcolumn($id), "</td></tr></table>";}

?>
</td></tr></table>
</div><!-- main -->
<?php tsw_footer(); ?>

