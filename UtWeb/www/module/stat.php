<?php
# $Id: stat.php,v 1.8 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
$host = $_SERVER['HTTP_HOST'];
$self = $_SERVER['PHP_SELF'];
import_request_variables("gP", "rvar_");
if($rvar_update == "true")
{
  system("sh ../../scripts/root.sh ../../scripts/TRDistMaker.C\\(\\\"/homes/tofp/public_html/_static/var/tmp\\\"\\)");
  header("Location: http://$host$self");
}
?>
<html>
<!-- $Id: stat.php,v 1.8 2008/05/29 19:28:14 tofp Exp $ -->
<link rel="stylesheet" type="text/css" href="../style/default.css" />
<title>stat</title>

<form action="<?php echo $self?>" method="get">
<input type="hidden" name="update" value="true">
<input type="submit" value="update histogram">
</form>
<img src="/~tofp/var/tmp/tr_dist.gif" />
</html>
