<?php
# $Id: list.php,v 1.5 2008/05/29 19:28:15 tofp Exp $
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
  echo "<div><a href='./notes.php'>Back to the module entry page</a></div>\n";
  echo "<div><a href='./datelist.php'>See this list by date</a></div>\n";
  echo "<div style='margin: 20px 10px 20px 10px;'>";
  $list=mysql_query("select m.serial as serial, h.pass as pass, m.vendor_id as vid from modules m inner join hvtest h on m.id=h.id order by h.pass, m.serial asc", $con);
  while($l=mysql_fetch_object($list)){
    $ser=$l->serial;
    $disp = ($pass=$l->pass == '1')? "<font color='green'>Pass</font>" : "<font color='red'>Fail</font>";
    echo "<div style='float: left; white-space: nowrap; width: 80px;'>\n";
    echo " <a href='./edit.php?ser={$ser}'>{$ser}</a> ${disp}\n";
    echo "</div>\n";
  }
  echo "<div style='clear: both;'></div>\n";
  echo "</div>\n";
?>
<?php tsw_footer(); ?>
