<?php
include_once("../../lib/orm.php");
include_once("../../lib/connect.php");
include_once("../../lib/utility.php");

$today = date("m/d/y");
$head = <<< EOF
<?xml version="1.0"?>
<?mso-application progid="Excel.Sheet"?>
<Workbook xmlns="urn:schemas-microsoft-com:office:spreadsheet"
 xmlns:o="urn:schemas-microsoft-com:office:office"
 xmlns:x="urn:schemas-microsoft-com:office:excel"
 xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"
 xmlns:html="http://www.w3.org/TR/REC-html40">
 <DocumentProperties xmlns="urn:schemas-microsoft-com:office:office">
  <Author>Jerry Hoffmann</Author>
  <LastAuthor>Andrew Oldag</LastAuthor>
  <LastPrinted>2008-09-15T20:16:40Z</LastPrinted>
  <Created>2008-08-05T18:06:08Z</Created>
  <LastSaved>2008-09-30T17:19:44Z</LastSaved>
  <Company>University of Texas</Company>
  <Version>11.9999</Version>
 </DocumentProperties>
 <ExcelWorkbook xmlns="urn:schemas-microsoft-com:office:excel">
  <WindowHeight>11955</WindowHeight>
  <WindowWidth>18975</WindowWidth>
  <WindowTopX>120</WindowTopX>
  <WindowTopY>15</WindowTopY>
  <ProtectStructure>False</ProtectStructure>
  <ProtectWindows>False</ProtectWindows>
 </ExcelWorkbook>
 <Styles>
  <Style ss:ID="Default" ss:Name="Normal">
   <Alignment ss:Vertical="Bottom"/>
   <Borders/>
   <Font ss:FontName="Calibri" x:Family="Swiss" ss:Size="11" ss:Color="#000000"/>
   <Interior/>
   <NumberFormat/>
   <Protection/>
  </Style>
  <Style ss:ID="m27219788">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <Font ss:FontName="Calibri" x:Family="Swiss" ss:Size="11" ss:Color="#000000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s62">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
  </Style>
  <Style ss:ID="s63">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
  </Style>
  <Style ss:ID="s64">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <Font ss:FontName="Calibri" x:Family="Swiss" ss:Size="11" ss:Color="#000000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s65">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <Interior/>
  </Style>
  <Style ss:ID="s66">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
  </Style>
  <Style ss:ID="s67">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <Font ss:FontName="Calibri" x:Family="Swiss" ss:Size="11" ss:Color="#000000"
    ss:Bold="1"/>
  </Style>
  <Style ss:ID="s68">
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
  </Style>
  <Style ss:ID="s69">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
  </Style>
  <Style ss:ID="s70">
   <Borders/>
  </Style>
  <Style ss:ID="s71">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders/>
  </Style>
  <Style ss:ID="s72">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Right" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <NumberFormat ss:Format="mm/dd/yy;@"/>
  </Style>
  <Style ss:ID="s73">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
  </Style>
  <Style ss:ID="s74">
   <Alignment ss:Horizontal="Center" ss:Vertical="Center" ss:WrapText="1"/>
   <Borders>
    <Border ss:Position="Bottom" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Left" ss:LineStyle="Continuous" ss:Weight="1"/>
    <Border ss:Position="Top" ss:LineStyle="Continuous" ss:Weight="1"/>
   </Borders>
   <NumberFormat ss:Format="[ENG]d\-mmm"/>
  </Style>
 </Styles>
 <Worksheet ss:Name="Sheet1">
  <Table ss:ExpandedColumnCount="13" ss:ExpandedRowCount="130" x:FullColumns="1"
   x:FullRows="1" ss:StyleID="s62" ss:DefaultColumnWidth="54"
   ss:DefaultRowHeight="14.25">
   <Column ss:StyleID="s62" ss:AutoFitWidth="0" ss:Width="54.75" ss:Span="1"/>

   <Column ss:Index="3" ss:StyleID="s62" ss:AutoFitWidth="0" ss:Width="140.0"/>
   <Column ss:StyleID="s62" ss:AutoFitWidth="0" ss:Width="64.0"/>
   <Column ss:StyleID="s62" ss:AutoFitWidth="0" ss:Width="140.0"/>
   <Column ss:StyleID="s62" ss:AutoFitWidth="0" ss:Width="24.0"/>
   <Column ss:StyleID="s71" ss:AutoFitWidth="0" ss:Width="24.0"/>
   <Column ss:StyleID="s71" ss:AutoFitWidth="0" ss:Width="24.0"/>
   <Column ss:StyleID="s71" ss:AutoFitWidth="0" ss:Width="24.0"/>
   <Column ss:StyleID="s70" ss:AutoFitWidth="0" ss:Width="24.0"/>
   <Column ss:StyleID="s70" ss:AutoFitWidth="0" ss:Width="64.0"/>
   <Column ss:StyleID="s70" ss:AutoFitWidth="0" ss:Width="64.0"/>
   <Row ss:AutoFitHeight="0" ss:Height="30">
    <Cell ss:MergeAcross="4" ss:StyleID="m27219788"><Data ss:Type="String">Tray Status {$today}</Data></Cell>
    <Cell ss:MergeAcross="4" ss:StyleID="s63"><Data ss:Type="String">Shipment</Data></Cell>
   </Row>
   <Row ss:AutoFitHeight="0" ss:Height="34.5">
    <Cell ss:StyleID="s64"><Data ss:Type="String">Tray</Data></Cell>
    <Cell ss:StyleID="s64"><Data ss:Type="String">Location</Data></Cell>
    <Cell ss:StyleID="s64"><Data ss:Type="String">Status</Data></Cell>
    <Cell ss:StyleID="s64"><Data ss:Type="String">Cosmic Test</Data></Cell>
    <Cell ss:StyleID="s67"><Data ss:Type="String">Notes</Data></Cell>
    <Cell ss:StyleID="s63"><Data ss:Type="String">#1</Data></Cell>
    <Cell ss:StyleID="s63"><Data ss:Type="String">#2</Data></Cell>
    <Cell ss:StyleID="s63"><Data ss:Type="String">#3</Data></Cell>
    <Cell ss:StyleID="s65"><Data ss:Type="String">#4</Data></Cell>
    <Cell ss:StyleID="s65"><Data ss:Type="String">#5</Data></Cell>
    <Cell ss:StyleID="s72"><Data ss:Type="String">Ideal build week</Data></Cell>
    <Cell ss:StyleID="s72"><Data ss:Type="String">Sealed Date</Data></Cell>
    </Row>

