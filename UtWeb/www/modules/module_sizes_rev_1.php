<html>
<head>
<title>Module Size Measurements</title>
<style type="text/css">
   table#module_size_list { font-size: 10pt; }
   .row0 td { background: #e0e0ff; }
   .row1 td { background: #e0e0e0; }
   td.size { text-align: right; width: 48pt;}
   td.size_bad { text-align: right; width: 48pt; background: #ff0000;}
   td.jig {text-align: center; } 
   td.jig_bad {text-align: center; background: #ff0000;} 

</style> 
</head> 
<body> 
<h1>Module Size Measurements</h1> 
<img src="figs/rpc_meas_diagram.jpg" border="0" alt="Board measurement locations">
<!-- <p>
<img src="figs/tolerances.jpg" border="0" height="150" alt="RPC tolerances">
</p>
-->
<h4>Mechanical tolerances:</h4>
<table style="border: 1px solid grey;">
<tr>
 <td></td>
 <td>nominal</td>
 <td>minimum</td>
 <td>maximum</td>
</tr>
<tr>
 <td style="text-align: center;">MRPC overall length</td>
 <td style="text-align: right; background: #cfc;">212.0 mm</td>
 <td style="text-align: right; background: #cff;">211.5 mm</td>
 <td style="text-align: right; background: #fcf;">212.5 mm</td>
</tr>
<tr>
 <td style="text-align: center;">width</td>
 <td style="text-align: right; background: #cfc;">94.0 mm</td>
 <td style="text-align: right; background: #cff;">93.5 mm</td>
 <td style="text-align: right; background: #fcf;">94.5 mm</td>
</tr>
<tr>
 <td style="text-align: center;">thickness</td>
 <td style="text-align: right; background: #cfc;">17.9 mm</td>
 <td style="text-align: right; background: #cff;">16.9 mm</td>
 <td style="text-align: right; background: #fcf;">18.9 mm</td>
</tr>
</table>

<hr/>
<p> This table shows the values found after measuring the first set of RPC 
boards from both Chinese universities.<br> In the column "Jig Pass", a 
value 
of 1 indicates that the RPC passed through an aluminum frame.<br> This 
frame 
is machined such that a board exceeding the maximum physical tolerances 
will not pass.<BR></p>
<p><b><font color="red">Any value found to exceed a physical tolerance is highlighted in red.</font><br></b></p>

 <table id="module_size_list"> 
<tr>
 <td><center>ID</td>
 <td><center>S/N</td>
 <td><center>Measurer</td>
 <td><center>Date</td>
 <td><center>Width 1</td>
 <td><center>Width 2</td>
 <td><center>Length 1</td>
 <td><center>Length 2</td>
 <td><center>Thickness 1</td>
 <td><center>Thickness 2</td>
 <td><center>Jig</td>
</tr>

<tr>
<td></td>
<td></td>
<td></td>
<td></td>
<td><center>A (mm)</td>
<td><center>B (mm)</td>
<td><center>C (mm)</td>
<td><center>D (mm)</td>
<td><center>E (mm)</td>
<td><center>F (mm)</td>
<td><center>Pass</td>
</tr>

<?php
 $con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
 mysql_select_db("TOF", $con);
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
   echo "  <td>", $r1->id, "</td>\n";
   echo "  <td>", $r2->serial, "</td>\n";
   echo "  <td><center>", $r3->name, "</td>\n";
   echo "  <td>", $r1->measured_on, "</td>\n";
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
  echo " </tr>\n";
 }
mysql_close($con)
?>
</table>
<img src="figs/dim_dist.gif">
</body>
</html>
