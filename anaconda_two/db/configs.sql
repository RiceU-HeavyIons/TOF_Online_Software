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
insert into config_sets values (1, "Standby",     "default mode uploaded when frontend starts");
insert into config_sets values (2, "Phys Mode",   "Physics mode");
insert into config_sets values (3, "Test Mode",   "Test mode");

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
insert into config_types values ( 24, "TCPU_PLDREG03"    );
insert into config_types values ( 25, "TCPU_PLDREG0E"    );
insert into config_types values ( 31, "TDIG_ENABLE"      );
insert into config_types values ( 32, "TDIG_TEMP_ALARM"  );
insert into config_types values ( 33, "TDIG_THRESHOLD"   );
insert into config_types values ( 41, "TDC_ENABLE"       );
insert into config_types values ( 42, "TDC_CONFIG_ID"    );
insert into config_types values ( 43, "TDC_CH_MASK"      );

-- action items
insert into config_types values (101, "INIT"  );
insert into config_types values (102, "CONFIG");
insert into config_types values (103, "RESET" );
insert into config_types values (104, "SYNC"  );

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
insert into configs values (null,   1,  1,   2,   1, 255,   0,   0,     45);
insert into configs values (null,   1,  2,  12,   1, 255, 255,   0,      0);
insert into configs values (null,   1,  3,  22,   2, 255,   0,   0,     45);
insert into configs values (null,   1,  4,  23,   2, 255,   0,   0,      0);
insert into configs values (null,   1,  5,  24,   2, 255,   0,   0,      0);
insert into configs values (null,   1,  6,  25,   2, 255,   0,   0,      0);
insert into configs values (null,   1,  7,  32,   2, 255, 255,   0,     45);
insert into configs values (null,   1,  8,  33,   2, 255, 255,   0,   2500);
insert into configs values (null,   1,  9,  42,   2, 255, 255, 255,     13);
insert into configs values (null,   1, 10,  42,   2, 255,   1,   1,     14);
insert into configs values (null,   1, 11,  42,   2, 255,   5,   1,     14);

-- Disable Tray 38, 39, 52, 75
insert into configs values (null,   1, 81,  21,   2,  38,   0,   0,      0);
insert into configs values (null,   1, 82,  21,   2,  38,   0,   0,      0);
insert into configs values (null,   1, 83,  21,   2,  38,   0,   0,      0);
insert into configs values (null,   1, 84,  21,   2,  38,   0,   0,      0);

-- Disable TDIG 4, 7, 8 for upVPD
insert into configs values (null,   1, 91,  31,   2, 121,   4,   0,      0);
insert into configs values (null,   1, 92,  31,   2, 121,   7,   0,      0);
insert into configs values (null,   1, 93,  31,   2, 121,   8,   0,      0);
insert into configs values (null,   1, 94,  31,   2, 122,   4,   0,      0);
insert into configs values (null,   1, 95,  31,   2, 122,   7,   0,      0);
insert into configs values (null,   1, 96,  31,   2, 122,   8,   0,      0);

---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
-- 1. Load TCPU FPGA from Eeprom 2
insert into configs values (null,   2, 11, 101,   2, 255,   0,   0,      1);

-- 2. Turn off all of the Serdes
insert into configs values (null,   2, 21,  12,   1, 255, 255,   0,      0);
insert into configs values (null,   2, 22, 102,   1, 255, 255,   0,      1);

-- 3. Reset all TDCs on TDIG
insert into configs values (null,   2, 31, 103,   2, 255, 255,   0,      2);

-- 4. Set TDIG threshold to 1500mV and put in run mode
insert into configs values (null,   2, 41,  33,   2, 255, 255,   0,   1500);
insert into configs values (null,   2, 42,  23,   2, 255,   0,   0,     15);
insert into configs values (null,   2, 43, 102,   2, 255,   0,   0,      2);

-- 5. Trun on THUB serdes channels
insert into configs values (null,   2, 51,  12,   1, 255, 255,   0,     16);
insert into configs values (null,   2, 52, 102,   1, 255, 255,   0,      1);

-- 6. TCPU NW 0x3a (58) seems to not always sync, toggle it
insert into configs values (null,   2, 61,  23,   2,  46,   0,   0,      0);
insert into configs values (null,   2, 62, 102,   2, 255,   0,   0,      1);
insert into configs values (null,   2, 63,  23,   2,  46,   0,   0,     15);
insert into configs values (null,   2, 64, 102,   2, 255,   0,   0,      1);



---------------------------   id  set  ord, typ ad1  ad2  ad3  ad4   value
insert into configs values (null,   3,  1,   1,   1, 255,   0,   0,      0);
insert into configs values (null,   3,  2,  12,   1, 255, 255,   0,     16);
insert into configs values (null,   3,  3,  23,   2, 255,   0,   0,     15);
insert into configs values (null,   3,  4,  33,   2, 255, 255,   0,   1500);
insert into configs values (null,   3,  5,  42,   2, 255, 255, 255,     13);
insert into configs values (null,   3,  6,  42,   2, 255,   1,   1,     14);
insert into configs values (null,   3,  7,  42,   2, 255,   5,   1,     14);
insert into configs values (null,   3,  8,  43,   2, 255, 255,   1,      7);
insert into configs values (null,   3,  9, 103,   2, 255,   0,   0,      3);

---- User Command 1 -------   id  set  ord, typ ad1  ad2  ad3  ad4   value
insert into configs values (null, 101,  1,  101,  1, 255,   0,   0,     2);
insert into configs values (null, 101,  2,  101,  2, 255,   0,   0,     4);

---- User Command 2 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 102,  1,  102,  2, 255,   0,   0,     4);

---- User Command 3 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 103,  1,  103,  1, 255,   0,   0,     2);
insert into configs values (null, 103,  2,  103,  2, 255,   0,   0,     4);

---- User Command 4 -------   id  set  ord, typ ad1  ad2  ad3  ad4  value
insert into configs values (null, 104,  1,  104,  1, 255,   0,   0,     2);
insert into configs values (null, 104,  2,  104,  2, 255,   0,   0,     4);
