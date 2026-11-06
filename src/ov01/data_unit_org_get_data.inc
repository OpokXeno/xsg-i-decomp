/*
 * OV01 TU 5 .data and const tables: the name tables, the think and map
 * tables, the position and the stat-file tables. Definitions stay in their
 * original order. The types XtxFileEntry, PosTblEntry, ThinkMapTable and
 * SpecTable are the TU-local declarations in the main file.
 */
extern char unitOrgDataYamamotoMotText[];
extern char unitOrgBinText[];
extern char unitOrgDataYamamotoMapText[];
extern char unitOrgMapText[];
extern char unitOrgDataYamamotoThinkText[];
extern char unitOrgThinkText[];
extern char unitOrgDataYamamotoStatStatXtxText[];
extern char unitOrgDataYamamotoStatManuXtxText[];
extern char unitOrgDataYamamotoStatCurXtxText[];
static const char *motNameBase = unitOrgDataYamamotoMotText;
static const char *motNameExt = unitOrgBinText;
static const char *mapNameBase = unitOrgDataYamamotoMapText;
static const char *mapNameExt = unitOrgBinText;
static const char *mapName = unitOrgMapText;
static const char *thinkNameBase = unitOrgDataYamamotoThinkText;
static const char *thinkNameExt = unitOrgBinText;
static const char *thinkName = unitOrgThinkText;
/* Destination addresses dataFileLoadNB reads the stat XTX files into (main RAM,
 * not CD sectors). */
#define YAMAMOTO_STAT_STAT_XTX_ADDRESS 0x01B13800
#define YAMAMOTO_STAT_MANU_XTX_ADDRESS 0x01B09800
#define YAMAMOTO_STAT_CUR_XTX_ADDRESS 0x01B04000
static XtxFileEntry xtxFile[4] = {
    { unitOrgDataYamamotoStatStatXtxText, YAMAMOTO_STAT_STAT_XTX_ADDRESS },
    { unitOrgDataYamamotoStatManuXtxText, YAMAMOTO_STAT_MANU_XTX_ADDRESS },
    { unitOrgDataYamamotoStatCurXtxText, YAMAMOTO_STAT_CUR_XTX_ADDRESS },
    { 0, 0 }
};

static unsigned short xtxAdr[0x13] = {
    0x3800, 0x3AE0, 0x3908, 0x3AE4, 0x38C0, 0x3AF4, 0x38D0,
    0x3AE4, 0x3840, 0x3AF0, 0x3AD4, 0x3880, 0x3AD0, 0x3800,
    0x3840, 0x3800, 0x3820, 0x3840, 0x3828
};

/* The position table's Vector4 and line fields are consumed by the TU and unit_cmd. */
static PosTblEntry posTbl[36] = {
    { {0.0f, 0.0f, 0.0f, 1.0f}, 0 },
    { {2.0f, 0.0f, 2.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, 0.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, -2.0f, 1.0f}, 1 },
    { {4.0f, 0.0f, 2.0f, 1.0f}, 2 },
    { {4.0f, 0.0f, 0.0f, 1.0f}, 2 },
    { {4.0f, 0.0f, -2.0f, 1.0f}, 2 },
    { {5.0f, 0.0f, -4.0f, 1.0f}, 0 },
    { {6.0f, 0.0f, 0.0f, 1.0f}, 0 },
    { {5.0f, 0.0f, 4.0f, 1.0f}, 0 },
    { {2.0f, 0.0f, -3.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, -2.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, -1.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, 0.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, 1.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, 2.0f, 1.0f}, 1 },
    { {2.0f, 0.0f, 3.0f, 1.0f}, 1 },
    { {3.0f, 0.0f, -4.0f, 1.0f}, 2 },
    { {3.0f, 0.0f, 4.0f, 1.0f}, 2 },
    { {4.0f, 0.0f, -2.0f, 1.0f}, 2 },
    { {4.0f, 0.0f, 0.0f, 1.0f}, 2 },
    { {4.0f, 0.0f, 2.0f, 1.0f}, 2 },
    { {5.0f, 0.0f, -5.0f, 1.0f}, 3 },
    { {5.0f, 0.0f, 5.0f, 1.0f}, 3 },
    { {6.0f, 0.0f, 0.0f, 1.0f}, 3 },
    { {8.0f, 0.0f, -3.0f, 1.0f}, 4 },
    { {8.0f, 0.0f, 0.0f, 1.0f}, 4 },
    { {8.0f, 0.0f, 3.0f, 1.0f}, 4 },
    { {14.0f, 0.0f, 0.0f, 1.0f}, 5 },
    { {7.0f, 0.0f, -5.0f, 1.0f}, 4 },
    { {7.0f, 0.0f, 5.0f, 1.0f}, 4 },
    { {13.0f, 0.0f, -9.0f, 1.0f}, 5 },
    { {13.0f, 0.0f, 9.0f, 1.0f}, 5 },
    { {9.0f, 0.0f, -9.0f, 1.0f}, 4 },
    { {9.0f, 0.0f, 9.0f, 1.0f}, 4 },
    { {18.0f, 0.0f, 0.0f, 1.0f}, 5 }
};

