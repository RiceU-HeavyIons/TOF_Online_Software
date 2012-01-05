#! /homes/tofp/c/ruby/bin/ruby
# $Id: make_html.rb,v 1.4 2007/08/21 11:18:11 tofp Exp $
# Copyright (C) 2005-2007, Kohei Kajimoto.

$mod = 4
$pad = 6
$off = 48

################################################################
### return figure link
def link_fig(fname)
  return "<a href=\"fig/#{fname}.gif\"><img src=\"fig/thumb/#{fname}.gif\" alt=\"#{fname}\" /></a>"
end

################################################################
### header and footer

def print_header(io, run, title)

io.print <<"__HEAD__"
<?xml version="1.0"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>#{title} -- #{run}</title>
<link rel="stylesheet" href="../../../default.css" type="text/css" />


<style type="text/css">
.tile { }
.tile-box { float: left; margin: 0px; padding: 2px; }
.title { margin: 0px; padding: 0px; font-size: small; text-align: left; }
.graph { margin: 1px; padding: 0px; }
.dlink { margin: 0px; padding: 0px; font-size: small; text-align: right; }
</style>

</head> 
<body>
<h1>#{run}</h1>
__HEAD__
end

def print_footer(io, run)
io.print <<"__FOOT__"
<hr />
<address>
<span style="float: right;"><a href="http://validator.w3.org/check?uri=referer"><img style="border: 0;" src="http://www.w3.org/Icons/valid-xhtml10" height="31" width="88" alt="Valid XHTML 1.0!" /></a></span>
<span style="clear: right;"></span>
</address>
</body>
</html>
__FOOT__
end


