#! /homes/tofp/c/ruby/bin/ruby
# $Id: notify.rb,v 1.5 2009/07/15 18:59:28 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
 
require 'rubygems'
require 'active_record'
require 'action_mailer'

host = 'lone.rhip.utexas.edu'
port = 25

ActiveRecord::Base.establish_connection({
      :adapter  => "mysql", 
      :database => "TOF",
      :socket   => "/var/lib/mysql/mysql.sock",
      :username => "tofp",
      :password => "t4flight"
})

class RunLogLevel    < ActiveRecord::Base; end
class User           < ActiveRecord::Base; end
class NotifierConfig < ActiveRecord::Base; end
class RunLog < ActiveRecord::Base
  belongs_to :user
  belongs_to :run_log_level
end

active = NotifierConfig.find(:first, :conditions => ["name='active'"])
updated = NotifierConfig.find(:first, :conditions => ["name='updated'"])
if(active.value == '1')

  # get log entries
  logs = RunLog.find(:all, :conditions => ["datetime > ?", updated.value], :order => "datetime desc")
  now = DateTime.now.strftime("%Y-%m-%d %X")

  # for all human users
  User.find(:all, :conditions => ["id > 100"]).each { |u|
    msg = ""
    logs.each { |r|
      if(r.run_log_level_id >= u.notify_level)
        msg += sprintf "[%s] %-7s \"%s\"\n",
          r.datetime.strftime("%Y-%m-%d %X"),
          r.run_log_level.name,
          r.message
      end
    }
    if(msg.length > 0)
      greeting = <<-"EOM"
Hi #{u.name_first} #{u.name_last},

Your current notify level is #{u.notify_level}
Here is message for you since [#{updated.value}]:

EOM
      mail = TMail::Mail.new
      mail.to = u.email
      mail.from = 'notifier <tofp@lone.rhip.utexas.edu>'
      mail.subject = "teststand alert [#{now}]"
      mail.body = greeting + msg
      Net::SMTP.start(host, port) { |smtp|
        smtp.send_message mail.encoded, 'tofp@lone.rhip.utexas.edu', u.email
      }
      print "mail is sent to #{u.email}\n"
    end
  }
  updated.value = now
  updated.save 
end
