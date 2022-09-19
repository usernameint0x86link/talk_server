create database if not exists chat;

use chat;

create table if not exists user(
	`id` int AUTO_INCREMENT,
	`name` varchar(50) not null,
	`password` varchar(50) not null,
	`state` enum('online', 'offline') default 'offline',
	PRIMARY KEY(`id`),
	UNIQUE KEY(`name`)
) default charset=utf8;

create table if not exists friend(
	`userid` int NOT NULL,
	`friendid` int NOT NULL,
	PRIMARY KEY(`userid`, `friendid`)
) default charset=utf8;

create table if not exists allgroup(
	`id` int AUTO_INCREMENT,
	`groupname` varchar(50) NOT NULL,
	`groupdesc` varchar(200) default '',
	PRIMARY KEY(`id`),
	UNIQUE KEY(`groupname`)
) default charset=utf8;

create table if not exists groupuser(
	`groupid` int NOT NULL,
	`userid` int NOT NULL,
	`grouprole` enum('creator', 'normal') default 'normal',
	PRIMARY KEY(`groupid`, `userid`)
) default charset=utf8;


create table if not exists offlinemessage(
	`userid` int NOT NULL,
	`message` varchar(500) NOT NULL,
	PRIMARY KEY(`userid`)
) default charset=utf8;
