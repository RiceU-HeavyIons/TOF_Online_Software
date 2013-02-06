-- $Id$

-- TCPUs
drop table if exists tcpus;
create table tcpus (
    id                  integer primary key autoincrement,
    device_id           integer not null,
    canbus_id           integer not null,
    tray_id             integer not null,
    tray_sn             char(3) not null,
    thub_id             integer not null,
    serdes              char(2) not null,
    lv_box              integer not null,
    lv_ch               integer not null,
    hv_box              integer not null,
    hv_ch               integer not null,
    installed           boolean  not null default 0
);

begin transaction;
-- North
insert into tcpus values (  1, 2, 33, 147, 216, 2, 'A0',  0,  6,  1,  1,  1);
insert into tcpus values (  2, 2, 34, 154, 219, 2, 'A1',  0,  7,  1,  1,  1);
insert into tcpus values (  3, 2, 35, 185, 254, 2, 'A2',  0,  8,  1,  1,  1);
insert into tcpus values (  4, 2, 36, 192, 268, 2, 'A3',  0,  9,  1,  1,  1);
insert into tcpus values (  5, 2, 37, 188, 262, 2, 'B0',  0, 10,  1,  1,  1);
insert into tcpus values (  6, 2, 38, 190, 260, 2, 'B1',  0, 11,  1,  1,  1);
insert into tcpus values (  7, 2, 39, 174, 245, 2, 'B2',  1,  0,  1,  1,  1);
insert into tcpus values (  8, 2, 40,   0,   0, 2, 'B3', -1,  0,  1,  1,  0);
insert into tcpus values (  9, 2, 41,   0,   0, 2, 'C0', -1,  0,  1,  1,  0);
insert into tcpus values ( 10, 2, 42, 196, 264, 2, 'C1',  1,  1,  1,  1,  0);
insert into tcpus values ( 11, 2, 43,   0,   0, 2, 'C2', -1,  0,  1,  1,  0);
insert into tcpus values ( 12, 2, 44,   0,   0, 2, 'C3', -1,  0,  1,  1,  0);
insert into tcpus values ( 13, 2, 45,   0,   0, 2, 'D0', -1,  0,  1,  1,  0);
insert into tcpus values ( 14, 2, 46,   0,   0, 2, 'D1', -1,  0,  1,  1,  0);
insert into tcpus values ( 15, 2, 47,   0,   0, 2, 'D2', -1,  0,  1,  1,  0);

-- South
insert into tcpus values ( 16, 1, 48,   0,   0, 1, 'A0', -1,  0,  1,  1,  0);
insert into tcpus values ( 17, 1, 49,   0,   0, 1, 'A1', -1,  0,  1,  1,  0);
insert into tcpus values ( 18, 1, 50,   0,   0, 1, 'A2', -1,  0,  1,  1,  0);
insert into tcpus values ( 19, 1, 51,   0,   0, 1, 'A3', -1,  0,  1,  1,  0);
insert into tcpus values ( 20, 1, 52,   0,   0, 1, 'B0', -1,  0,  1,  1,  0);
insert into tcpus values ( 21, 1, 53,   0,   0, 1, 'B1', -1,  0,  1,  1,  0);
insert into tcpus values ( 22, 1, 54, 181, 250, 1, 'B2',  1,  2,  1,  1,  1);
insert into tcpus values ( 23, 1, 55,   0,   0, 1, 'B3', -1,  0,  1,  1,  0);
insert into tcpus values ( 24, 1, 56,   0,   0, 1, 'C0', -1,  0,  1,  1,  0);
insert into tcpus values ( 25, 1, 57, 156, 218, 1, 'C1',  0,  0,  1,  1,  1);
insert into tcpus values ( 26, 1, 58, 140, 211, 1, 'C2',  0,  1,  1,  1,  1);
insert into tcpus values ( 27, 1, 59, 135, 206, 1, 'C3',  0,  2,  1,  1,  1);
insert into tcpus values ( 28, 1, 60, 131, 202, 1, 'D0',  0,  3,  1,  1,  1);
insert into tcpus values ( 29, 1, 61, 146, 215, 1, 'D1',  0,  4,  1,  1,  1);
insert into tcpus values ( 30, 1, 62, 148, 217, 1, 'D2',  0,  5,  1,  1,  1);

commit transaction;
