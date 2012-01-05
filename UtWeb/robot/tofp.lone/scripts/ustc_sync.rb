#! /homes/tofp/c/ruby/bin/ruby
# $Id: ustc_sync.rb,v 1.5 2009/07/15 18:59:28 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
# this scritp reads test results from chinese database and puts them into mysql database
# 

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

class Mrpc < ActiveRecord::Base; set_table_name "modules" end
class ChineseResult < ActiveRecord::Base; end

DEBUG = true
ADDR  = "mrpc.ustc.edu.cn" # "210.45.72.42"
DTIME = Time.now()
USER  = 1
FLIST = ['ustc', 'tshu', 'thu_notr']
# FLIST = ['thu_notr']

def sync_db(r)
   if( r[:sn] =~ /^UM/ )
      vend = 1
    elsif( r[:sn] =~ /^TM/ )
      vend = 2
    end

    pp r if DEBUG

    if (r[:sn] =~ /^TM0(\d{4})/) then
      r[:sn] = "TM#{$1}"
    end

    mrpc = Mrpc.find(:first, :conditions => ["serial = ?", r[:sn]])
    if(mrpc)
      mrpc[:chinese_id] = r[:id]
      mrpc[:chinese_link] = r[:uri].gsub(/id=\d+/, "id=%d")
    else
      mrpc = Mrpc.create(
        :serial => r[:sn],
        :received_on => DTIME,
        :user_id => USER,
        :vendor_id => vend,
        :chinese_id => r[:id],
        :chinese_link => r[:uri].gsub(/id=\d+/, "id=%d"),
        :note => "AUTOMATICALLY ADDED" )
    end
    pp mrpc if DEBUG
    mrpc.save

    mid = mrpc[:id]

    [1, 2, 3, 4].each do |type|
      cr = ChineseResult.find(:first, 
        :conditions => ["chinese_id=? AND vendor_id=? AND result_type=? AND data_uri like ?",
          r[:id], vend, type, r[:pt] ])
      res = r[ [:tr, :nr, :sr, :st][type-1] ]
      if (!res.nil? && res.length > 0 && cr.nil?) then
        cr = ChineseResult.create(
          :module_id => mid,
          :chinese_id => r[:id],
          :vendor_id => vend,
          :serial => r[:sn],
          :result_type => type,
          :pad1 => res[0],
          :pad2 => res[1],
          :pad3 => res[2],
          :pad4 => res[3],
          :pad5 => res[4],
          :pad6 => res[5],
          :avg  => res.inject(0) {|s,i| s += i} / res.length,
          :min  => res.min,
          :max  => res.max,
          :data_uri => r[:uri]
        )
        pp cr if DEBUG
        cr.save
      end
    end
end

def get_stats(itm)
  path = "/" + itm.join('')
  id = itm[1].to_i
  print "accessing #{path}...\n" if DEBUG
  res = Net::HTTP.get(ADDR, path)

  ar = []
  sn = ""
  res.each{ |line|
    if(line =~ /<tr><td>MRPC sn.+<\/td><td>(\w+)<\/td><\/tr>/)
      sn = $1
    end
    if(line =~ /<td width="100">(\d+|\d+\.\d+)<\/td>/)
      ar << $1.to_f
    elsif(line =~ /<td width="100">(\.\d+)<\/td>/)
      ar << ("0" + $1).to_f
    elsif(line =~ /<td width="100"><\/td>/)
      ar << 0.0
    end
  }
  p ar if DEBUG
  nr = ar[0, 6]
  sr = ar[6, 6]
  if (path =~ /readustc\.asp/)
    pt = "%readustc%"
    st = ar[12, 6]
    st = nil if !st.nil? && st.max <= 0
    tr = ar[18, 6]
    tr = nil if !tr.nil? && tr.max <= 0
  elsif (path =~ /readtemp\.asp/)
    pt = "%readtemp%"
    st = nil
    tr = ar[12, 6]
    tr = nil if !tr.nil? && tr.max <= 0
  elsif (path =~ /readthu_notr\.asp/)
    pt = "%readthu_notr%"
    st = nil
    tr = nil
  end
    
  ret = {
    :sn => sn,
    :id => id,
    :nr => nr,
    :st => st,
    :sr => sr,
    :tr => tr,
    :uri => "http://" + ADDR + path,
    :pt  => pt }

  pp ret if DEBUG

    return ret
end

def get_list(vendor)
#  p vendor
  print "making dictionary for #{vendor}...\n" if DEBUG
  print "#{vendor} page:" if DEBUG

  res = Net::HTTP.get(ADDR, "/list#{vendor}.asp")
  pages = res.scan(/page=(\d+)/).map{|a| a[0].to_i }.uniq.reverse
  pages.each do |page|
    print "#{page} " if DEBUG
    res = Net::HTTP.get(ADDR, "/list#{vendor}.asp?page=#{page}")
    itms = []
    res.scan(/location='([a-z_]+\.asp\?id=)(\d+)'/).each do |itm|
      itms << itm
    end
    itms.reverse.each do |itm|
      pack = get_stats(itm)
      sync_db(pack)
    end
  end
  print "\n" if DEBUG
end

FLIST.each {|l| get_list(l) }
