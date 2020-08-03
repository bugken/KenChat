CREATE TABLE User(
	id INTEGER PRIMARY KEY NOT NULL AUTO_INCREMENT,
	name VARCHAR(50) NOT NULL UNIQUE,
	password VARCHAR(50),
	state ENUM('online', 'offline') DEFAULT 'offline'
);

CREATE TABLE Friend(
	userid INTEGER NOT NULL,
    friendid INTEGER NOT NULL
);

CREATE TABLE AllGroup(
	groupname VARCHAR(50) PRIMARY KEY NOT NULL,
	groupdesc VARCHAR(200) NOT NULL DEFAULT ''
);

CREATE TABLE GroupUser(
	groupid INTEGER NOT NULL,
	userid INTEGER NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal'
);

CREATE TABLE OfflineMessage(
    userid INTEGER NOT NULL,
    message VARCHAR(500) NOT NULL
);



