
s = File.open("tdc_configs_init.sql", "w+")
Dir.glob("../config/cfig*.txt").each do |f|
  fname = File.basename(f)
  bit_string = File.open(f).read.gsub(/\r/, "")
  ary = bit_string.split("\n").map{|e| [e].pack("B8") }
  hex_ary = ary.map {|e| e.unpack("H2").first }
  num_ary = ary.map {|e| e.unpack("C").first }

  s.printf "insert into tdc_configs (name, length, checksum, hex_string, bit_string) values ('%s', %d, %d, '%s', '%s');\n",
    fname, hex_ary.length, num_ary.inject(0) {|sum, e| sum += e},
    hex_ary.join(''), bit_string
end
s.close

#TdcConfig.find(:all).each do |tc|
#  print "id=#{tc.id}, name=#{tc.name}, length=#{tc.length}\n"
#end

