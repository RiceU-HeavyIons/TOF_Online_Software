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
insert into config_types values (  1, "THUB_ENABLE"      );
insert into config_types values (  2, "THUB_TEMP_ALARM"  );
insert into config_types values ( 11, "SRDS_ENABLE"      );
insert into config_types values ( 12, "SRDS_PLDREG9XBASE");
insert into config_types values ( 21, "TCPU_ENABLE"      );
insert into config_types values ( 22, "TCPU_TEMP_ALARM"  );
insert into config_types values ( 23, "TCPU_PLDREG02"    );
insert into config_types values ( 31, "TDIG_ENABLE"      );
insert into config_types values ( 32, "TDIG_TEMP_ALARM"  );
insert into config_types values ( 33, "TDIG_THRESHOLD"   );
insert into config_types values ( 41, "TDC_ENABLE"       );
insert into config_types values ( 42, "TDC_CONFIG"       );
insert into config_types values ( 43, "TDC_MASK"         );

-- action items
insert into config_types values (101, "THUB_RESET");
insert into config_types values (102, "THUB_WRITE");
insert into config_types values (111, "SRDS_RESET");
insert into config_types values (112, "SRDS_WRITE");
insert into config_types values (121, "TCPU_RESET");
insert into config_types values (122, "TCPU_WRITE");
insert into config_types values (131, "TDIG_RESET");
insert into config_types values (132, "TDIG_WRITE");
insert into config_types values (141, "TDC_RESET");
insert into config_types values (142, "TDC_WRITE");
insert into config_types values (143, "TDC_CONFIG");

select * from config_types;

drop table if exists configs;
create table configs (
    id                  integer primary key autoincrement,
    config_set_id       integer,
    config_set_order    integer,
    config_type_id      integer,
    addr1               integer,
    addr2               integer,
    addr3               integer,
    addr4               integer,
    val                 integer
);

---------- ----------------   id  set ord  typ  ad1  ad2  ad3  ad4  value
insert into configs values (null,   1,  1,  12,   1, 255, 255,   0,      0);
insert into configs values (null,   1,  2,  23,   2, 255,   0,   0,      0);
insert into configs values (null,   1,  3,  33,   2, 255, 255,   0,   1500);
insert into configs values (null,   1,  4,  42,   2, 255, 255, 255,     13);
insert into configs values (null,   1,  5,  42,   2, 255,   1,   1,     14);
insert into configs values (null,   1,  6,  42,   2, 255,   5,   1,     14);

---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null,   2,  1,   1,   1, 255,   0,   0,      0);
insert into configs values (null,   2,  2,  12,   1, 255, 255,   0,     16);
insert into configs values (null,   2,  3,  23,   2, 255,   0,   0,     15);
insert into configs values (null,   2,  4,  33,   2, 255, 255,   0,   1500);
insert into configs values (null,   2,  5,  42,   2, 255, 255, 255,     13);
insert into configs values (null,   2,  6,  42,   2, 255,   1,   1,     14);
insert into configs values (null,   2,  7,  42,   2, 255,   5,   1,     14);
insert into configs values (null,   2,  8, 131,   2, 255, 255,   0,      0);

-- THUBs
drop   table if exists thubs;
create table thubs (
    id                  integer primary key,
    device_id           integer not null,
    canbus_id           integer not null,
    thub_id             integer not null,
    thub_sn             integer not null,
    active              boolean  not null default 0
);

insert into thubs values (  1,   1, 64,   1,   1,   1);
insert into thubs values (  2,   2, 64,   2,   2,   1);
insert into thubs values (  3,   3, 64,   3,   3,   1);
insert into thubs values (  4,   4, 64,   4,   4,   1);
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
    lv_box              integer not null,
    lv_ch               integer not null,
    hv_box              integer not null,
    hv_ch               integer not null,
    active              boolean  not null default 0
);