################################################################
### print summary
def print_summary(io, run)

  io.puts "<h3>Summary</h3>"
  io.puts "<div style=\"text-align: center;\">"
  io.puts "<table>"
  io.puts " <tr>"
  io.puts "  <td>" + link_fig("report") + "</td>"
  io.puts "  <td>" + link_fig("resmap") + "</td>"
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print dt distribution section
def print_dtdist(io, run)

  io.puts "<h3>dt distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"
  io.puts "<table>"
  io.puts " <tr>"
  (48..53).each { |ch|
    ch1 = ch
    ch2 = ch + 12
    ch3 = ch + 18
    io.puts "  <td>" + link_fig("#{run}_#{ch1}_#{ch2}_#{ch3}") + "</td>"
  }
  io.puts " </tr>"  
  io.puts " <tr>"  
  (48..53).each { |ch|
    ch1 = ch
    ch2 = ch + 12
    ch3 = ch + 18
    io.puts "  <td><a href=\"fig/#{run}_#{ch1}_#{ch2}_#{ch3}c.gif\"><img src=\"fig/thumb/#{run}_#{ch1}_#{ch2}_#{ch3}c.gif\" alt='' /></a></td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print tot distribution section
def print_totdist(io, run)

  io.puts "<h3>ToT distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"
  io.puts "<table>"
  (1..$mod).each { |row|
    io.puts " <tr>"
    (1..$pad).each { |col|
      ch = $off + $pad*(row-1) + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/tot#{ch}.gif\"><img src=\"fig/#{run}/thumb/tot#{ch}.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print hit number distribution section
def print_hitnumdist(io, run)

  io.puts "<h3>hit number distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  (1..4).each { |row|
    io.puts " <tr>"
    (1..6).each { |col|
      ch = 48 + 6*(row-1) + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/lehit#{ch}.gif\"><img src=\"fig/#{run}/thumb/lehit#{ch}.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  

  io.puts "<table>"
  (1..$mod).each { |row|
    io.puts " <tr>"
    (1..$pad).each { |col|
      ch = $off + $pad*(row-1) + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/tehit#{ch}.gif\"><img src=\"fig/#{run}/thumb/tehit#{ch}.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  

  io.puts "</div>"

end

################################################################
### print dt2 distribution
def print_dt2dist(io, run)

  pat = [[0,1], [0,2], [0,3], [1,2], [1,3], [2,3]]

  io.puts "<h3>dT(2) distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  pat.each { |a, b|
    io.puts " <tr>"
    (1..$pad).each { |col|
      ch1 = $off + $pad*a + (col-1)
      ch2 = $off + $pad*b + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/dt2_#{ch1}_#{ch2}.gif\"><img src=\"fig/#{run}/thumb/dt2_#{ch1}_#{ch2}.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print dt3 distribution
def print_dt3dist(io, run)

  pat = [[2,1,3]]

  io.puts "<h3>dT(3) distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  pat.each { |a, b, c|
    io.puts " <tr>"
    (1..6).each { |col|
      ch1 = 48 + 6*a + (col-1)
      ch2 = 48 + 6*b + (col-1)
      ch3 = 48 + 6*c + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/dt3_#{ch1}_#{ch2}_#{ch3}.gif\"><img src=\"fig/#{run}/thumb/dt3_#{ch1}_#{ch2}_#{ch3}.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"

end

def print_dt3dist_rev(io, run, pat=[2,1,3,0])

  a, b, c, d = pat

  io.puts "<h3>dT(3) distributions - &lt;#{a+1}&gt;-(&lt;#{b+1}&gt;+&lt;#{c+1}&gt;)/2</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  (1..6).each { |col|
    ch1 = 48 + 6*a + (col-1)
    ch2 = 48 + 6*b + (col-1)
    ch3 = 48 + 6*c + (col-1)
    io.puts "  <td><a href=\"fig/#{run}/dt3_#{ch1}_#{ch2}_#{ch3}_rev#{d}.gif\"><img src=\"fig/#{run}/thumb/dt3_#{ch1}_#{ch2}_#{ch3}_rev#{d}.gif\" alt='' /></a></td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print dt3 vs ToT distribution
def print_dt3vstot(io, run, pat=[[2,1,3]])

  io.puts "<h3>dT(3) vs ToT</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  pat.each { |a, b, c|
    io.puts " <tr>"
    (1..6).each { |col|
      ch1 = 48 + 6*a + (col-1)
      ch2 = 48 + 6*b + (col-1)
      ch3 = 48 + 6*c + (col-1)
      io.puts "  <td><a href=\"fig/#{run}/dt3_#{ch1}_#{ch2}_#{ch3}_tot.gif\"><img src=\"fig/#{run}/thumb/dt3_#{ch1}_#{ch2}_#{ch3}_tot.gif\" alt='' /></a></td>"
    }
    io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"

end

def print_dt3vstot_rev(io, run, pat=[2,0,3,1])

  io.puts "<h3>dT(3) vs ToT - Step #{pat[3]}</h3>"
  io.puts "<div style=\"text-align: center;\">"

  a, b, c, d = pat
  io.puts "<table>"
  io.puts " <tr>"
  (1..6).each { |col|
    ch1 = 48 + 6*a + (col-1)
    ch2 = 48 + 6*b + (col-1)
    ch3 = 48 + 6*c + (col-1)
    io.puts "  <td><a href=\"fig/#{run}/dt3_#{ch1}_#{ch2}_#{ch3}_tot_rev#{d}.gif\"><img src=\"fig/#{run}/thumb/dt3_#{ch1}_#{ch2}_#{ch3}_tot_rev#{d}.gif\" alt='' /></a></td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end


################################################################
### print dt4 distribution
def print_dt4avgdist_rev(io, run, rev)

  io.puts "<h3>dT_avg distributions</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  (1..$pad).each { |pad|
    io.puts "  <td>" + link_fig("dT4_avg_p#{pad}_rev#{rev}") + "</td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

def print_dt4dist_mod_rev(io, run, mod, rev)

  io.puts "<h3>dT(4) distributions (module=#{mod})</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  (1..$pad).each { |pad|
    io.puts "  <td>" + link_fig("dT4_m#{mod}_p#{pad}_rev#{rev}") + "</td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

def print_dt4dist_rev(io, run, pat=[0,1,2,3,0])

  a, b, c, d, rev = pat

  io.puts "<h3>dT(4) distributions - &lt;#{a+1}&gt;-(&lt;#{b+1}&gt;+&lt;#{c+1}&gt;+&lt;#{d+1}&gt;)/3</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  (1..6).each { |col|
    ch1 = 48 + 6*a + (col-1)
    ch2 = 48 + 6*b + (col-1)
    ch3 = 48 + 6*c + (col-1)
    ch4 = 48 + 6*d + (col-1)
    io.puts "  <td>" + link_fig("dt4_#{ch1}_#{ch2}_#{ch3}_#{ch4}_rev#{rev}") + "</td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

def print_dt4vstot_rev(io, run, pat=[0,1,2,3,1])

  a, b, c, d, rev = pat

  io.puts "<h3>dT(4) vs ToT - Step #{rev}</h3>"
  io.puts "<div style=\"text-align: center;\">"


  io.puts "<table>"
  io.puts " <tr>"
  (1..6).each { |col|
    ch1 = 48 + 6*a + (col-1)
    ch2 = 48 + 6*b + (col-1)
    ch3 = 48 + 6*c + (col-1)
    ch4 = 48 + 6*d + (col-1)
    io.puts "  <td>" + link_fig("dt4_#{ch1}_#{ch2}_#{ch3}_#{ch4}_tot_rev#{rev}") + "</td>"
  }
  io.puts " </tr>"  
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print dt(n) distribution

def print_dtdist_rev(io, run, rev)

  io.puts "<h3>dT distributions (rev=#{rev})</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  io.puts "  <td>average</td>"
  (1..$pad).each { |pad|
 	io.puts "  <td>" + link_fig("dT#{$mod}_avg_p#{pad}_rev#{rev}") + "</td>"
  }
  io.puts " </tr>"
  io.puts " <tr>"
  io.puts " <td></td>"
  (1..$pad).each { |p|
  	io.puts "<td>pad\##{p}</td>"
  }
  io.puts " </tr>"

  (1..$mod).each { |mod|
  	io.puts " <tr>"
 	io.puts "  <td>mod\##{mod}</td>"
 	(1..$pad).each { |pad|
 		io.puts "  <td>" + link_fig("dT#{$mod}_m#{mod}_p#{pad}_rev#{rev}") + "</td>"
  	}
  	io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"
end

################################################################
### print slew correction steps
def print_slew_steps_rev(io, run, rev)


  io.puts "<h3>Slewing Correction</h3>"
  io.puts "<div style=\"text-align: center;\">"


  io.puts "<table>"
  io.puts " <tr>"
  io.puts " <td></td>"
  (1..$pad).each { |p|
  	io.puts "<td>pad\##{p}</td>"
  }
  io.puts " </tr>"
  (1..rev).each { |r|
	io.puts " <tr>"
	io.puts "  <td>step #{r}</td>"
  	(1..$pad).each { |p|
  		m = (r-1) % $mod + 1
    	io.puts "  <td>" + link_fig("dT#{$mod}vsToT_m#{m}_p#{p}_rev#{r}") + "</td>"
  	}
  	io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"

end

################################################################
### print packet information
def print_packet_wise(io, run)
  io.puts "<h3>Packet-wise Analysis</h3>"
  io.puts "<div style=\"text-align: center;\">"
  io.puts "<table>"
  io.puts " <tr>"
  io.puts "  <td>" + link_fig("pid") + "</td>"
  io.puts "  <td>" + link_fig("ch") + "</td>"
  io.puts "  <td>" + link_fig("hit_event_ratio") + "</td>"
  io.puts " </tr>"
  io.puts "</table>"
  io.puts "</div>"

end

################################################################
### print detail link
def print_detail_link(io, run)
  io.puts "<h3>Details</h3>"
  io.puts "<ul>"
  io.puts " <li><a href=\"tot.html\">ToT Histograms</a></li>"
  io.puts "</ul>"
end

################################################################
### print ToT histograms

def print_tot(io, run)

  io.puts "<h3>ToT Histograms</h3>"
  io.puts "<div style=\"text-align: center;\">"

  io.puts "<table>"
  io.puts " <tr>"
  io.puts "  <td></td>"
  (1..$pad).each { |p|
  	io.puts "  <td>pad\##{p}</td>"
  }
  io.puts " </tr>"

  (1..$mod).each { |mod|
  	io.puts " <tr>"
 	io.puts "  <td>mod\##{mod}</td>"
 	(1..$pad).each { |pad|
 		io.puts "  <td>" + link_fig("ToT_m#{mod}_p#{pad}") + "</td>"
  	}
  	io.puts " </tr>"  
  }
  io.puts "</table>"  
  io.puts "</div>"
end


################################################################
### main function

dist = ARGV[0]
run  = ARGV[1]
$mod = ARGV[2].to_i if ARGV[2]
$pad = ARGV[3].to_i if ARGV[3]
$itr = ARGV[4].to_i if ARGV[4]

################################################################
### index.html
Dir.mkdir("#{dist}/#{run}") unless FileTest.exist?("#{dist}/#{run}")
Dir.mkdir("#{dist}/#{run}/fig") unless FileTest.exist?("#{dist}/#{run}/fig")
io = File.open("#{dist}/#{run}/index.html", "w")

print_header(io, run, "Test Stand Result")

print_summary(io, run)
print_packet_wise(io, run)
print_dtdist_rev(io, run, 1)
print_slew_steps_rev(io, run, $itr*$mod)
print_dtdist_rev(io, run, $itr*$mod+1)
print_detail_link(io, run)

print_footer(io, run)

io.close()

################################################################
### index.html
io = File.open("#{dist}/#{run}/tot.html", "w")

print_header(io, run, "Time over Thresholds")
print_tot(io, run)
print_footer(io, run)

io.close()

# end.
