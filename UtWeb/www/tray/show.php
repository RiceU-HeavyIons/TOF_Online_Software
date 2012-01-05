<?php echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<!-- $Id: show.php,v 1.4 2008/05/29 19:28:15 tofp Exp $ -->
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<?php
include("../../lib/connect.php");
$id = $rvar_id;
function title() {
	printf ("Tray %03d", $id);
}
function update() {
  echo "<a href='list.php'>No data found for this tray!</a>";
}
function back() {
  echo "<form action='list.php' method='post'>";
  echo "<input name='back' type='submit' value='Back'/></form>";
}
function edit($id){
  echo "<form action='edit.php' method='post'>";
  echo "<input name='id' type='hidden' value='$id'/>";
  echo "<input name='edit' type='submit' value='Edit Contents'/>";
  echo "</form>";
}

include("../../lib/session.php");
include("../../lib/template.php");
tsw_header("Tray ", $id, " Contents");
tsw_toolbar("tray");
?>
<div id="main">
<div class="title"><?php echo "Tray ", $id, " Contents" ?></div>

<table id="module_content">
<?php
$result1=mysql_query("select module_id from tray_module_link where tray_id=$id", $con);
$result2=mysql_query("select * from tcpu where id=(select tcpu_id from trays where id=$id)", $con);
$r1 = mysql_fetch_object($result1);
#check to see if there are any modules recorded in the tray, if not, set a variable
if($r1->module_id==''){$no_modules=1;}
else{$no_modules=0;}

$r2 = mysql_fetch_object($result2);
#check to see if a tcpu has been recorded in a tray, if not, set a variable
if($r2->serial =='' || $r2->serial == '0'){$no_tcpu=1;}
else{$no_tcpu=0;}

#If neither tcpu or modules are present, show update which links to the edit module contents page.

if($no_tcpu == 1 && $no_modules == 1){
  echo back();
  srand(time());
  $random = (rand()%3);
  if($random == '0'){ echo "<tr><th>It looks like this tray is empty to me.</th></tr>\n"; }
  elseif($random == '1'){  echo "<tr><th>I can't seem to find anything in this tray.</th></tr>\n"; }
  elseif($random == '2'){  echo "<tr><th>Are you sure there should be something there?</th></tr>\n"; }
  echo "<tr><td>",edit($id), "</td></tr>\n";}

