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
insert into config_sets values (6, "Test Mode 1", "Test Mode 1");

drop table if exists config_types;
create table config_types (
    id          integer primary key,
    name        char(32)
);

-- parameters
insert into config_types values (  1, "THUB_ENABLE");
insert into config_types values (  2, "THUB_TEMP_ALARM");
insert into config_types values ( 11, "TCPU_ENABLE");
insert into config_types values ( 12, "TCPU_PLDREG02");
insert into config_types values ( 13, "TCPU_TEMP_ALARM")
insert into config_types values ( 21, "TDIG_ENABLE");
insert into config_types values ( 21, "TDIG_THRESHOLD");
insert into config_types values ( 22, "TDIG_TEMP_ALARM");
insert into config_types values ( 31, "TDC_ENABLE");
insert into config_types values ( 31, "TDC_CONFIG");
insert into config_types values ( 32, "TDC_MASK");
insert into config_types values ( 41, "SRDS_ENABLE")
insert into config_types values ( 42, "SRDS_PLDREG9XBASE")
-- action items
insert into config_types values (101, "THUB_WRITE");
insert into config_types values (102, "THUB_RESET");
insert into config_types values (111, "TCPU_WRITE");
insert into config_types values (112, "TCPU_RESET");

insert into config_types values (121, "TDIG_WRITE");
insert into config_types values (122, "TDIG_RESET");
insert into config_types values (141, "SRDS_WRITE");

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
insert into configs values (null,   1,  31,  5,   2, 255,   5,   1,     14);
insert into configs values (null,   1,  42,  6,   1, 255, 255,   0,      0);

---------------------------   id  set  typ ord   ad1  ad2  ad3  ad4  value
insert into configs values (null,   2,  11,  1,   2, 255,   0,   0,    255);
insert into configs values (null,   2,  21,  2,   2, 255, 255,   0,   1500);
insert into configs values (null,   2,  31,  3,   2, 255, 255, 255,     13);
insert into configs values (null,   2,  31,  4,   2, 255,   1,   1,     14);
insert into configs values (null,   2,  31,  5,   2, 255,   5,   1,     14);
insert into configs values (null,   2,  42,  6,   1, 255, 255,   0,     16);
	
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
    tray_sn             char(3) not null,
    serdes              char(2) not null,
    active              boolean  not null default 0
);

-- west
insert into tcpus values (  1, 253, 43,  72,  68,  'C2', 1);
insert into tcpus values (  2, 253, 44,  58,  58,  'C3', 1);
insert into tcpus values (  3, 253, 45,  67,  63,  'D0', 1);
insert into tcpus values (  4, 253, 46,   9,   6,  'D1', 1);
insert into tcpus values (  5, 253, 47,  75,  71,  'D2', 1);
insert into tcpus values (  6, 253, 48,  71,  67,  'D3', 1);
insert into tcpus values (  7, 253, 49,  65,  61,  'E0', 1);
insert into tcpus values (  8, 253, 50,  54,  50,  'E1', 1);
insert into tcpus values (  9, 253, 51,  61,  57,  'E2', 1);
insert into tcpus values ( 10, 253, 52,  64,  60,  'E3', 1);
insert into tcpus values ( 11, 253, 53,  51,  47,  'F0', 1);
insert into tcpus values ( 12, 253, 54,  50,  46,  'F1',  1);
insert into tcpus values ( 13, 253, 55,  -1,  '',   '',  0);
insert into tcpus values ( 14, 253, 56,  -1,  '',   '',  0);
insert into tcpus values ( 15, 253, 57,  68,  64,  'G0', 1);
insert into tcpus values ( 16, 253, 58,  76,  72,  'G1', 1);
insert into tcpus values ( 17, 253, 59,  66,  62,  'G2', 1);
insert into tcpus values ( 18, 253, 60,  46,  42,  'G3', 1);
insert into tcpus values ( 19, 253, 51,  18,  14,  'H0', 1);
insert into tcpus values ( 20, 253, 62,  14,  11,  'H1', 1);