/* These lookup arrays translate game character, width and think-file ids. */
static short widChgTbl[116] = {
     0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3801,
     0x3001, 0x3002, 0x3003, 0x3004, 0x3005, 0x3006, 0x3007, 0x3000, 0x3008, 0x3009, 0x300A, 0x3000,
     0x3101, 0x3102, 0x3103, 0x3104, 0x3105, 0x3106, 0x3107, 0x3108, 0x3109, 0x310A, 0x310B, 0x310C,
     0x310D, 0x310E, 0x310F, 0x3110, 0x3111, 0x3201, 0x3202, 0x3203, 0x3204, 0x3205, 0x3301, 0x3302,
     0x3303, 0x3304, 0x3305, 0x3306, 0x3307, 0x3308, 0x3000, 0x3000, 0x3000, 0x3401, 0x3402, 0x3403,
     0x3404, 0x3405, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x3501, 0x3000, 0x3601,
     0x3602, 0x3603, 0x3608, 0x3605, 0x3606, 0x3607, 0x3604, 0x3609, 0x360A, 0x360B, 0x360C, 0x360D,
     0x360E, 0x360F, 0x3610, 0x3613, 0x3614, 0x3615, 0x3616, 0x3617, 0x3618, 0x3619, 0x361A, 0x361B,
     0x361C, 0x361D, 0x361E, 0x361F, 0x3620, 0x3623, 0x3624, 0x3625, 0x3626, 0x3628, 0x3629, 0x362A,
     0x362B, 0x362C, 0x362D, 0x362E, 0x362F, 0x3630, 0x3631, 0x3701
 };
 
static short cidChgTbl[196] = {
     0x0000, 0x0003, 0x0002, 0x0001, 0x0007, 0x0006, 0x0004, 0x0005, 0x0000, 0x0000, 0x0110, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2101,
     0x2102, 0x2103, 0x2104, 0x2105, 0x0000, 0x2201, 0x0000, 0x0000, 0x0000, 0x020E, 0x4001, 0x4002,
     0x4003, 0x4004, 0x4005, 0x4006, 0x4007, 0x4008, 0x4009, 0x400A, 0x400B, 0x400C, 0x400D, 0x400E,
     0x400F, 0x4010, 0x4011, 0x4012, 0x4013, 0x4014, 0x4015, 0x4016, 0x4017, 0x4018, 0x4019, 0x4001,
     0x4002, 0x4003, 0x4037, 0x4501, 0x4502, 0x4503, 0x4504, 0x4505, 0x4506, 0x4801, 0x4802, 0x4803,
     0x4601, 0x4701, 0x4702, 0x4703, 0x4704, 0x4101, 0x4102, 0x4103, 0x4104, 0x4105, 0x4106, 0x4107,
     0x4108, 0x4109, 0x410A, 0x410B, 0x410C, 0x410D, 0x410E, 0x4201, 0x4202, 0x4203, 0x4204, 0x4205,
     0x4206, 0x4401, 0x4402, 0x4403, 0x4404, 0x4405, 0x4406, 0x4407, 0x4301, 0x4302, 0x4303, 0x4304,
     0x4305, 0x4306, 0x4307, 0x4102, 0x4102, 0x4106, 0x4006, 0x4501, 0x4701, 0x4702, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x4701, 0x4701, 0x4701, 0x4701, 0x4701, 0x4F01, 0x4F02, 0x4F03, 0x4F04, 0x4F05,
     0x4F06, 0x4F07, 0x4F08, 0x4F09, 0x4F0A, 0x4F0B, 0x4F0C, 0x4F0D, 0x4F0E, 0x4F0F, 0x4F10, 0x4F11,
     0x4F12, 0x4F13, 0x4F14, 0x4F15, 0x4F16, 0x4F17, 0x4F18, 0x4F19, 0x4F1A, 0x4F1B, 0x4F1C, 0x4F1D,
     0x4F1E, 0x4F1F, 0x4F20, 0x4F21, 0x4F03, 0x4F04, 0x4F14, 0x4F22, 0x4F23, 0x4F24, 0x4F25, 0x4F26,
     0x4F27, 0x4F28, 0x4F29, 0x0000
 };

