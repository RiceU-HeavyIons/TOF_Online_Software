<html>
<head>
<title>New Module Dimension Entry Form</title>
</head>
<body>
<fieldset>
<legend>
Module Dimensions
</legend>
<form name="mod_dims" action="input.php" method="post">
A, Width 1:
<input type="text" name="width1">
<br>
B, Width 2:
<input type="text" name="width2">
<br>
C, Length 1:
<input type="text" name="length1">
<br>
D, Length 2:
<input type="text" name="length2">
<br>
E, Thickness 1:
<input type="text" name="thick1">
<br>
F, Thickness 2:
<input type="text" name="thick2">
<fieldset>
Jig Pass?
<br>
<input type="radio" name="jig_pass" value="Yes"> Passed
<br>
<input type="radio" name="jig_pass" value="No"> No Pass
</fieldset>
Notes:
<br>
<textarea rows="10" cols="50" name="comments">
</textarea>
<input type="submit" value="Submit">
</form>
</fieldset>
</body>
</html>
