drop table if exists devices;
create table devices (
	id			integer primary key,
	devid       integer not null,
	name		char(64) not null,
	installed	boolean  not null default 'f'
);
insert into devices values (1, 251, "CANBus NW", 't');
insert into devices values (2, 254, "CANBus NE", 't');
insert into devices values (3, 255, "CANBus SW", 't');
insert into devices values (4, 250, "CANBus SE", 't');
insert into devices values (5, 252, "CANBus VPD W", 't');
insert into devices values (6, 253, "CANBus VPD E", 't');
insert into devices values (7, 249, "CANBus MTD1", 't');
-- select * from devices;
