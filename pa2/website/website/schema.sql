DROP TABLE IF EXISTS Term;
CREATE TABLE Term (
    id integer primary key autoincrement,
    word varchar(255) not null
);

DROP TABLE IF EXISTS Loc;
CREATE TABLE Loc (
    id integer primary key autoincrement,
    line integer not null,
    pos integer not null
);
