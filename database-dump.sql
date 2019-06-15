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
INSERT INTO `channel_members` VALUES ('49dc2dce-509d-46af-a563-05bef5ddcc67','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('7d834d31-7f90-4519-a672-0f670622a810','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f','32b0358b-a54d-48f1-b7a6-2c48d9470d6a'),('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('48f66837-376e-4720-9c33-46fde29f1376','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('7d834d31-7f90-4519-a672-0f670622a810','530e9407-7cfd-4be4-9cc6-70ab38e6310c'),('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','5464a7ba-4417-4a21-895d-0902bf4f9343'),('ca297f48-7e30-4773-834e-5f6c4b12c6bd','5464a7ba-4417-4a21-895d-0902bf4f9343'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f','5464a7ba-4417-4a21-895d-0902bf4f9343'),('49dc2dce-509d-46af-a563-05bef5ddcc67','79bbcce2-01f4-4e12-b88d-c3f985d960f5');
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
INSERT INTO `channels` VALUES ('2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc',1,'2019-06-14 14:58:33'),('48f66837-376e-4720-9c33-46fde29f1376',1,'2019-06-15 00:01:35'),('49dc2dce-509d-46af-a563-05bef5ddcc67',1,'2019-06-14 13:12:45'),('7d834d31-7f90-4519-a672-0f670622a810',1,'2019-06-14 14:58:48'),('ca297f48-7e30-4773-834e-5f6c4b12c6bd',1,'2019-06-14 14:58:17'),('d13f2902-0581-4b6f-8a25-a8e2e0ed201f',1,'2019-06-14 14:58:03');
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
INSERT INTO `messages` VALUES ('040bd387-8ef8-42d8-9cd7-0c6971cb4986','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-15 00:00:52'),('1809b443-45b3-459a-9d6b-e22fa11abff9','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:32:17'),('1fdabf3c-e77d-45f7-b524-165fa1af62eb','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:46'),('2040da85-f09f-4f82-b552-d0aab9ab4f2c','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:27'),('257b1d47-ade6-406d-a771-1d81528ed952','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:26'),('26771906-2c72-4b19-82e2-ed206d5f02fb','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','7d834d31-7f90-4519-a672-0f670622a810','TESTING','2019-06-14 23:58:51'),('2d63fca7-6617-483a-a665-3f50159e4de8','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:55:32'),('2fd48ff0-5cb3-498d-81bf-e6183d97a7b6','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:48'),('34452de2-089d-4984-91eb-3764eeae76c6','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:25'),('39b3db57-9951-4ed8-8832-c05d997a18a9','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','7d834d31-7f90-4519-a672-0f670622a810','?','2019-06-14 17:41:23'),('3d7ba52c-5c16-42b0-89f5-4467a239e172','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-14 14:58:33'),('429536b9-4473-4634-ad55-988d732e0064','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:24:35'),('47fe6b4a-e261-498d-b37a-fb945565bcb9','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:26'),('495d04b0-60ff-4d21-b2e8-37dfb13df324','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:28'),('4e285ba3-93ea-4775-8243-30d2a21b90a9','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:47'),('5a8826a7-53a8-464f-843b-b8fba3248c8c','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:33:31'),('649843fb-ccc3-4d20-ad84-a6e70b76dc97','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:34:36'),('67f9ecf7-a361-4a79-bc00-bb2bfcbed488','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:34:42'),('77f399c5-948d-47ce-8624-8e5959ce7e7c','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:28'),('7c2bdaf6-e710-4319-b6f0-ed938593bf37','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:57:04'),('7c88b1b1-55eb-4ffd-87b8-96ea3c1feef1','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:47'),('8f51a047-8f99-42e2-991c-7ab625842739','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','d13f2902-0581-4b6f-8a25-a8e2e0ed201f','TESTING','2019-06-14 14:58:03'),('9272edf4-cc0d-4a83-b488-ca41c3319dd7','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','7d834d31-7f90-4519-a672-0f670622a810','TESTING','2019-06-14 23:56:48'),('9cbb87c4-e5f7-4a86-8a29-8c04d8e4ffee','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:34:25'),('a1ef0f65-ba34-4f5b-9a94-57276ac2e564','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','7d834d31-7f90-4519-a672-0f670622a810','TESTING','2019-06-14 23:59:56'),('a55237b7-9e4c-4c12-bb26-db9d21792082','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:53:32'),('a8084846-c9ac-4617-bd3b-e60987f3afdc','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:23:58'),('acba15d9-1a53-4b6c-84c5-92ab3a18ebda','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:56:18'),('b73e10b4-b64d-4c79-b135-12a295401a6e','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:48'),('c30673bc-f0ee-4a73-b277-1d8edf9981cc','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:41'),('c5472bdc-6397-4273-982e-8d3c585c276c','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:21:48'),('ca6de399-8c9d-4f84-963d-9277e38212b7','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:18'),('ccb76a78-b3a8-4eed-9ab8-c152b3106033','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','49dc2dce-509d-46af-a563-05bef5ddcc67','TESTING','2019-06-14 14:56:46'),('d76b44f7-4110-426f-8397-44d253731741','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:30:28'),('de8ee3b6-3e76-4d73-8ac0-9f276cceaedc','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-15 00:07:49'),('f3081ee1-8296-48cf-9231-9005e8c537d4','32b0358b-a54d-48f1-b7a6-2c48d9470d6a','7d834d31-7f90-4519-a672-0f670622a810','בדיקה','2019-06-14 17:40:53'),('fc7b9a9f-0650-4b92-9116-eabac4ad085b','5464a7ba-4417-4a21-895d-0902bf4f9343','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','?','2019-06-14 23:31:58'),('ffe3f00b-b68a-4433-bc44-3a8fd28007a9','530e9407-7cfd-4be4-9cc6-70ab38e6310c','2e98d4b8-8cf0-4726-a2d5-285bf3b3adfc','TESTING','2019-06-15 00:03:24');
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

-- Dump completed on 2019-06-15  3:11:42
