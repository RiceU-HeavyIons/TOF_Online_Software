-- Configuration Tables

drop table if exists component_types;
create table component_types (
    id                  integer primary key,
    name                char(32)
);
insert into component_types values (1, "THUB");
insert into component_types values (2, "TCPU");
insert into component_types values (3, "TDIG");
insert into component_types values (4, "TPMT");
select * from component_types;


drop table if exists config_sets;
create table config_sets (
    id                  integer primary key,
    name                char(32) not null,
    description         vchar(128)
);
insert into config_sets values (1, "Standby",     "default mode uploaded when frontend starts");
insert into config_sets values (2, "Phys Mode 1", "Physics Mode 1");
insert into config_sets values (3, "Phys Mode 2", "Physics Mode 2");
insert into config_sets values (4, "Phys Mode 3", "Physics Mode 3");
insert into config_sets values (5, "Phys Mode 4", "Physics Mode 4");
insert into config_sets values (6, "Test Mode 1", "Test Mode 1");
insert into config_sets values (7, "Test Mode 2", "Test Mode 2");

drop table if exists config_types;
create table config_types (
    id          integer primary key,
    name        char(32)
);

insert into config_types values ( 1, "THUB_SOMETHING");
insert into config_types values (11, "TCPU_PLDREG02");
insert into config_types values (21, "TDIG_THRESHOLD");
insert into config_types values (31, "TDC_CONFIG");
insert into config_types values (32, "TDC_MASK");
select * from config_types;

drop table if exists configurations;
create table configurations (
    id                  integer primary key autoincrement,
    config_set_id       integer,
    config_type_id      integer,
    rule_order          integer,
    addr1               smallint,
    addr2               smallint,
    addr3               smallint,
    addr4               smallint,
    val                 integer
);
--                                   id  set  typ ord   ad1  ad2  ad3  ad4   value
insert into configurations values (   1,   1,  11,  1,   2, 255,   0,   0,      0);
insert into configurations values (   2,   1,  21,  2,   2, 255, 255,   0,   1500);
insert into configurations values (   3,   1,  31,  3,   2, 255, 255, 255,     13);
insert into configurations values (   4,   1,  31,  4,   2, 255,   1,   1,     14);
insert into configurations values (   5,   1,  31,  5,   2, 255,   8,   1,     14);

insert into configurations values (   6,   2,  11,  1,   2, 255,   0,   0,   255);
insert into configurations values (   7,   2,  21,  2,   2, 255, 255,   0,  1500);
insert into configurations values (   8,   2,  31,  3,   2, 255, 255, 255,    13);
insert into configurations values (   9,   2,  31,  4,   2, 255,   1,   1,    14);
insert into configurations values (  10,   2,  31,  5,   2, 255,   8,   1,    14);

-- THUBs
drop   table thubs;
create table thubs (
    id                  integer primary key,
    device_id           smallint not null,
    canbus_id           smallint not null,
    ut_id               smallint not null,
    active              boolean  not null default 'f'
);

insert into thubs values (  1, 251, 64, 0, 't');
insert into thubs values (  2, 253, 64, 0, 't');
insert into thubs values (  3, 254, 64, 0, 't');
insert into thubs values (  4, 255, 64, 0, 't');
select * from thubs;

-- TCPUs
drop   table tcpus;
create table tcpus (
    id                  integer primary key autoincrement,
    device_id           smallint not null,
    canbus_id           smallint not null,
    ut_id               smallint not null,
    active              boolean  not null default 'f'
);

