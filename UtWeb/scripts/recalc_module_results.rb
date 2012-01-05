#!/usr/bin/env ruby
# $Id: recalc_module_results.rb,v 1.2 2007/08/21 11:45:32 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

require 'pp'
require 'net/http'
require 'yaml'

require 'rubygems'
require 'active_record'
ActiveRecord::Base.establish_connection(
  :adapter => "mysql",
  :database => "TOF",
  :host => "localhost",
  :socket => "/var/lib/mysql/mysql.sock",
  :username => "tofp",
  :password => "t4flight"
)

class ModuleResult < ActiveRecord::Base; end
class ChineseResult < ActiveRecord::Base; end

ModuleResult.find(:all).each do |mr|
	ar = []
	(1..6).each {|i| ar << mr["pad#{i}"] }
	mr[:avg] = ar.inject(0) {|s,i| s += i } / ar.length
	mr[:min] = ar.min
	mr[:max] = ar.max
	mr.save
end


ChineseResult.find(:all).each do |mr|
	ar = []
	(1..6).each {|i| ar << mr["pad#{i}"] }
	mr[:avg] = ar.inject(0) {|s,i| s += i } / ar.length
	mr[:min] = ar.min
	mr[:max] = ar.max
	mr.save
end
