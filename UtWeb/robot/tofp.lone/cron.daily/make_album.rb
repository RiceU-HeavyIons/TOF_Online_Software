#!/usr/bin/env ruby
# $Id: make_album.rb,v 1.15 2009/08/01 17:40:07 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

require 'date'
require 'yaml'
require 'tempfile'

PPP = 20 # pictures per page
ISIZE = 160 # thumbnail size
OVER_WRITE = false;

def a(link, text, target="_parent")
  target = " target=" + target if(target)
  "<a href=\"#{link}\"#{target}>#{text}</a>"
end

class File
  def println(str)
    self.print(str + "\n")
  end
end

def pagenate(page, max)
  ret = "<div>Page: "

  if(page == 1)
    ret += "Prev"
  else
    ret += a("index#{page-1}.html", "Prev")
  end

  (1..max).each {|n|
    if(n == page)
      ret += " <font color='red'>#{n}</font>"
    else
      ret += " " + a("index#{n}.html", n)
    end
  }

  if(page == max)
    ret += " Next"
  else
    ret += " " + a("index#{page+1}.html", "Next")
  end
  ret += "</div>"
end

def head(o, page, max)
  o.print <<-HEAD
   <html>
   <head>
   <style type="text/css">
   div.photo_box { 
      border: 2px solid #888;
      margin: 2px;
      float: left;
   }
   div.photo a { text-decoration: none; }
   div.caption { font-size: 80%; }
   </style>
   <script type="text/javascript" src="http://www.google-analytics.com/urchin.js"></script>
   <script type="text/javascript">
     _uacct="UA-1852182-1";
     urchinTracker();
   </script>
   </head>
   <body>
   <div id="main">
   HEAD
  o.print pagenate(page, max)
end

def foot(o)
  o.print <<-FOOT
   </div><!-- main -->
   </body>
   </html>
   FOOT
end

def search(alist, d)
  Dir.glob("#{d}/*").each do |f|
    f = File.expand_path(f)
    next if (f =~ /thumb/)
    search(alist, f) if( FileTest.directory?(f) )

    fn = File.basename(f)
    fe = File.extname(f)
    next unless (fe =~ /(jpg|gif|png)/i)

    File.chmod(0644, f)
    out = d + "/thumb/" + fn
    next if (alist[f] && FileTest.exists?(out))

    t = Tempfile.new("make_album")
    tmp = t.path + fe
    t.unlink

    c = `identify -verbose #{f} | grep "Date Time Original:"`.chop!
    c =~ /\s+Date Time Original: (\d+):(\d+):(\d+) (\d+):(\d+):(\d+)./
#   dt, tm = $1, $2
    dtime = Time.local($1, $2, $3, $4, $5, $6)
#   dtime = DateTime.parse(dt.split(":").join("-") + " " + tm)
    w, h = `identify #{f}`.split(" ")[2].split("x").map{|x| x.to_i}
    alist[f] = {
      'file'   => f,
      'thumb'  => out,
      'ctime'  => dtime,
      'width'  => w,
      'height' => h
    }

    if(w > h)
      geo = "#{ISIZE}"
    else
      geo = "x#{ISIZE}"
    end
    system("convert -thumbnail #{geo} #{f} #{tmp}")
    w, h = `identify #{tmp}`.split(" ")[2].split("x").map{|x| x.to_i}
    if(w > h)
      geo = "5x#{5+(w-h)/2}"
    else
      geo = "#{5+(h-w)/2}x5"
    end

    Dir::mkdir "#{d}/thumb" unless(File.exist?("#{d}/thumb"))
    if(!File.exists?(out) || OVER_WRITE) then
      system("convert -border #{geo} #{tmp} #{out}")
      File.chmod(0644, out)
    end
    File.delete(tmp)
  end
end

################
#### main

d = ARGV[0] || "/homes/tofp/public_html/_static/photo"
d = File.expand_path(d)
begin
  alist = YAML.load(File.open("#{d}/cache.yaml").read)
#  alist.each { |itm| # Time -> DateTime
#    itm[1]["ctime"] = DateTime.parse(itm[1]["ctime"].strftime("%Y-%m-%d %X"))
#  }
rescue
  alist = {}
end

alist = {} if(OVER_WRITE || alist.class != Hash)
search(alist, d)

File.open("#{d}/cache.yaml", "w+") {|f|
  yaml = YAML.dump(alist)
  f.puts yaml
}
maxp = (alist.size - 1) / PPP + 1
pg = 1
o = nil
cnt = 0
blist = alist.sort{ |a,b| b[1]["ctime"] <=> a[1]["ctime"] }
blist.each {|k, fi|
  if(cnt == 0)
    o = open("#{d}/index#{pg}.html", "w+")
    head(o, pg, maxp)
  end
  img = fi['file' ].sub("#{d}", ".")
  tmb = fi['thumb'].sub("#{d}", ".")
  fn = fi['file']
  dn = File.dirname(fn)
  fn = File.basename(fn)
  ct = fi["ctime"].strftime("%Y-%m-%d %H:%M:%S")
  o.println "<div class='photo_box'>"
  o.println "  <div class='photo'>" + a(img, "<img src='#{tmb}'></a></div>", "_blank")
  o.println "  <div class='caption'>#{ct}</div>"
  o.println "</div>"

  cnt += 1
  if(cnt >= PPP)
    foot(o)
    o.close
    cnt = 0
    pg += 1
  end
}

File.open("#{d}/index.json", "w+") {|f|
  ar = []
  i = 0;
  blist.each { |k, fi|
    txt  = "    {  \"file\": \""   + fi['file' ].gsub("#{d}", ".")  + "\",\n"
    txt += "       \"thumb\": \""  + fi['thumb'].gsub("#{d}", ".")  + "\",\n"
    txt += "       \"ctime\": \""  + fi['ctime'].strftime("%Y-%m-%d %X")  + "\",\n"
    txt += "       \"width\": \""  + fi['width'].to_s  + "\",\n"
    txt += "       \"height\": \"" + fi['height'].to_s + "\"}"
    i+=1
    ar.push(txt)
  }
  f.puts "{"
  f.puts "  \"list\": ["
  f.puts ar.join(",\n")
  f.puts "  ]"
  f.puts "}"
}

