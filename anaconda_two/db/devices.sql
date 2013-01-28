drop table if exists devices;
create table devices (
	id			integer primary key,
	devid       integer not null,
	name		char(64) not null,
	installed	boolean  not null default 'f'
);
begin transaction;
insert into devices values (1, 1, "CANBus NW", 't');
insert into devices values (2, 2, "CANBus NE", 't');
insert into devices values (3, 3, "CANBus SW", 't');
insert into devices values (4, 6, "CANBus SE", 't');
insert into devices values (5, 0, "CANBus VPD W", 't');
insert into devices values (6, 5, "CANBus VPD E", 't');
-- insert into devices values (7, 5, "CANBus MTD1", 't');
-- select * from devices;
commit transaction;