insert into tcpus values ( 21, 251, 33,  32,  28,  'A0', 1);
insert into tcpus values ( 22, 251, 34,  10,   7,  'A1', 1);
insert into tcpus values ( 23, 251, 35,  29,  25,  'A2', 1);
insert into tcpus values ( 24, 251, 36,  37,  33,  'A3', 1);
insert into tcpus values ( 25, 251, 37,  23,  19,  'B0', 1);
insert into tcpus values ( 26, 251, 38,  21,  17,  'B1', 1);
insert into tcpus values ( 27, 251, 39,  41,  37,  'B2', 1);
insert into tcpus values ( 28, 251, 40,  25,  21,  'B3', 1);
insert into tcpus values ( 29, 251, 41,  34,  30,  'C0', 1);
insert into tcpus values ( 30, 251, 42,  39,  35,  'C1', 1);
insert into tcpus values ( 31, 251, 43,  11,   8,  'C2', 1);
insert into tcpus values ( 32, 251, 44,  24,  20,  'C3', 1);
insert into tcpus values ( 33, 251, 45,  35,  31,  'D0', 1);
insert into tcpus values ( 34, 251, 46,  31,  27,  'D1', 1);
insert into tcpus values ( 35, 251, 47,  15,  12,  'D2', 1);
insert into tcpus values ( 36, 251, 48,  12,   9,  'D3', 1);
insert into tcpus values ( 37, 251, 49,  13,  10,  'E0', 1);
insert into tcpus values ( 38, 251, 50,  33,  29,  'E1', 1);
insert into tcpus values ( 39, 251, 51,  38,  34,  'E2', 1);
insert into tcpus values ( 40, 251, 52,  16,  13,  'E3', 1);
insert into tcpus values ( 41, 251, 53,  20,  16,  'F0', 1);
insert into tcpus values ( 42, 251, 54,  -1,  '',    '', 0);
insert into tcpus values ( 43, 251, 55,  -1,  '',    '', 0);
insert into tcpus values ( 44, 251, 56,  40,  36,  'F3', 1);
insert into tcpus values ( 45, 251, 57,  43,  39,  'G0', 1);
insert into tcpus values ( 46, 251, 58,  27,  23,  'G1', 1);
insert into tcpus values ( 47, 251, 59,  36,  32,  'G2', 1);
insert into tcpus values ( 48, 251, 60,  19,  15,  'G3', 1);
insert into tcpus values ( 49, 251, 61,  28,  24,  'H0', 1);
insert into tcpus values ( 50, 251, 62,  22,  18,  'H1', 1);

insert into tcpus values ( 51, 253, 33,  63,  59,  'A0', 1);
insert into tcpus values ( 52, 253, 34,  70,  66,  'A1', 1);
insert into tcpus values ( 53, 253, 35,  74,  70,  'A2', 1);
insert into tcpus values ( 54, 253, 36,  42,  38,  'A3', 1);
insert into tcpus values ( 55, 253, 37,  45,  41,  'B0', 1);
insert into tcpus values ( 56, 253, 38,  49,  45,  'B1', 1);
insert into tcpus values ( 57, 253, 39,  56,  52,  'B2', 1);
insert into tcpus values ( 58, 253, 40,  58,  54,  'B3', 1);
insert into tcpus values ( 59, 253, 41,  57,  53,  'C0', 1);
insert into tcpus values ( 60, 253, 42,  59,  55,  'C1', 1);


-- east
insert into tcpus values ( 61, 255, 58,  88,  84,  'G1', 1);
insert into tcpus values ( 62, 255, 59,  79,  75,  'G2', 1);
insert into tcpus values ( 63, 255, 60,  98,  94,  'G3', 1);
insert into tcpus values ( 64, 255, 61,  95,  91,  'H0', 1);
insert into tcpus values ( 65, 255, 62,  99,  95,  'H1', 1);