else{
  echo back();
  echo edit($id);
#if a tcpu is found, it is assumed that the TDIG and TINO boards are present as well as the modules.
  if($no_tcpu != '1'){
    echo "<tr><th>TCPU</th>";
    echo "<th>TDIG</th>";
    echo "<th>TINO</th>";
    echo "<th>Module Slot 1</th>";
    echo "<th>Module Slot 2</th>";
    echo "<th>Module Slot 3</th>";
    echo "<th>Module Slot 4</th></tr>";
    echo "<tr class='row1'><td class='tcpu'>", $r2->serial, "</td>\n";
    echo "</tr>";
#Get TCPU id number from tray table
    $result_tcpu=mysql_query("select tcpu_id from trays where id=$id", $con);
    $r_tcpu=mysql_fetch_object($result_tcpu);
    $tcpu=$r_tcpu->tcpu_id;
#Get the list of TDIGs attached to the TCPU from the tcpu_tdig_link table
    $result_tdig=mysql_query("select * from tcpu_tdig_link where tcpu_id=$tcpu", $con);
    $l=1;
    while ($r_tdig=mysql_fetch_object($result_tdig)){
      $l+=1;
      $ll=$l%2;
      echo "<tr class='row$ll'>\n";
#Get the serial numbers of the TDIGs
      $result_tdig_sn=mysql_query("select serial from tdig where id=$r_tdig->tdig_id", $con);
      $r_tdig_sn=mysql_fetch_object($result_tdig_sn);
      echo "<td></td><td class='tdig'>", $r_tdig_sn->serial, "</td>\n";
      echo "</tr>";
#Get the TINO id from the TDIG table.
      $result_tino=mysql_query("select tino_id from tdig where id=$r_tdig->tdig_id", $con);
      $r_tino=mysql_fetch_object($result_tino);
#Get the TINO serial number from the tino table.
      $result_tino_sn=mysql_query("select serial from tino where id=$r_tino->tino_id", $con);
      $r_tino_sn=mysql_fetch_object($result_tino_sn);
      $l+=1;
      $ll=$l%2;
      echo "<tr class='row$ll'>\n";
      echo "<td></td><td></td><td class='tino'>", $r_tino_sn->serial, "</td>\n";
      echo "</tr>";
      $l+=1;
      $ll=$l%2;
      echo "<tr class='row$ll'>\n";
      echo "<td></td><td></td><td></td>";
#Get list of modules associated with each TINO from the tino_module_link 
      $result_module=mysql_query("select module_id from tino_module_link where tino_id = $r_tino->tino_id", $con);
      while($r_module=mysql_fetch_object($result_module)){
#Get serial number of module from modules table
	$result_module_sn=mysql_query("select serial from modules where id=$r_module->module_id", $con);
	$r_module_sn=mysql_fetch_object($result_module_sn);
#link to detailed module information page.
	echo " <td class='module'><a href=\"../module/show.php?id=", $r_module->module_id, "\">", $r_module_sn->serial, "</td>\n";
      }
      echo "</tr>";     
    }
  }
  
#If no TCPU is found, but modules are found, it will report the modules. 
  elseif($no_tcpu=='1' && $no_modules=='0'){
    echo "<tr><th>Module</th><th>Tray Slot #</th></tr>";
    $result_module=mysql_query("select * from tray_module_link where tray_id=$id", $con);
    while($r_module=mysql_fetch_object($result_module)){
      $result_module_sn=mysql_query("select serial from modules where id=$r_module->module_id", $con);
      $r_module_sn=mysql_fetch_object($result_module_sn);
      echo " <tr><td class='module'><a href=\"../module/show.php?id=", $r_module->module_id, "\">", $r_module_sn->serial, "</td>\n";
      echo " <td>", $r_module->slot, "</td></tr>\n";
    }
  }
}
mysql_close($con)
?>
</table>


<!--
<pre>
[eta=0]
_______________________________ pad 123456
TINO 1 (AC177530004; 22), air core
01   1 TM0605001                    111111
02   2 TM0605002                    111111
03   3 TM0605005                    111111
04   4 TM0605006                    111111

TINO 2 (AC177530007; 11)
05   1 UM0002                       111111
06   2 UM0003                       011111
07   3 UM0007                       111110
08   4 UM0008                       011111

TINO 3 (AC177530002; 2)
09   1 TM0606001                    111111
10   2 TM0606002                    111111
11   3 TM0606003                    011111
12   4 TM0606004                    111111

TINO 4 (AC177530006; 16), air core
13   1 UM0009                       111111
14   2 UM0015                       111111
15   3 UM0019                       111111
16   4 UM0021                       111111

TINO 5 (AC177530001; 1)
17   1 TM0605008                    111111
18   2 TM0606007                    111111
19   3 TM0606008                    111111
20   4 TM0606009                    111111

TINO 6 (AC177530005; 13)
21   1 UM0020                       011110
22   2 UM0022                       111110
23   3 UM0023                       111110
24   4 UM0025                       111111

TINO 7 (AC1777530011; 21), air core
25   1 TM0606010                    111111
26   2 TM0606011                    111111
27   3 TM0606012                    111111
28   4 TM0606013                    111111

TINO 8 (AC177530012; 3) *J6 connector is broken (Mod30:Pad1-4, Mod31:Pad3-6)
29   1 UM0031                       111110
30   2 UM0033                       ****11
31   3 UM0034                       11****
32   4 UM0035                       011111
</pre>
-->
</div> <!-- id=main -->

<?php tsw_footer(); ?>
<?php mysql_close($con) ?>
