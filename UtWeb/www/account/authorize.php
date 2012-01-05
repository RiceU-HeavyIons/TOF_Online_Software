<?php
# $Id: authorize.php,v 1.2 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/session.php");
if(Session::authorize( $_POST )) {
	Session::goback("/");
} else {
	Session::login_again('Password is not valid!');
}
?>