#!env ruby
# $Id: copy_tray_notes.rb,v 1.2 2008/09/24 20:51:32 tofp Exp $
# Copyright (C) 2008, Kohei Kajimoto. All rights reserved.

# this script can be used to manipulate 'module_results' database entries
# and plots for them

require 'pp'
require 'rubygems'
require 'active_record'

ActiveRecord::Base.establish_connection(
  :adapter => 'mysql',
  :database => 'TOF',
  :socket => '/var/lib/mysql/mysql.sock',
  :username => 'tofp',
  :password => 't4flight')

class Tray < ActiveRecord::Base
  has_many :tray_note
end

class User < ActiveRecord::Base
  has_many :tray_note
end
class TrayNote < ActiveRecord::Base
  belongs_to :tray
  belongs_to :user
end

ar = Array.new
ar2 = Array.new

Tray::find(:all).each do |t|
  ar2.push({:tray_id => t.id, :tray_sn => t.sn, :notes => t.notes})
  t.notes.split("<br>").each do |e|
    if (e =~ /\s*(\d{4}-\d{2}-\d{2})\s*(.+)/) then
      d = Date.parse($1)
      ar.push({:tid => t.id, :date => d, :text => $2})
    end
  end
end

ar.sort{|a,b| a[:date] <=> b[:date]}.each do |e|
  e[:text].gsub!("\n", "")
  e[:text].gsub!("\t", "")

  tns = TrayNote.find(:all, :conditions => ["created_on=? and tray_id=? and note=?",
    e[:date], e[:tid], e[:text]])
  if(tns.size == 0) then
    tn = TrayNote.new({
      :user => User.find(107),
      :created_on => e[:date],
      :tray => Tray.find(e[:tid]),
      :note => e[:text]})
#    tn.save
    print tn.id, "|", tn.created_on, "|", tn.tray.id, "|", tn.note, "\n"
  end
end

ar2.each do |e|
  print e[:tray_id], ",", e[:tray_sn], ",\"", e[:notes], "\"\n";
end