-- west
insert into tcpus values (  1, 2, 43,  72,  68,  'C2',  2,  0,  1,  1,  1);
insert into tcpus values (  2, 2, 44,  58,  58,  'C3',  2,  1,  1,  2,  1);
insert into tcpus values (  3, 2, 45,  67,  63,  'D0',  2,  2,  1,  3,  1);
insert into tcpus values (  4, 2, 46,   9,   6,  'D1',  2,  3,  1,  4,  1);
insert into tcpus values (  5, 2, 47,  75,  71,  'D2',  2,  4,  1,  5,  1);
insert into tcpus values (  6, 2, 48,  71,  67,  'D3',  2,  5,  1,  6,  1);
insert into tcpus values (  7, 2, 49,  65,  61,  'E0',  2,  6,  1,  7,  1);
insert into tcpus values (  8, 2, 50,  54,  50,  'E1',  2,  7,  1,  8,  1);
insert into tcpus values (  9, 2, 51,  61,  57,  'E2',  2,  8,  1,  9,  1);
insert into tcpus values ( 10, 2, 52,  64,  60,  'E3',  2,  9,  1, 10,  1);
insert into tcpus values ( 11, 2, 53,  51,  47,  'F0',  2, 10,  1, 11,  1);
insert into tcpus values ( 12, 2, 54,  50,  46,  'F1',  2, 11,  1, 12,  1);
insert into tcpus values ( 13, 2, 55,  -1,  '',   '',  -1,  0, -1,  0,  0);
insert into tcpus values ( 14, 2, 56,  -1,  '',   '',  -1,  0, -1,  0,  0);
insert into tcpus values ( 15, 2, 57,  68,  64,  'G0',  8,  2,  2,  3,  1);
insert into tcpus values ( 16, 2, 58,  76,  72,  'G1',  8,  3,  2,  4,  1);
insert into tcpus values ( 17, 2, 59,  66,  62,  'G2',  8,  4,  2,  5,  1);
insert into tcpus values ( 18, 2, 60,  46,  42,  'G3',  8,  5,  2,  6,  1);
insert into tcpus values ( 19, 2, 61,  18,  14,  'H0',  8,  6,  2,  7,  1);
insert into tcpus values ( 20, 2, 62,  14,  11,  'H1',  8,  7,  2,  8,  1);

insert into tcpus values ( 21, 1, 33,  32,  28,  'A0',  8,  8,  2,  9,  1);
insert into tcpus values ( 22, 1, 34,  10,   7,  'A1',  8,  9,  2, 10,  1);
insert into tcpus values ( 23, 1, 35,  29,  25,  'A2',  8, 10,  2, 11,  1);
insert into tcpus values ( 24, 1, 36,  37,  33,  'A3',  8, 11,  2, 12,  1);
insert into tcpus values ( 25, 1, 37,  23,  19,  'B0',  9,  0,  3,  1,  1);
insert into tcpus values ( 26, 1, 38,  21,  17,  'B1',  9,  1,  3,  2,  1);
insert into tcpus values ( 27, 1, 39,  41,  37,  'B2',  9,  2,  3,  3,  1);
insert into tcpus values ( 28, 1, 40,  25,  21,  'B3',  9,  3,  3,  4,  1);
insert into tcpus values ( 29, 1, 41,  34,  30,  'C0',  9,  4,  3,  5,  1);
insert into tcpus values ( 30, 1, 42,  39,  35,  'C1',  9,  5,  3,  6,  1);
insert into tcpus values ( 31, 1, 43,  11,   8,  'C2',  9,  6,  3,  7,  1);
insert into tcpus values ( 32, 1, 44,  24,  20,  'C3',  9,  7,  3,  8,  1);
insert into tcpus values ( 33, 1, 45,  35,  31,  'D0',  9,  8,  3,  9,  1);
insert into tcpus values ( 34, 1, 46,  31,  27,  'D1',  9,  9,  3, 10,  1);
insert into tcpus values ( 35, 1, 47,  15,  12,  'D2',  9, 10,  3, 11,  1);
insert into tcpus values ( 36, 1, 48,  12,   9,  'D3',  9, 11,  3, 12,  1);
insert into tcpus values ( 37, 1, 49,  13,  10,  'E0',  0,  0,  4,  1,  1);
insert into tcpus values ( 38, 1, 50,  33,  29,  'E1',  0,  1,  4,  2,  1);
insert into tcpus values ( 39, 1, 51,  38,  34,  'E2',  0,  2,  4,  3,  1);
insert into tcpus values ( 40, 1, 52,  16,  13,  'E3',  0,  3,  4,  4,  1);
insert into tcpus values ( 41, 1, 53,  20,  16,  'F0',  0,  4,  4,  5,  1);
insert into tcpus values ( 42, 1, 54,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 43, 1, 55,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 44, 1, 56,  40,  36,  'F3',  0,  7,  4,  8,  1);
insert into tcpus values ( 45, 1, 57,  43,  39,  'G0',  0,  8,  4,  9,  1);
insert into tcpus values ( 46, 1, 58,  27,  23,  'G1',  0,  9,  4, 10,  1);
insert into tcpus values ( 47, 1, 59,  36,  32,  'G2',  0, 10,  4, 11,  1);
insert into tcpus values ( 48, 1, 60,  19,  15,  'G3',  0, 11,  4, 12,  1);
insert into tcpus values ( 49, 1, 61,  28,  24,  'H0',  1,  0,  5,  1,  1);
insert into tcpus values ( 50, 1, 62,  22,  18,  'H1',  1,  1,  5,  2,  1);

