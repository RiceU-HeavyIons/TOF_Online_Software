#! /homes/tofp/c/ruby/bin/ruby

# $Id: config_plot.rb,v 1.3 2009/07/15 18:59:58 tofp Exp $
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
class RunConfig < ActiveRecord::Base
    has_many :run
end
class Run < ActiveRecord::Base
  belongs_to :run_config
end
#

ENV['ROOTSYS'] = '/homes/tofp/c/root'
ENV['LD_LIBRARY_PATH'] = '/opt/intel/cc/10.0.023/lib:/homes/tofp/c/root/lib/root'
rootc = '/homes/tofp/c/root/bin/root'
cbase = '/homes/tofp/c/tof/tray_cosmic'
script   = "#{cbase}/macros/config_plot.C"

# update running configs' stop
Run.find(:all,
         :conditions => ["run_status_id = ?", 2],
         :order => "id desc").each do |r|

  rc = r.run_config
  rc.stop = Time.now
  rc.save
end

# then draw plots for configs whose stop is within 4 hours
RunConfig.find(:all,
          :conditions => ["stop > ?", 4.hours.ago]).each do |rc|
  system(rootc, '-b', '-q', script + "(" + rc.id.to_s + ",1)")
end