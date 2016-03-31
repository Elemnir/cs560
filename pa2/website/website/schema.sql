DROP TABLE IF EXISTS Term;
CREATE TABLE Term (
    id integer primary key autoincrement,
    word varchar(255) not null
);

DROP TABLE IF EXISTS Pub;
CREATE TABLE Pub (
    id integer primary key autoincrement,
    name text not null
);

DROP TABLE IF EXISTS Loc;
CREATE TABLE Entry (
    id integer primary key autoincrement,
    term integer not null,
    pub integer not null,
    line integer not null,
    pos integer not null
);
