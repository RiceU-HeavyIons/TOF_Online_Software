
all: configurations.db

configurations.db : configurations.sql devices.sql tdc_configurations.sql
	@for i in $^; do sqlite3 $@ < $$i; done
	ruby initialize.rb
	
clean:
	$(RM) configurations.db