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
-- Table structure for table `channel_members`
--

DROP TABLE IF EXISTS `channel_members`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `channel_members` (
  `channel_id` varchar(36) NOT NULL,
  `member_id` varchar(36) NOT NULL,
  PRIMARY KEY (`channel_id`,`member_id`),
  KEY `fk_user_id_idx` (`member_id`),
  CONSTRAINT `fk_channel_id` FOREIGN KEY (`channel_id`) REFERENCES `channels` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `fk_member_id` FOREIGN KEY (`member_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `channel_members`
--

LOCK TABLES `channel_members` WRITE;
/*!40000 ALTER TABLE `channel_members` DISABLE KEYS */;
INSERT INTO `channel_members` VALUES ('21472105-1e65-466f-b16c-2450eb18e1b4','0739e80c-b634-4f9e-a54f-5ee75d441a96'),('a7b13885-51f2-44ab-890d-f3263e4d9fa1','0739e80c-b634-4f9e-a54f-5ee75d441a96'),('4990dba2-0b8b-4ff0-83f5-eb5b22711c4c','20734f93-c8d3-48a8-a6a6-e554a42395d8'),('49dc2dce-509d-46af-a563-05bef5ddcc67','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('7d834d31-7f90-4519-a672-0f670622a810','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('9ddeb7ed-7f61-4352-9f38-908133626be3','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('48f66837-376e-4720-9c33-46fde29f1376','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('7d834d31-7f90-4519-a672-0f670622a810','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('0cd67371-c1de-4a9e-bb6e-431066ecfc19','5464a7ba-4417-4a21-895d-0902bf4f9343'),('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','5464a7ba-4417-4a21-895d-0902bf4f9343'),('4990dba2-0b8b-4ff0-83f5-eb5b22711c4c','5464a7ba-4417-4a21-895d-0902bf4f9343'),('8c5a7e79-b0e7-4b5b-a69e-0f8271e81413','5464a7ba-4417-4a21-895d-0902bf4f9343'),('94710058-8c4f-47f6-8c05-49b0d6942329','5464a7ba-4417-4a21-895d-0902bf4f9343'),('a6f2bba9-f0ac-4933-b420-f2af420573da','5464a7ba-4417-4a21-895d-0902bf4f9343'),('a7b13885-51f2-44ab-890d-f3263e4d9fa1','5464a7ba-4417-4a21-895d-0902bf4f9343'),('b234a52e-31ab-4c6e-b5ac-8ecd1cb870bf','5464a7ba-4417-4a21-895d-0902bf4f9343'),('ca297f48-7e30-4773-834e-5f6c4b12c6bd','5464a7ba-4417-4a21-895d-0902bf4f9343'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f','5464a7ba-4417-4a21-895d-0902bf4f9343'),('f1e5304d-cadf-4858-bb80-5971e2f0104e','5464a7ba-4417-4a21-895d-0902bf4f9343'),('0cd67371-c1de-4a9e-bb6e-431066ecfc19','68dcca8e-2704-46e9-9f30-b7b5b0a77e96'),('49dc2dce-509d-46af-a563-05bef5ddcc67','79bbcce2-01f4-4e12-b88d-c3f985d960f5'),('b234a52e-31ab-4c6e-b5ac-8ecd1cb870bf','79bbcce2-01f4-4e12-b88d-c3f985d960f5'),('21472105-1e65-466f-b16c-2450eb18e1b4','7d612d87-e2e9-4734-b6b2-b410b47d7995'),('9ddeb7ed-7f61-4352-9f38-908133626be3','7d612d87-e2e9-4734-b6b2-b410b47d7995'),('f1e5304d-cadf-4858-bb80-5971e2f0104e','7d612d87-e2e9-4734-b6b2-b410b47d7995'),('8c5a7e79-b0e7-4b5b-a69e-0f8271e81413','aa0722a9-f7ef-400e-94f5-abeea3748e7f'),('94710058-8c4f-47f6-8c05-49b0d6942329','ed6f6d9b-537c-43c1-96ae-84110fe5a9d5'),('a6f2bba9-f0ac-4933-b420-f2af420573da','f096a34d-a572-4dfe-b66e-7fad1274631e');
/*!40000 ALTER TABLE `channel_members` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `channels`
--

DROP TABLE IF EXISTS `channels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `channels` (
  `id` varchar(36) NOT NULL,
  `is_private` tinyint(4) NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `channels`
--

LOCK TABLES `channels` WRITE;
/*!40000 ALTER TABLE `channels` DISABLE KEYS */;
INSERT INTO `channels` VALUES ('0cd67371-c1de-4a9e-bb6e-431066ecfc19',1,'2019-06-25 10:36:47'),('21472105-1e65-466f-b16c-2450eb18e1b4',1,'2019-06-27 11:56:04'),('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc',1,'2019-06-14 14:58:33'),('48f66837-376e-4720-9c33-46fde29f1376',1,'2019-06-15 00:01:35'),('4990dba2-0b8b-4ff0-83f5-eb5b22711c4c',1,'2019-06-25 01:59:54'),('49dc2dce-509d-46af-a563-05bef5ddcc67',1,'2019-06-14 13:12:45'),('7d834d31-7f90-4519-a672-0f670622a810',1,'2019-06-14 14:58:48'),('8c5a7e79-b0e7-4b5b-a69e-0f8271e81413',1,'2019-06-25 10:41:27'),('94710058-8c4f-47f6-8c05-49b0d6942329',1,'2019-06-25 01:51:37'),('9ddeb7ed-7f61-4352-9f38-908133626be3',1,'2019-06-27 11:52:29'),('a6f2bba9-f0ac-4933-b420-f2af420573da',1,'2019-06-25 10:41:27'),('a7b13885-51f2-44ab-890d-f3263e4d9fa1',1,'2019-06-25 01:48:52'),('b234a52e-31ab-4c6e-b5ac-8ecd1cb870bf',1,'2019-06-25 10:39:59'),('ca297f48-7e30-4773-834e-5f6c4b12c6bd',1,'2019-06-14 14:58:17'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f',1,'2019-06-14 14:58:03'),('f1e5304d-cadf-4858-bb80-5971e2f0104e',1,'2019-06-25 01:51:24');
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
INSERT INTO `friendships` VALUES ('5464a7ba-4417-4a21-895d-0902bf4f9343','0739e80c-b634-4f9e-a54f-5ee75d441a96',0,'2019-06-27 10:00:24'),('5464a7ba-4417-4a21-895d-0902bf4f9343','32b0358b-a54d-48f1-b7a6-2c48d9470d6a',1,'2019-06-25 10:47:02');
/*!40000 ALTER TABLE `friendships` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `messages`
--

DROP TABLE IF EXISTS `messages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `messages` (
  `id` varchar(36) NOT NULL,
  `sender_id` varchar(36) NOT NULL,
  `channel_id` varchar(36) NOT NULL,
  `content` text NOT NULL,
  `sent_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `sender_id_fk_idx` (`sender_id`),
  KEY `channel_id_fk_idx` (`channel_id`),
  CONSTRAINT `channel_id_fk` FOREIGN KEY (`channel_id`) REFERENCES `channels` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `sender_id_fk` FOREIGN KEY (`sender_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `messages`
--

LOCK TABLES `messages` WRITE;
/*!40000 ALTER TABLE `messages` DISABLE KEYS */;
INSERT INTO `messages` VALUES ('037a83ff-940d-4609-b485-b365df509e6c','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','s','2019-06-28 22:05:36'),('24bb2f76-56a2-4219-a4b6-a998f53ec6d5','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','Omri?','2019-06-27 16:58:05'),('2581dfad-1008-43a9-94bb-9185c616f856','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','פאק','2019-06-27 14:45:03'),('29643e8e-f3b1-4059-8e12-6d2043f74b5c','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','בוא נרשום קצת עברית ו English','2019-06-27 14:45:01'),('2a1fc219-31de-456f-8253-60cdb88d771c','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','a','2019-06-27 14:33:45'),('2c3599ff-b54c-4202-9436-e814cc50690e','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-28 21:36:49'),('2f07f014-53e9-40e2-ad9d-5af25bc5ba5e','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','Sima alay','2019-06-27 14:44:22'),('4000d377-413b-4cfa-89a6-9d6dff54fbc7','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','a','2019-06-27 14:36:24'),('4620c2f8-8337-46bb-8118-33f6c3908c69','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','B','2019-06-27 14:44:18'),('4ba42ca8-232e-49ef-9cd9-5152deed42de','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-28 21:36:43'),('5424b175-d213-41a5-b3e9-a4445e5a2045','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','סימה זה סוף סוף עובד','2019-06-27 14:44:52'),('56e7d461-aec7-4382-a7fc-f32a4af7904a','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','?skjdhaksd?jhdfkjahsd?kjashdkjhad?kajshdjks?skjdhaksd?jhdfkjahsd?kjashdkjhad?kajshdjks','2019-06-27 16:48:21'),('6191969b-96c5-4f4a-b20e-8e1da1924301','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','?','2019-06-27 15:30:44'),('86491e95-ac94-40bf-bd89-3fcb0c8986f6','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-28 21:44:48'),('c073ecd7-7243-419e-9731-f4f203025034','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f',':)','2019-06-27 15:30:38'),('d411f0a9-699b-4fc1-956a-2f76940b0889','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','d','2019-06-27 14:34:35'),('e5152dc5-3900-449a-b668-f2908629c87e','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','כולכם פה עליי','2019-06-27 14:44:44'),('e8d38f48-dcfd-4603-94fe-89c7aeeb8247','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','?skjdhaksd?jhdfkjahsd?kjashdkjhad?kajshdjks','2019-06-27 16:46:03'),('f742da3e-3b30-41b0-8f7a-20a514b560e9','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','טוב יש משהו לתקן','2019-06-27 14:45:08'),('f896b520-b017-4594-81d6-0524f0a7aebe','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','YAY SESSIONS WORK','2019-06-28 22:07:22'),('f8fed696-4851-40b3-99f7-108114637b4f','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','Please make this work ?','2019-06-27 15:35:22'),('fb99d144-71f4-4f66-92b7-a5a1a68c8da1','5464a7ba-4417-4a21-895d-0902bf4f9343','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','Hello','2019-06-27 14:42:39');
/*!40000 ALTER TABLE `messages` ENABLE KEYS */;
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
-- Table structure for table `sessions`
--

DROP TABLE IF EXISTS `sessions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `sessions` (
  `session_id` varchar(36) NOT NULL,
  `user_id` varchar(36) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`session_id`,`user_id`),
  UNIQUE KEY `session_id_UNIQUE` (`session_id`),
  KEY `user_id_fk_idx` (`user_id`),
  CONSTRAINT `user_id_fk` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sessions`
--

LOCK TABLES `sessions` WRITE;
/*!40000 ALTER TABLE `sessions` DISABLE KEYS */;
INSERT INTO `sessions` VALUES ('976c20c7-fcb7-41b6-974a-7365a45747d5','5464a7ba-4417-4a21-895d-0902bf4f9343','2019-06-28 22:06:11');
/*!40000 ALTER TABLE `sessions` ENABLE KEYS */;
UNLOCK TABLES;

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
INSERT INTO `users` VALUES ('0739e80c-b634-4f9e-a54f-5ee75d441a96','almogh43','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','P5DH15NS8N@gmail.com','sdsd',8467),('14b69ea0-2f58-4849-bc9e-27878f35ce71','djskd','a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3','kjdklasjd@fgkdj.com','djskd',255),('20734f93-c8d3-48a8-a6a6-e554a42395d8','almogh37','aa2df71c706f0eb10a78087be19cab6eaa1becc6a34808730f76a3ece884d247','almogh212@gmail.com','Atomiqu',4049),('32b0358b-a54d-48f1-b7a6-2c48d9470d6a','almogh23','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','QCIRZP5WEU@gmail.com','sdsd',6849),('3779d970-e6ed-4906-ba2f-10c647c279ce','OMRI','a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3','almogh521@gmail.com','OMRI',4197),('530e9407-7cfd-4be4-9cc6-70ab38e6310c','W0QU2RDWLM','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','W0QU2RDWLM@gmail.com','sdsd',2055),('5464a7ba-4417-4a21-895d-0902bf4f9343','almogh52','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','almogh52@gmail.com','almogh',1755),('68dcca8e-2704-46e9-9f30-b7b5b0a77e96','XUJS97B3HA','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','XUJS97B3HA@gmail.com','sdsd',1055),('79bbcce2-01f4-4e12-b88d-c3f985d960f5','almogh13','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','Z6ALDZRWYY@gmail.com','sdsd',7784),('7d612d87-e2e9-4734-b6b2-b410b47d7995','almogh51','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','90NS0RW6TF@gmail.com','sdsd',9591),('aa0722a9-f7ef-400e-94f5-abeea3748e7f','TD2BO1FPA1','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','TD2BO1FPA1@gmail.com','sdsd',1125),('c0eb536f-aa7a-41c6-8ba1-b8b94bdb9f25','almogsdsd','e2f56ed6bbfb6035701d763130dcca34bfb0cb97fcd9d91d8c5116d29a9b6bd9','almoghwewe@gmail.com','almogsdsd',1591),('ed6f6d9b-537c-43c1-96ae-84110fe5a9d5','KFT7NM25LQ','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','KFT7NM25LQ@gmail.com','sdsd',6534),('f096a34d-a572-4dfe-b66e-7fad1274631e','almogh53','15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225','Q8RQ7TE3ED@gmail.com','sdsd',1841);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping events for database 'quesync'
--

--
-- Dumping routines for database 'quesync'
--
/*!50003 DROP PROCEDURE IF EXISTS `get_private_channel` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `get_private_channel`(
	IN user_1 VARCHAR(36),
    IN user_2 VARCHAR(36)
)
BEGIN
	SELECT c1.id
    FROM channels c1
    RIGHT JOIN
	(SELECT channel_id
    FROM channel_members
    WHERE member_id IN (user_1, user_2)
    GROUP BY channel_id
    HAVING COUNT(member_id) = 2) c2
    ON c1.id = c2.channel_id
    WHERE is_private = True;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

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

-- Dump completed on 2019-06-29  1:08:59
