<?php
# $Id: size.php,v 1.11 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto, Andrew Oldag. All rights reserved.
  include("../../lib/connect.php");
  include("../../lib/session.php");
  include("../../lib/template.php");
  
  tsw_header("Module Size Measurements");
?>
<style type="text/css">
   table#module_size_list { font-size: 11pt; border: 1px solid #aaa; }
   .row0 td { background: #e0e0ff; }
   .row1 td { background: #ffffff; }
   td.size { text-align: right; width: 48pt;}
   td.size_bad { text-align: right; width: 48pt; background: #ff0000;}
   td.jig {text-align: center; } 
   td.jig_bad {text-align: center; background: #ff0000;}
   img { border: 0px; }
   td.col1 { text-align: right; background: #cfc; }
   td.col2 { text-align: right; background: #cff; }
   td.col3 { text-align: right; background: #fcf; }
</style> 
<?php include("../../lib/toolbar.php"); tsw_toolbar('module'); ?>
<div id="main">
<div class="title">Module Size Measurements</div> 
<img src="figs/rpc_meas_diagram.jpg" alt="Board measurement locations" />
<!-- <p>
<img src="figs/tolerances.jpg" border="0" height="150" alt="RPC tolerances" />
</p>
-->
<div class="section">
<div class="title">Mechanical tolerances:</div>
<table style="border: 1px solid #aaa;">
<tr>
 <td></td>
 <td>nominal</td>
 <td>minimum</td>
 <td>maximum</td>
</tr>
<tr>
 <td style="text-align: center;">MRPC overall length</td>
 <td class="col1">212.0 mm</td>
 <td class="col2">211.5 mm</td>
 <td class="col3">212.5 mm</td>
</tr>
<tr>
 <td style="text-align: center;">width</td>
 <td class="col1">94.0 mm</td>
 <td class="col2">93.5 mm</td>
 <td class="col3">94.5 mm</td>
</tr>
<tr>
 <td style="text-align: center;">thickness</td>
 <td class="col1">17.9 mm</td>
 <td class="col2">16.9 mm</td>
 <td class="col3">18.9 mm</td>
</tr>
</table>
</div>

<div class="section">
<div class="title">Table</div>
<p>This table shows the values found after measuring the first set of MRPC modules 
from both Chinese universities.
In the column "Jig Pass", a value of 1 indicates that the MRPC passed through 
an aluminum frame.
This frame is machined such that a board exceeding the maximum physical tolerances 
will not pass.</p>
<p><b style="color: red;">Any value found to exceed a physical tolerance is 
highlighted in red.</b></p>

<table id="module_size_list"> 
<tr>
<!-- <th>ID</th> -->
 <th>S/N</th>
 <th>Measurer</th>
 <th>Date</th>
 <th>Width 1</th>
 <th>Width 2</th>
 <th>Length 1</th>
 <th>Length 2</th>
 <th>Thickness 1</th>
 <th>Thickness 2</th>
 <th>Jig</th>
 <th>Note</th>
</tr>

<tr>
<!-- <td></td> -->
<td></td>
<td></td>
<td></td>
<th>A (mm)</th>
<th>B (mm)</th>
<th>C (mm)</th>
<th>D (mm)</th>
<th>E (mm)</th>
<th>F (mm)</th>
<th>Pass</th>
</tr>

<?php
 $result1 = mysql_query("SELECT * from module_sizes", $con);
 $l = 0;
 while(($r1 = mysql_fetch_object($result1))) {
   $l += 1; $l = $l%2;
   $mid = $r1->module_id;
   $result2 = mysql_query("SELECT * from modules where id=$mid", $con);
   $r2 = mysql_fetch_object($result2);

   $uid = $r1->user_id;
   $result3 = mysql_query("SELECT * from users where id=$uid", $con);
   $r3 = mysql_fetch_object($result3);

   echo " <tr class='row$l'>\n";
#   echo "  <td>", $r1->id, "</td>\n";
   echo "  <td><a href=\"show.php?id=$r1->id\">", $r2->serial, "</a></td>\n";
   echo "  <td>", $r3->name, "</td>\n";
   echo "  <td>", substr($r1->measured_on,0,10), "</td>\n";
	if($r1->width1 >94.5 || $r1->width1 <93.5){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->width1), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->width1), "</td>\n";
}
	if($r1->width2 >94.5 || $r1->width2 <93.5){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->width2), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->width2), "</td>\n";
}
	if($r1->length1 >212.5 || $r1->length1 <211.5){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->length1), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->length1), "</td>\n";
}
	if($r1->length2 >212.5 || $r1->length2 <211.5){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->length2), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->length2), "</td>\n";
}
	if($r1->thickness1 >18.9 || $r1->thickness1 <16.9){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->thickness1), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->thickness1), "</td>\n";
}
	if($r1->thickness2 >18.9 || $r1->thickness2 <16.9){
   echo "  <td class='size_bad'>", printf("%5.2f", $r1->thickness2), "</td>\n";
}else{   echo "  <td class='size'>", printf("%5.2f", $r1->thickness2), "</td>\n";
}
	if($r1->jig_passed != 1){
   echo "  <td class='jig_bad'>", $r1->jig_passed, "</td>\n";
}else{   echo "  <td class='jig'>", $r1->jig_passed, "</td>\n";
}
  $note = $r1->note;
  if(strlen($note) > 20) {
    $note = substr($note, 0, 20) . "...";
  }
  echo "  <td>" . $note . "</td>\n";
  echo " </tr>\n";
 }
?>
</table>
</div>

<div class="section">
<div class="title">Histograms</div>
<img src="figs/dim_dist.gif" alt="dimention distribution"/>
</div>

</div>
<?php
  tsw_footer();
  mysql_close($con);
?>