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

drop table if exists configs;
create table configs (
    id                  integer primary key autoincrement,
    config_set_id       integer,
    config_type_id      integer,
    rule_order          integer,
    addr1               integer,
    addr2               integer,
    addr3               integer,
    addr4               integer,
    val                 integer
);
---------- ----------------   id  set  typ ord   ad1  ad2  ad3  ad4  value
insert into configs values (null,   1,  11,  1,   2, 255,   0,   0,      0);
insert into configs values (null,   1,  21,  2,   2, 255, 255,   0,   1500);
insert into configs values (null,   1,  31,  3,   2, 255, 255, 255,     13);
insert into configs values (null,   1,  31,  4,   2, 255,   1,   1,     14);
insert into configs values (null,   1,  31,  5,   2, 255,   8,   1,     14);

---------------------------   id  set  typ ord   ad1  ad2  ad3  ad4  value
insert into configs values (null,   2,  11,  1,   2, 255,   0,   0,    255);
insert into configs values (null,   2,  21,  2,   2, 255, 255,   0,   1500);
insert into configs values (null,   2,  31,  3,   2, 255, 255, 255,     13);
insert into configs values (null,   2,  31,  4,   2, 255,   1,   1,     14);
insert into configs values (null,   2,  31,  5,   2, 255,   8,   1,     14);

-- THUBs
drop   table thubs;
create table thubs (
    id                  integer primary key,
    device_id           integer not null,
    canbus_id           integer not null,
    thub_id             integer not null,
    thub_sn             integer not null,
    active              boolean  not null default 0
);

insert into thubs values (  1, 251, 64,   1,   1,   1);
insert into thubs values (  2, 253, 64,   2,   2,   1);
insert into thubs values (  3, 254, 64,   3,   3,   1);
insert into thubs values (  4, 255, 64,   4,   4,   1);
select * from thubs;

-- TCPUs
drop table if exists tcpus;
create table tcpus (
    id                  integer primary key autoincrement,
    device_id           integer not null,
    canbus_id           integer not null,
    tray_id             integer not null,
    tray_sn             char(4) not null,
    active              boolean  not null default 0
);

-- west
insert into tcpus values (  1, 253, 43,  72,  68,  1);
insert into tcpus values (  2, 253, 44,  58,  58,  1);
insert into tcpus values (  3, 253, 45,  67,  63,  1);
insert into tcpus values (  4, 253, 46,   9,   6,  1);
insert into tcpus values (  5, 253, 47,  75,  71,  1);
insert into tcpus values (  6, 253, 48,  71,  67,  1);
insert into tcpus values (  7, 253, 49,  65,  61,  1);
insert into tcpus values (  8, 253, 50,  54,  50,  1);
insert into tcpus values (  9, 253, 51,  61,  57,  1);
insert into tcpus values ( 10, 253, 52,  64,  60,  1);
insert into tcpus values ( 11, 253, 53,  51,  47,  1);
insert into tcpus values ( 12, 253, 54,  50,  46,  1);
insert into tcpus values ( 13, 253, 55,  -1,  '',  0);
insert into tcpus values ( 14, 253, 56,  -1,  '',  0);
insert into tcpus values ( 15, 253, 57,  68,  64,  1);
insert into tcpus values ( 16, 253, 58,  76,  72,  1);
insert into tcpus values ( 17, 253, 59,  66,  62,  1);
insert into tcpus values ( 18, 253, 60,  46,  42,  1);
insert into tcpus values ( 19, 253, 51,  18,  14,  1);
insert into tcpus values ( 20, 253, 62,  14,  11,  1);

insert into tcpus values ( 21, 251, 33,  32,  28,  1);
insert into tcpus values ( 22, 251, 34,  10,   7,  1);
insert into tcpus values ( 23, 251, 35,  29,  25,  1);
insert into tcpus values ( 24, 251, 36,  37,  33,  1);
insert into tcpus values ( 25, 251, 37,  23,  19,  1);
insert into tcpus values ( 26, 251, 38,  21,  17,  1);
insert into tcpus values ( 27, 251, 39,  41,  37,  1);
insert into tcpus values ( 28, 251, 40,  25,  21,  1);
insert into tcpus values ( 29, 251, 41,  34,  30,  1);
insert into tcpus values ( 30, 251, 42,  39,  35,  1);
insert into tcpus values ( 31, 251, 43,  11,   8,  1);
insert into tcpus values ( 32, 251, 44,  24,  20,  1);
insert into tcpus values ( 33, 251, 45,  35,  31,  1);
insert into tcpus values ( 34, 251, 46,  31,  27,  1);
insert into tcpus values ( 35, 251, 47,  15,  12,  1);
insert into tcpus values ( 36, 251, 48,  12,   9,  1);
insert into tcpus values ( 37, 251, 49,  13,  10,  1);
insert into tcpus values ( 38, 251, 50,  33,  29,  1);
insert into tcpus values ( 39, 251, 51,  38,  34,  1);
insert into tcpus values ( 40, 251, 52,  16,  13,  1);
insert into tcpus values ( 41, 251, 53,  20,  16,  1);
insert into tcpus values ( 42, 251, 54,  -1,  '',  0);
insert into tcpus values ( 43, 251, 55,  -1,  '',  0);
insert into tcpus values ( 44, 251, 56,  40,  36,  1);
insert into tcpus values ( 45, 251, 57,  43,  39,  1);
insert into tcpus values ( 46, 251, 58,  27,  23,  1);
insert into tcpus values ( 47, 251, 59,  36,  32,  1);
insert into tcpus values ( 48, 251, 60,  19,  15,  1);
insert into tcpus values ( 49, 251, 61,  28,  24,  1);
insert into tcpus values ( 50, 251, 62,  22,  18,  1);

