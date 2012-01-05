<?php
$host = $_SERVER['HTTP_HOST'];
$self = $_SERVER['PHP_SELF'];
import_request_variables("gP", "rvar_");
if($rvar_update == "true")
{
	system("sh ../../scripts/root.sh ../../scripts/NSDistMaker.C\\(\\\"/homes/tofp/public_html/_static/var/tmp\\\"\\)");
	header("Location: http://$host$self");
}
?>
<html>
<!-- $Id: ndiff.php,v 1.1 2012/01/05 17:27:33 tofp Exp $ -->
<link rel="stylesheet" type="text/css" href="../style/default.css" />
<title>Noise Differences</title>

<form action="<?php echo $self?>" method="get">
<input type="hidden" name="update" value="true">
<input type="submit" value="update histogram">
</form>
<p> Work in progress </p>


<img src="/~tofp/var/tmp/noise_diff.gif" />

</html>
