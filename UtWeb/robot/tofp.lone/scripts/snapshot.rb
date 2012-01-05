#! /homes/tofp/c/ruby/bin/ruby

# $Id: snapshot.rb,v 1.23 2009/07/15 18:59:28 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
 
require 'rubygems'
require 'active_record'


ActiveRecord::Base.establish_connection({
      :adapter  => "mysql", 
      :database => "TOF",
#      :socket   => "/tmp/mysql.socket",
      :socket   => "/var/lib/mysql/mysql.sock",
      :username => "tofp",
      :password => "t4flight"
})

class Run < ActiveRecord::Base
end
#

SSH_OPT = "-p"
BASE = "/homes/tofp/public_html/_static/var/r"
obase = '/homes/tofp/public_html/_static/var/r'

Run.find(:all,
         :conditions => ["run_status_id = ?", 2],
         :order => "id desc").each do |r|

  odir = obase + '/' + r.id.to_s
  system('mkdir', '-p', odir)
  Dir.chdir(odir)
  next if File.exist?(".lock")
  lock = File.open(".lock", "w+")

  # old big stand
  if (r.run_type_id == 2 || r.run_type_id == 3) then
    ANALYZER = "/homes/tofp/c/tof/TsAnalyzer/TsAnalyzer"

    u = URI.parse(r.data_uri)
    dest = BASE + "/" + r.id.to_s
#    system("mkdir -p #{dest}")
    system("scp #{SSH_OPT} tof@#{u.host}:#{u.path} #{dest}/.")
    fn = "#{dest}/" + File.basename(u.path)
    figd = "#{dest}/fig"
    if(!FileTest.exist?(figd) || File.mtime(fn) > File.mtime(figd))
      system("#{ANALYZER} -d #{dest} -o #{dest} #{r.id} > #{dest}/tsanalyzer.log")
    end
  end

  # submarine timing resolution test
  if (r.run_type_id == 6) then

#    ENV['ROOTSYS'] = '/usr/local/root'
#    ENV['LD_LIBRARY_PATH'] = '/usr/local/root/lib'
    ENV['ROOTSYS'] = '/homes/tofp/c/root'
    ENV['LD_LIBRARY_PATH'] = '/opt/intel/cc/10.0.023/lib:/homes/tofp/c/root/lib/root'
    rootc = 'root'
    hbase = 'http://www.rhip.utexas.edu/~tofp/var/tr'
    cbase = '/homes/tofp/c/tof/submarine'
    cmd   = "#{cbase}/cosmic"

    uri = URI.parse(r.data_uri)
    system('scp', "tof@#{uri.host}:#{uri.path}", '.')
    system(cmd, "-c", ".", File.basename(uri.path))
  end

  # tray cosmic stand
  if (r.run_type_id == 7) then

#    ENV['ROOTSYS'] = '/usr/local/root'
#    ENV['LD_LIBRARY_PATH'] = '/usr/local/root/lib'
    ENV['ROOTSYS'] = '/homes/tofp/c/root'
    ENV['LD_LIBRARY_PATH'] = '/opt/intel/cc/10.0.023/lib:/homes/tofp/c/root/lib/root'
    rootc = 'root'
    hbase = 'http://www.rhip.utexas.edu/~tofp/var/tr'
    cbase = '/homes/tofp/c/tof/tray_cosmic'
    cmd   = "#{cbase}/cosmic3r"
    scr   = "#{cbase}/macros/draw.C"

    uri = URI.parse(r.data_uri)
    system('scp', "tof@#{uri.host}:#{uri.path}", '.')
    system(cmd, File.basename(uri.path))
#    system(cmd, "-g", File.basename(uri.path))
    system(rootc, '-b', '-q', scr)
  end

  lock.close
  File.delete(".lock")
end
