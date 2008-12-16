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

insert into thubs values (  1,   1, 64,   1,   1,   1);
insert into thubs values (  2,   2, 64,   2,   2,   1);
insert into thubs values (  3,   3, 64,   3,   3,   1);
insert into thubs values (  4,   4, 64,   4,   4,   1);
