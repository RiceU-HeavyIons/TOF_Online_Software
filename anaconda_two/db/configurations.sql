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
drop table if exists tcpus;
create table tcpus (
    id                  integer primary key autoincrement,
    device_id           smallint not null,
    canbus_id           smallint not null,
    ut_id               smallint not null,
    active              boolean  not null default 'f'
);

-- west
insert into tcpus values (  1, 253, 41,  68,  1);
insert into tcpus values (  2, 253, 42,  58,  1);
insert into tcpus values (  3, 253, 43,  63,  1);
insert into tcpus values (  4, 253, 44,   6,  1);
insert into tcpus values (  5, 253, 45,  71,  1);
insert into tcpus values (  6, 253, 46,  67,  1);
insert into tcpus values (  7, 253, 47,  61,  1);
insert into tcpus values (  8, 253, 48,  50,  1);
insert into tcpus values (  9, 253, 49,  57,  1);
insert into tcpus values ( 10, 253, 50,  60,  1);
insert into tcpus values ( 11, 253, 51,  47,  1);
insert into tcpus values ( 12, 253, 52,  46,  1);
insert into tcpus values ( 13, 253, 53,   0,  0);
insert into tcpus values ( 14, 253, 54,   0,  0);
insert into tcpus values ( 15, 253, 55,  64,  1);
insert into tcpus values ( 16, 253, 56,  72,  1);
insert into tcpus values ( 17, 253, 57,  62,  1);
insert into tcpus values ( 18, 253, 58,  42,  1);
insert into tcpus values ( 19, 253, 59,  14,  1);
insert into tcpus values ( 20, 253, 60,  15,  1);

insert into tcpus values ( 21, 251, 33,  28,  1);
insert into tcpus values ( 22, 251, 34,   7,  0);
insert into tcpus values ( 23, 251, 35,  25,  0);
insert into tcpus values ( 24, 251, 36,  33,  1);
insert into tcpus values ( 25, 251, 37,  19,  1);
insert into tcpus values ( 26, 251, 38,  17,  1);
insert into tcpus values ( 27, 251, 39,  37,  1);
insert into tcpus values ( 28, 251, 40,  21,  1);
insert into tcpus values ( 29, 251, 41,  30,  1);
insert into tcpus values ( 30, 251, 42,  35,  1);
insert into tcpus values ( 31, 251, 43,   8,  1);
insert into tcpus values ( 32, 251, 44,  20,  1);
insert into tcpus values ( 33, 251, 45,  31,  1);
insert into tcpus values ( 34, 251, 46,  27,  1);
insert into tcpus values ( 35, 251, 47,  12,  1);
insert into tcpus values ( 36, 251, 48,   9,  1);
insert into tcpus values ( 37, 251, 49,  10,  1);
insert into tcpus values ( 38, 251, 50,  29,  1);
insert into tcpus values ( 39, 251, 51,  34,  1);
insert into tcpus values ( 40, 251, 52,  13,  1);
insert into tcpus values ( 41, 251, 53,  16,  1);
insert into tcpus values ( 42, 251, 54,   0,  0);
insert into tcpus values ( 43, 251, 55,   0,  0);
insert into tcpus values ( 44, 251, 56,  36,  1);
insert into tcpus values ( 45, 251, 57,  39,  1);
insert into tcpus values ( 46, 251, 58,  23,  1);
insert into tcpus values ( 47, 251, 59,  32,  1);
insert into tcpus values ( 48, 251, 60,  15,  1);
insert into tcpus values ( 49, 251, 61,  24,  1);
insert into tcpus values ( 50, 251, 62,  18,  1);

insert into tcpus values ( 51, 253, 33,  59,  1);
insert into tcpus values ( 52, 253, 34,  66,  1);
insert into tcpus values ( 53, 253, 35,  70,  1);
insert into tcpus values ( 54, 253, 36,  38,  1);
insert into tcpus values ( 55, 253, 37,  41,  1);
insert into tcpus values ( 56, 253, 38,  45,  1);
insert into tcpus values ( 57, 253, 39,  52,  1);
insert into tcpus values ( 58, 253, 40,  54,  1);
insert into tcpus values ( 59, 253, 41,  53,  1);
insert into tcpus values ( 60, 253, 42,  55,  1);