static const char *motFile[303] = { unitOrgChaoText, unitOrgKosmText, unitOrgShioText, unitOrgShitText, unitOrgZiggText, unitOrgMomoText, unitOrgJrText, unitOrgCeciText, unitOrgCathText, unitOrgVirgText, unitOrgChaoText, unitOrgChaoText, 0, 0, 0, 0, unitOrgVx01Text, unitOrgVx02Text, unitOrgVx03Text, unitOrgVx04Text, unitOrgVx05Text, unitOrgVx06Text, unitOrgAg01Text, unitOrgAg02Text, unitOrgAg03Text, unitOrgAg04Text, unitOrgAg05Text, unitOrgAg05Text, unitOrgUknText, 0, 0, 0, unitOrgSldText, unitOrgGuno001Text, unitOrgGuno002Text, unitOrgGuno003Text, unitOrgGuno004Text, unitOrgGuno005Text, unitOrgGuno006Text, unitOrgGuno007Text, unitOrgGuno008Text, unitOrgGuno009Text, unitOrgGuno010Text, unitOrgGuno011Text, unitOrgGuno012Text, unitOrgGuno013Text, unitOrgGuno014Text, unitOrgGuno015Text, unitOrgGuno016Text, unitOrgGuno017Text, unitOrgGuno018Text, unitOrgGuno019Text, unitOrgGuno020Text, unitOrgGuno021Text, unitOrgGuno022Text, unitOrgGuno023Text, unitOrgGuno024Text, unitOrgGuno025Text, unitOrgGuno001Text, unitOrgGuno002Text, unitOrgGuno003Text, unitOrgGuno114Text, unitOrgFema001Text, unitOrgFema002Text, unitOrgFema003Text, unitOrgFema004Text, unitOrgFema005Text, unitOrgFema006Text, unitOrgFere001Text, unitOrgFere002Text, unitOrgFere003Text, unitOrgFero001Text, unitOrgFeso001Text, unitOrgFeso002Text, unitOrgFeso003Text, unitOrgFeso004Text, unitOrgUtma001Text, unitOrgUtma002Text, unitOrgUtma003Text, unitOrgUtma004Text, unitOrgUtma005Text, unitOrgUtma006Text, unitOrgUtma007Text, unitOrgUtma008Text, unitOrgUtma009Text, unitOrgUtma010Text, unitOrgUtma011Text, unitOrgUtma012Text, unitOrgUtma013Text, unitOrgUtma014Text, unitOrgUtre001Text, unitOrgUtre002Text, unitOrgUtre003Text, unitOrgUtre004Text, unitOrgUtre005Text, unitOrgUtre006Text, unitOrgUtro001Text, unitOrgUtro002Text, unitOrgUtro003Text, unitOrgUtro004Text, unitOrgUtro005Text, unitOrgUtro006Text, unitOrgUtro007Text, unitOrgUtso001Text, unitOrgUtso002Text, unitOrgUtso003Text, unitOrgUtso004Text, unitOrgUtso005Text, unitOrgUtso006Text, unitOrgUtso007Text, unitOrgUtma002Text, unitOrgUtma002Text, unitOrgUtma006Text, unitOrgGuno006Text, unitOrgFema001Text, unitOrgFeso001Text, unitOrgFeso002Text, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, unitOrgFeso001Text, unitOrgFeso001Text, unitOrgFeso001Text, unitOrgFeso001Text, unitOrgFeso001Text, unitOrgBoss001Text, unitOrgBoss002Text, unitOrgBoss003Text, unitOrgBoss004Text, unitOrgBoss005Text, unitOrgBoss006Text, unitOrgBoss007Text, unitOrgBoss008Text, unitOrgBoss009Text, unitOrgBoss010Text, unitOrgBoss011Text, unitOrgBoss012Text, unitOrgBoss013Text, unitOrgBoss014Text, unitOrgBoss015Text, unitOrgBoss016Text, unitOrgBoss017Text, unitOrgBoss018Text, unitOrgBoss019Text, unitOrgBoss020Text, unitOrgBoss021Text, unitOrgBoss022Text, unitOrgBoss023Text, unitOrgBoss024Text, unitOrgBoss025Text, unitOrgBoss026Text, unitOrgBoss027Text, unitOrgBoss028Text, unitOrgBoss029Text, unitOrgBoss030Text, unitOrgBoss031Text, unitOrgBoss032Text, unitOrgBoss033Text, unitOrgBoss003Text, unitOrgBoss004Text, unitOrgBoss020Text, unitOrgSkjJr1Text, unitOrgSkjJr2Text, unitOrgSkjMomo1Text, unitOrgSkjShion1Text, unitOrgSkjShion2Text, unitOrgSkjShion3Text, unitOrgSkjShion4Text, unitOrgSkjKosmos1Text, unitOrgChaoSp1Text, unitOrgChaoSp2Text, unitOrgChaoSp3Text, unitOrgChaoSp4Text, unitOrgChaoSp5Text, unitOrgChaoSp6Text, unitOrgChaoSp7Text, unitOrgChaoSp8Text, unitOrgKosmSp1Text, unitOrgKosmSp2Text, unitOrgKosmSp3Text, unitOrgKosmSp4Text, unitOrgKosmSp5Text, unitOrgKosmSp6Text, unitOrgKosmSp7Text, unitOrgShioSp1Text, unitOrgShioSp2Text, unitOrgShioSp3Text, unitOrgShioSp4Text, unitOrgShioSp5Text, unitOrgShioSp6Text, unitOrgShioSp7Text, unitOrgShioSp8Text, unitOrgShitSp1Text, unitOrgShitSp2Text, unitOrgShitSp3Text, unitOrgShitSp4Text, unitOrgShitSp5Text, unitOrgShitSp6Text, unitOrgShitSp7Text, unitOrgShitSp8Text, unitOrgZiggSp1Text, unitOrgZiggSp2Text, unitOrgZiggSp3Text, unitOrgZiggSp4Text, unitOrgZiggSp5Text, unitOrgZiggSp6Text, unitOrgZiggSp7Text, unitOrgMomoSp1Text, unitOrgMomoSp2Text, unitOrgMomoSp3Text, unitOrgMomoSp4Text, unitOrgMomoSp5Text, unitOrgMomoSp6Text, unitOrgMomoSp7Text, unitOrgMomoSp8Text, unitOrgJrSp1Text, unitOrgJrSp2Text, unitOrgJrSp3Text, unitOrgJrSp4Text, unitOrgJrSp5Text, unitOrgJrSp6Text, unitOrgJrSp7Text, unitOrgJrSp8Text, unitOrgKosmWp1Text, unitOrgKosmWp2Text, unitOrgKosmWp3Text, unitOrgKosmWp4Text, unitOrgKosmWp5Text, unitOrgKosmWp6Text, unitOrgZiggWp1Text, unitOrgZiggWp2Text, unitOrgZiggWp3Text, unitOrgZiggWp4Text, unitOrgSut0Text, unitOrgAx0Text, unitOrgBd0Text, unitOrgBd1Text, unitOrgBdf0Text, unitOrgHg0Text, unitOrgHg1Text, unitOrgHmr0Text, unitOrgHmr1Text, unitOrgHmr2Text, unitOrgMg0Text, unitOrgMg1Text, unitOrgRf0Text, unitOrgRf1Text, unitOrgRf2Text, unitOrgBam0Text, unitOrgBs0Text, unitOrgLsr0Text, unitOrgPbk0Text, unitOrgFl0Text, unitOrgGnd0Text, unitOrgGlg0Text, unitOrgHb0Text, unitOrgMsp0Text, unitOrgMsp2Text, unitOrgBl0Text, unitOrgCb0Text, unitOrgEac0Text, unitOrgBp0Text, unitOrgLgc0Text, unitOrgAmp2Text, unitOrgShb0Text, unitOrgFsd0Text, unitOrgVsd0Text, unitOrgBbc0Text, unitOrgEad0Text, unitOrgEcm1Text, unitOrgEcm2Text, unitOrgSmp1Text, unitOrgWir0Text, unitOrgSol0Text, unitOrgDef0Text, unitOrgGlg0Text, unitOrgHb0Text, unitOrgMsp2Text };
 
