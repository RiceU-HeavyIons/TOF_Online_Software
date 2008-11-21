drop   table tdc_configurations;
create table tdc_configurations (
	id			INTEGER         PRIMARY KEY AUTOINCREMENT,
	name		varchar(256)	not null,
	length		smallint		not null,
	checksum	smallint		not null,
	hex_string  varchar(256)	not null,
	bit_string  varchar(1024)	not null
);
select * from tdc_configurations;