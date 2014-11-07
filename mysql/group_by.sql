create database test;
use test;
create table user (
    id bigint(11) auto_increment,
    name varchar(16) default '',
    sex tinyint(3) default 0,  
    city varchar(16) default '',
    primary key (id)
);
insert into user set name = 'a', sex = 1, city='beijing';
insert into user set name = 'b', sex = 0, city='beijing';
insert into user set name = 'c', sex = 1, city='beijing';
insert into user set name = 'd', sex = 0, city='beijing';
insert into user set name = 'e', sex = 1, city='beijing';
insert into user set name = 'f', sex = 0, city='beijing';
insert into user set name = 'g', sex = 1, city='beijing';
insert into user set name = 'h', sex = 0, city='beijing';
insert into user set name = 'a', sex = 1, city='shanghai';
insert into user set name = 'b', sex = 0, city='shanghai';
insert into user set name = 'c', sex = 1, city='shanghai';
insert into user set name = 'd', sex = 0, city='shanghai';
insert into user set name = 'e', sex = 1, city='shanghai';
insert into user set name = 'f', sex = 0, city='shanghai';
insert into user set name = 'g', sex = 1, city='shanghai';
insert into user set name = 'h', sex = 0, city='shanghai';

select id, name, sex, city from user order by name group by sex, city;
