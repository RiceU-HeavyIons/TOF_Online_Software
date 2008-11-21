require 'rubygems'
require 'active_record'
require 'pp'

db_name = "configurations.db"

ActiveRecord::Base.establish_connection(
  :adapter => "sqlite3",
  :database => db_name
)

# class Configuration < ActiveRecord::Base; end;
class TdcConfiguration < ActiveRecord::Base; end;

Dir.glob("../config/cfig*.txt").each do |f|
  fname = File.basename(f)
  bit_string = File.open(f).read.gsub(/\w\n/, "\r\n")
  ary = bit_string.split("\r\n").map{|e| [e].pack("B8") }
  hex_ary = ary.map {|e| e.unpack("H2").first }
  num_ary = ary.map {|e| e.unpack("C").first }

  tdc_config = TdcConfiguration.create(
    :name       => fname,
    :length     => hex_ary.length,
    :checksum   => num_ary.inject(0) {|sum, e| sum += e},
    :bit_string => bit_string,
    :hex_string => hex_ary.join('')
  )
end

TdcConfiguration.find(:all).each do |tc|
  print "id=#{tc.id}, name=#{tc.name}, length=#{tc.length}\n"
end

