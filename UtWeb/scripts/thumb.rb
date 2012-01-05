#! /bin/env ruby
# $Id: thumb.rb,v 1.3 2007/08/21 11:45:32 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

$width = 120

def conv_all_file(dir)
  Dir.glob("#{dir}/*").each { |d|
    if ( File.directory?(d) )
      next if d =~ /thumb$/
      conv_all_file(d)
    else
      next unless d =~ /(.gif)|(.jpg)$/
      src = d
      fb = File.basename(src)
      fd = File.dirname(src)
      dd = File.join(fd, "thumb")
      Dir.mkdir(dd) unless File.exist?(dd)
      dst = File.join(dd, fb)
      unless ( File.exist?(dst) && File.ctime(src) <= File.ctime(dst) ) 
        print "#{src} => #{dst}\n"
        IO.popen("convert -resize #{$width} #{src} #{dst}")
      end
    end
  }
end

conv_all_file(ARGV[0])