EOF;

$fname= tempnam("/homes/tofp/temp", "tray_status");

$fp = fopen($fname, "w");
fwrite($fp, $head);
# -----------------------------------------------------------------------------

$ship_dates = Tray::ship_dates();
$ship_num = array();
$l = 0;
foreach(array_keys($ship_dates) as $d) { $ship_num[$d] = $l++; }

$trays = Tray::find_all("", "(sn+1) asc", "");
$last = sizeof($trays) - 1;
if ($last < 118+3) { $last--; }
$last_sn = $trays[$last]->sn;
$last_dt = parse_sqldate($trays[$last]->start);
$next_mon = strtotime("next Monday", $last_dt);

$l = 1;
foreach ($trays as $tray) {

   if($tray->sn == 118 and $last < 118+3) continue;

   $l += 1;

   $id = $tray->id;

   if($tray->ship) {
      $ship_dates[$tray->ship] += 1;
   }

   $ll = $l % 2;
   fwrite($fp, "   <Row>\n");
   $type = ($tray->sn > 0) ? "Number" : "String";
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"${type}\">{$tray->sn}</Data></Cell>\n");
   if($tray->tray_status->id == 3) {
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">BNL</Data></Cell>\n");
   } else {
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">UT</Data></Cell>\n");
   }
   $status = $tray->tray_status->name;
   if($status == "Shipped") $status = "Finished";
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">{$status}</Data></Cell>\n");

   $cosmic = ($tray->cosmic_tested())? " Yes" : "No";
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">{$cosmic}</Data></Cell>\n");

   $notes = $tray->notes;
   if($tray->tray_status->id == 3 && $tray->ship && $notes == '') {
      $notes = "Shipped " . date("m/d/y", parse_sqldate($tray->ship));
   }
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">$notes</Data></Cell>\n");

   for($i = 0; $i < 5; $i++) {
      if ($tray->ship && $ship_num[$tray->ship] == $i) {
         fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"Number\">{$ship_dates[$tray->ship]}</Data></Cell>\n");
      } else {
         fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
      }
   }

   fwrite($fp, "    <Cell ss:StyleID=\"s72\"/>\n");

   if($sealed = $tray->sealed_date()) {
      $seal = $sealed . "T00:00:00";
      fwrite($fp, "    <Cell ss:StyleID=\"s72\"><Data ss:Type=\"DateTime\">{$seal}</Data></Cell>\n");
   } else{
      fwrite($fp, "    <Cell ss:StyleID=\"s72\"/>\n");
   }
   fwrite($fp, "   </Row>\n");
}
$week_in_sec = 7*24*60*60;

$i = 0;
for($sn = $last_sn + 1; $sn <= 120; $sn++) {
   $dt = $next_mon + $week_in_sec * floor($i / 3);
//   if($sn != 118) { $i++; }
   $i++;

   $l += 1;
   $ll = $l % 2;

   fwrite($fp, "   <Row>\n");
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"Number\">{$sn}</Data></Cell>\n");
   if($sn == 118) {
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">UT</Data></Cell>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">Testing</Data></Cell>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"><Data ss:Type=\"String\">Used for Submarine test</Data></Cell>\n");
   } else {
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
      fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   }

   fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   fwrite($fp, "    <Cell ss:StyleID=\"s63\"/>\n");
   if(false && $sn == 118) {
      fwrite($fp, "    <Cell ss:StyleID=\"s72\"/>\n");
   } else {
      fwrite($fp, "    <Cell ss:StyleID=\"s72\"><Data ss:Type=\"DateTime\">" . strftime("%Y-%m-%d", $dt) . "T00:00:00</Data></Cell>\n");
   }
   fwrite($fp, "    <Cell ss:StyleID=\"s72\"/>\n");
   fwrite($fp, "   </Row>\n");
}
fwrite($fp, "  </Table>\n");
fwrite($fp, " </Worksheet>\n");
fwrite($fp, "</Workbook>\n");
# --------------------------------------------------------------------
fclose($fp);

header("Content-type: applicaton/vnd.ms-excel");
header("Content-length: " . filesize($fname));
header("Content-disposition: attachement; filename=\"Tray_Status.xls\"");
readfile($fname);

unlink($fname);
?>

