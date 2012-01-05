#! /bin/env ruby
# $Id: repair_filename.rb,v 1.1 2007/09/25 11:48:25 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved. 
require 'rubygems'
require 'active_record'

ActiveRecord::Base.establish_connection(
{
	:adapter => "mysql",
	:socket => "/var/lib/mysql/mysql.sock",
	:database => "TOF",
	:username => "tofp",
	:password => "t4flight"
})

class Runs < ActiveRecord::Base; end

Runs.find(:all).each do |r|
  id = r.id
  yr = r.start.year
  if(id >= 10000034)
    fn = "file://pegasus/home/data/production/#{yr}/ts#{id}.dat"
  elsif(id >= 10000000)
    fn = "file://pegasus/home/data/test_stand/ts#{id}.dat"
  elsif (id == 5090601 || id == 5090602)
    i = id / 100
    e = id % 100
    fn = "file://pegasus/home/data/test_stand/noise200#{i}#{e}.dat"
  elsif (id == 5112902 || id == 5112903)
    i = id / 100
    e = id % 100 - 1
    fn = sprintf("file://pegasus/home/data/test_stand/noise200%d%02d.dat", i, e)
  elsif (id == 5083101 || id == 5083102 || id == 5083103 || id == 5083104)
    i = id / 100
    e = ["_10K_100", "_1K_100", "_5K_1200", "_5K_200"][id % 100 - 1]
    fn = "file://pegasus/home/data/test_stand/noise200#{i}#{e}.dat"
  elsif(r.run_type_id == 2)
    fn = "file://pegasus/home/data/test_stand/"
    fn += "data200"
    i = id / 100;
    e = id % 100;
    fn += "#{i}"
    fn += "-#{e}" if(e != 0)
    fn += ".dat"
    r.data_uri = fn
  elsif(r.run_type_id == 3)
    fn = "file://pegasus/home/data/test_stand/"
    fn += "noise200"
    i = id / 100;
    e = id % 100;
    fn += "#{i}"
    fn += "-#{e}" if(e != 0) if(i == 51115)
    fn += ".dat"
    r.data_uri = fn
  end
  r.data_uri = fn
  r.save
end
