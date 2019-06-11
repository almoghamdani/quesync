CREATE DATABASE  IF NOT EXISTS `quesync` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci */;
USE `quesync`;
-- MySQL dump 10.13  Distrib 8.0.15, for macos10.14 (x86_64)
--
-- Host: localhost    Database: quesync
-- ------------------------------------------------------
-- Server version	8.0.15

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
 SET NAMES utf8 ;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `channel_participants`
--

DROP TABLE IF EXISTS `channel_participants`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `channel_participants` (
  `channel_id` varchar(36) NOT NULL,
  `user_id` varchar(36) NOT NULL,
  PRIMARY KEY (`channel_id`,`user_id`),
  KEY `fk_user_id_idx` (`user_id`),
  CONSTRAINT `fk_channel_id` FOREIGN KEY (`channel_id`) REFERENCES `channels` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `fk_user_id` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `channel_participants`
--

LOCK TABLES `channel_participants` WRITE;
/*!40000 ALTER TABLE `channel_participants` DISABLE KEYS */;
INSERT INTO `channel_participants` VALUES ('98ca2d33-7b83-487f-909c-478020a592e0','0739e80c-b634-4f9e-a54f-5ee75d441a96'),('98ca2d33-7b83-487f-909c-478020a592e0','20734f93-c8d3-48a8-a6a6-e554a42395d8'),('9a1dfb07-f2e6-4b70-852b-12f415b47537','20734f93-c8d3-48a8-a6a6-e554a42395d8'),('98ca2d33-7b83-487f-909c-478020a592e0','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('9a1dfb07-f2e6-4b70-852b-12f415b47537','32b0358b-a54d-48f1-b7a6-2c48d9470d6a');
/*!40000 ALTER TABLE `channel_participants` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `channels`
--

DROP TABLE IF EXISTS `channels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `channels` (
  `id` varchar(36) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `channels`
--

LOCK TABLES `channels` WRITE;
/*!40000 ALTER TABLE `channels` DISABLE KEYS */;
INSERT INTO `channels` VALUES ('98ca2d33-7b83-487f-909c-478020a592e0','2019-06-10 17:08:36'),('9a1dfb07-f2e6-4b70-852b-12f415b47537','2019-06-10 17:08:36'),('aa557985-25e0-42a7-9b33-4258a610283f','2019-06-10 17:09:07');
/*!40000 ALTER TABLE `channels` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `friendships`
--

DROP TABLE IF EXISTS `friendships`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `friendships` (
  `requester_id` varchar(36) NOT NULL,
  `recipient_id` varchar(36) NOT NULL,
  `approved` tinyint(4) NOT NULL DEFAULT '0',
  `sent_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`requester_id`,`recipient_id`),
  KEY `fk_receipt_id_idx` (`recipient_id`),
  CONSTRAINT `fk_recipient_id` FOREIGN KEY (`recipient_id`) REFERENCES `users` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk_requester_id` FOREIGN KEY (`requester_id`) REFERENCES `users` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `friendships`
--

LOCK TABLES `friendships` WRITE;
/*!40000 ALTER TABLE `friendships` DISABLE KEYS */;
INSERT INTO `friendships` VALUES ('32b0358b-a54d-48f1-b7a6-2c48d9470d6a','5464a7ba-4417-4a21-895d-0902bf4f9343',0,'2019-06-08 13:07:59');
/*!40000 ALTER TABLE `friendships` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Temporary view structure for view `profiles`
--

DROP TABLE IF EXISTS `profiles`;
/*!50001 DROP VIEW IF EXISTS `profiles`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8mb4;
/*!50001 CREATE VIEW `profiles` AS SELECT 
 1 AS `id`,
 1 AS `nickname`,
 1 AS `tag`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `users` (
  `id` varchar(36) NOT NULL,
  `username` varchar(50) NOT NULL,
  `password` varchar(64) NOT NULL,
  `email` varchar(254) NOT NULL,
  `nickname` varchar(50) NOT NULL,
  `tag` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username_UNIQUE` (`username`),
  UNIQUE KEY `email_UNIQUE` (`email`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES ('0739e80c-b634-4f9e-a54f-5ee75d441a96','almogh43','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','P5DH15NS8N@gmail.com','sdsd',8467),('20734f93-c8d3-48a8-a6a6-e554a42395d8','almogh37','aa2df71c706f0eb10a78087be19cab6eaa1becc6a34808730f76a3ece884d247','almogh212@gmail.com','Atomiqu',4049),('32b0358b-a54d-48f1-b7a6-2c48d9470d6a','almogh23','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','QCIRZP5WEU@gmail.com','sdsd',6849),('530e9407-7cfd-4be4-9cc6-70ab38e6310c','W0QU2RDWLM','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','W0QU2RDWLM@gmail.com','sdsd',2055),('5464a7ba-4417-4a21-895d-0902bf4f9343','almogh52','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','almogh52@gmail.com','almogh',1755),('68dcca8e-2704-46e9-9f30-b7b5b0a77e96','XUJS97B3HA','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','XUJS97B3HA@gmail.com','sdsd',1055),('79bbcce2-01f4-4e12-b88d-c3f985d960f5','almogh13','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','Z6ALDZRWYY@gmail.com','sdsd',7784),('7d612d87-e2e9-4734-b6b2-b410b47d7995','almogh51','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','90NS0RW6TF@gmail.com','sdsd',9591),('aa0722a9-f7ef-400e-94f5-abeea3748e7f','TD2BO1FPA1','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','TD2BO1FPA1@gmail.com','sdsd',1125),('ed6f6d9b-537c-43c1-96ae-84110fe5a9d5','KFT7NM25LQ','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','KFT7NM25LQ@gmail.com','sdsd',6534),('f096a34d-a572-4dfe-b66e-7fad1274631e','almogh53','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','Q8RQ7TE3ED@gmail.com','sdsd',1841);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping events for database 'quesync'
--

--
-- Dumping routines for database 'quesync'
--

--
-- Final view structure for view `profiles`
--

/*!50001 DROP VIEW IF EXISTS `profiles`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8mb4_0900_ai_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `profiles` AS select `users`.`id` AS `id`,`users`.`nickname` AS `nickname`,`users`.`tag` AS `tag` from `users` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-06-12  0:40:30
