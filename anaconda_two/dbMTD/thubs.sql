-- THUBs
drop   table if exists thubs;
create table thubs (
    id                  integer primary key,
    device_id           integer not null,
    canbus_id           integer not null,
    thub_id             integer not null,
    thub_sn             integer not null,
    installed           boolean  not null default 0
);

begin transaction;
insert into thubs values (  1,   1, 64,   5,   5,   1);
insert into thubs values (  2,   2, 64,   6,   6,   1);
commit transaction;
