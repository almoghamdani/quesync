#!/bin/bash

# Generate database dump
mysqldump --replace --no-data --skip-comments --skip-add-drop-table --routines --databases quesync -u server -p > database-dump.sql

# Replace CREATE TABLE with CREATE TABLE IF NOT EXISTS
sed -i '' 's/CREATE TABLE/CREATE TABLE IF NOT EXISTS/g' database-dump.sql

# Replace CREATE VIEW with CREATE OR REPLACE VIEW
sed -i '' 's/CREATE VIEW/CREATE OR REPLACE VIEW/g' database-dump.sql

# Add DROP PROCEDURE
sed -i '' 's/CREATE DEFINER=`server`@`localhost` PROCEDURE `get_private_channel`/DROP PROCEDURE IF EXISTS `get_private_channel`;\'$'\nCREATE DEFINER=`server`@`localhost` PROCEDURE `get_private_channel`/g' database-dump.sql