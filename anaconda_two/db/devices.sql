drop table if exists devices;
create table devices (
	id			integer primary key,
	name		char(64) not null,
	active		boolean  not null default 'f'
);
insert into devices values (251, "TOF CAN 1", 't');
insert into devices values (253, "TOF CAN 2", 't');
insert into devices values (254, "TOF CAN 3", 't');
insert into devices values (255, "TOF CAN 4", 't');
-- select * from devices;
