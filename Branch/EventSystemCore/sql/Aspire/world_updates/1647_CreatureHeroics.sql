
-- Alternative table for npc in heroic mode
ALTER TABLE `worldmap_info` DROP COLUMN `lvl_mod_a`;
CREATE TABLE `creature_proto_heroic` (
  `entry` int(10) NOT NULL,
  `minlevel` int(10) unsigned NOT NULL DEFAULT '0',
  `maxlevel` int(10) unsigned NOT NULL DEFAULT '0',
  `minhealth` int(10) NOT NULL DEFAULT '1',
  `maxhealth` int(10) NOT NULL DEFAULT '1',
  `mindmg` int(10) NOT NULL DEFAULT '1',
  `maxdmg` int(10) NOT NULL DEFAULT '1',
  `mana` int(10) NOT NULL DEFAULT '1',
  `resistance1` int(10) NOT NULL DEFAULT '0',
  `resistance2` int(10) NOT NULL DEFAULT '0',
  `resistance3` int(10) NOT NULL DEFAULT '0',
  `resistance4` int(10) NOT NULL DEFAULT '0',
  `resistance5` int(10) NOT NULL DEFAULT '0',
  `resistance6` int(10) NOT NULL DEFAULT '0',
  `resistance7` int(10) NOT NULL DEFAULT '0',
  `auras` longtext NOT NULL,
  `auraimmune_flag` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