insert into tcpus values ( 51, 2, 33,  63,  59,  'A0',  1,  2,  5,  3,  1);
insert into tcpus values ( 52, 2, 34,  70,  66,  'A1',  1,  3,  5,  4,  1);
insert into tcpus values ( 53, 2, 35,  74,  70,  'A2',  1,  4,  5,  5,  1);
insert into tcpus values ( 54, 2, 36,  42,  38,  'A3',  1,  5,  5,  6,  1);
insert into tcpus values ( 55, 2, 37,  45,  41,  'B0',  1,  6,  5,  7,  1);
insert into tcpus values ( 56, 2, 38,  49,  45,  'B1',  1,  7,  5,  8,  1);
insert into tcpus values ( 57, 2, 39,  56,  52,  'B2',  1,  8,  5,  9,  1);
insert into tcpus values ( 58, 2, 40,  58,  54,  'B3',  1,  9,  5, 10,  1);
insert into tcpus values ( 59, 2, 41,  57,  53,  'C0',  1, 10,  5, 11,  1);
insert into tcpus values ( 60, 2, 42,  59,  55,  'C1',  1, 11,  5, 12,  1);


-- east
insert into tcpus values ( 61, 4, 58,  88,  84,  'G1',  3,  0,  6,  1,  1);
insert into tcpus values ( 62, 4, 59,  79,  75,  'G2',  3,  1,  6,  2,  1);
insert into tcpus values ( 63, 4, 60,  98,  94,  'G3',  3,  2,  6,  3,  1);
insert into tcpus values ( 64, 4, 61,  95,  91,  'H0',  3,  3,  6,  4,  1);
insert into tcpus values ( 65, 4, 62,  99,  95,  'H1',  3,  4,  6,  5,  1);

