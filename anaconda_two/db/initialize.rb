require 'rubygems'
require 'active_record'
require 'pp'

db_name = "configurations.db"

ActiveRecord::Base.establish_connection(
  :adapter => "sqlite3",
  :database => db_name
)

class Configuration    < ActiveRecord::Base; end;
class Thub             < ActiveRecord::Base; end;
class Tcpu             < ActiveRecord::Base; end;
class TdcConfiguration < ActiveRecord::Base; end;

devs = [251, 253, 254, 255]

# (0...120).each do |i|
#   r = Tcpu.new(:device_id => devs[i/30], :canbus_id => 0x20 + (i % 30),
#                :ut_id => 0, :active => 1)
#   r.id = 1 + i
#   r.save
# end

Dir.glob("../config/cfig*.txt").each do |f|
  fname = File.basename(f)
  bit_string = File.open(f).read.gsub(/\r/, "")
  ary = bit_string.split("\n").map{|e| [e].pack("B8") }
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

