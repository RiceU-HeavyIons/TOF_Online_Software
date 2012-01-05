#!/homes/tofp/c/ruby/bin/ruby

require 'yaml'

#### copied from tof_db.rb
require 'rubygems'
require 'active_record'

require 'pp'
require 'date'

class Array
  def avg
    return inject(0) {|s, e| s += e} / length
  end
end

class DateTime
  def to_time
    return Time.local(*[year, month, day, hour, min, sec])
  end
end

ActiveRecord::Base.establish_connection(
    :adapter  => 'mysql',
    :database => 'TOF',
    :host     => 'localhost',
    :socket   => '/var/lib/mysql/mysql.sock',
    :username => 'tofp',
    :password => 't4flight' )

class User < ActiveRecord::Base
  has_many :run
  has_many :module_result
end;

class RunConfigType < ActiveRecord::Base; end

class RunConfig < ActiveRecord::Base
  belongs_to :user, :foreign_key => "user_id"
  belongs_to :run_config_type
  has_many :run
  belongs_to :location
  serialize :module_ids
end

class RunType < ActiveRecord::Base
  has_many :module_result
end

class RunStatus < ActiveRecord::Base
  has_many :run
end

class Run < ActiveRecord::Base
  belongs_to :run_status
  belongs_to :run_type
  belongs_to :run_config
  belongs_to :user
  has_many :module_result
end


class ModuleResult < ActiveRecord::Base
  belongs_to :mrpc, :foreign_key => "module_id"
  belongs_to :user
  belongs_to :run
end

class Mrpc < ActiveRecord::Base
  set_table_name "modules"
  belongs_to :tray
end

class Tray < ActiveRecord::Base
  has_many :mrpc
  has_many :component
end

class GasReading < ActiveRecord::Base
end

class ComponentType < ActiveRecord::Base
  has_many :component
end

class Component < ActiveRecord::Base
  belongs_to :component_type, :foreign_key => 'component_type_id'
  belongs_to :tray
end

class Location < ActiveRecord::Base; end
### end of tof_db.rb

# ActiveRecord::Base.logger = Logger.new(STDOUT)
# $DEBUG=true

def print_success(rid)
  link="../run/show.php?id=#{rid}"

  print <<"EOL"
<html>
<head>
<link rel="stylesheet" type="text/css" href="../style/default.css"/>
<script type="text/javascript">
<!--
  function redirect() { window.location='#{link}'; }
</script>
</head>
<body class="login" onload="setTimeout('redirect()', 5000)">
<div id="container">
<div id="dialog">
 Upload Succeeded!<br />
 Redirecting in 5 seconds...</div>
</div>
</body>
</html>
EOL
end

def print_error()

  print <<"EOL"
<html>
<head>
<link rel="stylesheet" type="text/css" href="../style/default.css"/>
</head>
<body class="login">
<div id="container">
<div id="dialog">Error Occurred!<br/>
Please contact to administrator.</div>
</div>
</body>
</html>
EOL
end

begin

  fname   = ARGV[0]
  tarball = ARGV[1]

  now = Time.now
  File.umask(0003)

  obase = '/homes/tofp/public_html/_static/var/wah'
  hbase = 'http://www.rhip.utexas.edu/~tofp/var/wah'

  odir = File.basename(fname, ".tgz")

  Dir.chdir(obase)
# system('cp', tarball, fname)
  system('tar' ,'zxf', tarball)
  Dir.chdir(odir) do

    ydat = []
    File.open('report.yaml') do |f|
	  ydat = YAML.load(f.read)
    end

    start = Time.local(*ydat[:start].split(/[ :-]/))
    stop  = Time.local(*ydat[:stop].split(/[ :-]/))

    tray = Tray::find(ydat[:tray_id])
    user = User::find(ydat[:user_id])
    location = Location::find(ydat[:location_id])

    mods = tray.mrpc.sort{|a, b| a.slot <=> b.slot}

    if (ydat[:nr].length / 6 != mods.length) then
      raise "data length dosen't match with tray number"
    end

    rc = RunConfig::create(
      :user => user,
      :created_on => ydat[:start],
      :n_modules => ydat[:n_modules],
      :n_pads => ydat[:n_pads],

      :threshold => ydat[:threshold],
      :impedance => 0,
      :hv_plus => ydat[:hv_plus],
      :hv_minus => ydat[:hv_minus],
      :flow_rate1 => ydat[:flow_rate1],
      :flow_rate2 => ydat[:flow_rate2],
      :flow_rate3 => ydat[:flow_rate3],
      :note => ydat[:note],
      :module_ids => [tray.id],
      :run_config_type_id => ydat[:run_config_type_id],
      :start => start,
      :stop => stop,
      :hv_channel_plus => ydat[:hv_channel_plus],
      :hv_channel_minus => ydat[:hv_channel_minus],
      :gas_channel_1 => ydat[:gas_channel_1],
      :gas_channel_2 => ydat[:gas_channel_2],
      :gas_channel_3 => ydat[:gas_channel_3],
      :location => location
    )
    r = Run::create(
      :run_status_id => 3, # finished
      :run_type_id => 4, # Tray Noise Test
      :run_config => rc,
      :user => user,
      :start => start,
      :stop => stop,
      :run_time => ydat[:run_time],
      :events => ydat[:events],
      :data_uri => ydat[:data_uri],
      :note => ydat[:note]
    )

    slot = 1;
    mods.each do |m|
      pad = ydat[:nr].slice!(0, 6)
      mr = ModuleResult::create(
            :mrpc        => m,
            :analyzed_on => now,
            :user        => user,
            :run         => r,
            :slot        => slot,
            :pad1        => pad[0],
            :pad2        => pad[1],
            :pad3        => pad[2],
            :pad4        => pad[3],
            :pad5        => pad[4],
            :pad6        => pad[5],
            :avg         => pad.avg,
            :min         => pad.min,
            :max         => pad.max,
            :data_uri    => hbase + "/" + File.basename(odir) )
      slot += 1
    end
    print_success(r.id)
  end
rescue
  File.unlink(* Dir.glob(odir + "/*.*"))
  Dir.rmdir(odir)
end