insert into tcpus values ( 66, 3, 33,  52,  48,  'A0',  3,  5,  6,  6,  1);
insert into tcpus values ( 67, 3, 34,  69,  65,  'A1',  3,  6,  6,  7,  1);
insert into tcpus values ( 68, 3, 35,  26,  22,  'A2',  3,  7,  6,  8,  1);
insert into tcpus values ( 69, 3, 36,  44,  40,  'A3',  3,  8,  6,  9,  1);
insert into tcpus values ( 70, 3, 37,  53,  49,  'B0',  3,  9,  6, 10,  1);
insert into tcpus values ( 71, 3, 38,   8,   5,  'B1',  3, 10,  6, 11,  1);
insert into tcpus values ( 72, 3, 39,   5,   2,  'B2',  3, 11,  6, 12,  1);
insert into tcpus values ( 73, 3, 40,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 74, 3, 41,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 75, 3, 42,   4,   1,  'C1',  4,  2,  7,  3,  1);
insert into tcpus values ( 76, 3, 43,   7,   4,  'C2',  4,  3,  7,  4,  1);
insert into tcpus values ( 77, 3, 44,   6,   3,  'C3',  4,  4,  7,  5,  1);
insert into tcpus values ( 78, 3, 45,  55,  51,  'D0',  4,  5,  7,  6,  1);
insert into tcpus values ( 79, 3, 46,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 80, 3, 47,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 81, 3, 48,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 82, 3, 49,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 83, 3, 50,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 84, 3, 51,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 85, 3, 52,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 86, 3, 53,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 87, 3, 54,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 88, 3, 55,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 89, 3, 56,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 90, 3, 57,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 91, 3, 58,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 92, 3, 59,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 93, 3, 60,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 94, 3, 61,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 95, 3, 62,  -1,  '',    '', -1,  0, -1,  0,  0);

insert into tcpus values ( 96, 4, 33,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values ( 97, 4, 34,  81,  77,  'A1',  6,  0,  9,  1,  1);
insert into tcpus values ( 98, 4, 35,  93,  89,  'A2',  6,  1,  9,  2,  1);
insert into tcpus values ( 99, 4, 36,  91,  87,  'A3',  6,  2,  9,  3,  1);
insert into tcpus values (100, 4, 37,  94,  90,  'B0',  6,  3,  9,  4,  1);
insert into tcpus values (101, 4, 38,  97,  93,  'B1',  6,  4,  9,  5,  1);
insert into tcpus values (102, 4, 39,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values (103, 4, 40,  -1,  '',    '', -1,  0, -1,  0,  0);
insert into tcpus values (104, 4, 41,  77,  73,  'C0',  6,  7,  9,  8,  1);
insert into tcpus values (105, 4, 42,  80,  76,  'C1',  6,  8,  9,  9,  1);
insert into tcpus values (106, 4, 43,  87,  83,  'C2',  6,  9,  9, 10,  1);
insert into tcpus values (107, 4, 44,  73,  69,  'C3',  6, 10,  9, 11,  1);
insert into tcpus values (108, 4, 45,  84,  80,  'D0',  6, 11,  9, 12,  1);

insert into tcpus values (109, 4, 46,  86,  82,  'D1',  7,  0, 10,  1,  1);
insert into tcpus values (110, 4, 47,  78,  74,  'D2',  7,  1, 10,  2,  1);
insert into tcpus values (111, 4, 48,   3, 'C',  'D3',  7,  2, 10,  3,  1);
insert into tcpus values (112, 4, 49,  83,  79,  'E0',  7,  3, 10,  4,  1);
insert into tcpus values (113, 4, 50,  47,  43,  'E1',  7,  4, 10,  5,  1);
insert into tcpus values (114, 4, 51,  90,  86,  'E2',  7,  5, 10,  6,  1);
insert into tcpus values (115, 4, 52,  89,  85,  'E3',  7,  6, 10,  7,  1);
insert into tcpus values (116, 4, 53,   2, 'B',  'F0',  7,  7, 10,  8,  1);
insert into tcpus values (117, 4, 54,  85,  81,  'F1',  7,  8, 10,  9,  1);
insert into tcpus values (118, 4, 55,  96,  92,  'F2',  7,  9, 10, 10,  1);
insert into tcpus values (119, 4, 56, 100,  96,  'F3',  7, 10, 10, 11,  1);
insert into tcpus values (120, 4, 57,  84,  78,  'G0',  7, 11, 10, 12,  1);