insert into tcpus values ( 66, 254, 33,  52,  48,  'A0', 1);
insert into tcpus values ( 67, 254, 34,  69,  65,  'A1', 1);
insert into tcpus values ( 68, 254, 35,  26,  22,  'A2', 1);
insert into tcpus values ( 69, 254, 36,  44,  40,  'A3', 1);
insert into tcpus values ( 70, 254, 37,  53,  49,  'B0', 1);
insert into tcpus values ( 71, 254, 38,   8,   5,  'B1', 1);
insert into tcpus values ( 72, 254, 39,   5,   2,  'B2', 1);
insert into tcpus values ( 73, 254, 40,  -1,  '',    '', 0);
insert into tcpus values ( 74, 254, 41,  -1,  '',    '', 0);
insert into tcpus values ( 75, 254, 42,   4,   1,  'C1', 1);
insert into tcpus values ( 76, 254, 43,   7,   4,  'C2', 1);
insert into tcpus values ( 77, 254, 44,   6,   3,  'C3', 1);
insert into tcpus values ( 78, 254, 45,  55,  51,  'D0', 1);
insert into tcpus values ( 79, 254, 46,  -1,  '',    '', 0);
insert into tcpus values ( 80, 254, 47,  -1,  '',    '', 0);
insert into tcpus values ( 81, 254, 48,  -1,  '',    '', 0);
insert into tcpus values ( 82, 254, 49,  -1,  '',    '', 0);
insert into tcpus values ( 83, 254, 50,  -1,  '',    '', 0);
insert into tcpus values ( 84, 254, 51,  -1,  '',    '', 0);
insert into tcpus values ( 85, 254, 52,  -1,  '',    '', 0);
insert into tcpus values ( 86, 254, 53,  -1,  '',    '', 0);
insert into tcpus values ( 87, 254, 54,  -1,  '',    '', 0);
insert into tcpus values ( 88, 254, 55,  -1,  '',    '', 0);
insert into tcpus values ( 89, 254, 56,  -1,  '',    '', 0);
insert into tcpus values ( 90, 254, 57,  -1,  '',    '', 0);
insert into tcpus values ( 91, 254, 58,  -1,  '',    '', 0);
insert into tcpus values ( 92, 254, 59,  -1,  '',    '', 0);
insert into tcpus values ( 93, 254, 60,  -1,  '',    '', 0);
insert into tcpus values ( 94, 254, 61,  -1,  '',    '', 0);
insert into tcpus values ( 95, 254, 62,  -1,  '',    '', 0);
insert into tcpus values ( 96, 255, 33,  -1,  '',    '', 0);
insert into tcpus values ( 97, 255, 34,  81,  77,  'A1', 1);
insert into tcpus values ( 98, 255, 35,  93,  89,  'A2', 1);
insert into tcpus values ( 99, 255, 36,  91,  87,  'A3', 1);
insert into tcpus values (100, 255, 37,  94,  90,  'B0', 1);
insert into tcpus values (101, 255, 38,  97,  93,  'B1', 1);
insert into tcpus values (102, 255, 39,  -1,  '',    '', 0);
insert into tcpus values (103, 255, 40,  -1,  '',    '', 0);
insert into tcpus values (104, 255, 41,  77,  73,  'C0', 1);
insert into tcpus values (105, 255, 42,  80,  76,  'C1', 1);
insert into tcpus values (106, 255, 43,  87,  83,  'C2', 1);
insert into tcpus values (107, 255, 44,  73,  69,  'C3', 1);
insert into tcpus values (108, 255, 45,  84,  80,  'D0', 1);
insert into tcpus values (109, 255, 46,  86,  82,  'D1', 1);
insert into tcpus values (110, 255, 47,  78,  74,  'D2', 1);
insert into tcpus values (111, 255, 48,   3, 'C',  'D3', 1);
insert into tcpus values (112, 255, 49,  83,  79,  'E0', 1);
insert into tcpus values (113, 255, 50,  47,  43,  'E1', 1);
insert into tcpus values (114, 255, 51,  90,  86,  'E2', 1);
insert into tcpus values (115, 255, 52,  89,  85,  'E3', 1);
insert into tcpus values (116, 255, 53,   2, 'B',  'F0', 1);
insert into tcpus values (117, 255, 54,  85,  81,  'F1', 1);
insert into tcpus values (118, 255, 55,  96,  92,  'F2', 1);
insert into tcpus values (119, 255, 56, 100,  96,  'F3', 1);
insert into tcpus values (120, 255, 57,  84,  78,  'G0', 1);