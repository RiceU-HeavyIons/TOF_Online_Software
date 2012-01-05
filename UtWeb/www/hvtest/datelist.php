<?php
# $Id: datelist.php,v 1.5 2008/05/29 19:28:15 tofp Exp $
include("../../lib/connect.php");
include("../../lib/session.php");
include("../../lib/template.php");
tsw_header("HV Test Results List Page");
tsw_toolbar('hvtest');
?>
<div id="main">
<div class="title">HV Test Results List Page</div>
<?php
$all_count=mysql_query("select count(*) as count from hvtest", $con);
$all_c=mysql_fetch_object($all_count);
$count=$all_c->count;
$bad_count=mysql_query("select count(*) as count from hvtest where pass='0'", $con);
$bad_c=mysql_fetch_object($bad_count);
$bad=$bad_c->count;
$good=$count-$bad;

echo "<div>A total of {$count} modules have been tested. Of those {$bad} failed the test.</div>\n";
?>
<div><a href="./notes.php">Back to the module entry page</a></div>
<div><a href="./list.php">List of all modules tested</a></div>
<br />
<table>
<tr><th>Date</th><th>Tested</th><th>Failed</th></tr>
<?php
  $today=date("Y-m-d");
  $zero=0;
  $dates=mysql_query("select distinct date, count(date) as count from hvtest group by date order by date desc", $con); 
  while($d=mysql_fetch_object($dates)) {
    $date=$d->date;
    $count=$d->count;
    $fails=mysql_query("select count(*) as fail from hvtest where date='$date' and pass=$zero",$con);
    $e=mysql_fetch_object($fails);
    $fail_count=$e->fail;
    if($date==$today){
      echo "<tr><td>","<a href=\"./dlist.php?d=",$date,"\">Today</a></td><td>",$count,"</td><td>",$fail_count,"</td></tr>";
    }
    else{
      echo "<tr><td>","<a href=\"./dlist.php?d=",$date,"\">",$date,"</a></td><td>",$count,"</td><td>",$fail_count,"</td></tr>";
    }
  }
?>
</table>
<?php tsw_footer(); ?>
