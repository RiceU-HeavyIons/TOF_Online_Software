
all: configurations.db

configurations.db : configs.sql devices.sql thubs.sql tcpus.sql \
                    tdc_configs.sql
	@for i in $^; do sqlite3 $@ < $$i; done
	ruby initialize.rb
	
clean:
	$(RM) configurations.db