drop table if exists devices;
create table devices (
	id			integer primary key,
	devid       integer not null,
	name		char(64) not null,
	installed	boolean  not null default 'f'
);
begin transaction;
insert into devices values (1, 5, "CANBus M-S", 't');
insert into devices values (2, 7, "CANBus M-N", 't');
-- insert into devices values (7, 5, "CANBus MTD1", 't');
-- select * from devices;
commit transaction;
