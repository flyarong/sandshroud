/*Table structure for table `randomcardcreation` */

CREATE TABLE `randomcardcreation` (
  `SpellId` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId0` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId1` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId2` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId3` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId4` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId5` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId6` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId7` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId8` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId9` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId10` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId11` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId12` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId13` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId14` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId15` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId16` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId17` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId18` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId19` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId20` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId21` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId22` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId23` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId24` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId25` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId26` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId27` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId28` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId29` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId30` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemId31` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`SpellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*Data for the table `randomcardcreation` */

insert  into `randomcardcreation`(`SpellId`,`ItemId0`,`ItemId1`,`ItemId2`,`ItemId3`,`ItemId4`,`ItemId5`,`ItemId6`,`ItemId7`,`ItemId8`,`ItemId9`,`ItemId10`,`ItemId11`,`ItemId12`,`ItemId13`,`ItemId14`,`ItemId15`,`ItemId16`,`ItemId17`,`ItemId18`,`ItemId19`,`ItemId20`,`ItemId21`,`ItemId22`,`ItemId23`,`ItemId24`,`ItemId25`,`ItemId26`,`ItemId27`,`ItemId28`,`ItemId29`,`ItemId30`,`ItemId31`) values (59480,37145,37147,37159,37160,37161,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),(59491,44143,44154,44155,44156,44157,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),(48247,37140,37143,37156,37157,37158,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),(59487,44165,44144,44145,44146,44147,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),(44317,31882,31901,31910,31892,31883,31902,31911,31893,31884,31903,31912,31894,31885,31904,31913,31895,31886,31905,31915,31896,31887,31906,31916,31898,31888,31908,31917,31899,31889,31909,31918,31900),(59502,19227,19236,19233,19232,19235,19234,19231,19230,19268,19275,19272,19271,19274,19273,19270,19269,19276,19284,19281,19280,19283,19282,19279,19278,19258,19265,19262,19261,19264,19263,19260,19259),(59504,44277,44285,44281,44280,44284,44282,44279,44278,44286,44293,44290,44289,44292,44291,44288,44287,44268,44275,44272,44271,44274,44273,44270,44269,44260,44267,44264,44263,44266,44265,44262,44261);

/*Table structure for table `randomitemcreation` */

CREATE TABLE `randomitemcreation` (
  `SpellId` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemToCreate` int(10) unsigned NOT NULL DEFAULT '0',
  `Skill` int(10) unsigned NOT NULL DEFAULT '0',
  `Chance` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ItemToCreate`,`SpellId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*Data for the table `randomitemcreation` */

insert  into `randomitemcreation`(`SpellId`,`ItemToCreate`,`Skill`,`Chance`) values (48247,0,773,0),(59487,0,773,0),(59480,0,773,0),(53892,41482,755,25),(53866,42450,755,25),(53831,41432,755,25),(53835,41433,755,25),(53852,41444,755,25),(53874,41483,755,25),(53926,41463,755,25),(53877,41484,755,25),(53869,41451,755,25),(53880,41485,755,25),(53832,41434,755,25),(53884,41486,755,25),(53888,41487,755,25),(53918,41464,755,25),(53930,41465,755,25),(53873,41488,755,25),(53876,41489,755,25),(53844,41435,755,25),(53920,41466,755,25),(53845,41436,755,25),(53878,41491,755,25),(53862,41452,755,25),(53871,41453,755,25),(53867,41454,755,25),(53872,41492,755,25),(53925,41467,755,25),(53916,41468,755,25),(53928,41469,755,25),(53879,41493,755,25),(53881,41494,755,25),(53941,41440,755,25),(53922,41470,755,25),(53865,41455,755,25),(53857,41445,755,25),(53929,41471,755,25),(53882,41495,755,25),(54017,41437,755,25),(53887,41496,755,25),(53870,41456,755,25),(53863,41457,755,25),(53856,41446,755,25),(53931,41472,755,25),(53885,41497,755,25),(53893,41498,755,25),(53875,41499,755,25),(53854,41447,755,25),(53864,41459,755,25),(53834,41438,755,25),(53921,41473,755,25),(53933,41474,755,25),(53860,41460,755,25),(53923,41475,755,25),(53853,41448,755,25),(53934,41441,755,25),(53859,41461,755,25),(53940,41442,755,25),(53890,41500,755,25),(53889,41501,755,25),(53919,41476,755,25),(53943,41443,755,25),(53927,41447,755,25),(53932,41478,755,25),(53861,41462,755,25),(53855,41449,755,25),(53894,41479,755,25),(53924,41480,755,25),(53883,41502,755,25),(53917,41481,755,25),(53886,41429,755,25),(53843,41439,755,25),(53868,41458,755,25),(53891,41490,755,25),(59491,0,773,0),(44317,0,773,0),(59502,0,773,0),(59504,0,773,0);
