--
drop   table devices;
create table devices (
	id			integer primary key autoincrement,
	name		char(64) not null,
	active		boolean  not null default 0
);

insert into devices values (251, "TOF CAN 1", 1);
insert into devices values (253, "TOF CAN 2", 1);
insert into devices values (254, "TOF CAN 3", 1);
insert into devices values (255, "TOF CAN 4", 1);
select * from devices;

drop   table types;
create table types (
	id			smallint not null,
	name		char(16) not null,
	primary key(id)
);
insert into types values (1, "THUB");
insert into types values (2, "TCPU");
insert into types values (3, "TDIG");
insert into types values (4, "TPMT");
select * from types;

-- Configuration table
drop   table configurations;
create table configurations (
 	id			smallint not null,
	type_id		smallint not null,
 	device_id	smallint not null,
	canbus_id	smallint not null,
	ut_id		smallint not null,
	active		boolean  not null default 0,
	primary key(id)
);

-- TCPUs
insert into configurations values (  1, 2, 251, 50, 0, 1);
insert into configurations values (  2, 2, 251, 51, 0, 1);
insert into configurations values (  3, 2, 251, 52, 0, 1);
insert into configurations values (  4, 2, 251, 53, 0, 1);
insert into configurations values (  5, 2, 251, 54, 0, 1);

-- THUBs
insert into configurations values (  6, 1, 251, 64, 0, 1);
insert into configurations values (  7, 1, 253, 64, 0, 1);
insert into configurations values (  8, 1, 254, 64, 0, 0);
insert into configurations values (  9, 1, 255, 64, 0, 0);

select * from configurations;
