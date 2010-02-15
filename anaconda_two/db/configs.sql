-- $Id$
-- 
-- Kohei Kajimoto
--
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


drop table if exists config_sets;
create table config_sets (
    id                  integer primary key,
    name                char(32) not null,
    description         vchar(128)
);
insert into config_sets values (1, "Default",               "default mode uploaded when frontend starts");
insert into config_sets values (2, "Standby",               "Standby");
insert into config_sets values (3, "Phys Mode",             "Physics mode");
insert into config_sets values (4, "Phys Mode (monitor)",   "Physics Mode (monitor)");

drop table if exists config_types;
create table config_types (
    id          integer primary key,
    name        char(32)
);

-- parameters
insert into config_types values (  1, "THUB_ENABLE"       );
insert into config_types values (  2, "THUB_TEMP_ALARM"   );
insert into config_types values ( 11, "SRDS_ENABLE"       );
insert into config_types values ( 12, "SRDS_PLDREG9XBASE" );

insert into config_types values ( 21, "TCPU_ENABLE"       );
insert into config_types values ( 22, "TCPU_TEMP_ALARM"   );
insert into config_types values ( 23, "TCPU_PLDREG02"     );
insert into config_types values ( 24, "TCPU_PLDREG03"     );
insert into config_types values ( 25, "TCPU_PLDREG0E"     );
insert into config_types values ( 26, "TCPU_EEPROM"       );
insert into config_types values ( 27, "TCPU_MULTGATEPHASE");

insert into config_types values ( 31, "TDIG_ENABLE"       );
insert into config_types values ( 32, "TDIG_TEMP_ALARM"   );
insert into config_types values ( 33, "TDIG_THRESHOLD"    );
insert into config_types values ( 34, "TDIG_EEPROM"       );

insert into config_types values ( 41, "TDC_ENABLE"        );
insert into config_types values ( 42, "TDC_CONFIG_ID"     );
insert into config_types values ( 43, "TDC_CH_MASK"       );

-- action items
insert into config_types values (101, "INIT"  );
insert into config_types values (102, "CONFIG");
insert into config_types values (103, "RESET" );
insert into config_types values (104, "QRESET");
insert into config_types values (105, "SYNC"  );
insert into config_types values (106, "USLEEP");

insert into config_types values (201, "THUB_BUNCH_RESET");
insert into config_types values (202, "THUB_RECOVERY_MSG");
insert into config_types values (211, "TCPU_RESYNC");
insert into config_types values (212, "TCPU_RECOVERY");
insert into config_types values (213, "THUB_RECOVERY");

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

-- Set 1:
---------- ----------------   id  set ord  typ  ad1  ad2  ad3  ad4   value
-- THUB
insert into configs values (null,   1,  1,   2,   1, 255,   0,   0,     50);
insert into configs values (null,   1,  2,  12,   1, 255, 255,   0,      0);
-- TCPU
insert into configs values (null,   1, 11,  22,   2, 255,   0,   0,     45);
insert into configs values (null,   1, 12,  23,   2, 255,   0,   0,      0);
insert into configs values (null,   1, 13,  24,   2, 255,   0,   0,      1);
insert into configs values (null,   1, 14,  25,   2, 255,   0,   0,      0);
insert into configs values (null,   1, 15,  26,   2, 255,   0,   0,      2);
insert into configs values (null,   1, 15,  27,   2, 255,   0,   0,      0); -- 0x0
-- insert into configs values (null,   1, 15,  27,   2, 255,   0,   0,    224); -- 0xe0
-- TDIG
insert into configs values (null,   1, 21,  32,   2, 255, 255,   0,     45);
insert into configs values (null,   1, 22,  33,   2, 255, 255,   0,   1200);
insert into configs values (null,   1, 22,  33,   2, 121, 255,   0,    500);
insert into configs values (null,   1, 22,  33,   2, 122, 255,   0,    500);
insert into configs values (null,   1, 23,  34,   2, 255, 255,   0,      2);
--insert into configs values (null,   1, 24,  34,   2,  58,   2,   0,      1);
--insert into configs values (null,   1, 23,  34,   2, 255, 255,   0,      1);
-- TDC
insert into configs values (null,   1, 31,  42,   2, 255, 255, 255,     13);
insert into configs values (null,   1, 32,  42,   2, 255,   1,   1,     14);
insert into configs values (null,   1, 33,  42,   2, 255,   5,   1,     14);

