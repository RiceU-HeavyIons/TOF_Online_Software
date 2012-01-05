#!env ruby
# $Id: update_plots.rb,v 1.2 2007/08/21 11:45:32 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

# this script can be used to manipulate 'module_results' database entries
# and plots for them

require 'rubygems'
require 'active_record'

ActiveRecord::Base.establish_connection(
	:adapter => 'mysql',
	:database => 'TOF',
	:socket => '/var/lib/mysql/mysql.sock',
	:username => 'tofp',
	:password => 't4flight')

class ModuleResult < ActiveRecord::Base
  belongs_to :run
  belongs_to :mrpc, {:foreign_key => 'module_id'}
end

class Mrpc < ActiveRecord::Base
  set_table_name 'modules'
end

class Run < ActiveRecord::Base
end


def update_filename
  Run.find_all.each do |r|
    fn1 = r.data_uri
    fn2 = fn1.gsub(/file:\/\/pegasus\/+\/home\/data/,
                   "file://pegasus/home/data")
    fn2 = fn1.gsub(/file:\/\/pegasus\/\/tmp\/data/,
                   "file://pegasus/home/data/production")

    fn2 = fn1.gsub("file://pegasus/home/data/production/ts",
                   "file://pegasus/home/data/production/2006/ts")
    if(fn1 != fn2)
      p r.data_uri = fn2
      r.save
    end

  end
end

def update_plots
  rlist = {}

  ModuleResult.find_all.each do |mr|

    p fn1 = mr.run.data_uri
    fn2 = fn1.gsub(/file:\/\/pegasus\/+home\/data/,
                   "file://pegasus/home/data")

    fn3 = fn2.gsub("file://pegasus/home/data", "/data/star/tof/backup_pegasus")

    if (fn2 != fn1) # regularize fine name
      r = mr.run
      r.data_uri = fn2
      r.save
    end

    if mr.run.run_type_id == 2 # timing resolution
    elsif mr.run.run_type_id == 3 # noise rate
      # 	print [
      # 		mr.mrpc.id,
      # 		mr.mrpc.serial,
      # 		mr.mrpc.vendor_id,
      # 		mr.run.id,
      # 		mr.slot,
      # 		mr.pad1,
      # 		mr.pad2,
      # 		mr.pad3,
      # 		mr.pad4,
      # 		mr.pad5,
      # 		mr.pad6
      # 	].join(",")
      # 	print "\n"
      src = File.dirname(fn3)
      rlist[mr.run_id.to_i] = src
    end

  end

  dry = true

  cbase = "/homes/tofp/c/tof/TsAnalyzer/TsAnalyzer"
  dbase = "/homes/tofp/public_html/_static/var/mr/"
  ubase = "http://www.rhip.utexas.edu/~tofp/var/mr/"

  rlist.sort{|a,b| a[0] <=> b[0]}.each do |r|
    id = r[0]
    src = r[1]
    ext = DateTime.now.strftime("%Y%m%d%H%M%S")
    dst = dbase + ext
    p  cmd = cbase + " -d #{src} -o #{dst} #{id}"
    system(cmd) unless(dry)

    ModuleResult.find_all(["run_id = ?", id]).each do |mr|
      p uri = ubase + ext
      mr.data_uri = uri
      mr.save unless(dry)
    end

    sleep 2
  end
end

update_filename
