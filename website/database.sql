DROP DATABASE IF EXISTS vsp_database;
CREATE DATABASE vsp_database;
USE vsp_database;

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

DROP TABLE IF EXISTS `readings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `readings` (
  `index` bigint NOT NULL AUTO_INCREMENT,
  `speed` int DEFAULT NULL,
  `pressure` int DEFAULT NULL,
  `co2` int DEFAULT NULL,
  `ah` int DEFAULT NULL,
  `rh` int DEFAULT NULL,
  `temp` int DEFAULT NULL,
  `timestamp` datetime DEFAULT CURRENT_TIMESTAMP,	
  PRIMARY KEY (`index`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

DROP TABLE IF EXISTS `logmessages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `logmessages` (
  `index` bigint NOT NULL AUTO_INCREMENT,
  `logcode` text DEFAULT NULL,
  `timestamp` datetime DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`index`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `readings` WRITE;
/*!40000 ALTER TABLE `readings` DISABLE KEYS */;
INSERT INTO `readings` (`speed`, `pressure`, `co2`, `ah`, `rh`, `temp`) VALUES
  (0, 0, 0, 0, 0, 0),
  (100, 50, 200, 100, 50, 20)
;
/*!40000 ALTER TABLE `readings` ENABLE KEYS */;
UNLOCK TABLES;

LOCK TABLES `logmessages` WRITE;
/*!40000 ALTER TABLE `logmessages` DISABLE KEYS */;
INSERT INTO `logmessages` (`logcode`) VALUES
  ('message'),
  ('another'),
  ('yet another')
;
/*!40000 ALTER TABLE `logmessages` ENABLE KEYS */;
UNLOCK TABLES;

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

CREATE USER IF NOT EXISTS 'vsp'@'localhost' IDENTIFIED BY 'vsp-user';
GRANT ALL PRIVILEGES ON vsp_database.* TO 'vsp'@'localhost';