/* The indexed special-base fields continue at the next original data symbol. */
static ThinkMapTable thinkMapTbl = {
    { 0x00000000, 0x00000024, 0x00000026, 0x00000027, 0x00000028, 0x00000029, 0x0000002F, 0x00000030, 0x00000032, 0x00000036, 0x00000039, 0x0000044C, 0x0000044D, 0x0000044F, 0x00000450, 0x00000451, 0x00000075, 0x00000076, 0x00000077, 0x0000007A, 0x0000007B, 0x000005DE, 0x000005DF, 0x000005E0, 0x000005E1, 0x000005E4, 0x000006A4, 0x000006A5, 0x000006A6, 0x000006A7, 0x000006A8, 0x000006A9, 0x000006AA, 0x000006AB, 0x000006AD, 0x000006AE, 0x000006AF, 0x000004B2, 0x000004B3, 0x000004B7, 0x000004BB, 0x000004BC, 0x000004BD, 0x000004BE, 0x000004BF, 0x000004C0, 0x000004C2, 0x000004C3, 0x000004CE, 0x00000523, 0x00000524, 0x00000525, 0x00000526, 0x00000527, 0x00000528, 0x00000529, 0x0000052A, 0x00000384, 0x00000385, 0x00000386, 0x00000387, 0x00000388, 0x00000389, 0x0000038A, 0x0000038B, 0x0000038C, 0x0000038D, 0x00000390, 0x00000324, 0x00000329, 0x0000032B, 0x0000032C, 0x0000032D, 0x0000032E, 0x0000032F, 0x00000579, 0x0000057B, 0x00000580, 0x00000581, 0x00000582, 0x00000583, 0x00000584, 0x00000585, 0x00000589, 0x0000058A, 0x0000058B, 0x0000058D, 0x0000058F, 0x00000331, 0x00000517, 0x00000037, 0x00000327, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};

static SpecTable specTbl = {
    { 0x0000002D, 0x00000035, 0x0000003C, 0x00000044, 0x0000004C, 0x00000053, 0x0000005B, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00000000 }
};
 
/* Error and file-name messages from this TU's original rodata. */
static const char unitOrgDataSpecLearnSetErrText[] = "** dataSpecLearnSet: err %d\n";
static const char unitOrgDataSpecLearnGetErrText[] = "** dataSpecLearnGet: err %d\n";
static const unsigned char unitOrgDataCidGetErrText[] = "** dataCidGet: err %d\n";
static const unsigned char unitOrgDataWidGetErrText[] = "** dataWidGet: err %d";
static const char unitOrgNBreadErrText[] = "** NBread: err %d\n";
static const char unitOrgNBreadEndText[] = "** NBread: end %d *\n";
static const char unitOrgDataUnitFileLoadErrText[] = "** dataUnitFileLoad: err\n";
static const char unitOrgDataChildActorCreateErrText[] = "** dataChildActorCreate: err\n";
static const char unitOrgDataXtxAdrGetErrText[] = "** dataXtxAdrGet: err %d\n";
static const char unitOrgDataYamamotoBatdatBinText[] = "data\\yamamoto\\batdat.bin";
static const char unitOrgJpText[] = "%03d";
static const char unitOrgDataMapLoadErrText[] = "** dataMapLoad: err -> %d\n";