-- Disable Tray 4, 38, 39, 52, 75, 64
--insert into configs values (null,   1, 81,  21,   2,   4,   0,   0,      0);
--insert into configs values (null,   1, 82,  21,   2,  38,   0,   0,      0);
--insert into configs values (null,   1, 83,  21,   2,  39,   0,   0,      0);
--insert into configs values (null,   1, 84,  21,   2,  52,   0,   0,      0);
--insert into configs values (null,   1, 85,  21,   2,  75,   0,   0,      0);
--insert into configs values (null,   1, 86,  21,   2,  64,   0,   0,      0);

-- Disable TDIG 4, 7, 8 for upVPD
insert into configs values (null,   1, 91,  31,   2, 121,   4,   0,      0);
insert into configs values (null,   1, 92,  31,   2, 121,   7,   0,      0);
insert into configs values (null,   1, 93,  31,   2, 121,   8,   0,      0);
insert into configs values (null,   1, 94,  31,   2, 122,   4,   0,      0);
insert into configs values (null,   1, 95,  31,   2, 122,   7,   0,      0);
insert into configs values (null,   1, 96,  31,   2, 122,   8,   0,      0);


-- Set 2
---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
-- 1. Put TCPUs in stanby mode
insert into configs values (null,   2, 42,  23,   2, 255,   0,   0,      0);
insert into configs values (null,   2, 43,  24,   2, 255,   0,   0,      1);
insert into configs values (null,   2, 44, 102,   2, 255,   0,   0,      2);

-- 2. Turn off all of the Serdes
insert into configs values (null,   2, 51,  12,   1, 255, 255,   0,      0);
insert into configs values (null,   2, 52, 102,   1, 255, 255,   0,      1);

-- 3. Sync All
insert into configs values (null,   2, 61, 105,   1, 255,   0,   0,      3);
insert into configs values (null,   2, 62, 105,   2, 255,   0,   0,      3);


