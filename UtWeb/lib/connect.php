<?php
# $Id: connect.php,v 1.4 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
import_request_variables("gP", "rvar_");
$con = mysql_pconnect("localhost:3306", "tofp", "t4flight");
mysql_select_db("TOF", $con);
?>
