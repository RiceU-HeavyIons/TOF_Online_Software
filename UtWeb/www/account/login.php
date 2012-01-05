<?php
# $Id: login.php,v 1.6 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
echo "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<?php
include("../../lib/session.php");
$err = Session::error(array("clear" => 1));
$usr = Session::username();
?>
<html>
<head>
<title>login</title>
<link rel="stylesheet" type="text/css" href="../style/default.css"/>
<script type="text/javascript">
function checkForm() {
	if(document.getElementById('username').value == '' ) {
		window.alert("You must enter a username.");
		return false;
	}

	if(document.getElementById('password').value == '') {
		window.alert("No password entered.");
		return false;
	}
	return true;
}
</script>
</head>
<body class="login" onload="document.forms[0].elements[<?php echo ($usr==null)? 0 : 1;?>].focus();">
<div id="container">
<div id="dialog">
<div class='error'><?php if($err) { echo $err; }?></div>
<form action="./authorize.php" id="loginForm" method="post" onsubmit="return(checkForm());">
  <dl>
    <dt>Username:</dt>
    <dd><input id="username" name="username" type="username" value="<?php echo $usr?>"/></dd>
    <dt>Password:</dt>
    <dd><input id="password" name="password" type="password" /></dd>
    <dd><input type="submit" value="login" /></dd>
  </dl>
</form>
</div>
</div>
</body>
</html>
