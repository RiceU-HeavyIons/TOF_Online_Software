require 'webrick'

OPTIONS = {
  :port            => 3000,
  :ip              => "0.0.0.0",
  :server_root     => File.expand_path(File.dirname(__FILE__) + "/../www/"),
  :server_type     => WEBrick::SimpleServer,
  :charset         => "UTF-8",
  :mime_types      => WEBrick::HTTPUtils::DefaultMimeTypes
}


# DispatchServlet.dispatch(OPTIONS)
options = OPTIONS

params = { :Port         => options[:port].to_i,
           :ServerType   => options[:server_type],
           :BindAddress  => options[:ip],
           :DocumentBoot => options[:server_root]}
           
params[:MimeTypes] = options[:mime_types] if options[:mime_types]

p options[:server_root]

server = WEBrick::HTTPServer.new(params)
server.mount("/", WEBrick::HTTPServlet::FileHandler, options[:server_root], true)
# server.mount('/', DispatchServlet, options)

trap("INT") { server.shutdown }

# require File.join(@server_options[:server_root], "..", "config", "environment") unless defined?(RAILS_ROOT)
# require "dispatcher"

server.start