insert into tcpus values ( 51, 253, 33,  63,  59,  1);
insert into tcpus values ( 52, 253, 34,  70,  66,  1);
insert into tcpus values ( 53, 253, 35,  74,  70,  1);
insert into tcpus values ( 54, 253, 36,  42,  38,  1);
insert into tcpus values ( 55, 253, 37,  45,  41,  1);
insert into tcpus values ( 56, 253, 38,  49,  45,  1);
insert into tcpus values ( 57, 253, 39,  56,  52,  1);
insert into tcpus values ( 58, 253, 40,  58,  54,  1);
insert into tcpus values ( 59, 253, 41,  57,  53,  1);
insert into tcpus values ( 60, 253, 42,  59,  55,  1);


-- east
insert into tcpus values ( 61, 255, 58,  88,  84,  1);
insert into tcpus values ( 62, 255, 59,  79,  75,  1);
insert into tcpus values ( 63, 255, 60,  98,  94,  1);
insert into tcpus values ( 64, 255, 61,  95,  91,  1);
insert into tcpus values ( 65, 255, 62,  99,  95,  1);

insert into tcpus values ( 66, 254, 33,  52,  48,  1);
insert into tcpus values ( 67, 254, 34,  69,  65,  1);
insert into tcpus values ( 68, 254, 35,  26,  22,  1);
insert into tcpus values ( 69, 254, 36,  44,  40,  1);
insert into tcpus values ( 70, 254, 37,  53,  49,  1);
insert into tcpus values ( 71, 254, 38,   8,   5,  1);
insert into tcpus values ( 72, 254, 39,   5,   2,  1);
insert into tcpus values ( 73, 254, 40,  -1,  '',  0);
insert into tcpus values ( 74, 254, 41,  -1,  '',  0);
insert into tcpus values ( 75, 254, 42,   4,   1,  1);
insert into tcpus values ( 76, 254, 43,   7,   4,  1);
insert into tcpus values ( 77, 254, 44,   6,   3,  1);
insert into tcpus values ( 78, 254, 45,  55,  51,  1);
insert into tcpus values ( 79, 254, 46,  -1,  '',  0);
insert into tcpus values ( 80, 254, 47,  -1,  '',  0);
insert into tcpus values ( 81, 254, 48,  -1,  '',  0);
insert into tcpus values ( 82, 254, 49,  -1,  '',  0);
insert into tcpus values ( 83, 254, 50,  -1,  '',  0);
insert into tcpus values ( 84, 254, 51,  -1,  '',  0);
insert into tcpus values ( 85, 254, 52,  -1,  '',  0);
insert into tcpus values ( 86, 254, 53,  -1,  '',  0);
insert into tcpus values ( 87, 254, 54,  -1,  '',  0);
insert into tcpus values ( 88, 254, 55,  -1,  '',  0);
insert into tcpus values ( 89, 254, 56,  -1,  '',  0);
insert into tcpus values ( 90, 254, 57,  -1,  '',  0);
insert into tcpus values ( 91, 254, 58,  -1,  '',  0);
insert into tcpus values ( 92, 254, 59,  -1,  '',  0);
insert into tcpus values ( 93, 254, 60,  -1,  '',  0);
insert into tcpus values ( 94, 254, 61,  -1,  '',  0);
insert into tcpus values ( 95, 254, 62,  -1,  '',  0);
insert into tcpus values ( 96, 255, 33,  -1,  '',  0);
insert into tcpus values ( 97, 255, 34,  81,  77,  1);
insert into tcpus values ( 98, 255, 35,  93,  89,  1);
insert into tcpus values ( 99, 255, 36,  91,  87,  1);
insert into tcpus values (100, 255, 37,  94,  90,  1);
insert into tcpus values (101, 255, 38,  97,  93,  1);
insert into tcpus values (102, 255, 39,  -1,  '',  0);
insert into tcpus values (103, 255, 40,  -1,  '',  0);
insert into tcpus values (104, 255, 41,  77,  73,  1);
insert into tcpus values (105, 255, 42,  80,  76,  1);
insert into tcpus values (106, 255, 43,  87,  83,  1);
insert into tcpus values (107, 255, 44,  73,  69,  1);
insert into tcpus values (108, 255, 45,  84,  80,  1);
insert into tcpus values (109, 255, 46,  86,  82,  1);
insert into tcpus values (110, 255, 47,  78,  74,  1);
insert into tcpus values (111, 255, 48,   3, 'C',  1);
insert into tcpus values (112, 255, 49,  83,  79,  1);
insert into tcpus values (113, 255, 50,  47,  43,  1);
insert into tcpus values (114, 255, 51,  90,  86,  1);
insert into tcpus values (115, 255, 52,  89,  85,  1);
insert into tcpus values (116, 255, 53,   2, 'B',  1);
insert into tcpus values (117, 255, 54,  85,  81,  1);
insert into tcpus values (118, 255, 55,  96,  92,  1);
insert into tcpus values (119, 255, 56, 100,  96,  1);
insert into tcpus values (120, 255, 57,  84,  78,  1);