-- Set 3
---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
-- 1. Load TCPU FPGA from Eeprom 2, also include TDIG FPGAs (level = 2
insert into configs values (null,   3, 11, 101,   2, 255,   0,   0,      2);
--insert into configs values (null,   3, 11, 101,   2, 255,   0,   0,      1);

-- 2. Turn off all of the Serdes
insert into configs values (null,   3, 21,  12,   1, 255, 255,   0,      0);
insert into configs values (null,   3, 22, 102,   1, 255, 255,   0,      1);

-- 3. Reset all TDCs on TDIG
--insert into configs values (null,   3, 31, 103,   2, 255, 255,   0,      2);

-- 4. Set TDIG threshold to 1200mV and 500mV for upVPD, and then put in run mode
insert into configs values (null,   3, 41,  33,   2, 255, 255,   0,   1200);
insert into configs values (null,   3, 41,  33,   2, 121, 255,   0,    500);
insert into configs values (null,   3, 41,  33,   2, 122, 255,   0,    500);
insert into configs values (null,   3, 42,  23,   2, 255,   0,   0,     15);
insert into configs values (null,   3, 43,  24,   2, 255,   0,   0,      2);
insert into configs values (null,   3, 44, 102,   2, 255,   0,   0,      2);

-- 5. Turn on THUB serdes channels
insert into configs values (null,   3, 51,  12,   1, 255, 255,   0,     16);
--insert into configs values (null,   3, 52,  12,   1,   2,   5,   0,      0);
--insert into configs values (null,   3, 53,  12,   1,   2,   6,   0,      0);
--insert into configs values (null,   3, 54,  12,   1,   2,   7,   0,      0);
insert into configs values (null,   3, 55, 102,   1, 255, 255,   0,      1);

-- 6. TCPU NW 0x3a (58) seems to not always sync, toggle it
--insert into configs values (null,   3, 61, 211,   2,  46,   0,   0,      1);

-- 7. sleep a little (700ms)
insert into configs values (null,   3, 70, 106,   0,   0,   0,   0, 700000);

-- 8. Bunch Reset
insert into configs values (null,   3, 71, 201,   1,   1,   0,   0,      1);

-- 9. Turn on (val = 0) Recovery Alert Messages from THUB 1
insert into configs values (null,   3, 72, 202,   1,   1,   0,   0,      0);

-- 10. Sync All
insert into configs values (null,   3, 81, 105,   1, 255,   0,   0,      3);
insert into configs values (null,   3, 82, 105,   2, 255,   0,   0,      2);

-------------------------------------------------------------------------------
-- Set 4 
---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
-- 1. Load TCPU FPGA from Eeprom 2
-- insert into configs values (null,   4, 11, 101,   2, 255,   0,   0,      1);

-- 2. Turn off all of the Serdes
insert into configs values (null,   4, 21,  12,   1, 255, 255,   0,      0);
-- insert into configs values (null,   4, 22, 102,   1, 255, 255,   0,      1);

-- 3. Reset all TDCs on TDIG
-- insert into configs values (null,   4, 31, 103,   2, 255, 255,   0,      2);

-- 4. Set TDIG threshold to 1200mV and 500mV for upVPD, and then put in run mode
insert into configs values (null,   4, 41,  33,   2, 255, 255,   0,   1200);
insert into configs values (null,   4, 41,  33,   2, 121, 255,   0,    500);
insert into configs values (null,   4, 41,  33,   2, 122, 255,   0,    500);
insert into configs values (null,   4, 42,  23,   2, 255,   0,   0,     15);
insert into configs values (null,   4, 43,  24,   2, 255,   0,   0,      2);
-- insert into configs values (null,   4, 44, 102,   2, 255,   0,   0,      2);

-- 5. Turn on THUB serdes channels
insert into configs values (null,   4, 51,  12,   1, 255, 255,   0,     16);
--            turn off Serdes 5,6,7 of THUB NE (2)
-- insert into configs values (null,   4, 52,  12,   1,   2,   5,   0,      0);
-- insert into configs values (null,   4, 53,  12,   1,   2,   6,   0,      0);
-- insert into configs values (null,   4, 54,  12,   1,   2,   7,   0,      0);
-- insert into configs values (null,   4, 55, 102,   1, 255, 255,   0,      1);
-- insert into configs values (null,   4, 52, 102,   1, 255, 255,   0,      1);

-- 6. TCPU NW 0x3a (58) seems to not always sync, toggle it
-- insert into configs values (null,   3, 62, 211,   2, 255,   0,   0,      1);
-- insert into configs values (null,   3, 64, 211,   2, 255,   0,   0,      1);

-- 7. Bunch Reset
-- insert into configs values (null,   4, 71, 201,   1,   1,   0,   0,      1);

-- 8. Sync All
insert into configs values (null,   4, 81, 105,   1, 255,   0,   0,      3);
insert into configs values (null,   4, 82, 105,   2, 255,   0,   0,      2);

-- ---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
-- insert into configs values (null,   4,  1,   1,   1, 255,   0,   0,      0);
-- insert into configs values (null,   4,  2,  12,   1, 255, 255,   0,     16);
-- insert into configs values (null,   4,  3,  23,   2, 255,   0,   0,     15);
-- insert into configs values (null,   4,  4,  33,   2, 255, 255,   0,   1500);
-- insert into configs values (null,   4,  5,  42,   2, 255, 255, 255,     13);
-- insert into configs values (null,   4,  6,  42,   2, 255,   1,   1,     14);
-- insert into configs values (null,   4,  7,  42,   2, 255,   5,   1,     14);
-- insert into configs values (null,   4,  8,  43,   2, 255, 255,   1,      7);
-- insert into configs values (null,   4,  9, 103,   2, 255,   0,   0,      3);

---- User Command 1 -------   id  set  ord, typ ad1  ad2  ad3  ad4   value
insert into configs values (null, 101,  1, 201,   1,   1,   0,   0,      1);
-- insert into configs values (null, 101,  1,  101,  1, 255,   0,   0,     2);
-- insert into configs values (null, 101,  2,  101,  2, 255,   0,   0,     4);

---- User Command 2 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 102,  1,  104,  2, 255,   0,   0,     2);

---- User Command 3 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 103,  1,  105,   1, 255,   0,   0,      1);
insert into configs values (null, 103,  2,  213,   1, 255,   0,   0,      2);
insert into configs values (null, 103,  3,  105,   2, 255,   0,   0,      2);
insert into configs values (null, 103,  4,  212,   2, 255,   0,   0,      2);
insert into configs values (null, 103,  5,  106,   0,   0,   0,   0, 700000);
insert into configs values (null, 103,  6,  201,   1,   1,   0,   0,      2);

---- User Command 4 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 104,  1,  212,   2, 255,   0,   0,      1);
insert into configs values (null, 104,  2,  201,   1,   1,   0,   0,      1);
