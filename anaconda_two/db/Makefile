
all: configurations.db

configurations.db : configs.sql devices.sql thubs.sql tcpus.sql \
                    tdc_configs.sql tdc_configs_init.sql
	@for i in $^; do sqlite3 $@ < $$i; done

tdc_configs_init.sql:
	ruby init_tdc_config.rb

dummy:
	ruby initialize.rb
	
clean:
	$(RM) configurations.db
