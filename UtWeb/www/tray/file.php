<?php
import_request_variables("gP", "rvar_");
$con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
mysql_select_db("toftstand", $con);
$oid = $rvar_id;

$q = mysql_query("SELECT data from xoops_dms_config where name='doc_path'", $con);
$r = mysql_fetch_object($q);
$b = $r->data;

$q = mysql_query("SELECT * from xoops_dms_object_versions where obj_id=$oid order by major_version desc, minor_version desc, sub_minor_version desc limit 1", $con);
$r = mysql_fetch_object($q);
header('Content-Length: '.$r->file_size);
header('Cache-control: private');
header('Content-Type: ' . $r->file_type);
header('Content-Disposition: inline; filename="'.$r->file_name.'"');
header('Pragma: public');   // Apache/IE/SSL download fix.
header('Content-Transfer-Encoding:  binary');

// Read the file
readfile($b . '/' . $r->file_path);
?>