-- east
insert into tcpus values ( 61, 255, 58,  84,  1);
insert into tcpus values ( 62, 255, 59,  75,  1);
insert into tcpus values ( 63, 255, 60,  94,  1);
insert into tcpus values ( 64, 255, 61,  91,  1);
insert into tcpus values ( 65, 255, 62,  95,  1);

insert into tcpus values ( 66, 254, 33,  48,  1);
insert into tcpus values ( 67, 254, 34,  65,  1);
insert into tcpus values ( 68, 254, 35,  22,  1);
insert into tcpus values ( 69, 254, 36,  40,  1);
insert into tcpus values ( 70, 254, 37,  49,  1);
insert into tcpus values ( 71, 254, 38,   5,  1);
insert into tcpus values ( 72, 254, 39,   2,  1);
insert into tcpus values ( 73, 254, 40,   0,  0);
insert into tcpus values ( 74, 254, 41,   0,  0);
insert into tcpus values ( 75, 254, 42,   1,  1);
insert into tcpus values ( 76, 254, 43,   4,  1);
insert into tcpus values ( 77, 254, 44,   3,  1);
insert into tcpus values ( 78, 254, 45,  51,  1);
insert into tcpus values ( 79, 254, 46,   0,  0);
insert into tcpus values ( 80, 254, 47,   0,  0);
insert into tcpus values ( 81, 254, 48,   0,  0);
insert into tcpus values ( 82, 254, 49,   0,  0);
insert into tcpus values ( 83, 254, 50,   0,  0);
insert into tcpus values ( 84, 254, 51,   0,  0);
insert into tcpus values ( 85, 254, 52,   0,  0);
insert into tcpus values ( 86, 254, 53,   0,  0);
insert into tcpus values ( 87, 254, 54,   0,  0);
insert into tcpus values ( 88, 254, 55,   0,  0);
insert into tcpus values ( 89, 254, 56,   0,  0);
insert into tcpus values ( 90, 254, 57,   0,  0);
insert into tcpus values ( 91, 254, 58,   0,  0);
insert into tcpus values ( 92, 254, 59,   0,  0);
insert into tcpus values ( 93, 254, 60,   0,  0);
insert into tcpus values ( 94, 254, 61,   0,  0);
insert into tcpus values ( 95, 254, 62,   0,  0);

insert into tcpus values ( 96, 255, 33,   0,  0);
insert into tcpus values ( 97, 255, 34,  77,  1);
insert into tcpus values ( 98, 255, 35,  89,  1);
insert into tcpus values ( 99, 255, 36,  87,  1);
insert into tcpus values (100, 255, 37,  90,  1);
insert into tcpus values (101, 255, 38,  93,  1);
insert into tcpus values (102, 255, 39,   0,  0);
insert into tcpus values (103, 255, 40,   0,  0);
insert into tcpus values (104, 255, 41,  73,  1);
insert into tcpus values (105, 255, 42,  76,  1);
insert into tcpus values (106, 255, 43,  83,  1);
insert into tcpus values (107, 255, 44,  69,  1);
insert into tcpus values (108, 255, 45,  80,  1);
insert into tcpus values (109, 255, 46,  82,  1);
insert into tcpus values (110, 255, 47,  74,  1);
insert into tcpus values (111, 255, 48, 123,  1);
insert into tcpus values (112, 255, 49,  79,  1);
insert into tcpus values (113, 255, 50,  43,  1);
insert into tcpus values (114, 255, 51,  86,  1);
insert into tcpus values (115, 255, 52,  85,  1);
insert into tcpus values (116, 255, 53, 122,  1);
insert into tcpus values (117, 255, 54,  81,  1);
insert into tcpus values (118, 255, 55,  92,  1);
insert into tcpus values (119, 255, 56,  96,  1);
insert into tcpus values (120, 255, 57,  78,  1);