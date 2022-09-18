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
