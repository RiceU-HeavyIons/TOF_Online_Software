<?php
# $Id: connect.php,v 1.4 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include_once("mysql.php");
extract($_REQUEST, EXTR_PREFIX_ALL|EXTR_REFS, 'rvar');
$con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
mysql_select_db("TOF", $con);
?>
