<?php
# $Id: dlist.php,v 1.6 2008/05/29 19:28:15 tofp Exp $
$date=$_POST['d'];
$date=$_GET['d'];
include("../../lib/connect.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header("HV Test Results List Page");
tsw_toolbar('hvtest');
?>
<div id="main">
<div class="title">HV Test Results List Page</div>
<div><a href="./notes.php">Back to the module entry page</a></div>
<div><a href="./datelist.php">Back to the list of dates</a></div>
<br />
<table>
<?php
  $tomo=mysql_query("select date from hvtest where date>'$date' order by date asc limit 1", $con);
  $t=mysql_fetch_object($tomo);
  $tomorrow=$t->date;
  $yest=mysql_query("select date from hvtest where date<'$date' order by date desc limit 1", $con);
  $y=mysql_fetch_object($yest);
  $olde=mysql_query("select min(date) as date from hvtest", $con);
  $o=mysql_fetch_object($olde);
  $oldest=$o->date;
  $yesterday=$y->date;
  $today=date("Y-m-d");

  if($date!=$oldest){echo "<a href=\"./dlist.php?d=",$yesterday,"\">Previous</a>";}
  echo " Entries for ",$date, " ";
  if($date!=$today){echo "<a href=\"./dlist.php?d=",$tomorrow,"\">Next</a>";}

  $list=mysql_query("select m.serial as serial, h.pass as pass, m.vendor_id as vid from modules m inner join hvtest h on m.id=h.id where h.date='$date' order by h.pass, m.serial asc", $con);
  while($l=mysql_fetch_object($list)){
    $ser=$l->serial;
    $pass=$l->pass;
    if($pass=='1'){
      echo "<tr><td>","<a href=\"./edit.php?ser=",$ser,"\">",$ser,"</a></td><td>Pass</td><td></tr>";
    }
    else{
      echo "<tr><td>","<a href=\"./edit.php?ser=",$ser,"\">",$ser,"</a></td><td><font color='red'>Fail&lt;----</td></tr>";
    }
  }
//echo "<br>", mod_entry(), "<br>";
?>
</table>
<?php tsw_footer() ?>
