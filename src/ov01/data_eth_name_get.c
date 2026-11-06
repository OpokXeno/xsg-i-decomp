/*
 * OV01 original TU 11: 0x00a2c5a8..0x00a2c9b8 (13 functions)
 */
#include "common.h"
#include "ov01/data_eth_name_get.h"

#define ENEMY_NAME_FIRST_ID 33
#define PLAYER_NAME_COUNT 32
#define PLAYER_ID_ZIGGURAT8 5
/* From this scenario number on, dataPlayerNameGet returns ziggyName for
 * PLAYER_ID_ZIGGURAT8 (the value is the code's threshold, not a documented name). */
#define ZIGGURAT8_NAME_SCENARIO_FIRST 108
#define SPC_NAME_FIRST_ID 45

/*
 * One row of the name tables. The accessors return the address of `name`, so
 * callers read the help text as element [1] through that pointer (menu.c does
 * this for the ethName family).
 */
typedef struct EthNameHelp {
    const char *help;
    const char *initial;
} EthNameHelp;

typedef struct EthName {
    const char *name;
    EthNameHelp text;
} EthName;

extern const char ethNameSelfIncreaseSpeed50Format[];
extern const char ethNameSpeedBoostText[];
extern const char ethNameOneEnemy1xOnly80RiticalText[];
extern const char ethNameRedMarkText[];
extern const char ethNameSelf1xOnlyHPEPRecoveryText[];
extern const char ethNameChargeXText[];
extern const char ethNameSelfFocusEtherAtkText[];
extern const char ethNameEtherShiftBText[];
extern const char ethNameSelfFocusEtherDefText[];
extern const char ethNameEtherShiftAText[];
extern const char ethNameSelfHPRecoveryMidText[];
extern const char ethNameRechargeText[];
extern const char ethNameSelfIncreaseStrDefWhenMOMOText[];
extern const char ethNameBodyguardText[];
extern const char ethNameSelfFocusPhysDefText[];
extern const char ethNameMyGuardText[];
extern const char ethNameReserveText[];
extern const char ethNameAllEnemies1xPerBattleSummonText[];
extern const char ethNameBusterJoeText[];
extern const char ethNameOneEnemy1xPerBattleSummonText[];
extern const char ethNameMagnumJoeText[];
extern const char ethNameSpeedMachineText[];
extern const char ethNameOneAllyBlockStatChangesSupportText[];
extern const char ethNameCoinLockText[];
extern const char ethNameOneEnemyDisableBGEnemiesText[];
extern const char ethNameMistyText[];
extern const char ethNameOneEnemyEvadeDownForBText[];
extern const char ethNameChainText[];
extern const char ethNameOneEnemyStealItemsText[];
extern const char ethNamePsychoPocketText[];
extern const char ethNameSelf1xOnlyReviveWhenFemaleText[];
extern const char ethNameDandyismText[];
extern const char ethNameOneEnemyAllyIncreaseEtherEffectFormat[];
extern const char ethNameEtherFlareText[];
extern const char ethNameSelfIncreasePhysAtkText[];
extern const char ethNamePsychoArmText[];
extern const char ethNameAllEnemies1xPerStarlightNonText[];
extern const char ethNameStarBunnieText[];
extern const char ethNameOneEnemyOnlyDuringStarlightNonText[];
extern const char ethNameStarBellText[];
extern const char ethNameSelf1xPerBattleUseStarlightText[];
extern const char ethNameStarlightText[];
extern const char ethNameOneAlly1xPerStarWindText[];
extern const char ethNameStarActionText[];
extern const char ethNameAllAlliesOnlyDuringStarWindFormat[];
extern const char ethNameStarVeilText[];
extern const char ethNameSelf1xPerBattleUseStarText[];
extern const char ethNameStarWindText[];
extern const char ethNameOneEnemyReduceMEnemyActionFormat[];
extern const char ethNameJunkBeamText[];
extern const char ethNameOneAllyReviveHPRecoveryLowText[];
extern const char ethNameRefineText[];
extern const char ethNameSelfAHeavenlyGiftText[];
extern const char ethNamePrayerText[];
extern const char ethNameOneEnemyPutMEnemyPilotsText[];
extern const char ethNameSheepBeamText[];
extern const char ethNameOneEnemyNonElementalEtherAtkText[];
extern const char ethNameMiracleStarText[];
extern const char ethNameOneEnemyAllyHPRecoveryMidText[];
extern const char ethNameLifeShotText[];
extern const char ethNameAllEnemiesAttackClearAllStText[];
extern const char ethNameLightWingsText[];
extern const char ethNameCText[];
extern const char ethNameAllEnemiesResetAttackWaitTimeText[];
extern const char ethNameCataclysmText[];
extern const char ethNameOneEnemyHPRecoveryDown1Text[];
extern const char ethNameSupremeJudgmentText[];
extern const char ethNameAllAllies1xPerBattleReviveText[];
extern const char ethNameBestAllyText[];
extern const char ethNameLText[];
extern const char ethNameOneEnemyLightningLightningEtherAtkText[];
extern const char ethNameLightningWingsText[];
extern const char ethNameFText[];
extern const char ethNameOneEnemyFireFireEtherAtkText[];
extern const char ethNameFlameWingsText[];
extern const char ethNameAllAlliesClearAllStatusChangesText[];
extern const char ethNamePurifyingStormText[];
extern const char ethNameIText[];
extern const char ethNameOneEnemyIceIceEtherAtkText[];
extern const char ethNameIceWingsText[];
extern const char ethNameHText[];
extern const char ethNameAllAlliesHPRecoveryLowText[];
extern const char ethNameHealingDewText[];
extern const char ethNamePText[];
extern const char ethNameOneEnemyAllyEtherEffectDownFormat[];
extern const char ethNameProtectiveWearText[];
extern const char ethNameAllEnemiesLightningLightningEtherAtkText[];
extern const char ethNameDexEtherLeText[];
extern const char ethNameAllEnemiesFireFireEtherAtkText[];
extern const char ethNameDexEtherFaText[];
extern const char ethNameAllEnemiesIceIceEtherAtkText[];
extern const char ethNameDexEtherRaText[];
extern const char ethNameAllEnemiesBeam1xPerBattleText[];
extern const char ethNameSatelliteText[];
extern const char ethNameSelfEtherAtkDamageAndCostText[];
extern const char ethNameEtherLimitText[];
extern const char ethNameOneEnemyOccasionallyInstantKOHPText[];
extern const char ethNameGateText[];
extern const char ethNameOneEnemyEtherDownForBText[];
extern const char ethNameDownEtherText[];
extern const char ethNameOneEnemyPhysAtkDownForText[];
extern const char ethNameDownForceText[];
extern const char ethNameOneEnemyDexterityDownForBText[];
extern const char ethNameDownDexText[];
extern const char ethNameSelfFocusPhysAtkText[];
extern const char ethNameModeA7Text[];
extern const char ethNameEText[];
extern const char ethNameAllEnemiesSlash1xPerBattleText[];
extern const char ethNameErdeKaiserText[];
extern const char ethNameDText[];
extern const char ethNameAllEnemiesHit1xPerBattleText[];
extern const char ethNameDominionTankText[];
extern const char ethNameTText[];
extern const char ethNameAllEnemiesSlash1xPerBattleText2[];
extern const char ethNameThroniBladeText[];
extern const char ethNameAllEnemiesBeam1xPerBattleText2[];
extern const char ethNameSeraphimBirdText[];
extern const char ethNameOneEnemyOccasionallyInstantKOStealText[];
extern const char ethNameQueenSKissText[];
extern const char ethNameQText[];
extern const char ethNameOneAllyIncreaseSpeed25Format[];
extern const char ethNameQuickText[];
extern const char ethNameSText[];
extern const char ethName1xPerAllySurviveWith1Text[];
extern const char ethNameSafetyLevelText[];
extern const char ethNameOneAllyReviveHPRecoveryMidText[];
extern const char ethNameRevertText[];
extern const char ethNameOneEnemyAllyHPRecoveryMaxText[];
extern const char ethNameMedicaRestText[];
extern const char ethNameJText[];
extern const char ethNameOneEnemyDexterityDownForMText[];
extern const char ethNameJammingText[];
extern const char ethNameBText[];
extern const char ethNameOneAllyIncreaseBoostBy1Text[];
extern const char ethNameBoost1Text[];
extern const char ethNameRText[];
extern const char ethNameOneAllyClearAllStatusChangesText[];
extern const char ethNameRefreshText[];
extern const char ethNameAllAlliesHPRecoveryMidText[];
extern const char ethNameMedicaAllText[];
extern const char ethNameGText[];
extern const char ethNameAllyEscapeFromBattleText[];
extern const char ethNameGoodbyeText[];
extern const char ethNameAText[];
extern const char ethNameOneEnemyAnalyzeItemsAndHPText[];
extern const char ethNameAnalyzeText[];
extern const char ethNameMText[];
extern const char ethNameOneEnemyAllyHPRecoveryLowText[];
extern const char ethNameMedicaText[];
extern const char ethNameDiamondText[];
extern const char ethNameEmeraldText[];
extern const char ethNameRubyText[];
extern const char ethNameSapphireText[];
extern const char ethNameGarnetText[];
extern const char ethNameJunkedCircuitBText[];
extern const char ethNamePreciousStoneText[];
extern const char ethNameKText[];
extern const char ethNameKoboldBladeText[];
extern const char ethNameUText[];
extern const char ethNameUnicornHornText[];
extern const char ethNameOneAllySPts100Text[];
extern const char ethNameSkillUpgradeZText[];
extern const char ethNameOneAllySPts50Text[];
extern const char ethNameSkillUpgradeSText[];
extern const char ethNameOneAllySPts10Text[];
extern const char ethNameSkillUpgradeAText[];
extern const char ethNameOneAllyEPts100Text[];
extern const char ethNameEtherUpgradeZText[];
extern const char ethNameOneAllyEPts50Text[];
extern const char ethNameEtherUpgradeSText[];
extern const char ethNameOneAllyEPts10Text[];
extern const char ethNameEtherUpgradeAText[];
extern const char ethNameOneAllyTPts100Text[];
extern const char ethNameTechUpgradeZText[];
extern const char ethNameOneAllyTPts50Text[];
extern const char ethNameTechUpgradeSText[];
extern const char ethNameOneAllyTPts10Text[];
extern const char ethNameTechUpgradeAText[];
extern const char ethNameOneAllySkillLevel1Text[];
extern const char ethNameSkillUpgradeText[];
extern const char ethNameJunkedCircuitAText[];
extern const char ethNameBarterItemText[];
extern const char ethNameScrapIronText[];
extern const char ethNameAGWSFHPRecoveryText[];
extern const char ethNameFrameRepairZText[];
extern const char ethNameAGWSFHPRecoveryText2[];
extern const char ethNameFrameRepairAText[];
extern const char ethNameAllAlliesSavePointsOnlyHPText[];
extern const char ethNameBioSphereText[];
extern const char ethNameAntiVeilText[];
extern const char ethNameVText[];
extern const char ethNameOneEnemyAllyWPressAtFormat[];
extern const char ethNameVeilText[];
extern const char ethNameOneAllyWPressAtAP6Format[];
extern const char ethNameSpeedStimDXText[];
extern const char ethNameOneAllyWPressAtAP6Text[];
extern const char ethNameDefenseShieldText[];
extern const char ethNameOneAllyIncreasesSpeed50Format[];
extern const char ethNameSpeedStimSXText[];
extern const char ethNameOneAllyWPressAtAP6Format2[];
extern const char ethNameSpeedStimText[];
extern const char ethNameOneAllyHP1Text[];
extern const char ethNameHemlockText[];
extern const char ethNameOneAllyWPressAtAP6Format3[];
extern const char ethNameStimDXText[];
extern const char ethNameOneAllyWPressAtAP6Format4[];
extern const char ethNameStimText[];
extern const char ethNameOneAllyEscapeFromBattleText[];
extern const char ethNameEscapePackText[];
extern const char ethNameOneAllyBoost1Text[];
extern const char ethNameBoosterPackText[];
extern const char ethNameNText[];
extern const char ethNameOneAllyWPressAtAP6Text2[];
extern const char ethNameNeuroStimText[];
extern const char ethNameOneAllyWPressAtAP6Text3[];
extern const char ethNameAntidoteText[];
extern const char ethNameOneAllyWPressAtAP6Text4[];
extern const char ethNameCureAllText[];
extern const char ethNameOneAllyWPressAtAP6Text5[];
extern const char ethNameReviveDXText[];
extern const char ethNameOneAllyWPressAtAP6Text6[];
extern const char ethNameReviveText[];
extern const char ethNameOneAllyWPressAtAP6Text7[];
extern const char ethNameRejuvenatorText[];
extern const char ethNameOneAllyWPressAtAP6Text8[];
extern const char ethNameEtherPackMAXText[];
extern const char ethNameOneAllyWPressAtAP6Text9[];
extern const char ethNameEtherPackDXText[];
extern const char ethNameOneAllyWPressAtAP6Text10[];
extern const char ethNameEtherPackSText[];
extern const char ethNameOneAllyWPressAtAP6Text11[];
extern const char ethNameEtherPackText[];
extern const char ethNameOneEnemyAllyWPressAtText[];
extern const char ethNameMedKitMAXText[];
extern const char ethNameOneEnemyAllyWPressAtText2[];
extern const char ethNameMedKitDXText[];
extern const char ethNameOneEnemyAllyWPressAtText3[];
extern const char ethNameMedKitSText[];
extern const char ethNameOneEnemyAllyWPressAtText4[];
extern const char ethNameMedKitText[];
extern const char ethNamePreventsStopText[];
extern const char ethNameStopGuardText[];
extern const char ethNamePreventsConfusionText[];
extern const char ethNameConfusionGuardText[];
extern const char ethNameDisplaysEnemyHPInformationText[];
extern const char ethNameSearchEyesText[];
extern const char ethNameAutoBoostWhenAttackedCounter10Text[];
extern const char ethNameCBOnText[];
extern const char ethNameIncreasesEtherDefAsAlliesAreText[];
extern const char ethNameLonelyEDUPText[];
extern const char ethNameIncreasesPhysDefAsAlliesAreText[];
extern const char ethNameLonelyPDUPText[];
extern const char ethNameEtherDef42xEffectWhenText[];
extern const char ethNameEDEF4Text[];
extern const char ethNamePhysDef42xEffectWhenText[];
extern const char ethNamePDEF4Text[];
extern const char ethNameReducesSlshPrceTypeDamage20Format[];
extern const char ethNameSword20Text[];
extern const char ethNameReducesBTypeDamage202xFormat[];
extern const char ethNameBeam20Text[];
extern const char ethNameReducesITypeDamage202xFormat[];
extern const char ethNameIce20Text[];
extern const char ethNameReducesLTypeDamage202xFormat[];
extern const char ethNameLightning20Text[];
extern const char ethNameReducesFTypeDamage202xFormat[];
extern const char ethNameFire20Text[];
extern const char ethNameBoost1AtStartOfBattleText[];
extern const char ethNameBattleBC1Text[];
extern const char ethNameStrIncreasesAsHPDecreasesText[];
extern const char ethNameHPStrongText[];
extern const char ethNameIncreaseSPtsEarned252xFormat[];
extern const char ethNameSkillP25Text[];
extern const char ethNameIncreasesEPtsEarned252xFormat[];
extern const char ethNameEtherP25Text[];
extern const char ethNameIncreasesTPtsEarned252xFormat[];
extern const char ethNameTechP25Text[];
extern const char ethNameIncreasesExperiencePoints252xEffectFormat[];
extern const char ethNameExperienceP25Text[];
extern const char ethNameHISlotTechAttackIncreasesInText[];
extern const char ethNameTechAttackACText[];
extern const char ethNameHISlotTechAttackEP0Text[];
extern const char ethNameTechAttackAEText[];
extern const char ethNameWText[];
extern const char ethName2xTechAttackPossibleText[];
extern const char ethNameWSpecialText[];
extern const char ethNameFluctuatingDamageDependingOnHPText[];
extern const char ethNameDamageUDText[];
extern const char ethNameEachTurnAP1CharacterOnlyText[];
extern const char ethNameAP1Text[];
extern const char ethNameIncreasesRareItemAcquisitionRate30Format[];
extern const char ethNameRare30Text[];
extern const char ethNameIncreasesRareItemAcquisitionRate10Format[];
extern const char ethNameRare10Text[];
extern const char ethNameIncreasesMaxEP152xEffectFormat[];
extern const char ethNameEPMAX15Text[];
extern const char ethNameIncreasesMaxHP152xEffectFormat[];
extern const char ethNameHPMAX15Text[];
extern const char ethNameReducesDamage102xEffectWhenFormat[];
extern const char ethNameDamage10Text[];
extern const char ethNameIncreasesCounterRate102xEffectFormat[];
extern const char ethNameCounter10Text[];
extern const char ethNameIncreasesCriticalHitRate52xFormat[];
extern const char ethNameCRTC5Text[];
extern const char ethNameAgility12xEffectWhenCombinedText[];
extern const char ethNameAGL1Text[];
extern const char ethNameEtherDef22xEffectWhenText[];
extern const char ethNameEDEF2Text[];
extern const char ethNameEtherAtk22xEffectWhenText[];
extern const char ethNameEATK2Text[];
extern const char ethNameEvade22xEffectWhenCombinedText[];
extern const char ethNameEVA2Text[];
extern const char ethNameDexterity22xEffectWhenCombinedText[];
extern const char ethNameDEX2Text[];
extern const char ethNamePhysDef22xEffectWhenText[];
extern const char ethNamePDEF2Text[];
extern const char ethNamePhysAtk22xEffectWhenText[];
extern const char ethNamePATK2Text[];
extern const char ethNameAntiVeilSupportEffectText[];
extern const char ethNameEtherPowerUText[];
extern const char ethNameVeilSupportEffectText[];
extern const char ethNameEtherPowerDText[];
extern const char ethNameRecoveryAmountX2Text[];
extern const char ethNameRecoveryDoubleText[];
extern const char ethNameStatusEffectTimeX2Text[];
extern const char ethNameSTDoubleText[];
extern const char ethNameEPConsumption12Text[];
extern const char ethNameEPHalfText[];
extern const char ethNameIncreasesSpeed252xEffectWhenFormat[];
extern const char ethNameSpeed25Text[];
extern const char ethNamePreventsEPDamageText[];
extern const char ethNameEPGuardText[];
extern const char ethNamePreventsAllStatusAbnormalitiesSupportEffectsText[];
extern const char ethNameAllGuardText[];
extern const char ethNamePreventsAttackDisableText[];
extern const char ethNameSpecialGuardText[];
extern const char ethNamePreventsInstantKOText[];
extern const char ethNameKOGuardText[];
extern const char ethNamePreventsPoisonText[];
extern const char ethNamePoisonGuardText[];
extern const char ethNamePreventsSlowText[];
extern const char ethNameSlowGuardText[];
extern const char ethNamePreventsAPHalfText[];
extern const char ethNameAPGuardText[];
extern const char ethNamePreventsEtherAtkDownText[];
extern const char ethNameEATKGuardText[];
extern const char ethNamePreventsLostText[];
extern const char ethNameLostGuardText[];
extern const char ethNamePreventsSleepText[];
extern const char ethNameSleepGuardText[];
extern const char ethNamePreventsPhysDefDownText[];
extern const char ethNamePDEFGuardText[];
extern const char ethNamePreventsPhysAtkDownText[];
extern const char ethNamePATKGuardText[];
extern const char ethNamePreventsEvadeDownText[];
extern const char ethNameEVAGuardText[];
extern const char ethNamePreventsDexterityDownText[];
extern const char ethNameDEXGuardText[];
extern const char ethNamePreventsEPOverconsumptionText[];
extern const char ethNameEPDoubleGuardText[];
extern const char ethNamePreventsBindText[];
extern const char ethNameBindGuardText[];
extern const char ethNamePreventsAttackPoisonText[];
extern const char ethNameAPoisonGuardText[];
extern const char ethNamePreventsCriticalHitText[];
extern const char ethNameCRTCGuardText[];
extern const char ethNameReducesBTypeDamage25Format[];
extern const char ethNameAntiBeamArmorText[];
extern const char ethNameReducesLTypeDamage25Format[];
extern const char ethNameAntiLightningArmorText[];
extern const char ethNameReducesITypeDamage25Format[];
extern const char ethNameAntiIceArmorText[];
extern const char ethNameReducesFTypeDamage25Format[];
extern const char ethNameAntiFireArmorText[];
extern const char ethNameEtherDef6Text[];
extern const char ethNameEFCircuitCText[];
extern const char ethNameEFCircuitBText[];
extern const char ethNameEFCircuitAText[];
extern const char ethNameArmor6Text[];
extern const char ethNameAuxiliaryArmorCText[];
extern const char ethNameArmor4Text[];
extern const char ethNameAuxiliaryArmorBText[];
extern const char ethNameArmor2Text[];
extern const char ethNameAuxiliaryArmorAText[];
extern const char ethNameRecovers10FMaxFHPWhenText[];
extern const char ethNameGuardRecoveryText[];
extern const char ethNameClearsStatusAbnormalitiesWhenGuardingText[];
extern const char ethNameGuardCleanerText[];
extern const char ethNameTunedCircuitText[];
extern const char ethNamePreventsPilotConfusionSleepText[];
extern const char ethNameCockpitGuardText[];
extern const char ethNameWCircuitShieldText[];
extern const char ethNameAPShieldText[];
extern const char ethNamePreventsWearText[];
extern const char ethNameMChipGuardText[];
extern const char ethNamePreventsEtherCrashText[];
extern const char ethNameECircuitShieldText[];
extern const char ethNameGearShieldText[];
extern const char ethNamePreventsEngineStopText[];
extern const char ethNameEngineShieldText[];
extern const char ethNamePreventsPowerLossText[];
extern const char ethNamePowerShieldText[];
extern const char ethNamePreventsDexterityDownEvadeDownText[];
extern const char ethNameScopeShieldText[];
extern const char ethNamePreventsArmorFailureText[];
extern const char ethNameArmorProtectUnitText[];
extern const char ethNameEnablesUseOfBoostText[];
extern const char ethNameBMAXCircuitText[];
extern const char ethNameIncreasesSpeed50Format[];
extern const char ethNameFastCircuit50Text[];
extern const char ethNameFastCircuit25Text[];
extern const char ethNameBoostPackText[];
extern const char ethNameAutoBoostWhenAttackedCounter10Text2[];
extern const char ethNameRevengePowerText[];
extern const char ethNameFieldRingText[];
extern const char ethNameBladeSoulText[];
extern const char ethNameScopeText[];
extern const char ethNameEtherDef4Text[];
extern const char ethNameOrangeRingSText[];
extern const char ethNameEtherAtk4Text[];
extern const char ethNamePurpleRingSText[];
extern const char ethNameEvade4Text[];
extern const char ethNameCobaltRingSText[];
extern const char ethNameDexterity4Text[];
extern const char ethNameBlueRingSText[];
extern const char ethNamePhysDef4Text[];
extern const char ethNameGreenRingSText[];
extern const char ethNamePhysAtk4Text[];
extern const char ethNameRedRingSText[];
extern const char ethNameAntiVeilSupportEffectText2[];
extern const char ethNameRedTopazText[];
extern const char ethNameBlueTopazText[];
extern const char ethNameIncreasesStrAsHPDecreasesText[];
extern const char ethNameBravesoulText[];
extern const char ethNameIncreasesExperiencePoints25Format[];
extern const char ethNameMasterSPendantText[];
extern const char ethNameLifeStoneText[];
extern const char ethNameGeminiClockText[];
extern const char ethNameAngelRingText[];
extern const char ethNameIncreasesHISlotTechAttackCriticalText[];
extern const char ethNameBattleMaskText[];
extern const char ethNameDoubleBusterText[];
extern const char ethNameIncreasesSpeed25Format[];
extern const char ethNameSpeedShoesText[];
extern const char ethNameGoldenDiceText[];
extern const char ethNameEachTurnAP1CharactersOnlyText[];
extern const char ethNameCommanderSCrestText[];
extern const char ethNameIncreasesRareItemAcquisitionRate30Format2[];
extern const char ethNameCatBurglarGlovesText[];
extern const char ethNameIncreasesRareItemAcquisitionRate10Format2[];
extern const char ethNameThiefRingText[];
extern const char ethNameIncreasesMaxEP15Format[];
extern const char ethNameSilverCrownText[];
extern const char ethNameYText[];
extern const char ethNameIncreasesMaxHP15Format[];
extern const char ethNameYamatoBeltText[];
extern const char ethNameDamageDown10Format[];
extern const char ethNameCrossText[];
extern const char ethNameIncreasesCounterRate1012Format[];
extern const char ethNameSamuraiHeartText[];
extern const char ethNameIncreasesCriticalHitRate5Format[];
extern const char ethNameHunterGoggleText[];
extern const char ethNameAgility1Text[];
extern const char ethNameWhiteRingText[];
extern const char ethNameOText[];
extern const char ethNameEtherDef2Text[];
extern const char ethNameOrangeRingText[];
extern const char ethNameEtherAtk2Text[];
extern const char ethNamePurpleRingText[];
extern const char ethNameEvade2Text[];
extern const char ethNameKoboldRingText[];
extern const char ethNameDexterity2Text[];
extern const char ethNameBlueRingText[];
extern const char ethNamePhysDef2Text[];
extern const char ethNameGreenRingText[];
extern const char ethNamePhysAtk2Text[];
extern const char ethNameRedRingText[];
extern const char ethNameChakraShieldText[];
extern const char ethNamePreventsAttackDisableText2[];
extern const char ethNameSoldierSHonorText[];
extern const char ethNamePreventsInstantKOText2[];
extern const char ethNameDefibrillatorVestText[];
extern const char ethNamePreventsPoisonText2[];
extern const char ethNameSnakeHunterText[];
extern const char ethNamePreventsSlowText2[];
extern const char ethNameClockShieldText[];
extern const char ethNamePreventsAPHalfText2[];
extern const char ethNameJadeMaskText[];
extern const char ethNamePreventsEtherAtkDownText2[];
extern const char ethNameSpiritPendantText[];
extern const char ethNamePreventsPhysDefDownText2[];
extern const char ethNameGuardPendantText[];
extern const char ethNamePreventsPhysAtkDownText2[];
extern const char ethNamePowerBraceText[];
extern const char ethNamePreventsEvadeDownText2[];
extern const char ethNameHyperShoesText[];
extern const char ethNamePreventsDexterityDownText2[];
extern const char ethNameSniperGogglesText[];
extern const char ethNamePreventsEPOverconsumptionText2[];
extern const char ethNameRuneCrystalText[];
extern const char ethNamePreventsCurseText[];
extern const char ethNameWoodenIdolText[];
extern const char ethNameVenomBlockText[];
extern const char ethNamePreventsCriticalHitsText[];
extern const char ethNameTraumaPlateText[];
extern const char ethNameAuxiliaryArmorForArmsPhysDefText[];
extern const char ethNameProtectorText[];
extern const char ethNameIncreasesEPtsEarned25PhysFormat[];
extern const char ethNameStarHatText[];
extern const char ethNamePreventsLostPhysDef4Text[];
extern const char ethNameMagicalHatText[];
extern const char ethNamePreventsSleepPhysDef4Text[];
extern const char ethNameNightwalkerText[];
extern const char ethNameHatReserveText[];
extern const char ethNameMatchesTechtronClothesOutfitPhysDefText[];
extern const char ethNameTechtronHelmetText[];
extern const char ethNameMatchesRubySuitOutfitPhysDefText[];
extern const char ethNameRubyHelmetText[];
extern const char ethNameSpaceUseHelmetPhysDef4Text[];
extern const char ethNameSpaceHelmetText[];
extern const char ethNameMatchesSurvivalWearOutfitPhysDefText[];
extern const char ethNameCowboyHatText[];
extern const char ethNameMatchesMetalWearOutfitPhysDefText[];
extern const char ethNameMetalHelmetText[];
extern const char ethNameIncreasesTPtsEarned25PhysFormat[];
extern const char ethNameSwimsuitText[];
extern const char ethNameSpecialClothingReserveText[];
extern const char ethNameIncreasesEtherDefAsAlliesAreText2[];
extern const char ethNameSpiritText[];
extern const char ethNameIncreasesPhysDefAsAlliesAreText2[];
extern const char ethNameSoulText[];
extern const char ethNameReducesSlshPrceTypeDamage20Format2[];
extern const char ethNameCoatSwordText[];
extern const char ethNameReducesBTypeDamage20PhysFormat[];
extern const char ethNameCoatBeamText[];
extern const char ethNameReducesITypeDamage20PhysFormat[];
extern const char ethNameCoatIceText[];
extern const char ethNameReducesLTypeDamage20PhysFormat[];
extern const char ethNameCoatLightningText[];
extern const char ethNameReducesFTypeDamage20PhysFormat[];
extern const char ethNameCoatFireText[];
extern const char ethNameIncreasesSPtsEarned25PhysFormat[];
extern const char ethNameCraftApronText[];
extern const char ethNamePreventsAllStatusEffectsAbnormalitiesPhysText[];
extern const char ethNameShieldArmorText[];
extern const char ethNameCyborgArmorUnitPhysDef44Text[];
extern const char ethNameVOLG50Text[];
extern const char ethNameCyborgArmorUnitPhysDef36Text[];
extern const char ethNameVOLG40Text[];
extern const char ethNameCyborgArmorUnitPhysDef28Text[];
extern const char ethNameVOLG30Text[];
extern const char ethNameCyborgArmorUnitPhysDef16Text[];
extern const char ethNameVOLG20Text[];
extern const char ethNameCyborgArmorUnitPhysDef3Text[];
extern const char ethNameVOLG10Text[];
extern const char ethNameClothingMadeForAdventureEnthusiastsPhysText[];
extern const char ethNameSurvivalWearText[];
extern const char ethNameJpText[];
extern const char ethNameRegularClothingReserveText[];
extern const char ethNameKOSMOSArmorUnitPhysDefText[];
extern const char ethNameDUnitV6Text[];
extern const char ethNameKOSMOSArmorUnitPhysDefText2[];
extern const char ethNameDUnitV5Text[];
extern const char ethNameKOSMOSArmorUnitPhysDefText3[];
extern const char ethNameDUnitV4Text[];
extern const char ethNameKOSMOSArmorUnitPhysDefText4[];
extern const char ethNameDUnitV3Text[];
extern const char ethNameKOSMOSArmorUnitPhysDefText5[];
extern const char ethNameDUnitV2Text[];
extern const char ethNameKOSMOSArmorUnitPhysDefText6[];
extern const char ethNameDUnitV1Text[];
extern const char ethNameTheLatestTopOfTheLineText[];
extern const char ethNameTechtronClothesText[];
extern const char ethNameReinforcedClothingThatGeneratesADefensiveText[];
extern const char ethNameRubySuitText[];
extern const char ethNameHasEDEF4SkillPhysDefText[];
extern const char ethNameNeoArmorText[];
extern const char ethNameHasPDEF4SkillPhysDefText[];
extern const char ethNameNeoArmorText2[];
extern const char ethNameReinforcedClothingMadeForBattlesInText[];
extern const char ethNameBattleGearText[];
extern const char ethNameReinforcedClothingThatAutoAdjustsToText[];
extern const char ethNameStylishArmorText[];
extern const char ethNameClothingWovenOutOfHighStrengthText[];
extern const char ethNameFiberSuitText[];
extern const char ethNameClothingWovenWithSpecialAlloyThreadsText[];
extern const char ethNameMetalWearText[];
extern const char ethNameUniformForFemaleVectorEmployeesPhysText[];
extern const char ethNameUniformText[];
extern const char ethNameAllAirPierceFederationArmyHandText[];
extern const char ethNameAllAlliesEtherShieldDefensiveShieldText[];
extern const char ethNameDEFVXText[];
extern const char ethNameAllAirBeamEA300LanceManufacturedFormat[];
extern const char ethNameLWVX2Text[];
extern const char ethNameSingleNearSlashWACTElectroText[];
extern const char ethNameERVXText[];
extern const char ethNameAllAirHitFederationArmyMissileText[];
extern const char ethNameSMP53AGText[];
extern const char ethNameAllAirSECMPodManufacturedText[];
extern const char ethNameECM2VXText[];
extern const char ethNameSingleLineSECMPodManufacturedText[];
extern const char ethNameECM1VXText[];
extern const char ethNameAllAirBeamEA250FederationArmyFormat[];
extern const char ethNameAIRDAG2Text[];
extern const char ethNameSingleLineBeamEA600FederationArmyFormat[];
extern const char ethNameBBCAG5Text[];
extern const char ethNameDefenseShieldManufacturedByVectorText[];
extern const char ethNameSHD12VXText[];
extern const char ethNameDefenseFederationArmyShieldText[];
extern const char ethNameSHD02AGText[];
extern const char ethNameSingleAirBeamEA130DefenseFederationFormat[];
extern const char ethNameSHB67AGText[];
extern const char ethNameAllAirHitFederationArmyLargeText[];
extern const char ethNameBMPAG5Text[];
extern const char ethNameSingleLinePierceFederationArmyLongText[];
extern const char ethNameLCAG5Text[];
extern const char ethNameAllAirBeamEA130WideBeamFormat[];
extern const char ethNameBMP45VXText[];
extern const char ethNameAIRCAG2Text[];
extern const char ethNameEnemiesAllAlliesAirSChaffText[];
extern const char ethNameCB85VXText[];
extern const char ethNameSingleLineBeamEA200FederationArmyFormat[];
extern const char ethNameBL24AGText[];
extern const char ethNameHMPAG5Text[];
extern const char ethNameAllAirHitFederationArmyHandText[];
extern const char ethNameHMP33AGText[];
extern const char ethNameHGGAG5Text[];
extern const char ethNameSingleLinePierceWACTFederationText[];
extern const char ethNameGLG76AGText[];
extern const char ethNameSingleLineFireWACTFederationText[];
extern const char ethNameGRD20AGText[];
extern const char ethNameSingleLineFireEA150WACTFormat[];
extern const char ethNameFLM64AGText[];
extern const char ethNameSingleNearPierceFederationArmyBunkerText[];
extern const char ethNamePB55AGText[];
extern const char ethNameSingleNearSlashWACTLancerText[];
extern const char ethNameLM11VXText[];
extern const char ethNameSingleNearSlashBeamWACTText[];
extern const char ethNameBSW13AGText[];
extern const char ethNameSingleLineBeamEA185WACTFormat[];
extern const char ethNameBA15VXText[];
extern const char ethNameSingleLineBeamEA300LongBeamFormat[];
extern const char ethNameLG100VXText[];
extern const char ethNameSingleLineBeamEA120WACTFormat[];
extern const char ethNameLG24VXText[];
extern const char ethNameSingleLinePierceWACTFederationText2[];
extern const char ethNameLG10AGText[];
extern const char ethNameSingleLinePierceWACTSubmachineText[];
extern const char ethNameSMG32VXText[];
extern const char ethNameSingleLinePierceWACTFederationText3[];
extern const char ethNameSMG99AGText[];
extern const char ethNameSingleNearHitFederationArmyHeavyText[];
extern const char ethNameHMRAG5Text[];
extern const char ethNameSingleNearSlashWACTFederationText[];
extern const char ethNameWCT02AG4Text[];
extern const char ethNameSingleNearHitWACTFederationText[];
extern const char ethNameHMR55AGText[];
extern const char ethNameHG75VXText[];
extern const char ethNameSingleLinePierceWACTHandgunText[];
extern const char ethNameHG45VXText[];
extern const char ethNameSingleNearPierceWACTFederationText[];
extern const char ethNameDLCO2AG4Text[];
extern const char ethNameSingleNearSlashWACTVectorText[];
extern const char ethNameSWD34VXText[];
extern const char ethNameSingleNearSlashWACTFederationText2[];
extern const char ethNameSWD21AGText[];
extern const char ethNameSingleNearSlashWACTFederationText3[];
extern const char ethNameAXE11AGText[];
extern const char ethNameSingleNearHitText[];
extern const char ethNameHANDText[];
extern const char ethNameShionSpecialWeaponManufacturedByVectorText[];
extern const char ethNameMWSText[];
extern const char ethNameJrLegendaryGunHailedAsTheText[];
extern const char ethNameBLOOD9Text[];
extern const char ethNameJrAnOriginalGunMadeByText[];
extern const char ethNameCROSSText[];
extern const char ethNameJrSecondGenerationGunBySaifarText[];
extern const char ethNameSAIFAR45Text[];
extern const char ethNameJrAnHeirloomQualityAntiqueGunText[];
extern const char ethNameMAKAROVText[];
extern const char ethNameJrStandardRookCoGunText[];
extern const char ethNameROOK505Text[];
extern const char ethNameAllLineHitZiggySMissileText[];
extern const char ethNameMSP89SXText[];
extern const char ethNameAllLineBeamEA135ZiggySFormat[];
extern const char ethNameBMP55SXText[];
extern const char ethNameMOMOSlowAgainstBGTypesText[];
extern const char ethNamePenguinRodText[];
extern const char ethNameMOMOScepterWithConcentratedEnergyWithinText[];
extern const char ethNameDragonRodText[];
extern const char ethNameMOMOAHeroSScepterMadeText[];
extern const char ethNameSaintRodText[];
extern const char ethNameMOMOConfusionAgainstBGTypesText[];
extern const char ethNameWHammerRodText[];
extern const char ethNameMOMOScepterMadeFromRareMaterialsText[];
extern const char ethNameMithrilRodText[];
extern const char ethNameMOMOLightPowerfulScepterPhysAtkText[];
extern const char ethNamePlatinumRodText[];
extern const char ethNameMOMOHardAndHeavyBattleScepterText[];
extern const char ethNameMetalRodText[];
extern const char ethNameMOMOTypicalSelfDefenseScepterPhysText[];
extern const char ethNameBattleRodText[];
extern const char ethNameAllLineNoneEA300ScytheThatFormat[];
extern const char ethNameFSCYTHEText[];
extern const char ethNameSingleLinePierceShortRailCannonText[];
extern const char ethNameFRSHOTText[];
extern const char ethNameSingleLineBeamEA120DragonSkullFormat[];
extern const char ethNameFBSHOTText[];
extern const char ethNameAllAirHitArmMissilePodText[];
extern const char ethNameFMSHOTText[];
extern const char ethNameSingleLinePierceTripleGatlingGunText[];
extern const char ethNameFGSHOTText[];
extern const char ethNameChaosWornOutGlovesWithHolesText[];
extern const char ethNameHoleyGlovesText[];
extern const char ethNameChaosGlovesForDoItYourselfersText[];
extern const char ethNameWorkGlovesText[];
extern const char ethNameChaosTypicalRidingGlovesForWinterText[];
extern const char ethNameRiderGlovesText[];
extern const char ethNameChaosMilitaryIssueGlovesMadeOfText[];
extern const char ethNameNavyGlovesText[];
extern const char ethNameChaosGlovesUsedWhenWorkingInText[];
extern const char ethNameSpaceGlovesText[];
extern const char ethNameReserve10Text[];
extern const char ethNameReserve9Text[];
extern const char ethNameReserve8Text[];
extern const char ethNameReserve7Text[];
extern const char ethNameReserve6Text[];
extern const char ethNameReserve5Text[];
extern const char ethNameReserve4Text[];
extern const char ethNameReserve3Text[];
extern const char ethNameReserve2Text[];
extern const char ethNameReserve1Text[];
extern const char ethNameLCAG5LongCannonAmmoText[];
extern const char ethNameLCB1Text[];
extern const char ethNameHGGAG5GatlingGunBulletsText[];
extern const char ethNameHGGB1Text[];
extern const char ethNameHMPAG5HandMissilePodAmmoText[];
extern const char ethNameHMPB1Text[];
extern const char ethNameBMPB1Text[];
extern const char ethNameECM2VXECMPodBulletsWearText[];
extern const char ethNameECM2B5Text[];
extern const char ethNameECM2VXECMPodBulletsSlowText[];
extern const char ethNameECM2B4Text[];
extern const char ethNameECM2VXECMPodBulletsDexterityText[];
extern const char ethNameECM2B3Text[];
extern const char ethNameECM2VXECMPodBulletsPhysText[];
extern const char ethNameECM2B2Text[];
extern const char ethNameECM2VXECMPodBulletsPhysText2[];
extern const char ethNameECM2B1Text[];
extern const char ethNameSMG32VXSubmachineGunBulletsText[];
extern const char ethNameSMG32B1Text[];
extern const char ethNameSMP53AGMissilePodAmmoText[];
extern const char ethNameSMP53B1Text[];
extern const char ethNameAXE11B1Text[];
extern const char ethNameGLG76AGGatlingGunBulletsText[];
extern const char ethNameGLG76B1Text[];
extern const char ethNameHG75VXHandgunBulletsText[];
extern const char ethNameHG75B1Text[];
extern const char ethNameECM1VXECMPodBulletsWearText[];
extern const char ethNameECM1B5Text[];
extern const char ethNameECM1VXECMPodBulletsSlowText[];
extern const char ethNameECM1B4Text[];
extern const char ethNameECM1VXECMPodBulletsDexterityText[];
extern const char ethNameECM1B3Text[];
extern const char ethNameECM1VXECMPodBulletsArmorText[];
extern const char ethNameECM1B2Text[];
extern const char ethNameECM1VXECMPodBulletsPowerText[];
extern const char ethNameECM1B1Text[];
extern const char ethNameCB85B5Text[];
extern const char ethNameCB85VXChaffBoxBulletsPhysDefText[];
extern const char ethNameCB85B4Text[];
extern const char ethNameCB85VXChaffBoxBulletsPhysAtkText[];
extern const char ethNameCB85B3Text[];
extern const char ethNameCB85VXChaffBoxBulletsEtherAtkText[];
extern const char ethNameCB85B2Text[];
extern const char ethNameCB85VXChaffBoxBulletsDexterityDownText[];
extern const char ethNameCB85B1Text[];
extern const char ethNameNoBulletsText[];
extern const char ethNameLG100B1Text[];
extern const char ethNameHMP33HandMissilePodAmmoText[];
extern const char ethNameHMP33B1Text[];
extern const char ethNameGRD20AGGrenadeAmmoText[];
extern const char ethNameGRD20B1Text[];
extern const char ethNameSMG99AGSubmachineGunBulletsText[];
extern const char ethNameSMG99B1Text[];
extern const char ethNameLG10AGRifleBulletsText[];
extern const char ethNameLG10B1Text[];
extern const char ethNameHG45VXHandgunBulletsText[];
extern const char ethNameHG45B1Text[];
extern const char ethNameJrBulletsForCROSSEtherAtkText[];
extern const char ethNameCS700UVLText[];
extern const char ethNameJrBulletsForCROSSPhysDefText[];
extern const char ethNameCS700DFDText[];
extern const char ethNameJrBulletsForCROSSPhysAtkText[];
extern const char ethNameCS700PWDText[];
extern const char ethNameJrBulletsForCROSSSlowAgainstText[];
extern const char ethNameCS700SLWText[];
extern const char ethNameJrBulletsForBLOOD9PhysAtkText[];
extern const char ethNameBD900Text[];
extern const char ethNameJrBulletsForCROSSPhysAtkText2[];
extern const char ethNameCS700Text[];
extern const char ethNameJrBulletsForSAIFARPhysAtkText[];
extern const char ethNameSR500Text[];
extern const char ethNameJrBulletsForROOK505PhysAtkText[];
extern const char ethNameRK280Text[];
extern const char ethNameJrValuableBulletsForMAKAROVGunText[];
extern const char ethNameMAKAROVBulletsText[];
extern const char ethNameJrBulletsForROOK505PhysAtkText2[];
extern const char ethNameRK200Text[];
extern const char ethNameMSS580Text[];
extern const char ethNameMSS560Text[];
extern const char ethNameZiggyMissilesForMSP89SXPhysAtkText[];
extern const char ethNameMSS540Text[];
extern const char ethNameMSS520Text[];
extern const char ethNameZiggyMissilesForMSP89SXPhysAtkText2[];
extern const char ethNameMSS500Text[];
extern const char ethNameKRS54VText[];
extern const char ethNameKMS38VText[];
extern const char ethNameKGS16VText[];
extern const char ethNameKMS36VText[];
extern const char ethNameKRS52VText[];
extern const char ethNameKMS34VText[];
extern const char ethNameKGS14VText[];
extern const char ethNameKOSMOSBulletsForFRSHOTText[];
extern const char ethNameKRS50VText[];
extern const char ethNameKMS32VText[];
extern const char ethNameKGS12VText[];
extern const char ethNameKOSMOSBulletsForFMSHOTText[];
extern const char ethNameKMS30VText[];
extern const char ethNameKOSMOSBulletsForFGSHOTText[];
extern const char ethNameKGS10VText[];
extern const char ethNameBXS015VText[];
extern const char ethNameBXS014VText[];
extern const char ethNameBXS013VText[];
extern const char ethNameBXS012VText[];
extern const char ethNameBXS011VText[];
extern const char ethNameBXS010VText[];
extern const char ethNameBXS009VText[];
extern const char ethNameBXS008VText[];
extern const char ethNameBXS007VText[];
extern const char ethNameBXS006VText[];
extern const char ethNameShionTechAttackShockBladePhysText[];
extern const char ethNameBXS005VText[];
extern const char ethNameShionTechAttackShockBladePhysText2[];
extern const char ethNameBXS004VText[];
extern const char ethNameShionTechAttackShockBladeArmorText[];
extern const char ethNameBXS003VText[];
extern const char ethNameShionTechAttackShockBladePowerText[];
extern const char ethNameBXS002VText[];
extern const char ethNameShionTechAttackShockBladeSlowText[];
extern const char ethNameBXS001VText[];
extern const char ethNameRushText[];
extern const char ethNameRifleShotText[];
extern const char ethNameHipShotText[];
extern const char ethNameCrossfireText[];
extern const char ethNameCoinSnapText[];
extern const char ethNameTrickShotText[];
extern const char ethNameSouthpawText[];
extern const char ethNameCrossShotText[];
extern const char ethNameCosmicFlipText[];
extern const char ethNameFlowerStormText[];
extern const char ethNameDreamWhirlText[];
extern const char ethNameTwinStarsText[];
extern const char ethNameMelodyRayText[];
extern const char ethNameStardustText[];
extern const char ethNameJackBladeText[];
extern const char ethNameLaserSwingText[];
extern const char ethNameIgnitionText[];
extern const char ethNameJackKnifeText[];
extern const char ethNameLaserBladeText[];
extern const char ethNameHighKickText[];
extern const char ethNameSpinKickText[];
extern const char ethNameCherryBombText[];
extern const char ethNameStunShockText[];
extern const char ethNamePowerKickText[];
extern const char ethNameFirecrackerText[];
extern const char ethNameKnuckleText[];
extern const char ethNameNEEDLEText[];
extern const char ethNameSWORDText[];
extern const char ethNameSSAULTText[];
extern const char ethNameBLASTERText[];
extern const char ethNamePUNCHText[];
extern const char ethNameStarThrustText[];
extern const char ethNameAngelShotText[];
extern const char ethNameElectroUpperText[];
extern const char ethNameSeraphimRushText[];
extern const char ethNameArrowText[];
extern const char ethNameTornadoSlashText[];
extern const char ethNameOneEnemyLineSpiritAGunText[];
extern const char ethNameSoulRhapsodyText[];
extern const char ethNameAllEnemiesAirPierceSnipeFromText[];
extern const char ethNameAngelicRequiemText[];
extern const char ethNameAllEnemiesAirSpiritAttackWithText[];
extern const char ethNameMysticNocturneText[];
extern const char ethNameOneEnemyLinePierceConcentratedHeavyText[];
extern const char ethNameLastSymphonyText[];
extern const char ethNameAllEnemiesLinePierceCoinShotText[];
extern const char ethNameStormWaltzText[];
extern const char ethNameOneEnemyLineSpiritAttackWithText[];
extern const char ethNameMoonlitSerenadeText[];
extern const char ethNameOneEnemyLinePierceCrossShotText[];
extern const char ethNamePreludeToBattleText[];
extern const char ethNameOneEnemyLineCanBeUsedText[];
extern const char ethNameMagicCasterText[];
extern const char ethNameOneEnemyLineBeamCanBeText[];
extern const char ethNameMOMOSKissText[];
extern const char ethNameOneEnemyNearHitTurnGText[];
extern const char ethNameDarkScepterText[];
extern const char ethNameAllEnemiesAirPierceAttackWithText[];
extern const char ethNameAngelArrowText[];
extern const char ethNameAllEnemiesAirHitStrikeWithText[];
extern const char ethNameStarCannonText[];
extern const char ethNameOneEnemyLineNoneAttackFromText[];
extern const char ethNameMeteorStormText[];
extern const char ethNameOneEnemyNearSlashSlashWithText[];
extern const char ethNameFloralTempestText[];
extern const char ethNameOneEnemyNearHitStrikeWithText[];
extern const char ethNameStarStrikeText[];
extern const char ethNameOneEnemyNearSlashFireFireText[];
extern const char ethNameHellFireText[];
extern const char ethNameAllEnemiesAirLightningLightningAttackText[];
extern const char ethNameExecutionerText[];
extern const char ethNameOneEnemyNearPierceACrossedText[];
extern const char ethNameCrossLancerText[];
extern const char ethNameOneEnemyNearSlashATornadoText[];
extern const char ethNameCycloneText[];
extern const char ethNameAllEnemiesAirFireFireAttackText[];
extern const char ethNameMeteorShotText[];
extern const char ethNameOneEnemyLineLightningLightningAttackText[];
extern const char ethNameLightningFistText[];
extern const char ethNameOneEnemyNearHitFireHundredText[];
extern const char ethNameCyberKickText[];
extern const char ethNameAllEnemiesAirBeamRainOfText[];
extern const char ethNameRainBladeText[];
extern const char ethNameOneEnemyNearSlashBeamSliceText[];
extern const char ethNameLunarBladeText[];
extern const char ethNameOneEnemyNearHitSSlowText[];
extern const char ethNameGravityWellText[];
extern const char ethNameOneEnemyNearS1TimeText[];
extern const char ethNameEtherAmpText[];
extern const char ethNameOneEnemyNearHitFireViciousText[];
extern const char ethNameThermalBlastText[];
extern const char ethNameOneEnemyNearPierceSDifferentText[];
extern const char ethNameShockBladeText[];
extern const char ethNameOneEnemyLineBeamConcentratedEtherText[];
extern const char ethNameSpellRayText[];
extern const char ethNameOneEnemyNearHitLightningLightningText[];
extern const char ethNameLightningBlastText[];
extern const char ethNameOneEnemyNearHitAttackWithText[];
extern const char ethNameRDRAGONText[];
extern const char ethNameOneEnemyNearHitAttackWithText2[];
extern const char ethNameRHAMMERText[];
extern const char ethNameOneEnemyLineSMultipleStatusText[];
extern const char ethNameSCHAINText[];
extern const char ethNameXText[];
extern const char ethNameAllEnemiesLineBeamAbdominalSpreadText[];
extern const char ethNameXBUSTERText[];
extern const char ethNameOneEnemyNearPierceAttackWithText[];
extern const char ethNameRDRILLText[];
extern const char ethNameOneEnemyLineBeamAttackWithText[];
extern const char ethNameRCANNONText[];
extern const char ethNameOneEnemyNearSlashAttackWithText[];
extern const char ethNameRBLADEText[];
extern const char ethNameOneEnemyNearPierceSpiritOnlyText[];
extern const char ethNameDivineSpearText[];
extern const char ethNameAllEnemiesAirIceHurlDownText[];
extern const char ethNameArcticBlastText[];
extern const char ethNameOneEnemyLineSCurseAgainstText[];
extern const char ethNameDemonBanisherText[];
extern const char ethNameOneEnemyNearSpiritSPilotText[];
extern const char ethNameChainedBlastText[];
extern const char ethNameAllEnemiesAirSpiritGiantChiText[];
extern const char ethNameAngelBlowText[];
extern const char ethNameOneEnemyNearSlashLightningSlashText[];
extern const char ethNameHeavenSWrathText[];
extern const char ethNameOneEnemyLineSpiritAChiText[];
extern const char ethNameLunarSealText[];
extern const char ethNameOneEnemyNearHitSpiritImpactText[];
extern const char ethNameAngelWingsText[];
extern const char ethNameBOSS120Text[];
extern const char ethNameAlbedoText[];
extern const char ethNameBOSS032Text[];
extern const char ethNameBOSS031Text[];
extern const char ethNameMintiaText[];
extern const char ethNameGreatJoeText[];
extern const char ethNameBOSS028Text[];
extern const char ethNameBOSS027Text[];
extern const char ethNameSophiePeithosText[];
extern const char ethNameRatatoskText[];
extern const char ethNameJaldabaothText[];
extern const char ethNameBOSS023Text[];
extern const char ethNameBOSS022Text[];
extern const char ethNameSimeonText[];
extern const char ethNameMargulisText[];
extern const char ethNameDoppelwogelText[];
extern const char ethNameProtoDoraText[];
extern const char ethNameRianonSeText[];
extern const char ethNameGigasText[];
extern const char ethNameDragonText[];
extern const char ethNameEinRugelText[];
extern const char ethNameTiamatText[];
extern const char ethNameGargoyleText[];
extern const char ethNameStribogText[];
extern const char ethNamePerunText[];
extern const char ethNameSvarozicText[];
extern const char ethNameBOSS008Text[];
extern const char ethNameDroneGXText[];
extern const char ethNameBOSS006Text[];
extern const char ethNameDOMOCarrierText[];
extern const char ethNameGolemText[];
extern const char ethNameGremlinText[];
extern const char ethNameMinotaurText[];
extern const char ethNameCyclopsText[];
extern const char ethNameMr300000PointsText[];
extern const char ethNameMr100000PointsText[];
extern const char ethNameMr10000PointsText[];
extern const char ethNameMr1000PointsText[];
extern const char ethNameMr100PointsText[];
extern const char ethNameMONS035Text[];
extern const char ethNameMONS034Text[];
extern const char ethNameMONS033Text[];
extern const char ethNameMONS032Text[];
extern const char ethNameMONS031Text[];
extern const char ethNameMONS030Text[];
extern const char ethNameMONS029Text[];
extern const char ethNameMONS028Text[];
extern const char ethNameMONS027Text[];
extern const char ethNameMONS026Text[];
extern const char ethNameMONS025Text[];
extern const char ethNameMONS024Text[];
extern const char ethNameMONS023Text[];
extern const char ethNameMONS022Text[];
extern const char ethNameMONS021Text[];
extern const char ethNameMONS020Text[];
extern const char ethNameMONS019Text[];
extern const char ethNameMONS018Text[];
extern const char ethNameMONS017Text[];
extern const char ethNameMONS016Text[];
extern const char ethNameMONS015Text[];
extern const char ethNameMONS014Text[];
extern const char ethNameMONS013Text[];
extern const char ethNameMONS012Text[];
extern const char ethNameMONS011Text[];
extern const char ethNameMONS010Text[];
extern const char ethNameMONS009Text[];
extern const char ethNameMONS008Text[];
extern const char ethNameDOMOText[];
extern const char ethNameAcePilotText[];
extern const char ethNameUTICSoldierXText[];
extern const char ethNameEinzatsText[];
extern const char ethNameDirlewangerText[];
extern const char ethNameUTICSoldierText[];
extern const char ethNameXanthosisText[];
extern const char ethNameIosysText[];
extern const char ethNameAmbixText[];
extern const char ethNameZolfoText[];
extern const char ethNameMercurioText[];
extern const char ethNameUTRE006Text[];
extern const char ethNameAthra26SeriesText[];
extern const char ethNameByproduct172Text[];
extern const char ethNameByproduct145Text[];
extern const char ethNameByproduct103Text[];
extern const char ethNameUTRE001Text[];
extern const char ethNameUTMA014Text[];
extern const char ethNameDinGarethText[];
extern const char ethNameSchutzText[];
extern const char ethNameKubelText[];
extern const char ethNameUTMA010Text[];
extern const char ethNameGertzogUTText[];
extern const char ethNameCaptoMortumText[];
extern const char ethNameWorkDroidText[];
extern const char ethNameGardisF10Text[];
extern const char ethNameGardisM1Text[];
extern const char ethNameShotCrabText[];
extern const char ethNameCyberCrabText[];
extern const char ethNameDOMOBText[];
extern const char ethNameDOMOAText[];
extern const char ethNameFedSoldierText[];
extern const char ethNameDroneFText[];
extern const char ethNameDroneMText[];
extern const char ethNameDroneSPXText[];
extern const char ethNameFERE003Text[];
extern const char ethNameViveText[];
extern const char ethNameCalxText[];
extern const char ethNameFEMA006Text[];
extern const char ethNameMeldGarethText[];
extern const char ethNameFEMA004Text[];
extern const char ethNameFEMA003Text[];
extern const char ethNameGertzogFEText[];
extern const char ethNameAttackDroneText[];
extern const char ethNameOudogogueText[];
extern const char ethNameDroneG3Text[];
extern const char ethNameDroneG2Text[];
extern const char ethNameDroneG1Text[];
extern const char ethNameBaraqijalText[];
extern const char ethNameArmarosText[];
extern const char ethNameAzazelText[];
extern const char ethNameDelphyneText[];
extern const char ethNameBasiliskText[];
extern const char ethNameBugbearText[];
extern const char ethNameWyrmText[];
extern const char ethNameUnicornText[];
extern const char ethNameLizardmanText[];
extern const char ethNameHydraText[];
extern const char ethNameDemonText[];
extern const char ethNameGelFishText[];
extern const char ethNameCerberusText[];
extern const char ethNameLarvaFaceText[];
extern const char ethNameLarvaDollText[];
extern const char ethNameFairyText[];
extern const char ethNameTrollText[];
extern const char ethNameOgreText[];
extern const char ethNameKoboldText[];
extern const char ethNameSkyFishText[];
extern const char ethNameManticoreText[];
extern const char ethNameGoblinText[];
extern const char ethNameDebugsterText[];
extern const char ethNameBunnieText[];
extern const char ethNameAG06Text[];
extern const char ethNameAG05Text[];
extern const char ethNameAG04Text[];
extern const char ethNameAG03Text[];
extern const char ethNameAG02Text[];
extern const char ethNameAG01Text[];
extern const char ethNameVX06Text[];
extern const char ethNameVX05Text[];
extern const char ethNameVX04Text[];
extern const char ethNameVX03Text[];
extern const char ethNameVX02Text[];
extern const char ethNameVX01Text[];
extern const char ethNameMaryText[];
extern const char ethNameVirgilText[];
extern const char ethNameJrText[];
extern const char ethNameMOMOText[];
extern const char ethNameZiggurat8Text[];
extern const char ethNameShionText[];
extern const char ethNameKOSMOSText[];
extern const char ethNameChaosText[];
extern const char ethNameZiggyText[];
extern const char ethNameAG05G06Text[];
extern const char ethNameAG04G06Text[];
extern const char ethNameAG04G05Text[];
extern const char ethNameAG02G06Text[];
extern const char ethNameAG02G05Text[];
extern const char ethNameAG02G04Text[];
extern const char ethNameAG02G03Text[];
extern const char ethNameAGO2G02Text[];
extern const char ethNameVX06G06Text[];
extern const char ethNameVX06G05Text[];
extern const char ethNameVX4000Text[];
extern const char ethNameVX06G04Text[];
extern const char ethNameVX02G06Text[];
extern const char ethNameVX02G05Text[];
extern const char ethNameVX02G04Text[];
extern const char ethNameVX7000Text[];
extern const char ethNameVX02G03Text[];
extern const char ethNameVX01G06Text[];
extern const char ethNameVX01G05Text[];
extern const char ethNameVX01G04Text[];
extern const char ethNameVX01G03Text[];
extern const char ethNameVX01G02Text[];
extern const char ethNameVX10000Text[];
extern const char ethNameVX01G01Text[];
extern const char ethNameAG05F06Text[];
extern const char ethNameAG04F06Text[];
extern const char ethNameAG04F05Text[];
extern const char ethNameAG02F06Text[];
extern const char ethNameAG02F05Text[];
extern const char ethNameAG02F04Text[];
extern const char ethNameAG02F03Text[];
extern const char ethNameAG02F02Text[];
extern const char ethNameVX06F06Text[];
extern const char ethNameVX06F05Text[];
extern const char ethNameVX06F04Text[];
extern const char ethNameVX02F06Text[];
extern const char ethNameVX02F05Text[];
extern const char ethNameVX02F04Text[];
extern const char ethNameVX02F03Text[];
extern const char ethNameVX01F06Text[];
extern const char ethNameVX01F05Text[];
extern const char ethNameVX01F04Text[];
extern const char ethNameVX01F03Text[];
extern const char ethNameVX01F02Text[];
extern const char ethNameVX01F01Text[];
extern const char ethNameReverseText[];
extern const char ethNameHPHalfText[];
extern const char ethNameAPHalfText[];
extern const char ethNameAttackDisabledText[];
extern const char ethNamePoisonText[];
extern const char ethNameDexterityDownText[];
extern const char ethNameStopText[];
extern const char ethNameSlowText[];
extern const char ethNamePhysAtkDownText[];
extern const char ethNameEvadeDownText[];
extern const char ethNamePhysDefDownText[];
extern const char ethNameEtherBombText[];
extern const char ethNameCurseText[];
extern const char ethNameEPOverconsumptionText[];
extern const char ethNameEtherPoisonText[];
extern const char ethNameConfusionText[];
extern const char ethNameSleepText[];
extern const char ethNameEtherAtkDownText[];
extern const char ethNameLostText[];
extern const char ethNameBlockBoostText[];
extern const char ethNameAttackPoisonText[];
extern const char ethNameFocusPhysAtkText[];
extern const char ethNameSTLockText[];
extern const char ethNameSpeed50Text[];
extern const char ethNameSafetyText[];
extern const char ethNameAutoReviveText[];
extern const char ethNamePhysDefUpText[];
extern const char ethNameAPMAXText[];
extern const char ethNameEvadeUpText[];
extern const char ethNameDexterityUpText[];
extern const char ethNameSpeed25Text2[];
extern const char ethNamePhysAtkUpText[];
extern const char ethNameFocusPhysDefText[];
extern const char ethNameFocusEtherAtkText[];
extern const char ethNameFocusEtherDefText[];
extern const char ethNameMOMOGuardText[];
extern const char ethNameRecoveryX2Text[];
extern const char ethNameChivalryText[];
extern const char ethNameEPX2Text[];
extern const char ethNameEngineStopText[];
extern const char ethNamePowerLossText[];
extern const char ethNameCRTCMarkText[];
extern const char ethNameArmorFailureText[];
extern const char ethNameWearText[];
extern const char ethNameEtherCrashText[];
extern const char ethNamePilotKOText[];
extern const char ethNamePilotConfusionText[];
extern const char ethNamePilotSleepText[];
extern const char ethNameEtherShieldText[];

static const char * aa1[13];
static const char * aa2[3];
static const char * ab[4];
static const char * ba[14];
static const char * bb[12];
static const char * ma[11];
static const char * mb[8];

extern int printf(const char *format, ...);
extern int MenuScenarioNoGet(void);
const char ethNameDataEthnameGetErrText[] = "** dataEthnameGet: err %d\n";

static EthName ethNameTbl[79] = {
    {ethNameMedicaText, {ethNameOneEnemyAllyHPRecoveryLowText, ethNameMText}},
    {ethNameAnalyzeText, {ethNameOneEnemyAnalyzeItemsAndHPText, ethNameAText}},
    {ethNameGoodbyeText, {ethNameAllyEscapeFromBattleText, ethNameGText}},
    {ethNameMedicaAllText, {ethNameAllAlliesHPRecoveryMidText, ethNameMText}},
    {ethNameRefreshText, {ethNameOneAllyClearAllStatusChangesText, ethNameRText}},
    {ethNameBoost1Text, {ethNameOneAllyIncreaseBoostBy1Text, ethNameBText}},
    {ethNameJammingText, {ethNameOneEnemyDexterityDownForMText, ethNameJText}},
    {ethNameMedicaRestText, {ethNameOneEnemyAllyHPRecoveryMaxText, ethNameMText}},
    {ethNameRevertText, {ethNameOneAllyReviveHPRecoveryMidText, ethNameRText}},
    {ethNameSafetyLevelText, {ethName1xPerAllySurviveWith1Text, ethNameSText}},
    {ethNameQuickText, {ethNameOneAllyIncreaseSpeed25Format, ethNameQText}},
    {ethNameQueenSKissText, {ethNameOneEnemyOccasionallyInstantKOStealText, ethNameQText}},
    {ethNameSeraphimBirdText, {ethNameAllEnemiesBeam1xPerBattleText2, ethNameSText}},
    {ethNameThroniBladeText, {ethNameAllEnemiesSlash1xPerBattleText2, ethNameTText}},
    {ethNameDominionTankText, {ethNameAllEnemiesHit1xPerBattleText, ethNameDText}},
    {ethNameErdeKaiserText, {ethNameAllEnemiesSlash1xPerBattleText, ethNameEText}},
    {ethNameModeA7Text, {ethNameSelfFocusPhysAtkText, ethNameMText}},
    {ethNameDownDexText, {ethNameOneEnemyDexterityDownForBText, ethNameDText}},
    {ethNameDownForceText, {ethNameOneEnemyPhysAtkDownForText, ethNameDText}},
    {ethNameDownEtherText, {ethNameOneEnemyEtherDownForBText, ethNameDText}},
    {ethNameGateText, {ethNameOneEnemyOccasionallyInstantKOHPText, ethNameGText}},
    {ethNameEtherLimitText, {ethNameSelfEtherAtkDamageAndCostText, ethNameEText}},
    {ethNameSatelliteText, {ethNameAllEnemiesBeam1xPerBattleText, ethNameSText}},
    {ethNameDexEtherRaText, {ethNameAllEnemiesIceIceEtherAtkText, ethNameDText}},
    {ethNameDexEtherFaText, {ethNameAllEnemiesFireFireEtherAtkText, ethNameDText}},
    {ethNameDexEtherLeText, {ethNameAllEnemiesLightningLightningEtherAtkText, ethNameDText}},
    {ethNameProtectiveWearText, {ethNameOneEnemyAllyEtherEffectDownFormat, ethNamePText}},
    {ethNameHealingDewText, {ethNameAllAlliesHPRecoveryLowText, ethNameHText}},
    {ethNameIceWingsText, {ethNameOneEnemyIceIceEtherAtkText, ethNameIText}},
    {ethNamePurifyingStormText, {ethNameAllAlliesClearAllStatusChangesText, ethNamePText}},
    {ethNameFlameWingsText, {ethNameOneEnemyFireFireEtherAtkText, ethNameFText}},
    {ethNameLightningWingsText, {ethNameOneEnemyLightningLightningEtherAtkText, ethNameLText}},
    {ethNameBestAllyText, {ethNameAllAllies1xPerBattleReviveText, ethNameBText}},
    {ethNameSupremeJudgmentText, {ethNameOneEnemyHPRecoveryDown1Text, ethNameSText}},
    {ethNameCataclysmText, {ethNameAllEnemiesResetAttackWaitTimeText, ethNameCText}},
    {ethNameLightWingsText, {ethNameAllEnemiesAttackClearAllStText, ethNameLText}},
    {ethNameLifeShotText, {ethNameOneEnemyAllyHPRecoveryMidText, ethNameLText}},
    {ethNameMiracleStarText, {ethNameOneEnemyNonElementalEtherAtkText, ethNameMText}},
    {ethNameSheepBeamText, {ethNameOneEnemyPutMEnemyPilotsText, ethNameSText}},
    {ethNamePrayerText, {ethNameSelfAHeavenlyGiftText, ethNamePText}},
    {ethNameRefineText, {ethNameOneAllyReviveHPRecoveryLowText, ethNameRText}},
    {ethNameJunkBeamText, {ethNameOneEnemyReduceMEnemyActionFormat, ethNameJText}},
    {ethNameStarWindText, {ethNameSelf1xPerBattleUseStarText, ethNameSText}},
    {ethNameStarVeilText, {ethNameAllAlliesOnlyDuringStarWindFormat, ethNameSText}},
    {ethNameStarActionText, {ethNameOneAlly1xPerStarWindText, ethNameSText}},
    {ethNameStarlightText, {ethNameSelf1xPerBattleUseStarlightText, ethNameSText}},
    {ethNameStarBellText, {ethNameOneEnemyOnlyDuringStarlightNonText, ethNameSText}},
    {ethNameStarBunnieText, {ethNameAllEnemies1xPerStarlightNonText, ethNameSText}},
    {ethNamePsychoArmText, {ethNameSelfIncreasePhysAtkText, ethNamePText}},
    {ethNameEtherFlareText, {ethNameOneEnemyAllyIncreaseEtherEffectFormat, ethNameEText}},
    {ethNameDandyismText, {ethNameSelf1xOnlyReviveWhenFemaleText, ethNameDText}},
    {ethNamePsychoPocketText, {ethNameOneEnemyStealItemsText, ethNamePText}},
    {ethNameChainText, {ethNameOneEnemyEvadeDownForBText, ethNameCText}},
    {ethNameMistyText, {ethNameOneEnemyDisableBGEnemiesText, ethNameMText}},
    {ethNameCoinLockText, {ethNameOneAllyBlockStatChangesSupportText, ethNameCText}},
    {ethNameSpeedMachineText, {ethNameOneAllyIncreaseSpeed25Format, ethNameSText}},
    {ethNameMagnumJoeText, {ethNameOneEnemy1xPerBattleSummonText, ethNameMText}},
    {ethNameBusterJoeText, {ethNameAllEnemies1xPerBattleSummonText, ethNameBText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameMyGuardText, {ethNameSelfFocusPhysDefText, ethNameMText}},
    {ethNameBodyguardText, {ethNameSelfIncreaseStrDefWhenMOMOText, ethNameBText}},
    {ethNameRechargeText, {ethNameSelfHPRecoveryMidText, ethNameRText}},
    {ethNameEtherShiftAText, {ethNameSelfFocusEtherDefText, ethNameEText}},
    {ethNameEtherShiftBText, {ethNameSelfFocusEtherAtkText, ethNameEText}},
    {ethNameChargeXText, {ethNameSelf1xOnlyHPEPRecoveryText, ethNameCText}},
    {ethNameRedMarkText, {ethNameOneEnemy1xOnly80RiticalText, ethNameRText}},
    {ethNameSpeedBoostText, {ethNameSelfIncreaseSpeed50Format, ethNameBText}}
};

static EthName itmNameTbl[72] = {
    {ethNameMedKitText, {ethNameOneEnemyAllyWPressAtText4, ethNameMText}},
    {ethNameMedKitSText, {ethNameOneEnemyAllyWPressAtText3, ethNameMText}},
    {ethNameMedKitDXText, {ethNameOneEnemyAllyWPressAtText2, ethNameMText}},
    {ethNameMedKitMAXText, {ethNameOneEnemyAllyWPressAtText, ethNameMText}},
    {ethNameEtherPackText, {ethNameOneAllyWPressAtAP6Text11, ethNameEText}},
    {ethNameEtherPackSText, {ethNameOneAllyWPressAtAP6Text10, ethNameEText}},
    {ethNameEtherPackDXText, {ethNameOneAllyWPressAtAP6Text9, ethNameEText}},
    {ethNameEtherPackMAXText, {ethNameOneAllyWPressAtAP6Text8, ethNameEText}},
    {ethNameRejuvenatorText, {ethNameOneAllyWPressAtAP6Text7, ethNameRText}},
    {ethNameReviveText, {ethNameOneAllyWPressAtAP6Text6, ethNameRText}},
    {ethNameReviveDXText, {ethNameOneAllyWPressAtAP6Text5, ethNameRText}},
    {ethNameCureAllText, {ethNameOneAllyWPressAtAP6Text4, ethNameCText}},
    {ethNameAntidoteText, {ethNameOneAllyWPressAtAP6Text3, ethNameAText}},
    {ethNameNeuroStimText, {ethNameOneAllyWPressAtAP6Text2, ethNameNText}},
    {ethNameBoosterPackText, {ethNameOneAllyBoost1Text, ethNameBText}},
    {ethNameEscapePackText, {ethNameOneAllyEscapeFromBattleText, ethNameEText}},
    {ethNameStimText, {ethNameOneAllyWPressAtAP6Format4, ethNameSText}},
    {ethNameStimDXText, {ethNameOneAllyWPressAtAP6Format3, ethNameSText}},
    {ethNameHemlockText, {ethNameOneAllyHP1Text, ethNameHText}},
    {ethNameSpeedStimText, {ethNameOneAllyWPressAtAP6Format2, ethNameSText}},
    {ethNameSpeedStimSXText, {ethNameOneAllyIncreasesSpeed50Format, ethNameSText}},
    {ethNameDefenseShieldText, {ethNameOneAllyWPressAtAP6Text, ethNameDText}},
    {ethNameSpeedStimDXText, {ethNameOneAllyWPressAtAP6Format, ethNameSText}},
    {ethNameVeilText, {ethNameOneEnemyAllyWPressAtFormat, ethNameVText}},
    {ethNameAntiVeilText, {ethNameOneEnemyAllyWPressAtFormat, ethNameAText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameBioSphereText, {ethNameAllAlliesSavePointsOnlyHPText, ethNameBText}},
    {ethNameFrameRepairAText, {ethNameAGWSFHPRecoveryText2, ethNameFText}},
    {ethNameFrameRepairZText, {ethNameAGWSFHPRecoveryText, ethNameFText}},
    {ethNameScrapIronText, {ethNameBarterItemText, ethNameSText}},
    {ethNameJunkedCircuitAText, {ethNameBarterItemText, ethNameJText}},
    {ethNameSkillUpgradeText, {ethNameOneAllySkillLevel1Text, ethNameSText}},
    {ethNameTechUpgradeAText, {ethNameOneAllyTPts10Text, ethNameTText}},
    {ethNameTechUpgradeSText, {ethNameOneAllyTPts50Text, ethNameTText}},
    {ethNameTechUpgradeZText, {ethNameOneAllyTPts100Text, ethNameTText}},
    {ethNameEtherUpgradeAText, {ethNameOneAllyEPts10Text, ethNameEText}},
    {ethNameEtherUpgradeSText, {ethNameOneAllyEPts50Text, ethNameEText}},
    {ethNameEtherUpgradeZText, {ethNameOneAllyEPts100Text, ethNameEText}},
    {ethNameSkillUpgradeAText, {ethNameOneAllySPts10Text, ethNameSText}},
    {ethNameSkillUpgradeSText, {ethNameOneAllySPts50Text, ethNameSText}},
    {ethNameSkillUpgradeZText, {ethNameOneAllySPts100Text, ethNameSText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameUnicornHornText, {ethNameBarterItemText, ethNameUText}},
    {ethNameKoboldBladeText, {ethNameBarterItemText, ethNameKText}},
    {ethNamePreciousStoneText, {ethNameBarterItemText, ethNamePText}},
    {ethNameJunkedCircuitBText, {ethNameBarterItemText, ethNameJText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameGarnetText, {ethNameBarterItemText, ethNameGText}},
    {ethNameSapphireText, {ethNameBarterItemText, ethNameSText}},
    {ethNameRubyText, {ethNameBarterItemText, ethNameRText}},
    {ethNameEmeraldText, {ethNameBarterItemText, ethNameEText}},
    {ethNameDiamondText, {ethNameBarterItemText, ethNameDText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}}
};

static EthName sklNameTbl[67] = {
    {ethNameCRTCGuardText, {ethNamePreventsCriticalHitText, ethNameCText}},
    {ethNameAPoisonGuardText, {ethNamePreventsAttackPoisonText, ethNameAText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameBindGuardText, {ethNamePreventsBindText, ethNameBText}},
    {ethNameEPDoubleGuardText, {ethNamePreventsEPOverconsumptionText, ethNameEText}},
    {ethNameDEXGuardText, {ethNamePreventsDexterityDownText, ethNameDText}},
    {ethNameEVAGuardText, {ethNamePreventsEvadeDownText, ethNameEText}},
    {ethNamePATKGuardText, {ethNamePreventsPhysAtkDownText, ethNamePText}},
    {ethNamePDEFGuardText, {ethNamePreventsPhysDefDownText, ethNamePText}},
    {ethNameSleepGuardText, {ethNamePreventsSleepText, ethNameSText}},
    {ethNameLostGuardText, {ethNamePreventsLostText, ethNameLText}},
    {ethNameEATKGuardText, {ethNamePreventsEtherAtkDownText, ethNameEText}},
    {ethNameAPGuardText, {ethNamePreventsAPHalfText, ethNameAText}},
    {ethNameSlowGuardText, {ethNamePreventsSlowText, ethNameSText}},
    {ethNamePoisonGuardText, {ethNamePreventsPoisonText, ethNamePText}},
    {ethNameKOGuardText, {ethNamePreventsInstantKOText, ethNameKText}},
    {ethNameSpecialGuardText, {ethNamePreventsAttackDisableText, ethNameSText}},
    {ethNameAllGuardText, {ethNamePreventsAllStatusAbnormalitiesSupportEffectsText, ethNameAText}},
    {ethNameEPGuardText, {ethNamePreventsEPDamageText, ethNameEText}},
    {ethNameSpeed25Text, {ethNameIncreasesSpeed252xEffectWhenFormat, ethNameSText}},
    {ethNameEPHalfText, {ethNameEPConsumption12Text, ethNameEText}},
    {ethNameSTDoubleText, {ethNameStatusEffectTimeX2Text, ethNameSText}},
    {ethNameRecoveryDoubleText, {ethNameRecoveryAmountX2Text, ethNameRText}},
    {ethNameEtherPowerDText, {ethNameVeilSupportEffectText, ethNameEText}},
    {ethNameEtherPowerUText, {ethNameAntiVeilSupportEffectText, ethNameEText}},
    {ethNamePATK2Text, {ethNamePhysAtk22xEffectWhenText, ethNamePText}},
    {ethNamePDEF2Text, {ethNamePhysDef22xEffectWhenText, ethNamePText}},
    {ethNameDEX2Text, {ethNameDexterity22xEffectWhenCombinedText, ethNameDText}},
    {ethNameEVA2Text, {ethNameEvade22xEffectWhenCombinedText, ethNameEText}},
    {ethNameEATK2Text, {ethNameEtherAtk22xEffectWhenText, ethNameEText}},
    {ethNameEDEF2Text, {ethNameEtherDef22xEffectWhenText, ethNameEText}},
    {ethNameAGL1Text, {ethNameAgility12xEffectWhenCombinedText, ethNameAText}},
    {ethNameCRTC5Text, {ethNameIncreasesCriticalHitRate52xFormat, ethNameCText}},
    {ethNameCounter10Text, {ethNameIncreasesCounterRate102xEffectFormat, ethNameCText}},
    {ethNameDamage10Text, {ethNameReducesDamage102xEffectWhenFormat, ethNameDText}},
    {ethNameHPMAX15Text, {ethNameIncreasesMaxHP152xEffectFormat, ethNameHText}},
    {ethNameEPMAX15Text, {ethNameIncreasesMaxEP152xEffectFormat, ethNameEText}},
    {ethNameRare10Text, {ethNameIncreasesRareItemAcquisitionRate10Format, ethNameRText}},
    {ethNameRare30Text, {ethNameIncreasesRareItemAcquisitionRate30Format, ethNameRText}},
    {ethNameAP1Text, {ethNameEachTurnAP1CharacterOnlyText, ethNameAText}},
    {ethNameDamageUDText, {ethNameFluctuatingDamageDependingOnHPText, ethNameDText}},
    {ethNameWSpecialText, {ethName2xTechAttackPossibleText, ethNameWText}},
    {ethNameTechAttackAEText, {ethNameHISlotTechAttackEP0Text, ethNameTText}},
    {ethNameTechAttackACText, {ethNameHISlotTechAttackIncreasesInText, ethNameTText}},
    {ethNameExperienceP25Text, {ethNameIncreasesExperiencePoints252xEffectFormat, ethNameEText}},
    {ethNameTechP25Text, {ethNameIncreasesTPtsEarned252xFormat, ethNameTText}},
    {ethNameEtherP25Text, {ethNameIncreasesEPtsEarned252xFormat, ethNameEText}},
    {ethNameSkillP25Text, {ethNameIncreaseSPtsEarned252xFormat, ethNameSText}},
    {ethNameHPStrongText, {ethNameStrIncreasesAsHPDecreasesText, ethNameHText}},
    {ethNameBattleBC1Text, {ethNameBoost1AtStartOfBattleText, ethNameBText}},
    {ethNameFire20Text, {ethNameReducesFTypeDamage202xFormat, ethNameFText}},
    {ethNameLightning20Text, {ethNameReducesLTypeDamage202xFormat, ethNameLText}},
    {ethNameIce20Text, {ethNameReducesITypeDamage202xFormat, ethNameIText}},
    {ethNameBeam20Text, {ethNameReducesBTypeDamage202xFormat, ethNameBText}},
    {ethNameSword20Text, {ethNameReducesSlshPrceTypeDamage20Format, ethNameSText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNamePDEF4Text, {ethNamePhysDef42xEffectWhenText, ethNamePText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameEDEF4Text, {ethNameEtherDef42xEffectWhenText, ethNameEText}},
    {ethNameLonelyPDUPText, {ethNameIncreasesPhysDefAsAlliesAreText, ethNameLText}},
    {ethNameLonelyEDUPText, {ethNameIncreasesEtherDefAsAlliesAreText, ethNameLText}},
    {ethNameCBOnText, {ethNameAutoBoostWhenAttackedCounter10Text, ethNameCText}},
    {ethNameSearchEyesText, {ethNameDisplaysEnemyHPInformationText, ethNameSText}},
    {ethNameConfusionGuardText, {ethNamePreventsConfusionText, ethNameCText}},
    {ethNameStopGuardText, {ethNamePreventsStopText, ethNameSText}}
};

static EthName accNameTbl[180] = {
    {ethNameUniformText, {ethNameUniformForFemaleVectorEmployeesPhysText, ethNameUText}},
    {ethNameMetalWearText, {ethNameClothingWovenWithSpecialAlloyThreadsText, ethNameMText}},
    {ethNameFiberSuitText, {ethNameClothingWovenOutOfHighStrengthText, ethNameFText}},
    {ethNameStylishArmorText, {ethNameReinforcedClothingThatAutoAdjustsToText, ethNameSText}},
    {ethNameBattleGearText, {ethNameReinforcedClothingMadeForBattlesInText, ethNameBText}},
    {ethNameNeoArmorText2, {ethNameHasPDEF4SkillPhysDefText, ethNameNText}},
    {ethNameNeoArmorText, {ethNameHasEDEF4SkillPhysDefText, ethNameNText}},
    {ethNameRubySuitText, {ethNameReinforcedClothingThatGeneratesADefensiveText, ethNameRText}},
    {ethNameTechtronClothesText, {ethNameTheLatestTopOfTheLineText, ethNameTText}},
    {ethNameDUnitV1Text, {ethNameKOSMOSArmorUnitPhysDefText6, ethNameDText}},
    {ethNameDUnitV2Text, {ethNameKOSMOSArmorUnitPhysDefText5, ethNameDText}},
    {ethNameDUnitV3Text, {ethNameKOSMOSArmorUnitPhysDefText4, ethNameDText}},
    {ethNameDUnitV4Text, {ethNameKOSMOSArmorUnitPhysDefText3, ethNameDText}},
    {ethNameDUnitV5Text, {ethNameKOSMOSArmorUnitPhysDefText2, ethNameDText}},
    {ethNameDUnitV6Text, {ethNameKOSMOSArmorUnitPhysDefText, ethNameDText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameSurvivalWearText, {ethNameClothingMadeForAdventureEnthusiastsPhysText, ethNameSText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameVOLG10Text, {ethNameCyborgArmorUnitPhysDef3Text, ethNameVText}},
    {ethNameVOLG20Text, {ethNameCyborgArmorUnitPhysDef16Text, ethNameVText}},
    {ethNameVOLG30Text, {ethNameCyborgArmorUnitPhysDef28Text, ethNameVText}},
    {ethNameVOLG40Text, {ethNameCyborgArmorUnitPhysDef36Text, ethNameVText}},
    {ethNameVOLG50Text, {ethNameCyborgArmorUnitPhysDef44Text, ethNameVText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRegularClothingReserveText, {ethNameJpText, ethNameRText}},
    {ethNameShieldArmorText, {ethNamePreventsAllStatusEffectsAbnormalitiesPhysText, ethNameSText}},
    {ethNameCraftApronText, {ethNameIncreasesSPtsEarned25PhysFormat, ethNameCText}},
    {ethNameCoatFireText, {ethNameReducesFTypeDamage20PhysFormat, ethNameCText}},
    {ethNameCoatLightningText, {ethNameReducesLTypeDamage20PhysFormat, ethNameCText}},
    {ethNameCoatIceText, {ethNameReducesITypeDamage20PhysFormat, ethNameCText}},
    {ethNameCoatBeamText, {ethNameReducesBTypeDamage20PhysFormat, ethNameCText}},
    {ethNameCoatSwordText, {ethNameReducesSlshPrceTypeDamage20Format2, ethNameCText}},
    {ethNameSoulText, {ethNameIncreasesPhysDefAsAlliesAreText2, ethNameSText}},
    {ethNameSpiritText, {ethNameIncreasesEtherDefAsAlliesAreText2, ethNameSText}},
    {ethNameSpecialClothingReserveText, {ethNameJpText, ethNameSText}},
    {ethNameSpecialClothingReserveText, {ethNameJpText, ethNameSText}},
    {ethNameSpecialClothingReserveText, {ethNameJpText, ethNameSText}},
    {ethNameSpecialClothingReserveText, {ethNameJpText, ethNameSText}},
    {ethNameSwimsuitText, {ethNameIncreasesTPtsEarned25PhysFormat, ethNameSText}},
    {ethNameMetalHelmetText, {ethNameMatchesMetalWearOutfitPhysDefText, ethNameMText}},
    {ethNameCowboyHatText, {ethNameMatchesSurvivalWearOutfitPhysDefText, ethNameCText}},
    {ethNameSpaceHelmetText, {ethNameSpaceUseHelmetPhysDef4Text, ethNameSText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameRubyHelmetText, {ethNameMatchesRubySuitOutfitPhysDefText, ethNameRText}},
    {ethNameTechtronHelmetText, {ethNameMatchesTechtronClothesOutfitPhysDefText, ethNameTText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameNightwalkerText, {ethNamePreventsSleepPhysDef4Text, ethNameNText}},
    {ethNameMagicalHatText, {ethNamePreventsLostPhysDef4Text, ethNameMText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameStarHatText, {ethNameIncreasesEPtsEarned25PhysFormat, ethNameSText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameHatReserveText, {ethNameJpText, ethNameHText}},
    {ethNameProtectorText, {ethNameAuxiliaryArmorForArmsPhysDefText, ethNamePText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameTraumaPlateText, {ethNamePreventsCriticalHitsText, ethNameTText}},
    {ethNameVenomBlockText, {ethNamePreventsAttackPoisonText, ethNameVText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameWoodenIdolText, {ethNamePreventsCurseText, ethNameWText}},
    {ethNameRuneCrystalText, {ethNamePreventsEPOverconsumptionText2, ethNameRText}},
    {ethNameSniperGogglesText, {ethNamePreventsDexterityDownText2, ethNameSText}},
    {ethNameHyperShoesText, {ethNamePreventsEvadeDownText2, ethNameHText}},
    {ethNamePowerBraceText, {ethNamePreventsPhysAtkDownText2, ethNamePText}},
    {ethNameGuardPendantText, {ethNamePreventsPhysDefDownText2, ethNameGText}},
    {ethNameSpiritPendantText, {ethNamePreventsEtherAtkDownText2, ethNameSText}},
    {ethNameJadeMaskText, {ethNamePreventsAPHalfText2, ethNameJText}},
    {ethNameClockShieldText, {ethNamePreventsSlowText2, ethNameCText}},
    {ethNameSnakeHunterText, {ethNamePreventsPoisonText2, ethNameSText}},
    {ethNameDefibrillatorVestText, {ethNamePreventsInstantKOText2, ethNameDText}},
    {ethNameSoldierSHonorText, {ethNamePreventsAttackDisableText2, ethNameSText}},
    {ethNameChakraShieldText, {ethNamePreventsEPDamageText, ethNameCText}},
    {ethNameRedRingText, {ethNamePhysAtk2Text, ethNameRText}},
    {ethNameGreenRingText, {ethNamePhysDef2Text, ethNameGText}},
    {ethNameBlueRingText, {ethNameDexterity2Text, ethNameBText}},
    {ethNameKoboldRingText, {ethNameEvade2Text, ethNameKText}},
    {ethNamePurpleRingText, {ethNameEtherAtk2Text, ethNamePText}},
    {ethNameOrangeRingText, {ethNameEtherDef2Text, ethNameOText}},
    {ethNameWhiteRingText, {ethNameAgility1Text, ethNameWText}},
    {ethNameHunterGoggleText, {ethNameIncreasesCriticalHitRate5Format, ethNameHText}},
    {ethNameSamuraiHeartText, {ethNameIncreasesCounterRate1012Format, ethNameSText}},
    {ethNameCrossText, {ethNameDamageDown10Format, ethNameCText}},
    {ethNameYamatoBeltText, {ethNameIncreasesMaxHP15Format, ethNameYText}},
    {ethNameSilverCrownText, {ethNameIncreasesMaxEP15Format, ethNameSText}},
    {ethNameThiefRingText, {ethNameIncreasesRareItemAcquisitionRate10Format2, ethNameTText}},
    {ethNameCatBurglarGlovesText, {ethNameIncreasesRareItemAcquisitionRate30Format2, ethNameCText}},
    {ethNameCommanderSCrestText, {ethNameEachTurnAP1CharactersOnlyText, ethNameCText}},
    {ethNameGoldenDiceText, {ethNameFluctuatingDamageDependingOnHPText, ethNameGText}},
    {ethNameSpeedShoesText, {ethNameIncreasesSpeed25Format, ethNameSText}},
    {ethNameDoubleBusterText, {ethName2xTechAttackPossibleText, ethNameDText}},
    {ethNameBattleMaskText, {ethNameIncreasesHISlotTechAttackCriticalText, ethNameBText}},
    {ethNameAngelRingText, {ethNameEPConsumption12Text, ethNameAText}},
    {ethNameGeminiClockText, {ethNameStatusEffectTimeX2Text, ethNameGText}},
    {ethNameLifeStoneText, {ethNameRecoveryAmountX2Text, ethNameLText}},
    {ethNameMasterSPendantText, {ethNameIncreasesExperiencePoints25Format, ethNameMText}},
    {ethNameReserveText, {ethNameReserveText, ethNameRText}},
    {ethNameBravesoulText, {ethNameIncreasesStrAsHPDecreasesText, ethNameBText}},
    {ethNameBlueTopazText, {ethNameVeilSupportEffectText, ethNameBText}},
    {ethNameRedTopazText, {ethNameAntiVeilSupportEffectText2, ethNameRText}},
    {ethNameRedRingSText, {ethNamePhysAtk4Text, ethNameRText}},
    {ethNameGreenRingSText, {ethNamePhysDef4Text, ethNameGText}},
    {ethNameBlueRingSText, {ethNameDexterity4Text, ethNameBText}},
    {ethNameCobaltRingSText, {ethNameEvade4Text, ethNameCText}},
    {ethNamePurpleRingSText, {ethNameEtherAtk4Text, ethNamePText}},
    {ethNameOrangeRingSText, {ethNameEtherDef4Text, ethNameOText}},
    {ethNameScopeText, {ethNameDisplaysEnemyHPInformationText, ethNameSText}},
    {ethNameBladeSoulText, {ethNamePreventsConfusionText, ethNameBText}},
    {ethNameFieldRingText, {ethNamePreventsStopText, ethNameFText}},
    {ethNameRevengePowerText, {ethNameAutoBoostWhenAttackedCounter10Text2, ethNameRText}},
    {ethNameBoostPackText, {ethNameBoost1AtStartOfBattleText, ethNameBText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameFastCircuit25Text, {ethNameIncreasesSpeed25Format, ethNameFText}},
    {ethNameFastCircuit50Text, {ethNameIncreasesSpeed50Format, ethNameFText}},
    {ethNameBMAXCircuitText, {ethNameEnablesUseOfBoostText, ethNameBText}},
    {ethNameArmorProtectUnitText, {ethNamePreventsArmorFailureText, ethNameAText}},
    {ethNameScopeShieldText, {ethNamePreventsDexterityDownEvadeDownText, ethNameSText}},
    {ethNamePowerShieldText, {ethNamePreventsPowerLossText, ethNamePText}},
    {ethNameEngineShieldText, {ethNamePreventsEngineStopText, ethNameEText}},
    {ethNameGearShieldText, {ethNamePreventsSlowText, ethNameGText}},
    {ethNameECircuitShieldText, {ethNamePreventsEtherCrashText, ethNameEText}},
    {ethNameMChipGuardText, {ethNamePreventsWearText, ethNameMText}},
    {ethNameAPShieldText, {ethNamePreventsAPHalfText, ethNameAText}},
    {ethNameWCircuitShieldText, {ethNamePreventsAttackDisableText, ethNameWText}},
    {ethNameCockpitGuardText, {ethNamePreventsPilotConfusionSleepText, ethNameCText}},
    {ethNameTunedCircuitText, {ethNameAgility1Text, ethNameTText}},
    {ethNameGuardCleanerText, {ethNameClearsStatusAbnormalitiesWhenGuardingText, ethNameGText}},
    {ethNameGuardRecoveryText, {ethNameRecovers10FMaxFHPWhenText, ethNameGText}},
    {ethNameAuxiliaryArmorAText, {ethNameArmor2Text, ethNameAText}},
    {ethNameAuxiliaryArmorBText, {ethNameArmor4Text, ethNameAText}},
    {ethNameAuxiliaryArmorCText, {ethNameArmor6Text, ethNameAText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameEFCircuitAText, {ethNameEtherDef2Text, ethNameEText}},
    {ethNameEFCircuitBText, {ethNameEtherDef4Text, ethNameEText}},
    {ethNameEFCircuitCText, {ethNameEtherDef6Text, ethNameEText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameAntiFireArmorText, {ethNameReducesFTypeDamage25Format, ethNameAText}},
    {ethNameAntiIceArmorText, {ethNameReducesITypeDamage25Format, ethNameAText}},
    {ethNameAntiLightningArmorText, {ethNameReducesLTypeDamage25Format, ethNameAText}},
    {ethNameAntiBeamArmorText, {ethNameReducesBTypeDamage25Format, ethNameAText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}}
};

static EthName wepNameTbl[114] = {
    {ethNameSpaceGlovesText, {ethNameChaosGlovesUsedWhenWorkingInText, ethNameSText}},
    {ethNameNavyGlovesText, {ethNameChaosMilitaryIssueGlovesMadeOfText, ethNameNText}},
    {ethNameRiderGlovesText, {ethNameChaosTypicalRidingGlovesForWinterText, ethNameRText}},
    {ethNameWorkGlovesText, {ethNameChaosGlovesForDoItYourselfersText, ethNameWText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHoleyGlovesText, {ethNameChaosWornOutGlovesWithHolesText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameFGSHOTText, {ethNameSingleLinePierceTripleGatlingGunText, ethNameFText}},
    {ethNameFMSHOTText, {ethNameAllAirHitArmMissilePodText, ethNameFText}},
    {ethNameFBSHOTText, {ethNameSingleLineBeamEA120DragonSkullFormat, ethNameFText}},
    {ethNameFRSHOTText, {ethNameSingleLinePierceShortRailCannonText, ethNameFText}},
    {ethNameFSCYTHEText, {ethNameAllLineNoneEA300ScytheThatFormat, ethNameFText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameBattleRodText, {ethNameMOMOTypicalSelfDefenseScepterPhysText, ethNameBText}},
    {ethNameMetalRodText, {ethNameMOMOHardAndHeavyBattleScepterText, ethNameMText}},
    {ethNamePlatinumRodText, {ethNameMOMOLightPowerfulScepterPhysAtkText, ethNamePText}},
    {ethNameMithrilRodText, {ethNameMOMOScepterMadeFromRareMaterialsText, ethNameMText}},
    {ethNameWHammerRodText, {ethNameMOMOConfusionAgainstBGTypesText, ethNameWText}},
    {ethNameSaintRodText, {ethNameMOMOAHeroSScepterMadeText, ethNameSText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameDragonRodText, {ethNameMOMOScepterWithConcentratedEnergyWithinText, ethNameDText}},
    {ethNamePenguinRodText, {ethNameMOMOSlowAgainstBGTypesText, ethNamePText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameBMP55SXText, {ethNameAllLineBeamEA135ZiggySFormat, ethNameBText}},
    {ethNameMSP89SXText, {ethNameAllLineHitZiggySMissileText, ethNameMText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameROOK505Text, {ethNameJrStandardRookCoGunText, ethNameRText}},
    {ethNameMAKAROVText, {ethNameJrAnHeirloomQualityAntiqueGunText, ethNameMText}},
    {ethNameSAIFAR45Text, {ethNameJrSecondGenerationGunBySaifarText, ethNameSText}},
    {ethNameCROSSText, {ethNameJrAnOriginalGunMadeByText, ethNameCText}},
    {ethNameBLOOD9Text, {ethNameJrLegendaryGunHailedAsTheText, ethNameBText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameMWSText, {ethNameShionSpecialWeaponManufacturedByVectorText, ethNameMText}},
    {ethNameHANDText, {ethNameSingleNearHitText, ethNameHText}},
    {ethNameAXE11AGText, {ethNameSingleNearSlashWACTFederationText3, ethNameAText}},
    {ethNameSWD21AGText, {ethNameSingleNearSlashWACTFederationText2, ethNameSText}},
    {ethNameSWD34VXText, {ethNameSingleNearSlashWACTVectorText, ethNameSText}},
    {ethNameDLCO2AG4Text, {ethNameSingleNearPierceWACTFederationText, ethNameDText}},
    {ethNameHG45VXText, {ethNameSingleLinePierceWACTHandgunText, ethNameHText}},
    {ethNameHG75VXText, {ethNameSingleLinePierceWACTHandgunText, ethNameHText}},
    {ethNameHMR55AGText, {ethNameSingleNearHitWACTFederationText, ethNameHText}},
    {ethNameWCT02AG4Text, {ethNameSingleNearSlashWACTFederationText, ethNameWText}},
    {ethNameHMRAG5Text, {ethNameSingleNearHitFederationArmyHeavyText, ethNameHText}},
    {ethNameSMG99AGText, {ethNameSingleLinePierceWACTFederationText3, ethNameSText}},
    {ethNameSMG32VXText, {ethNameSingleLinePierceWACTSubmachineText, ethNameSText}},
    {ethNameLG10AGText, {ethNameSingleLinePierceWACTFederationText2, ethNameLText}},
    {ethNameLG24VXText, {ethNameSingleLineBeamEA120WACTFormat, ethNameLText}},
    {ethNameLG100VXText, {ethNameSingleLineBeamEA300LongBeamFormat, ethNameLText}},
    {ethNameBA15VXText, {ethNameSingleLineBeamEA185WACTFormat, ethNameBText}},
    {ethNameBSW13AGText, {ethNameSingleNearSlashBeamWACTText, ethNameBText}},
    {ethNameLM11VXText, {ethNameSingleNearSlashWACTLancerText, ethNameLText}},
    {ethNamePB55AGText, {ethNameSingleNearPierceFederationArmyBunkerText, ethNamePText}},
    {ethNameFLM64AGText, {ethNameSingleLineFireEA150WACTFormat, ethNameFText}},
    {ethNameGRD20AGText, {ethNameSingleLineFireWACTFederationText, ethNameGText}},
    {ethNameGLG76AGText, {ethNameSingleLinePierceWACTFederationText, ethNameGText}},
    {ethNameHGGAG5Text, {ethNameSingleLinePierceWACTFederationText, ethNameHText}},
    {ethNameHMP33AGText, {ethNameAllAirHitFederationArmyHandText, ethNameHText}},
    {ethNameHMPAG5Text, {ethNameAllAirHitFederationArmyHandText, ethNameHText}},
    {ethNameBL24AGText, {ethNameSingleLineBeamEA200FederationArmyFormat, ethNameBText}},
    {ethNameCB85VXText, {ethNameEnemiesAllAlliesAirSChaffText, ethNameCText}},
    {ethNameAIRCAG2Text, {ethNameJpText, ethNameAText}},
    {ethNameBMP45VXText, {ethNameAllAirBeamEA130WideBeamFormat, ethNameBText}},
    {ethNameLCAG5Text, {ethNameSingleLinePierceFederationArmyLongText, ethNameLText}},
    {ethNameBMPAG5Text, {ethNameAllAirHitFederationArmyLargeText, ethNameBText}},
    {ethNameSHB67AGText, {ethNameSingleAirBeamEA130DefenseFederationFormat, ethNameSText}},
    {ethNameSHD02AGText, {ethNameDefenseFederationArmyShieldText, ethNameSText}},
    {ethNameSHD12VXText, {ethNameDefenseShieldManufacturedByVectorText, ethNameSText}},
    {ethNameBBCAG5Text, {ethNameSingleLineBeamEA600FederationArmyFormat, ethNameBText}},
    {ethNameAIRDAG2Text, {ethNameAllAirBeamEA250FederationArmyFormat, ethNameAText}},
    {ethNameECM1VXText, {ethNameSingleLineSECMPodManufacturedText, ethNameEText}},
    {ethNameECM2VXText, {ethNameAllAirSECMPodManufacturedText, ethNameEText}},
    {ethNameSMP53AGText, {ethNameAllAirHitFederationArmyMissileText, ethNameSText}},
    {ethNameERVXText, {ethNameSingleNearSlashWACTElectroText, ethNameEText}},
    {ethNameLWVX2Text, {ethNameAllAirBeamEA300LanceManufacturedFormat, ethNameLText}},
    {ethNameDEFVXText, {ethNameAllAlliesEtherShieldDefensiveShieldText, ethNameDText}},
    {ethNameGLG76AGText, {ethNameSingleLinePierceWACTFederationText, ethNameGText}},
    {ethNameHGGAG5Text, {ethNameSingleLinePierceWACTFederationText, ethNameHText}},
    {ethNameHMPAG5Text, {ethNameAllAirPierceFederationArmyHandText, ethNameHText}}
};

static EthName bltNameTbl[178] = {
    {ethNameBXS001VText, {ethNameShionTechAttackShockBladeSlowText, ethNameBText}},
    {ethNameBXS002VText, {ethNameShionTechAttackShockBladePowerText, ethNameBText}},
    {ethNameBXS003VText, {ethNameShionTechAttackShockBladeArmorText, ethNameBText}},
    {ethNameBXS004VText, {ethNameShionTechAttackShockBladePhysText2, ethNameBText}},
    {ethNameBXS005VText, {ethNameShionTechAttackShockBladePhysText, ethNameBText}},
    {ethNameBXS006VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS007VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS008VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS009VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS010VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS011VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS012VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS013VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS014VText, {ethNameJpText, ethNameJpText}},
    {ethNameBXS015VText, {ethNameJpText, ethNameJpText}},
    {ethNameKGS10VText, {ethNameKOSMOSBulletsForFGSHOTText, ethNameKText}},
    {ethNameKMS30VText, {ethNameKOSMOSBulletsForFMSHOTText, ethNameKText}},
    {ethNameKGS12VText, {ethNameJpText, ethNameJpText}},
    {ethNameKMS32VText, {ethNameJpText, ethNameJpText}},
    {ethNameKRS50VText, {ethNameKOSMOSBulletsForFRSHOTText, ethNameKText}},
    {ethNameKGS14VText, {ethNameJpText, ethNameJpText}},
    {ethNameKMS34VText, {ethNameJpText, ethNameJpText}},
    {ethNameKRS52VText, {ethNameJpText, ethNameJpText}},
    {ethNameKMS36VText, {ethNameJpText, ethNameJpText}},
    {ethNameKGS16VText, {ethNameJpText, ethNameJpText}},
    {ethNameKMS38VText, {ethNameJpText, ethNameJpText}},
    {ethNameKRS54VText, {ethNameJpText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameMSS500Text, {ethNameZiggyMissilesForMSP89SXPhysAtkText2, ethNameMText}},
    {ethNameMSS520Text, {ethNameJpText, ethNameJpText}},
    {ethNameMSS540Text, {ethNameZiggyMissilesForMSP89SXPhysAtkText, ethNameMText}},
    {ethNameMSS560Text, {ethNameJpText, ethNameJpText}},
    {ethNameMSS580Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameRK200Text, {ethNameJrBulletsForROOK505PhysAtkText2, ethNameRText}},
    {ethNameMAKAROVBulletsText, {ethNameJrValuableBulletsForMAKAROVGunText, ethNameMText}},
    {ethNameRK280Text, {ethNameJrBulletsForROOK505PhysAtkText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameSR500Text, {ethNameJrBulletsForSAIFARPhysAtkText, ethNameSText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameCS700Text, {ethNameJrBulletsForCROSSPhysAtkText2, ethNameCText}},
    {ethNameBD900Text, {ethNameJrBulletsForBLOOD9PhysAtkText, ethNameBText}},
    {ethNameCS700SLWText, {ethNameJrBulletsForCROSSSlowAgainstText, ethNameCText}},
    {ethNameCS700PWDText, {ethNameJrBulletsForCROSSPhysAtkText, ethNameCText}},
    {ethNameCS700DFDText, {ethNameJrBulletsForCROSSPhysDefText, ethNameCText}},
    {ethNameCS700UVLText, {ethNameJrBulletsForCROSSEtherAtkText, ethNameCText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHG45B1Text, {ethNameHG45VXHandgunBulletsText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameLG10B1Text, {ethNameLG10AGRifleBulletsText, ethNameLText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameSMG99B1Text, {ethNameSMG99AGSubmachineGunBulletsText, ethNameSText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameGRD20B1Text, {ethNameGRD20AGGrenadeAmmoText, ethNameGText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHMP33B1Text, {ethNameHMP33HandMissilePodAmmoText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameLG100B1Text, {ethNameNoBulletsText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameCB85B1Text, {ethNameCB85VXChaffBoxBulletsDexterityDownText, ethNameCText}},
    {ethNameCB85B2Text, {ethNameCB85VXChaffBoxBulletsEtherAtkText, ethNameCText}},
    {ethNameCB85B3Text, {ethNameCB85VXChaffBoxBulletsPhysAtkText, ethNameCText}},
    {ethNameCB85B4Text, {ethNameCB85VXChaffBoxBulletsPhysDefText, ethNameCText}},
    {ethNameCB85B5Text, {ethNameJpText, ethNameCText}},
    {ethNameECM1B1Text, {ethNameECM1VXECMPodBulletsPowerText, ethNameEText}},
    {ethNameECM1B2Text, {ethNameECM1VXECMPodBulletsArmorText, ethNameEText}},
    {ethNameECM1B3Text, {ethNameECM1VXECMPodBulletsDexterityText, ethNameEText}},
    {ethNameECM1B4Text, {ethNameECM1VXECMPodBulletsSlowText, ethNameEText}},
    {ethNameECM1B5Text, {ethNameECM1VXECMPodBulletsWearText, ethNameEText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHG75B1Text, {ethNameHG75VXHandgunBulletsText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameGLG76B1Text, {ethNameGLG76AGGatlingGunBulletsText, ethNameGText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameAXE11B1Text, {ethNameNoBulletsText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameSMP53B1Text, {ethNameSMP53AGMissilePodAmmoText, ethNameSText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameSMG32B1Text, {ethNameSMG32VXSubmachineGunBulletsText, ethNameSText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameECM2B1Text, {ethNameECM2VXECMPodBulletsPhysText2, ethNameEText}},
    {ethNameECM2B2Text, {ethNameECM2VXECMPodBulletsPhysText, ethNameEText}},
    {ethNameECM2B3Text, {ethNameECM2VXECMPodBulletsDexterityText, ethNameEText}},
    {ethNameECM2B4Text, {ethNameECM2VXECMPodBulletsSlowText, ethNameEText}},
    {ethNameECM2B5Text, {ethNameECM2VXECMPodBulletsWearText, ethNameEText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameBMPB1Text, {ethNameNoBulletsText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHMPB1Text, {ethNameHMPAG5HandMissilePodAmmoText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHGGB1Text, {ethNameHGGAG5GatlingGunBulletsText, ethNameHText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameLCB1Text, {ethNameLCAG5LongCannonAmmoText, ethNameLText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserve1Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve2Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve3Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve4Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve5Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve6Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve7Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve8Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve9Text, {ethNameJpText, ethNameJpText}},
    {ethNameReserve10Text, {ethNameJpText, ethNameJpText}}
};

static EthName nrmNameTbl[44] = {
    {ethNameTornadoSlashText, {ethNameJpText, ethNameJpText}},
    {ethNameArrowText, {ethNameJpText, ethNameJpText}},
    {ethNameSeraphimRushText, {ethNameJpText, ethNameJpText}},
    {ethNameElectroUpperText, {ethNameJpText, ethNameJpText}},
    {ethNameAngelShotText, {ethNameJpText, ethNameJpText}},
    {ethNameStarThrustText, {ethNameJpText, ethNameJpText}},
    {ethNamePUNCHText, {ethNameJpText, ethNameJpText}},
    {ethNameBLASTERText, {ethNameJpText, ethNameJpText}},
    {ethNameSSAULTText, {ethNameJpText, ethNameJpText}},
    {ethNameSWORDText, {ethNameJpText, ethNameJpText}},
    {ethNameBLASTERText, {ethNameJpText, ethNameJpText}},
    {ethNameNEEDLEText, {ethNameJpText, ethNameJpText}},
    {ethNameKnuckleText, {ethNameJpText, ethNameJpText}},
    {ethNameFirecrackerText, {ethNameJpText, ethNameJpText}},
    {ethNamePowerKickText, {ethNameJpText, ethNameJpText}},
    {ethNameStunShockText, {ethNameJpText, ethNameJpText}},
    {ethNameCherryBombText, {ethNameJpText, ethNameJpText}},
    {ethNameSpinKickText, {ethNameJpText, ethNameJpText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameHighKickText, {ethNameJpText, ethNameJpText}},
    {ethNameLaserBladeText, {ethNameJpText, ethNameJpText}},
    {ethNameJackKnifeText, {ethNameJpText, ethNameJpText}},
    {ethNameIgnitionText, {ethNameJpText, ethNameJpText}},
    {ethNameLaserSwingText, {ethNameJpText, ethNameJpText}},
    {ethNameJackBladeText, {ethNameJpText, ethNameJpText}},
    {ethNameStardustText, {ethNameJpText, ethNameJpText}},
    {ethNameMelodyRayText, {ethNameJpText, ethNameJpText}},
    {ethNameTwinStarsText, {ethNameJpText, ethNameJpText}},
    {ethNameDreamWhirlText, {ethNameJpText, ethNameJpText}},
    {ethNameFlowerStormText, {ethNameJpText, ethNameJpText}},
    {ethNameCosmicFlipText, {ethNameJpText, ethNameJpText}},
    {ethNameCrossShotText, {ethNameJpText, ethNameJpText}},
    {ethNameSouthpawText, {ethNameJpText, ethNameJpText}},
    {ethNameTrickShotText, {ethNameJpText, ethNameJpText}},
    {ethNameCoinSnapText, {ethNameJpText, ethNameJpText}},
    {ethNameCrossfireText, {ethNameJpText, ethNameJpText}},
    {ethNameHipShotText, {ethNameJpText, ethNameJpText}},
    {ethNameRifleShotText, {ethNameJpText, ethNameJpText}},
    {ethNameRushText, {ethNameJpText, ethNameJpText}}
};

static EthName spcNameTbl[53] = {
    {ethNameAngelWingsText, {ethNameOneEnemyNearHitSpiritImpactText, ethNameAText}},
    {ethNameLunarSealText, {ethNameOneEnemyLineSpiritAChiText, ethNameLText}},
    {ethNameHeavenSWrathText, {ethNameOneEnemyNearSlashLightningSlashText, ethNameHText}},
    {ethNameAngelBlowText, {ethNameAllEnemiesAirSpiritGiantChiText, ethNameAText}},
    {ethNameChainedBlastText, {ethNameOneEnemyNearSpiritSPilotText, ethNameCText}},
    {ethNameDemonBanisherText, {ethNameOneEnemyLineSCurseAgainstText, ethNameDText}},
    {ethNameArcticBlastText, {ethNameAllEnemiesAirIceHurlDownText, ethNameAText}},
    {ethNameDivineSpearText, {ethNameOneEnemyNearPierceSpiritOnlyText, ethNameDText}},
    {ethNameRBLADEText, {ethNameOneEnemyNearSlashAttackWithText, ethNameRText}},
    {ethNameRCANNONText, {ethNameOneEnemyLineBeamAttackWithText, ethNameRText}},
    {ethNameRDRILLText, {ethNameOneEnemyNearPierceAttackWithText, ethNameRText}},
    {ethNameXBUSTERText, {ethNameAllEnemiesLineBeamAbdominalSpreadText, ethNameXText}},
    {ethNameSCHAINText, {ethNameOneEnemyLineSMultipleStatusText, ethNameSText}},
    {ethNameRHAMMERText, {ethNameOneEnemyNearHitAttackWithText2, ethNameRText}},
    {ethNameRDRAGONText, {ethNameOneEnemyNearHitAttackWithText, ethNameRText}},
    {ethNameLightningBlastText, {ethNameOneEnemyNearHitLightningLightningText, ethNameLText}},
    {ethNameSpellRayText, {ethNameOneEnemyLineBeamConcentratedEtherText, ethNameSText}},
    {ethNameShockBladeText, {ethNameOneEnemyNearPierceSDifferentText, ethNameSText}},
    {ethNameThermalBlastText, {ethNameOneEnemyNearHitFireViciousText, ethNameTText}},
    {ethNameEtherAmpText, {ethNameOneEnemyNearS1TimeText, ethNameEText}},
    {ethNameGravityWellText, {ethNameOneEnemyNearHitSSlowText, ethNameGText}},
    {ethNameLunarBladeText, {ethNameOneEnemyNearSlashBeamSliceText, ethNameLText}},
    {ethNameRainBladeText, {ethNameAllEnemiesAirBeamRainOfText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameCyberKickText, {ethNameOneEnemyNearHitFireHundredText, ethNameCText}},
    {ethNameLightningFistText, {ethNameOneEnemyLineLightningLightningAttackText, ethNameLText}},
    {ethNameMeteorShotText, {ethNameAllEnemiesAirFireFireAttackText, ethNameMText}},
    {ethNameCycloneText, {ethNameOneEnemyNearSlashATornadoText, ethNameCText}},
    {ethNameCrossLancerText, {ethNameOneEnemyNearPierceACrossedText, ethNameCText}},
    {ethNameExecutionerText, {ethNameAllEnemiesAirLightningLightningAttackText, ethNameEText}},
    {ethNameHellFireText, {ethNameOneEnemyNearSlashFireFireText, ethNameHText}},
    {ethNameStarStrikeText, {ethNameOneEnemyNearHitStrikeWithText, ethNameSText}},
    {ethNameFloralTempestText, {ethNameOneEnemyNearSlashSlashWithText, ethNameFText}},
    {ethNameMeteorStormText, {ethNameOneEnemyLineNoneAttackFromText, ethNameMText}},
    {ethNameStarCannonText, {ethNameAllEnemiesAirHitStrikeWithText, ethNameSText}},
    {ethNameAngelArrowText, {ethNameAllEnemiesAirPierceAttackWithText, ethNameAText}},
    {ethNameDarkScepterText, {ethNameOneEnemyNearHitTurnGText, ethNameDText}},
    {ethNameMOMOSKissText, {ethNameOneEnemyLineBeamCanBeText, ethNameMText}},
    {ethNameMagicCasterText, {ethNameOneEnemyLineCanBeUsedText, ethNameMText}},
    {ethNamePreludeToBattleText, {ethNameOneEnemyLinePierceCrossShotText, ethNamePText}},
    {ethNameMoonlitSerenadeText, {ethNameOneEnemyLineSpiritAttackWithText, ethNameMText}},
    {ethNameStormWaltzText, {ethNameAllEnemiesLinePierceCoinShotText, ethNameSText}},
    {ethNameLastSymphonyText, {ethNameOneEnemyLinePierceConcentratedHeavyText, ethNameLText}},
    {ethNameMysticNocturneText, {ethNameAllEnemiesAirSpiritAttackWithText, ethNameMText}},
    {ethNameAngelicRequiemText, {ethNameAllEnemiesAirPierceSnipeFromText, ethNameAText}},
    {ethNameSoulRhapsodyText, {ethNameOneEnemyLineSpiritAGunText, ethNameSText}}
};

static const char *enemyNameTbl[154] = {
    ethNameDebugsterText, ethNameGoblinText, ethNameManticoreText, ethNameSkyFishText, ethNameKoboldText, ethNameOgreText, ethNameTrollText, ethNameFairyText, ethNameLarvaDollText, ethNameLarvaFaceText, ethNameCerberusText, ethNameGelFishText, ethNameDemonText, ethNameHydraText, ethNameLizardmanText, ethNameUnicornText, ethNameWyrmText, ethNameBugbearText, ethNameBasiliskText, ethNameDelphyneText, ethNameAzazelText, ethNameArmarosText, ethNameBaraqijalText, ethNameDroneG1Text, ethNameDroneG2Text, ethNameDroneG3Text, ethNameGoblinText, ethNameManticoreText, ethNameSkyFishText, ethNameOudogogueText, ethNameAttackDroneText, ethNameGertzogFEText, ethNameFEMA003Text, ethNameFEMA004Text, ethNameMeldGarethText, ethNameFEMA006Text, ethNameCalxText, ethNameViveText, ethNameFERE003Text, ethNameDroneSPXText, ethNameDroneMText, ethNameDroneFText, ethNameFedSoldierText, ethNameFedSoldierText, ethNameDOMOAText, ethNameDOMOBText, ethNameCyberCrabText, ethNameShotCrabText, ethNameGardisM1Text, ethNameGardisF10Text, ethNameWorkDroidText, ethNameCaptoMortumText, ethNameGertzogUTText, ethNameUTMA010Text, ethNameKubelText, ethNameSchutzText, ethNameDinGarethText, ethNameUTMA014Text, ethNameUTRE001Text, ethNameByproduct103Text, ethNameByproduct145Text, ethNameByproduct172Text, ethNameAthra26SeriesText, ethNameUTRE006Text, ethNameMercurioText, ethNameZolfoText, ethNameMercurioText, ethNameZolfoText, ethNameAmbixText, ethNameIosysText, ethNameXanthosisText, ethNameUTICSoldierText, ethNameUTICSoldierText, ethNameDirlewangerText, ethNameUTICSoldierText, ethNameEinzatsText, ethNameUTICSoldierXText, ethNameAcePilotText, ethNameDOMOText, ethNameDOMOText, ethNameGardisF10Text, ethNameTrollText, ethNameAttackDroneText, ethNameDroneMText, ethNameDroneFText, ethNameMONS008Text, ethNameMONS009Text, ethNameMONS010Text, ethNameMONS011Text, ethNameMONS012Text, ethNameMONS013Text, ethNameMONS014Text, ethNameMONS015Text, ethNameMONS016Text, ethNameMONS017Text, ethNameMONS018Text, ethNameMONS019Text, ethNameMONS020Text, ethNameMONS021Text, ethNameMONS022Text, ethNameMONS023Text, ethNameMONS024Text, ethNameMONS025Text, ethNameMONS026Text, ethNameMONS027Text, ethNameMONS028Text, ethNameMONS029Text, ethNameMONS030Text, ethNameMONS031Text, ethNameMONS032Text, ethNameMONS033Text, ethNameMONS034Text, ethNameMONS035Text, ethNameMr100PointsText, ethNameMr1000PointsText, ethNameMr10000PointsText, ethNameMr100000PointsText, ethNameMr300000PointsText, ethNameCyclopsText, ethNameMinotaurText, ethNameGremlinText, ethNameGolemText, ethNameDOMOCarrierText, ethNameBOSS006Text, ethNameDroneGXText, ethNameBOSS008Text, ethNameSvarozicText, ethNamePerunText, ethNameStribogText, ethNameGargoyleText, ethNameTiamatText, ethNameEinRugelText, ethNameDragonText, ethNameGigasText, ethNameRianonSeText, ethNameProtoDoraText, ethNameDoppelwogelText, ethNameMargulisText, ethNameSimeonText, ethNameBOSS022Text, ethNameBOSS023Text, ethNameJaldabaothText, ethNameRatatoskText, ethNameSophiePeithosText, ethNameBOSS027Text, ethNameBOSS028Text, ethNameGreatJoeText, ethNameMintiaText, ethNameBOSS031Text, ethNameBOSS032Text, ethNameAlbedoText, ethNameGremlinText, ethNameGolemText, ethNameBOSS120Text
};

static const char **statNameTbl[8] = {
    ba, ma, bb, mb, aa1, aa2, ab, 0
};

static const char *playerNameTbl[PLAYER_NAME_COUNT] = {
    ethNameChaosText, ethNameKOSMOSText, ethNameShionText, ethNameJpText, ethNameZiggurat8Text, ethNameMOMOText, ethNameJrText, ethNameJpText, ethNameJpText, ethNameVirgilText, ethNameMaryText, ethNameJpText, ethNameJpText, ethNameJpText, ethNameJpText, ethNameJpText, ethNameVX01Text, ethNameVX02Text, ethNameVX03Text, ethNameVX04Text, ethNameVX05Text, ethNameVX06Text, ethNameAG01Text, ethNameAG02Text, ethNameAG03Text, ethNameAG04Text, ethNameAG05Text, ethNameAG06Text, ethNameBunnieText, ethNameJpText, ethNameJpText, ethNameJpText
};

static const char *ziggyName = ethNameZiggyText;

static EthName engNameTbl[40] = {
    {ethNameVX01G01Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G02Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G03Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G04Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G04Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G05Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01G06Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX02G03Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02G04Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02G04Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02G05Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02G06Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX06G04Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06G04Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06G05Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06G06Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameAGO2G02Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG02G03Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG02G04Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG02G04Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG02G05Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG02G06Text, {ethNameAG02Text, ethNameAText}},
    {ethNameAG04G05Text, {ethNameAG04Text, ethNameAText}},
    {ethNameAG04G06Text, {ethNameAG04Text, ethNameAText}},
    {ethNameAG05G06Text, {ethNameAG05Text, ethNameAText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}}
};

static EthName frmNameTbl[40] = {
    {ethNameVX01F01Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F02Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F03Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F04Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F04Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F05Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX01F06Text, {ethNameVX10000Text, ethNameVText}},
    {ethNameVX02F03Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02F04Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02F04Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02F05Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX02F06Text, {ethNameVX7000Text, ethNameVText}},
    {ethNameVX06F04Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06F04Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06F05Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameVX06F06Text, {ethNameVX4000Text, ethNameVText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameAG02F02Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG02F03Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG02F04Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG02F04Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG02F05Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG02F06Text, {ethNameAG02Text, ethNameVText}},
    {ethNameAG04F05Text, {ethNameAG04Text, ethNameVText}},
    {ethNameAG04F06Text, {ethNameAG04Text, ethNameVText}},
    {ethNameAG05F06Text, {ethNameAG05Text, ethNameVText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}},
    {ethNameReserveText, {ethNameJpText, ethNameRText}}
};


const char **dataEthNameGet(int ethId)
{
    if (ethId <= 0)
    {
        printf(ethNameDataEthnameGetErrText, ethId);
        return 0;
    }
    return &ethNameTbl[ethId - 1].name;
}

const char ethNameDataItmNameGetErrText[] = "** dataItmNameGet: err %d\n";

const char **dataItmNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataItmNameGetErrText, index);
        return 0;
    }
    return &itmNameTbl[index - 1].name;
}

const char ethNameDataSklNameGetErrText[] = "** dataSklNameGet: err %d\n";

const char **dataSklNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataSklNameGetErrText, index);
        return 0;
    }
    return &sklNameTbl[index - 1].name;
}

const char ethNameDataAccNameGetErrText[] = "** dataAccNameGet: err %d\n";

const char **dataAccNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataAccNameGetErrText, index);
        return 0;
    }
    return &accNameTbl[index - 1].name;
}

const char ethNameDataWepNameGetErrText[] = "** dataWepNameGet: err %d\n";

const char **dataWepNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataWepNameGetErrText, index);
        return 0;
    }
    return &wepNameTbl[index - 1].name;
}

const char ethNameDataBltNameGetErrText[] = "** dataBltNameGet: err %d\n";

const char **dataBltNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataBltNameGetErrText, index);
        return 0;
    }
    return &bltNameTbl[index - 1].name;
}

const char ethNameDataNrmNameGetErrText[] = "** dataNrmNameGet: err %d\n";

const char **dataNrmNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataNrmNameGetErrText, index);
        return 0;
    }
    return &nrmNameTbl[index - 1].name;
}

const char ethNameDataSpcNameGetErrText[] = "** dataSpcNameGet: err %d\n";
/*
 * Special item ids continue after the normal item ids (nrmNameTbl), so the
 * table index here is (index - SPC_NAME_FIRST_ID).
 */

const char **dataSpcNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataSpcNameGetErrText, index);
        return 0;
    }
    return &spcNameTbl[index - SPC_NAME_FIRST_ID].name;
}

const char ethNameDataEnemyNameGetErrText[] = "** dataEnemyNameGet: err %d\n";
/* enemyNameTbl holds one name pointer per enemy id, starting at ENEMY_NAME_FIRST_ID. */

const char **dataEnemyNameGet(int index)
{
    if (index < ENEMY_NAME_FIRST_ID)
    {
        printf(ethNameDataEnemyNameGetErrText, index);
        return 0;
    }
    return &enemyNameTbl[index - ENEMY_NAME_FIRST_ID];
}

/* statNameTbl holds one name-table pointer per stat type; each is indexed by `index`.
 * The bound below is 9 although the table has 8 entries: statType 8 passes the
 * check and reads statNameTbl[8], one entry past the end (the original does the
 * same; kept as is). */

const char **dataStatNameGet(int statType, int index)
{
    const char **name;

    name = 0;
    if (statType < 9)
    {
        name = statNameTbl[statType] + index;
    }
    return name;
}

const char ethNameDataPlayerNameGetErrText[] = "** dataPlayerNameGet: err %d\n";

const char **dataPlayerNameGet(int id)
{
    int no;

    if (id > PLAYER_NAME_COUNT)
    {
        printf(ethNameDataPlayerNameGetErrText, id);
        return 0;
    }
    if (id == PLAYER_ID_ZIGGURAT8)
    {
        no = MenuScenarioNoGet();
        if (no >= ZIGGURAT8_NAME_SCENARIO_FIRST)
        {
            return &ziggyName;
        }
    }
    return &playerNameTbl[id - 1];
}

const char ethNameDataEngNameGetErrText[] = "** dataEngNameGet: err %d\n";

const char **dataEngNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataEngNameGetErrText, index);
        return 0;
    }
    return &engNameTbl[index - 1].name;
}

const char ethNameDataFrmNameGetErrText[40] = "** dataFrmNameGet: err %d\n";

const char **dataFrmNameGet(int index)
{
    if (index <= 0)
    {
        printf(ethNameDataFrmNameGetErrText, index);
        return 0;
    }
    return &frmNameTbl[index - 1].name;
}

static const char * ba[14] = { ethNameStopText, ethNameSlowText, ethNamePoisonText, ethNameDexterityDownText, ethNameEvadeDownText, ethNamePhysAtkDownText, ethNamePhysDefDownText, ethNameAPHalfText, ethNameEtherBombText, ethNameReverseText, ethNameHPHalfText, ethNameAttackDisabledText, ethNameAttackDisabledText, ethNameAttackDisabledText };

static const char * ma[11] = { ethNameConfusionText, ethNameSleepText, ethNameLostText, ethNameEtherAtkDownText, ethNameAttackPoisonText, ethNameCurseText, ethNameEPHalfText, ethNameEtherPoisonText, ethNameEPOverconsumptionText, ethNameSTDoubleText, ethNameBlockBoostText };

static const char * bb[12] = { ethNameAutoReviveText, ethNameSTLockText, ethNamePhysAtkUpText, ethNamePhysDefUpText, ethNameFocusPhysAtkText, ethNameFocusPhysDefText, ethNameSpeed25Text2, ethNameSpeed50Text, ethNameDexterityUpText, ethNameEvadeUpText, ethNameAPMAXText, ethNameSafetyText };

static const char * mb[8] = { ethNameChivalryText, ethNameMOMOGuardText, ethNameRecoveryX2Text, ethNameEPX2Text, ethNameFocusEtherAtkText, ethNameFocusEtherDefText, ethNameVeilText, ethNameAntiVeilText };

static const char * aa1[13] = { ethNameEngineStopText, ethNamePowerLossText, ethNameSlowText, ethNameArmorFailureText, ethNameDexterityDownText, ethNameEvadeDownText, ethNameEtherCrashText, ethNameWearText, ethNameAPHalfText, ethNameCRTCMarkText, ethNameAttackDisabledText, ethNameAttackDisabledText, ethNameAttackDisabledText };

static const char * aa2[3] = { ethNamePilotKOText, ethNamePilotSleepText, ethNamePilotConfusionText };

static const char * ab[4] = { ethNameSpeed25Text2, ethNameSpeed50Text, ethNameAPMAXText, ethNameEtherShieldText };

const char ethNameSelfIncreaseSpeed50Format[24] = "Self\nIncrease Speed 50%";

const char ethNameSpeedBoostText[16] = "Speed Boost";

const char ethNameOneEnemy1xOnly80RiticalText[64] = "One Enemy/1x only\n80% Critical Hit rate against M enemies";

const char ethNameRedMarkText[16] = "Red Mark";

const char ethNameSelf1xOnlyHPEPRecoveryText[40] = "Self/1x only\nHP & EP recovery x2";

const char ethNameChargeXText[16] = "Charge X";

const char ethNameSelfFocusEtherAtkText[24] = "Self\nFocus Ether Atk";

const char ethNameEtherShiftBText[16] = "Ether Shift B";

const char ethNameSelfFocusEtherDefText[24] = "Self\nFocus Ether Def";

const char ethNameEtherShiftAText[16] = "Ether Shift A";

const char ethNameSelfHPRecoveryMidText[24] = "Self\nHP recovery/Mid";

const char ethNameRechargeText[16] = "Recharge";

const char ethNameSelfIncreaseStrDefWhenMOMOText[48] = "Self\nIncrease Str/Def when MOMO is behind";

const char ethNameBodyguardText[16] = "Bodyguard";

const char ethNameSelfFocusPhysDefText[24] = "Self\nFocus Phys Def";

const char ethNameMyGuardText[16] = "My Guard";

const char ethNameReserveText[8] = "Reserve";

const char ethNameAllEnemies1xPerBattleSummonText[48] = "All enemies/1x per battle\nSummon Great Joe";

const char ethNameBusterJoeText[16] = "Buster Joe";

const char ethNameOneEnemy1xPerBattleSummonText[48] = "One enemy/1x per battle\nSummon Great Joe";

const char ethNameMagnumJoeText[16] = "Magnum Joe";

const char ethNameSpeedMachineText[16] = "Speed Machine";

const char ethNameOneAllyBlockStatChangesSupportText[48] = "One ally\nBlock stat. changes & support effects";

const char ethNameCoinLockText[16] = "Coin Lock";

const char ethNameOneEnemyDisableBGEnemiesText[40] = "One enemy\nDisable B & G enemies' spells";

const char ethNameMistyText[8] = "Misty";

const char ethNameOneEnemyEvadeDownForBText[40] = "One enemy\nEvade down for B & G enemies";

const char ethNameChainText[8] = "Chain";

const char ethNameOneEnemyStealItemsText[24] = "One enemy\nSteal Items";

const char ethNamePsychoPocketText[16] = "Psycho Pocket";

const char ethNameSelf1xOnlyReviveWhenFemaleText[56] = "Self/1x only\nRevive when female character is behind";

const char ethNameDandyismText[16] = "Dandyism";

const char ethNameOneEnemyAllyIncreaseEtherEffectFormat[48] = "One enemy/ally\nIncrease Ether effect 25% ";

const char ethNameEtherFlareText[16] = "Ether Flare";

const char ethNameSelfIncreasePhysAtkText[24] = "Self\nIncrease Phys Atk";

const char ethNamePsychoArmText[16] = "Psycho Arm";

const char ethNameAllEnemies1xPerStarlightNonText[56] = "All enemies/1x per Starlight\nNon-elemental Ether Atk";

const char ethNameStarBunnieText[16] = "Star Bunnie";

const char ethNameOneEnemyOnlyDuringStarlightNonText[56] = "One enemy/Only during Starlight\nNon-elemental Ether Atk";

const char ethNameStarBellText[16] = "Star Bell";

const char ethNameSelf1xPerBattleUseStarlightText[56] = "Self/1x per battle\nUse starlight power to transform";

const char ethNameStarlightText[16] = "Starlight";

const char ethNameOneAlly1xPerStarWindText[40] = "One ally/1x per Star Wind\nMax AP";

const char ethNameStarActionText[16] = "Star Action";

const char ethNameAllAlliesOnlyDuringStarWindFormat[56] = "All allies/Only during Star Wind\nEther effect down 25%";

const char ethNameStarVeilText[16] = "Star Veil";

const char ethNameSelf1xPerBattleUseStarText[56] = "Self/1x per battle\nUse star wind power to transform";

const char ethNameStarWindText[16] = "Star Wind";

const char ethNameOneEnemyReduceMEnemyActionFormat[40] = "One enemy\nReduce M enemy action 50%";

const char ethNameJunkBeamText[16] = "Junk Beam";

const char ethNameOneAllyReviveHPRecoveryLowText[40] = "One ally\nRevive + HP recovery/Low";

const char ethNameRefineText[8] = "Refine";

const char ethNameSelfAHeavenlyGiftText[24] = "Self\nA heavenly gift";

const char ethNamePrayerText[8] = "Prayer";

const char ethNameOneEnemyPutMEnemyPilotsText[40] = "One enemy\nPut M enemy pilots to sleep";

const char ethNameSheepBeamText[16] = "Sheep Beam";

const char ethNameOneEnemyNonElementalEtherAtkText[40] = "One enemy\nNon-elemental Ether Atk";

const char ethNameMiracleStarText[16] = "Miracle Star";

const char ethNameOneEnemyAllyHPRecoveryMidText[32] = "One enemy/ally\nHP Recovery/Mid";

const char ethNameLifeShotText[16] = "Life Shot";

const char ethNameAllEnemiesAttackClearAllStText[48] = "All enemies\nAttack + clear all St changes";

const char ethNameLightWingsText[16] = "Light & Wings";

const char ethNameCText[8] = "C";

const char ethNameAllEnemiesResetAttackWaitTimeText[40] = "All enemies\nReset attack & wait time";

const char ethNameCataclysmText[16] = "Cataclysm";

const char ethNameOneEnemyHPRecoveryDown1Text[40] = "One enemy\nHP recovery down (1/2)";

const char ethNameSupremeJudgmentText[24] = "Supreme Judgment";

const char ethNameAllAllies1xPerBattleReviveText[32] = "All allies/1x per battle\nRevive";

const char ethNameBestAllyText[16] = "Best Ally";

const char ethNameLText[8] = "L";

const char ethNameOneEnemyLightningLightningEtherAtkText[40] = "One enemy/Lightning\nLightning Ether Atk";

const char ethNameLightningWingsText[16] = "Lightning Wings";

const char ethNameFText[8] = "F";

const char ethNameOneEnemyFireFireEtherAtkText[32] = "One enemy/Fire\nFire Ether Atk";

const char ethNameFlameWingsText[16] = "Flame Wings";

const char ethNameAllAlliesClearAllStatusChangesText[40] = "All allies\nClear all status changes";

const char ethNamePurifyingStormText[16] = "Purifying Storm";

const char ethNameIText[8] = "I";

const char ethNameOneEnemyIceIceEtherAtkText[32] = "One enemy/Ice\nIce Ether Atk";

const char ethNameIceWingsText[16] = "Ice Wings";

const char ethNameHText[8] = "H";

const char ethNameAllAlliesHPRecoveryLowText[32] = "All allies\nHP recovery/Low";

const char ethNameHealingDewText[16] = "Healing Dew";

const char ethNamePText[8] = "P";

const char ethNameOneEnemyAllyEtherEffectDownFormat[40] = "One enemy/ally\nEther effect down 25%";

const char ethNameProtectiveWearText[16] = "Protective Wear";

const char ethNameAllEnemiesLightningLightningEtherAtkText[48] = "All enemies/Lightning\nLightning Ether Atk";

const char ethNameDexEtherLeText[16] = "Dex Ether Le";

const char ethNameAllEnemiesFireFireEtherAtkText[32] = "All enemies/Fire\nFire Ether Atk";

const char ethNameDexEtherFaText[16] = "Dex Ether Fa";

const char ethNameAllEnemiesIceIceEtherAtkText[32] = "All enemies/Ice\nIce Ether Atk";

const char ethNameDexEtherRaText[16] = "Dex Ether Ra";

const char ethNameAllEnemiesBeam1xPerBattleText[56] = "All enemies/Beam/1x per battle\nSatellite beam weapon";

const char ethNameSatelliteText[16] = "Satellite";

const char ethNameSelfEtherAtkDamageAndCostText[40] = "Self\nEther Atk damage and cost x2";

const char ethNameEtherLimitText[16] = "Ether Limit";

const char ethNameOneEnemyOccasionallyInstantKOHPText[72] = "One enemy/Occasionally \"Instant KO\"\nHP/4 damage on B & G enemies";

const char ethNameGateText[8] = "Gate";

const char ethNameOneEnemyEtherDownForBText[40] = "One enemy\nEther down for B & G enemies";

const char ethNameDownEtherText[16] = "Down Ether";

const char ethNameOneEnemyPhysAtkDownForText[48] = "One enemy\nPhys Atk down for B & G enemies";

const char ethNameDownForceText[16] = "Down Force";

const char ethNameOneEnemyDexterityDownForBText[48] = "One enemy\nDexterity down for B & G enemies";

const char ethNameDownDexText[16] = "Down Dex";

const char ethNameSelfFocusPhysAtkText[24] = "Self\nFocus Phys Atk";

const char ethNameModeA7Text[8] = "Mode A7";

const char ethNameEText[8] = "E";

const char ethNameAllEnemiesSlash1xPerBattleText[72] = "All enemies/Slash/1x per battle\nUltimate attack using combined form";

const char ethNameErdeKaiserText[16] = "Erde Kaiser";

const char ethNameDText[8] = "D";

const char ethNameAllEnemiesHit1xPerBattleText[56] = "All enemies/Hit/1x per battle\nUltra heavy weight attack";

const char ethNameDominionTankText[16] = "Dominion Tank";

const char ethNameTText[8] = "T";

const char ethNameAllEnemiesSlash1xPerBattleText2[56] = "All enemies/Slash/1x per battle\nGiant top attack";

const char ethNameThroniBladeText[16] = "Throni Blade";

const char ethNameAllEnemiesBeam1xPerBattleText2[56] = "All enemies/Beam/1x per battle\nSpread beam attack";

const char ethNameSeraphimBirdText[16] = "Seraphim Bird";

const char ethNameOneEnemyOccasionallyInstantKOStealText[48] = "One enemy/Occasionally \"Instant KO\"\nSteal items";

const char ethNameQueenSKissText[16] = "Queen's Kiss";

const char ethNameQText[8] = "Q";

const char ethNameOneAllyIncreaseSpeed25Format[32] = "One ally\nIncrease speed 25%";

const char ethNameQuickText[8] = "Quick";

const char ethNameSText[8] = "S";

const char ethName1xPerAllySurviveWith1Text[32] = "1x per ally\nSurvive with 1 HP";

const char ethNameSafetyLevelText[16] = "Safety Level";

const char ethNameOneAllyReviveHPRecoveryMidText[40] = "One ally\nRevive & HP recovery/Mid";

const char ethNameRevertText[8] = "Revert";

const char ethNameOneEnemyAllyHPRecoveryMaxText[32] = "One enemy/ally\nHP recovery/Max";

const char ethNameMedicaRestText[16] = "Medica Rest";

const char ethNameJText[8] = "J";

const char ethNameOneEnemyDexterityDownForMText[40] = "One enemy\nDexterity down for M enemies";

const char ethNameJammingText[8] = "Jamming";

const char ethNameBText[8] = "B";

const char ethNameOneAllyIncreaseBoostBy1Text[32] = "One ally\nIncrease Boost by 1";

const char ethNameBoost1Text[8] = "Boost 1";

const char ethNameRText[8] = "R";

const char ethNameOneAllyClearAllStatusChangesText[40] = "One ally\nClear all status changes";

const char ethNameRefreshText[8] = "Refresh";

const char ethNameAllAlliesHPRecoveryMidText[32] = "All allies\nHP recovery/Mid";

const char ethNameMedicaAllText[16] = "Medica All";

const char ethNameGText[8] = "G";

const char ethNameAllyEscapeFromBattleText[24] = "Ally\nEscape from battle";

const char ethNameGoodbyeText[8] = "Goodbye";

const char ethNameAText[8] = "A";

const char ethNameOneEnemyAnalyzeItemsAndHPText[32] = "One enemy\nAnalyze items and HP";

const char ethNameAnalyzeText[8] = "Analyze";

const char ethNameMText[8] = "M";

const char ethNameOneEnemyAllyHPRecoveryLowText[32] = "One enemy/ally\nHP recovery/Low";

const char ethNameMedicaText[8] = "Medica";

const char ethNameDiamondText[8] = "Diamond";

const char ethNameEmeraldText[8] = "Emerald";

const char ethNameRubyText[8] = "Ruby";

const char ethNameSapphireText[16] = "Sapphire";

const char ethNameGarnetText[8] = "Garnet";

const char ethNameJunkedCircuitBText[24] = "Junked Circuit B";

const char ethNamePreciousStoneText[16] = "Precious Stone";

const char ethNameKText[8] = "K";

const char ethNameKoboldBladeText[16] = "Kobold Blade";

const char ethNameUText[8] = "U";

const char ethNameUnicornHornText[16] = "Unicorn Horn";

const char ethNameOneAllySPts100Text[24] = "One ally\nS.Pts +100";

const char ethNameSkillUpgradeZText[16] = "Skill Upgrade Z";

const char ethNameOneAllySPts50Text[24] = "One ally\nS.Pts +50";

const char ethNameSkillUpgradeSText[16] = "Skill Upgrade S";

const char ethNameOneAllySPts10Text[24] = "One ally\nS.Pts +10";

const char ethNameSkillUpgradeAText[16] = "Skill Upgrade A";

const char ethNameOneAllyEPts100Text[24] = "One ally\nE.Pts +100";

const char ethNameEtherUpgradeZText[16] = "Ether Upgrade Z";

const char ethNameOneAllyEPts50Text[24] = "One ally\nE.Pts +50";

const char ethNameEtherUpgradeSText[16] = "Ether Upgrade S";

const char ethNameOneAllyEPts10Text[24] = "One ally\nE.Pts +10";

const char ethNameEtherUpgradeAText[16] = "Ether Upgrade A";

const char ethNameOneAllyTPts100Text[24] = "One ally\nT.Pts +100";

const char ethNameTechUpgradeZText[16] = "Tech Upgrade Z";

const char ethNameOneAllyTPts50Text[24] = "One ally\nT.Pts +50";

const char ethNameTechUpgradeSText[16] = "Tech Upgrade S";

const char ethNameOneAllyTPts10Text[24] = "One ally\nT.Pts +10";

const char ethNameTechUpgradeAText[16] = "Tech Upgrade A";

const char ethNameOneAllySkillLevel1Text[24] = "One ally\nSkill level +1";

const char ethNameSkillUpgradeText[16] = "Skill Upgrade";

const char ethNameJunkedCircuitAText[24] = "Junked Circuit A";

const char ethNameBarterItemText[16] = "Barter item";

const char ethNameScrapIronText[16] = "Scrap Iron";

const char ethNameAGWSFHPRecoveryText[40] = "A.G.W.S.\nFHP recovery/50% recovery";

const char ethNameFrameRepairZText[16] = "Frame Repair Z";

const char ethNameAGWSFHPRecoveryText2[40] = "A.G.W.S.\nFHP recovery/25% recovery";

const char ethNameFrameRepairAText[16] = "Frame Repair A";

const char ethNameAllAlliesSavePointsOnlyHPText[56] = "All allies/Save Points only\nHP & EP full recovery";

const char ethNameBioSphereText[16] = "Bio Sphere";

const char ethNameAntiVeilText[16] = "Anti-Veil";

const char ethNameVText[8] = "V";

const char ethNameOneEnemyAllyWPressAtFormat[64] = "One enemy/ally/W (Press \242\244 at AP6) \nEther effects down 25%";

const char ethNameVeilText[8] = "Veil";

const char ethNameOneAllyWPressAtAP6Format[56] = "One ally/W (Press \242\244 at AP6) \nIncreases speed 50%";

const char ethNameSpeedStimDXText[16] = "Speed Stim DX";

const char ethNameOneAllyWPressAtAP6Text[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Def";

const char ethNameDefenseShieldText[16] = "Defense Shield";

const char ethNameOneAllyIncreasesSpeed50Format[32] = "One ally\nIncreases speed 50%";

const char ethNameSpeedStimSXText[16] = "Speed Stim SX";

const char ethNameOneAllyWPressAtAP6Format2[56] = "One ally/W (Press \242\244 at AP6) \nIncreases speed 25%";

const char ethNameSpeedStimText[16] = "Speed Stim";

const char ethNameOneAllyHP1Text[16] = "One ally\nHP=1";

const char ethNameHemlockText[8] = "Hemlock";

const char ethNameOneAllyWPressAtAP6Format3[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Atk 50%";

const char ethNameStimDXText[8] = "Stim DX";

const char ethNameOneAllyWPressAtAP6Format4[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Atk 25%";

const char ethNameStimText[8] = "Stim";

const char ethNameOneAllyEscapeFromBattleText[32] = "One ally\nEscape from battle";

const char ethNameEscapePackText[16] = "Escape Pack";

const char ethNameOneAllyBoost1Text[24] = "One ally\nBoost+1";

const char ethNameBoosterPackText[16] = "Booster Pack";

const char ethNameNText[8] = "N";

const char ethNameOneAllyWPressAtAP6Text2[56] = "One ally/W (Press \242\244 at AP6) \nPsych Status Clear";

const char ethNameNeuroStimText[16] = "Neuro Stim";

const char ethNameOneAllyWPressAtAP6Text3[48] = "One ally/W (Press \242\244 at AP6) \nPhys Status Clear";

const char ethNameAntidoteText[16] = "Antidote";

const char ethNameOneAllyWPressAtAP6Text4[48] = "One ally/W (Press \242\244 at AP6) \nAll Status Clear";

const char ethNameCureAllText[16] = "Cure-All";

const char ethNameOneAllyWPressAtAP6Text5[72] = "One ally/W (Press \242\244 at AP6) \nRevives a KO'd ally & HP recovery/Max";

const char ethNameReviveDXText[16] = "Revive DX";

const char ethNameOneAllyWPressAtAP6Text6[72] = "One ally/W (Press \242\244 at AP6) \nRevives a KO'd ally & HP recovery/Low";

const char ethNameReviveText[8] = "Revive";

const char ethNameOneAllyWPressAtAP6Text7[56] = "One ally/W (Press \242\244 at AP6) \nHP & EP recovery/Max";

const char ethNameRejuvenatorText[16] = "Rejuvenator";

const char ethNameOneAllyWPressAtAP6Text8[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Max";

const char ethNameEtherPackMAXText[16] = "Ether Pack MAX";

const char ethNameOneAllyWPressAtAP6Text9[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/High";

const char ethNameEtherPackDXText[16] = "Ether Pack DX";

const char ethNameOneAllyWPressAtAP6Text10[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Mid";

const char ethNameEtherPackSText[16] = "Ether Pack S";

const char ethNameOneAllyWPressAtAP6Text11[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Low";

const char ethNameEtherPackText[16] = "Ether Pack";

const char ethNameOneEnemyAllyWPressAtText[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Max";

const char ethNameMedKitMAXText[16] = "Med Kit MAX";

const char ethNameOneEnemyAllyWPressAtText2[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/High";

const char ethNameMedKitDXText[16] = "Med Kit DX";

const char ethNameOneEnemyAllyWPressAtText3[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Mid";

const char ethNameMedKitSText[16] = "Med Kit S";

const char ethNameOneEnemyAllyWPressAtText4[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Low";

const char ethNameMedKitText[8] = "Med Kit";

const char ethNamePreventsStopText[16] = "Prevents \"Stop\"";

const char ethNameStopGuardText[16] = "Stop Guard";

const char ethNamePreventsConfusionText[24] = "Prevents \"Confusion\"";

const char ethNameConfusionGuardText[16] = "Confusion Guard";

const char ethNameDisplaysEnemyHPInformationText[32] = "Displays enemy HP information";

const char ethNameSearchEyesText[16] = "Search Eyes";

const char ethNameAutoBoostWhenAttackedCounter10Text[56] = "Auto-Boost when attacked \n\"Counter+10\" skill necessary";

const char ethNameCBOnText[8] = "CB On";

const char ethNameIncreasesEtherDefAsAlliesAreText[40] = "Increases Ether Def as allies are KO'd";

const char ethNameLonelyEDUPText[16] = "Lonely EDUP";

const char ethNameIncreasesPhysDefAsAlliesAreText[40] = "Increases Phys Def as allies are KO'd";

const char ethNameLonelyPDUPText[16] = "Lonely PDUP";

const char ethNameEtherDef42xEffectWhenText[56] = "Ether Def +4\n2x effect when combined with an accessory";

const char ethNameEDEF4Text[8] = "EDEF+4";

const char ethNamePhysDef42xEffectWhenText[56] = "Phys Def +4\n2x effect when combined with an accessory";

const char ethNamePDEF4Text[8] = "PDEF+4";

const char ethNameReducesSlshPrceTypeDamage20Format[80] = "Reduces Slsh/Prce-type damage 20%\n2x effect when combined with an accessory";

const char ethNameSword20Text[16] = "Sword-20";

const char ethNameReducesBTypeDamage202xFormat[72] = "Reduces B-type damage 20%\n2x effect when combined with an accessory";

const char ethNameBeam20Text[8] = "Beam-20";

const char ethNameReducesITypeDamage202xFormat[72] = "Reduces I-type damage 20%\n2x effect when combined with an accessory";

const char ethNameIce20Text[8] = "Ice-20";

const char ethNameReducesLTypeDamage202xFormat[72] = "Reduces L-type damage 20%\n2x effect when combined with an accessory";

const char ethNameLightning20Text[16] = "Lightning-20";

const char ethNameReducesFTypeDamage202xFormat[72] = "Reduces F-type damage 20%\n2x effect when combined with an accessory";

const char ethNameFire20Text[8] = "Fire-20";

const char ethNameBoost1AtStartOfBattleText[32] = "Boost +1 at start of battle";

const char ethNameBattleBC1Text[16] = "Battle BC+1";

const char ethNameStrIncreasesAsHPDecreasesText[32] = "Str increases as HP decreases";

const char ethNameHPStrongText[16] = "HP Strong";

const char ethNameIncreaseSPtsEarned252xFormat[72] = "Increase S.Pts earned 25%\n2x effect when combined with an accessory";

const char ethNameSkillP25Text[16] = "Skill P+25";

const char ethNameIncreasesEPtsEarned252xFormat[72] = "Increases E.Pts earned 25%\n2x effect when combined with an accessory";

const char ethNameEtherP25Text[16] = "Ether P+25";

const char ethNameIncreasesTPtsEarned252xFormat[72] = "Increases T.Pts earned 25%\n2x effect when combined with an accessory";

const char ethNameTechP25Text[16] = "Tech P+25";

const char ethNameIncreasesExperiencePoints252xEffectFormat[80] = "Increases experience points 25%\n2x effect when combined with an accessory";

const char ethNameExperienceP25Text[16] = "Experience P+25";

const char ethNameHISlotTechAttackIncreasesInText[56] = "HI Slot Tech Attack increases in Critical Hit rate";

const char ethNameTechAttackACText[16] = "Tech Attack AC";

const char ethNameHISlotTechAttackEP0Text[32] = "HI Slot Tech Attack EP=0";

const char ethNameTechAttackAEText[16] = "Tech Attack AE";

const char ethNameWText[8] = "W";

const char ethName2xTechAttackPossibleText[40] = "\242\242\241\373\241\373, \242\244\241\373\241\373 2x Tech Attack possible";

const char ethNameWSpecialText[16] = "W Special";

const char ethNameFluctuatingDamageDependingOnHPText[40] = "Fluctuating damage depending on HP";

const char ethNameDamageUDText[16] = "Damage UD";

const char ethNameEachTurnAP1CharacterOnlyText[40] = "Each turn AP +1\n (Character only) ";

const char ethNameAP1Text[8] = "AP+1";

const char ethNameIncreasesRareItemAcquisitionRate30Format[88] = "Increases rare item acquisition rate 30%\n2x effect when combined with an accessory";

const char ethNameRare30Text[8] = "Rare+30";

const char ethNameIncreasesRareItemAcquisitionRate10Format[88] = "Increases rare item acquisition rate 10%\n2x effect when combined with an accessory";

const char ethNameRare10Text[8] = "Rare+10";

const char ethNameIncreasesMaxEP152xEffectFormat[64] = "Increases Max EP 15%\n2x effect when combined with an accessory";

const char ethNameEPMAX15Text[16] = "EPMAX+15";

const char ethNameIncreasesMaxHP152xEffectFormat[64] = "Increases Max HP 15%\n2x effect when combined with an accessory";

const char ethNameHPMAX15Text[16] = "HPMAX+15";

const char ethNameReducesDamage102xEffectWhenFormat[64] = "Reduces damage 10%\n2x effect when combined with an accessory";

const char ethNameDamage10Text[16] = "Damage-10";

const char ethNameIncreasesCounterRate102xEffectFormat[72] = "Increases counter rate 10%\n2x effect when combined with an accessory";

const char ethNameCounter10Text[16] = "Counter+10";

const char ethNameIncreasesCriticalHitRate52xFormat[80] = "Increases Critical Hit rate 5%\n2x effect when combined with an accessory";

const char ethNameCRTC5Text[8] = "CRTC+5";

const char ethNameAgility12xEffectWhenCombinedText[56] = "Agility +1\n2x effect when combined with an accessory";

const char ethNameAGL1Text[8] = "AGL+1";

const char ethNameEtherDef22xEffectWhenText[56] = "Ether Def +2\n2x effect when combined with an accessory";

const char ethNameEDEF2Text[8] = "EDEF+2";

const char ethNameEtherAtk22xEffectWhenText[56] = "Ether Atk +2\n2x effect when combined with an accessory";

const char ethNameEATK2Text[8] = "EATK+2";

const char ethNameEvade22xEffectWhenCombinedText[56] = "Evade +2\n2x effect when combined with an accessory";

const char ethNameEVA2Text[8] = "EVA+2";

const char ethNameDexterity22xEffectWhenCombinedText[56] = "Dexterity +2\n2x effect when combined with an accessory";

const char ethNameDEX2Text[8] = "DEX+2";

const char ethNamePhysDef22xEffectWhenText[56] = "Phys Def +2\n2x effect when combined with an accessory";

const char ethNamePDEF2Text[8] = "PDEF+2";

const char ethNamePhysAtk22xEffectWhenText[56] = "Phys Atk +2\n2x effect when combined with an accessory";

const char ethNamePATK2Text[8] = "PATK+2";

const char ethNameAntiVeilSupportEffectText[32] = "\"Anti-Veil\" support effect";

const char ethNameEtherPowerUText[16] = "Ether Power U";

const char ethNameVeilSupportEffectText[24] = "\"Veil\" support effect";

const char ethNameEtherPowerDText[16] = "Ether Power D";

const char ethNameRecoveryAmountX2Text[24] = "Recovery amount x2";

const char ethNameRecoveryDoubleText[16] = "Recovery Double";

const char ethNameStatusEffectTimeX2Text[24] = "Status effect time x2";

const char ethNameSTDoubleText[16] = "ST Double";

const char ethNameEPConsumption12Text[24] = "EP consumption 1/2";

const char ethNameEPHalfText[8] = "EP Half";

const char ethNameIncreasesSpeed252xEffectWhenFormat[64] = "Increases speed 25%\n2x effect when combined with an accessory";

const char ethNameSpeed25Text[16] = "Speed +25";

const char ethNamePreventsEPDamageText[24] = "Prevents EP damage";

const char ethNameEPGuardText[16] = "EP Guard";

const char ethNamePreventsAllStatusAbnormalitiesSupportEffectsText[56] = "Prevents all status abnormalities & support effects";

const char ethNameAllGuardText[16] = "All Guard";

const char ethNamePreventsAttackDisableText[32] = "Prevents \"Attack Disable\"";

const char ethNameSpecialGuardText[16] = "Special Guard";

const char ethNamePreventsInstantKOText[24] = "Prevents \"Instant KO\"";

const char ethNameKOGuardText[16] = "KO Guard";

const char ethNamePreventsPoisonText[24] = "Prevents \"Poison\"";

const char ethNamePoisonGuardText[16] = "Poison Guard";

const char ethNamePreventsSlowText[16] = "Prevents \"Slow\"";

const char ethNameSlowGuardText[16] = "Slow Guard";

const char ethNamePreventsAPHalfText[24] = "Prevents \"AP Half\"";

const char ethNameAPGuardText[16] = "AP Guard";

const char ethNamePreventsEtherAtkDownText[32] = "Prevents \"Ether Atk Down\"";

const char ethNameEATKGuardText[16] = "EATK Guard";

const char ethNamePreventsLostText[16] = "Prevents \"Lost\"";

const char ethNameLostGuardText[16] = "Lost Guard";

const char ethNamePreventsSleepText[24] = "Prevents \"Sleep\"";

const char ethNameSleepGuardText[16] = "Sleep Guard";

const char ethNamePreventsPhysDefDownText[32] = "Prevents \"Phys Def Down\"";

const char ethNamePDEFGuardText[16] = "PDEF Guard";

const char ethNamePreventsPhysAtkDownText[32] = "Prevents \"Phys Atk Down\"";

const char ethNamePATKGuardText[16] = "PATK Guard";

const char ethNamePreventsEvadeDownText[24] = "Prevents \"Evade Down\"";

const char ethNameEVAGuardText[16] = "EVA Guard";

const char ethNamePreventsDexterityDownText[32] = "Prevents \"Dexterity Down\"";

const char ethNameDEXGuardText[16] = "DEX Guard";

const char ethNamePreventsEPOverconsumptionText[32] = "Prevents \"EP Overconsumption\"";

const char ethNameEPDoubleGuardText[16] = "EP Double Guard";

const char ethNamePreventsBindText[16] = "Prevents \"Bind\"";

const char ethNameBindGuardText[16] = "Bind Guard";

const char ethNamePreventsAttackPoisonText[32] = "Prevents \"Attack Poison\"";

const char ethNameAPoisonGuardText[16] = "A-Poison Guard";

const char ethNamePreventsCriticalHitText[24] = "Prevents \"Critical Hit\"";

const char ethNameCRTCGuardText[16] = "CRTC Guard";

const char ethNameReducesBTypeDamage25Format[32] = "Reduces B-type damage 25%";

const char ethNameAntiBeamArmorText[16] = "Anti-Beam Armor";

const char ethNameReducesLTypeDamage25Format[32] = "Reduces L-type damage 25%";

const char ethNameAntiLightningArmorText[24] = "Anti-Lightning Armor";

const char ethNameReducesITypeDamage25Format[32] = "Reduces I-type damage 25%";

const char ethNameAntiIceArmorText[16] = "Anti-Ice Armor";

const char ethNameReducesFTypeDamage25Format[32] = "Reduces F-type damage 25%";

const char ethNameAntiFireArmorText[16] = "Anti-Fire Armor";

const char ethNameEtherDef6Text[16] = "Ether Def +6";

const char ethNameEFCircuitCText[16] = "EF Circuit C";

const char ethNameEFCircuitBText[16] = "EF Circuit B";

const char ethNameEFCircuitAText[16] = "EF Circuit A";

const char ethNameArmor6Text[16] = "Armor +6";

const char ethNameAuxiliaryArmorCText[24] = "Auxiliary Armor C";

const char ethNameArmor4Text[16] = "Armor +4";

const char ethNameAuxiliaryArmorBText[24] = "Auxiliary Armor B";

const char ethNameArmor2Text[16] = "Armor +2";

const char ethNameAuxiliaryArmorAText[24] = "Auxiliary Armor A";

const char ethNameRecovers10FMaxFHPWhenText[40] = "Recovers 10% of Max FHP when guarding";

const char ethNameGuardRecoveryText[16] = "Guard Recovery";

const char ethNameClearsStatusAbnormalitiesWhenGuardingText[48] = "Clears status abnormalities when guarding";

const char ethNameGuardCleanerText[16] = "Guard Cleaner";

const char ethNameTunedCircuitText[16] = "Tuned Circuit";

const char ethNamePreventsPilotConfusionSleepText[40] = "Prevents \"Pilot Confusion & Sleep\"";

const char ethNameCockpitGuardText[16] = "Cockpit Guard";

const char ethNameWCircuitShieldText[24] = "W Circuit Shield";

const char ethNameAPShieldText[16] = "AP Shield";

const char ethNamePreventsWearText[16] = "Prevents \"Wear\"";

const char ethNameMChipGuardText[16] = "M Chip Guard";

const char ethNamePreventsEtherCrashText[24] = "Prevents \"Ether Crash\"";

const char ethNameECircuitShieldText[24] = "E Circuit Shield";

const char ethNameGearShieldText[16] = "Gear Shield";

const char ethNamePreventsEngineStopText[24] = "Prevents \"Engine Stop\"";

const char ethNameEngineShieldText[16] = "Engine Shield";

const char ethNamePreventsPowerLossText[24] = "Prevents \"Power Loss\"";

const char ethNamePowerShieldText[16] = "Power Shield";

const char ethNamePreventsDexterityDownEvadeDownText[40] = "Prevents \"Dexterity Down\" \"Evade Down\"";

const char ethNameScopeShieldText[16] = "Scope Shield";

const char ethNamePreventsArmorFailureText[32] = "Prevents \"Armor Failure\"";

const char ethNameArmorProtectUnitText[24] = "Armor Protect Unit";

const char ethNameEnablesUseOfBoostText[24] = "Enables use of Boost";

const char ethNameBMAXCircuitText[16] = "B-MAX Circuit";

const char ethNameIncreasesSpeed50Format[24] = "Increases speed 50%";

const char ethNameFastCircuit50Text[16] = "Fast Circuit 50";

const char ethNameFastCircuit25Text[16] = "Fast Circuit 25";

const char ethNameBoostPackText[16] = "Boost Pack";

const char ethNameAutoBoostWhenAttackedCounter10Text2[56] = "Auto-Boost when attacked\n\"Counter+10\" skill necessary";

const char ethNameRevengePowerText[16] = "Revenge Power";

const char ethNameFieldRingText[16] = "Field Ring";

const char ethNameBladeSoulText[16] = "Blade Soul";

const char ethNameScopeText[8] = "Scope";

const char ethNameEtherDef4Text[16] = "Ether Def +4";

const char ethNameOrangeRingSText[16] = "Orange Ring S";

const char ethNameEtherAtk4Text[16] = "Ether Atk +4";

const char ethNamePurpleRingSText[16] = "Purple Ring S";

const char ethNameEvade4Text[16] = "Evade +4";

const char ethNameCobaltRingSText[16] = "Cobalt Ring S";

const char ethNameDexterity4Text[16] = "Dexterity +4";

const char ethNameBlueRingSText[16] = "Blue Ring S";

const char ethNamePhysDef4Text[16] = "Phys Def +4";

const char ethNameGreenRingSText[16] = "Green Ring S";

const char ethNamePhysAtk4Text[16] = "Phys Atk +4";

const char ethNameRedRingSText[16] = "Red Ring S";

const char ethNameAntiVeilSupportEffectText2[32] = " \"Anti-Veil\" support effect";

const char ethNameRedTopazText[16] = "Red Topaz";

const char ethNameBlueTopazText[16] = "Blue Topaz";

const char ethNameIncreasesStrAsHPDecreasesText[32] = "Increases Str as HP decreases";

const char ethNameBravesoulText[16] = "Bravesoul";

const char ethNameIncreasesExperiencePoints25Format[32] = "Increases experience points 25%";

const char ethNameMasterSPendantText[24] = "Master's Pendant";

const char ethNameLifeStoneText[16] = "Life Stone";

const char ethNameGeminiClockText[16] = "Gemini Clock";

const char ethNameAngelRingText[16] = "Angel Ring";

const char ethNameIncreasesHISlotTechAttackCriticalText[48] = "Increases HI Slot Tech Attack Critical Hit rate";

const char ethNameBattleMaskText[16] = "Battle Mask";

const char ethNameDoubleBusterText[16] = "Double Buster";

const char ethNameIncreasesSpeed25Format[24] = "Increases speed 25%";

const char ethNameSpeedShoesText[16] = "Speed Shoes";

const char ethNameGoldenDiceText[16] = "Golden Dice";

const char ethNameEachTurnAP1CharactersOnlyText[40] = "Each turn AP +1\n(characters only) ";

const char ethNameCommanderSCrestText[24] = "Commander's Crest";

const char ethNameIncreasesRareItemAcquisitionRate30Format2[48] = "Increases rare item acquisition rate 30%";

const char ethNameCatBurglarGlovesText[24] = "Cat Burglar Gloves";

const char ethNameIncreasesRareItemAcquisitionRate10Format2[48] = "Increases rare item acquisition rate 10%";

const char ethNameThiefRingText[16] = "Thief Ring";

const char ethNameIncreasesMaxEP15Format[24] = "Increases Max EP 15%";

const char ethNameSilverCrownText[16] = "Silver Crown";

const char ethNameYText[8] = "Y";

const char ethNameIncreasesMaxHP15Format[24] = "Increases Max HP 15%";

const char ethNameYamatoBeltText[16] = "Yamato Belt";

const char ethNameDamageDown10Format[16] = "Damage down 10%";

const char ethNameCrossText[8] = "Cross";

const char ethNameIncreasesCounterRate1012Format[64] = "Increases counter rate 10%\n1/2 BG accumulation during counter";

const char ethNameSamuraiHeartText[16] = "Samurai Heart";

const char ethNameIncreasesCriticalHitRate5Format[32] = "Increases Critical Hit rate 5%";

const char ethNameHunterGoggleText[16] = "Hunter Goggle";

const char ethNameAgility1Text[16] = "Agility +1";

const char ethNameWhiteRingText[16] = "White Ring";

const char ethNameOText[8] = "O";

const char ethNameEtherDef2Text[16] = "Ether Def +2";

const char ethNameOrangeRingText[16] = "Orange Ring";

const char ethNameEtherAtk2Text[16] = "Ether Atk +2";

const char ethNamePurpleRingText[16] = "Purple Ring";

const char ethNameEvade2Text[16] = "Evade +2";

const char ethNameKoboldRingText[16] = "Kobold Ring";

const char ethNameDexterity2Text[16] = "Dexterity +2";

const char ethNameBlueRingText[16] = "Blue Ring";

const char ethNamePhysDef2Text[16] = "Phys Def +2";

const char ethNameGreenRingText[16] = "Green Ring";

const char ethNamePhysAtk2Text[16] = "Phys Atk +2";

const char ethNameRedRingText[16] = "Red Ring";

const char ethNameChakraShieldText[16] = "Chakra Shield";

const char ethNamePreventsAttackDisableText2[32] = "Prevents \"Attack Disable\" ";

const char ethNameSoldierSHonorText[16] = "Soldier's Honor";

const char ethNamePreventsInstantKOText2[24] = "Prevents \"Instant KO\" ";

const char ethNameDefibrillatorVestText[24] = "Defibrillator Vest";

const char ethNamePreventsPoisonText2[24] = "Prevents \"Poison\" ";

const char ethNameSnakeHunterText[16] = "Snake Hunter";

const char ethNamePreventsSlowText2[24] = "Prevents \"Slow\" ";

const char ethNameClockShieldText[16] = "Clock Shield";

const char ethNamePreventsAPHalfText2[24] = "Prevents \"AP Half\" ";

const char ethNameJadeMaskText[16] = "Jade Mask";

const char ethNamePreventsEtherAtkDownText2[32] = "Prevents \"Ether Atk Down\" ";

const char ethNameSpiritPendantText[16] = "Spirit Pendant";

const char ethNamePreventsPhysDefDownText2[32] = "Prevents \"Phys Def Down\" ";

const char ethNameGuardPendantText[16] = "Guard Pendant";

const char ethNamePreventsPhysAtkDownText2[32] = "Prevents \"Phys Atk Down\" ";

const char ethNamePowerBraceText[16] = "Power Brace";

const char ethNamePreventsEvadeDownText2[24] = "Prevents \"Evade Down\" ";

const char ethNameHyperShoesText[16] = "Hyper Shoes";

const char ethNamePreventsDexterityDownText2[32] = "Prevents \"Dexterity Down\" ";

const char ethNameSniperGogglesText[16] = "Sniper Goggles";

const char ethNamePreventsEPOverconsumptionText2[32] = "Prevents \"EP Overconsumption\" ";

const char ethNameRuneCrystalText[16] = "Rune Crystal";

const char ethNamePreventsCurseText[24] = "Prevents \"Curse\" ";

const char ethNameWoodenIdolText[16] = "Wooden Idol";

const char ethNameVenomBlockText[16] = "Venom Block";

const char ethNamePreventsCriticalHitsText[32] = "Prevents \"Critical Hits\"";

const char ethNameTraumaPlateText[16] = "Trauma Plate";

const char ethNameAuxiliaryArmorForArmsPhysDefText[40] = "Auxiliary armor for arms\nPhys Def +2";

const char ethNameProtectorText[16] = "Protector";

const char ethNameIncreasesEPtsEarned25PhysFormat[40] = "Increases E.Pts earned 25%\nPhys Def +4";

const char ethNameStarHatText[16] = "Star Hat";

const char ethNamePreventsLostPhysDef4Text[32] = "Prevents \"Lost\" \nPhys Def +4";

const char ethNameMagicalHatText[16] = "Magical Hat";

const char ethNamePreventsSleepPhysDef4Text[32] = "Prevents \"Sleep\"\nPhys Def +4";

const char ethNameNightwalkerText[16] = "Nightwalker";

const char ethNameHatReserveText[16] = "Hat Reserve";

const char ethNameMatchesTechtronClothesOutfitPhysDefText[48] = "Matches Techtron Clothes outfit\nPhys Def +8";

const char ethNameTechtronHelmetText[16] = "Techtron Helmet";

const char ethNameMatchesRubySuitOutfitPhysDefText[40] = "Matches Ruby Suit outfit\nPhys Def +6";

const char ethNameRubyHelmetText[16] = "Ruby Helmet";

const char ethNameSpaceUseHelmetPhysDef4Text[32] = "Space-use helmet\nPhys Def +4";

const char ethNameSpaceHelmetText[16] = "Space Helmet";

const char ethNameMatchesSurvivalWearOutfitPhysDefText[48] = "Matches Survival Wear outfit\nPhys Def +2";

const char ethNameCowboyHatText[16] = "Cowboy Hat";

const char ethNameMatchesMetalWearOutfitPhysDefText[40] = "Matches Metal Wear outfit\nPhys Def +2";

const char ethNameMetalHelmetText[16] = "Metal Helmet";

const char ethNameIncreasesTPtsEarned25PhysFormat[40] = "Increases T.Pts earned 25%\nPhys Def +1";

const char ethNameSwimsuitText[16] = "Swimsuit";

const char ethNameSpecialClothingReserveText[32] = "Special Clothing Reserve";

const char ethNameIncreasesEtherDefAsAlliesAreText2[56] = "Increases Ether Def as allies are KO'd\nPhys Def +20";

const char ethNameSpiritText[8] = "Spirit";

const char ethNameIncreasesPhysDefAsAlliesAreText2[56] = "Increases Phys Def as allies are KO'd\nPhys Def +20";

const char ethNameSoulText[8] = "Soul";

const char ethNameReducesSlshPrceTypeDamage20Format2[48] = "Reduces Slsh/Prce-type damage 20%\nPhys Def +2";

const char ethNameCoatSwordText[16] = "Coat\241\246Sword";

const char ethNameReducesBTypeDamage20PhysFormat[40] = "Reduces B-type damage 20%\nPhys Def +2";

const char ethNameCoatBeamText[16] = "Coat\241\246Beam";

const char ethNameReducesITypeDamage20PhysFormat[40] = "Reduces I-type damage 20%\nPhys Def +2";

const char ethNameCoatIceText[16] = "Coat\241\246Ice";

const char ethNameReducesLTypeDamage20PhysFormat[40] = "Reduces L-type damage 20%\nPhys Def +2";

const char ethNameCoatLightningText[16] = "Coat\241\246Lightning";

const char ethNameReducesFTypeDamage20PhysFormat[40] = "Reduces F-type damage 20%\nPhys Def +2";

const char ethNameCoatFireText[16] = "Coat\241\246Fire";

const char ethNameIncreasesSPtsEarned25PhysFormat[40] = "Increases S.Pts earned 25%\nPhys Def +2";

const char ethNameCraftApronText[16] = "Craft Apron";

const char ethNamePreventsAllStatusEffectsAbnormalitiesPhysText[56] = "Prevents all status effects/abnormalities\nPhys Def +36";

const char ethNameShieldArmorText[16] = "Shield Armor";

const char ethNameCyborgArmorUnitPhysDef44Text[32] = "Cyborg Armor Unit\nPhys Def +44";

const char ethNameVOLG50Text[8] = "VOLG50";

const char ethNameCyborgArmorUnitPhysDef36Text[32] = "Cyborg Armor Unit\nPhys Def +36";

const char ethNameVOLG40Text[8] = "VOLG40";

const char ethNameCyborgArmorUnitPhysDef28Text[32] = "Cyborg Armor Unit\nPhys Def +28";

const char ethNameVOLG30Text[8] = "VOLG30";

const char ethNameCyborgArmorUnitPhysDef16Text[32] = "Cyborg Armor Unit\nPhys Def +16";

const char ethNameVOLG20Text[8] = "VOLG20";

const char ethNameCyborgArmorUnitPhysDef3Text[32] = "Cyborg Armor Unit\nPhys Def +3";

const char ethNameVOLG10Text[8] = "VOLG10";

const char ethNameClothingMadeForAdventureEnthusiastsPhysText[56] = "Clothing made for adventure enthusiasts\nPhys Def +12";

const char ethNameSurvivalWearText[16] = "Survival Wear";

const char ethNameRegularClothingReserveText[32] = "Regular Clothing Reserve";

const char ethNameKOSMOSArmorUnitPhysDefText[32] = "KOS-MOS Armor Unit\nPhys Def +48";

const char ethNameDUnitV6Text[16] = "D Unit V6";

const char ethNameKOSMOSArmorUnitPhysDefText2[32] = "KOS-MOS Armor Unit\nPhys Def +38";

const char ethNameDUnitV5Text[16] = "D Unit V5";

const char ethNameKOSMOSArmorUnitPhysDefText3[32] = "KOS-MOS Armor Unit\nPhys Def +30";

const char ethNameDUnitV4Text[16] = "D Unit V4";

const char ethNameKOSMOSArmorUnitPhysDefText4[32] = "KOS-MOS Armor Unit\nPhys Def +20";

const char ethNameDUnitV3Text[16] = "D Unit V3";

const char ethNameKOSMOSArmorUnitPhysDefText5[32] = "KOS-MOS Armor Unit\nPhys Def +14";

const char ethNameDUnitV2Text[16] = "D Unit V2";

const char ethNameKOSMOSArmorUnitPhysDefText6[32] = "KOS-MOS Armor Unit\nPhys Def +4";

const char ethNameDUnitV1Text[16] = "D Unit V1";

const char ethNameTheLatestTopOfTheLineText[56] = "The latest top-of-the-line combat wear\nPhys Def +38";

const char ethNameTechtronClothesText[24] = "Techtron Clothes";

const char ethNameReinforcedClothingThatGeneratesADefensiveText[72] = "Reinforced clothing that generates a defensive force-field\nPhys Def +30";

const char ethNameRubySuitText[16] = "Ruby Suit";

const char ethNameHasEDEF4SkillPhysDefText[32] = "Has \"EDEF+4\" skill\nPhys Def +30";

const char ethNameNeoArmorText[16] = "Neo Armor \246\302";

const char ethNameHasPDEF4SkillPhysDefText[32] = "Has \"PDEF+4\" skill\nPhys Def +30";

const char ethNameNeoArmorText2[16] = "Neo Armor \246\301";

const char ethNameReinforcedClothingMadeForBattlesInText[64] = "Reinforced clothing made for battles in the arctic\nPhys Def +24";

const char ethNameBattleGearText[16] = "Battle Gear";

const char ethNameReinforcedClothingThatAutoAdjustsToText[80] = "Reinforced clothing that auto-adjusts to people's body shape\nPhys Def +18";

const char ethNameStylishArmorText[16] = "Stylish Armor";

const char ethNameClothingWovenOutOfHighStrengthText[56] = "Clothing woven out of high-strength fibers\nPhys Def +12";

const char ethNameFiberSuitText[16] = "Fiber Suit";

const char ethNameClothingWovenWithSpecialAlloyThreadsText[56] = "Clothing woven with special alloy threads\nPhys Def +8";

const char ethNameMetalWearText[16] = "Metal Wear";

const char ethNameUniformForFemaleVectorEmployeesPhysText[48] = "Uniform for female Vector employees\nPhys Def +3";

const char ethNameUniformText[8] = "Uniform";

const char ethNameAllAirPierceFederationArmyHandText[48] = "All/Air/Pierce\nFederation Army hand missile pod";

const char ethNameAllAlliesEtherShieldDefensiveShieldText[64] = "All Allies/Ether Shield\nDefensive shield manufactured by Vector";

const char ethNameDEFVXText[8] = "DEF-VX";

const char ethNameAllAirBeamEA300LanceManufacturedFormat[56] = "All/Air/Beam EA300%\nLance manufactured by Vector";

const char ethNameLWVX2Text[8] = "LW-VX2";

const char ethNameSingleNearSlashWACTElectroText[64] = "Single/Near/Slash/W-ACT\nElectro shooter manufactured by Vector";

const char ethNameERVXText[8] = "ER-VX";

const char ethNameAllAirHitFederationArmyMissileText[40] = "All/Air/Hit\nFederation Army missile pod";

const char ethNameSMP53AGText[8] = "SMP53AG";

const char ethNameAllAirSECMPodManufacturedText[48] = "All/Air/S\nECM pod manufactured by Vector";

const char ethNameECM2VXText[8] = "ECM2-VX";

const char ethNameSingleLineSECMPodManufacturedText[48] = "Single/Line/S\nECM pod manufactured by Vector";

const char ethNameECM1VXText[8] = "ECM1-VX";

const char ethNameAllAirBeamEA250FederationArmyFormat[48] = "All/Air/Beam EA250%\nFederation Army aerials";

const char ethNameAIRDAG2Text[16] = "AIRD-AG2";

const char ethNameSingleLineBeamEA600FederationArmyFormat[64] = "Single/Line/Beam EA600%\nFederation Army large beam cannon";

const char ethNameBBCAG5Text[8] = "BBC-AG5";

const char ethNameDefenseShieldManufacturedByVectorText[40] = "Defense\nShield manufactured by Vector";

const char ethNameSHD12VXText[8] = "SHD12VX";

const char ethNameDefenseFederationArmyShieldText[32] = "Defense\nFederation Army shield";

const char ethNameSHD02AGText[8] = "SHD02AG";

const char ethNameSingleAirBeamEA130DefenseFederationFormat[72] = "Single/Air/Beam EA130%/Defense\nFederation Army shield beam rifle";

const char ethNameSHB67AGText[8] = "SHB67AG";

const char ethNameAllAirHitFederationArmyLargeText[48] = "All/Air/Hit\nFederation Army large missile pod";

const char ethNameBMPAG5Text[8] = "BMP-AG5";

const char ethNameSingleLinePierceFederationArmyLongText[48] = "Single/Line/Pierce\nFederation Army long cannon";

const char ethNameLCAG5Text[8] = "LC-AG5";

const char ethNameAllAirBeamEA130WideBeamFormat[64] = "All/Air/Beam EA130%\nWide beam pod manufactured by Vector";

const char ethNameBMP45VXText[8] = "BMP45VX";

const char ethNameAIRCAG2Text[16] = "AIRC-AG2";

const char ethNameEnemiesAllAlliesAirSChaffText[64] = "Enemies & All Allies/Air/S\nChaff box manufactured by Vector";

const char ethNameCB85VXText[8] = "CB85VX";

const char ethNameSingleLineBeamEA200FederationArmyFormat[56] = "Single/Line/Beam EA200%\nFederation Army beam launcher";

const char ethNameBL24AGText[8] = "BL24AG";

const char ethNameHMPAG5Text[8] = "HMP-AG5";

const char ethNameAllAirHitFederationArmyHandText[48] = "All/Air/Hit\nFederation Army hand missile pod";

const char ethNameHMP33AGText[8] = "HMP33AG";

const char ethNameHGGAG5Text[8] = "HGG-AG5";

const char ethNameSingleLinePierceWACTFederationText[56] = "Single/Line/Pierce/W-ACT\nFederation Army gatling gun";

const char ethNameGLG76AGText[8] = "GLG76AG";

const char ethNameSingleLineFireWACTFederationText[56] = "Single/Line/Fire/W-ACT\nFederation Army grenade launcher";

const char ethNameGRD20AGText[8] = "GRD20AG";

const char ethNameSingleLineFireEA150WACTFormat[64] = "Single/Line/Fire EA150%/W-ACT\nFederation Army flame launcher";

const char ethNameFLM64AGText[8] = "FLM64AG";

const char ethNameSingleNearPierceFederationArmyBunkerText[56] = "Single/Near/Pierce\nFederation Army bunker buster";

const char ethNamePB55AGText[8] = "PB55AG";

const char ethNameSingleNearSlashWACTLancerText[64] = "Single/Near/Slash/W-ACT\nLancer arm manufactured by Vector";

const char ethNameLM11VXText[8] = "LM11VX";

const char ethNameSingleNearSlashBeamWACTText[56] = "Single/Near/Slash/Beam/W-ACT\nFederation Army beam sword";

const char ethNameBSW13AGText[8] = "BSW13AG";

const char ethNameSingleLineBeamEA185WACTFormat[64] = "Single/Line/Beam EA185%/W-ACT\nBeam arm manufactured by Vector";

const char ethNameBA15VXText[8] = "BA15VX";

const char ethNameSingleLineBeamEA300LongBeamFormat[64] = "Single/Line/Beam EA300%\nLong beam rifle manufactured by Vector";

const char ethNameLG100VXText[8] = "LG100VX";

const char ethNameSingleLineBeamEA120WACTFormat[64] = "Single/Line/Beam EA120%/W-ACT\nBeam rifle manufactured by Vector";

const char ethNameLG24VXText[8] = "LG24VX";

const char ethNameSingleLinePierceWACTFederationText2[48] = "Single/Line/Pierce/W-ACT\nFederation Army rifle";

const char ethNameLG10AGText[8] = "LG10AG";

const char ethNameSingleLinePierceWACTSubmachineText[64] = "Single/Line/Pierce/W-ACT\nSubmachine gun manufactured by Vector";

const char ethNameSMG32VXText[8] = "SMG32VX";

const char ethNameSingleLinePierceWACTFederationText3[56] = "Single/Line/Pierce/W-ACT\nFederation Army submachine gun";

const char ethNameSMG99AGText[8] = "SMG99AG";

const char ethNameSingleNearHitFederationArmyHeavyText[48] = "Single/Near/Hit\nFederation Army heavy hammer";

const char ethNameHMRAG5Text[8] = "HMR-AG5";

const char ethNameSingleNearSlashWACTFederationText[56] = "Single/Near/Slash/W-ACT\nFederation Army gyro saucer";

const char ethNameWCT02AG4Text[16] = "WCT02AG4";

const char ethNameSingleNearHitWACTFederationText[48] = "Single/Near/Hit/W-ACT\nFederation Army hammer";

const char ethNameHMR55AGText[8] = "HMR55AG";

const char ethNameHG75VXText[8] = "HG75VX";

const char ethNameSingleLinePierceWACTHandgunText[56] = "Single/Line/Pierce/W-ACT\nHandgun manufactured by Vector";

const char ethNameHG45VXText[8] = "HG45VX";

const char ethNameSingleNearPierceWACTFederationText[56] = "Single/Near/Pierce/W-ACT\nFederation Army drill claw";

const char ethNameDLCO2AG4Text[16] = "DLCO2AG4";

const char ethNameSingleNearSlashWACTVectorText[40] = "Single/Near/Slash/W-ACT\nVector Sword";

const char ethNameSWD34VXText[8] = "SWD34VX";

const char ethNameSingleNearSlashWACTFederationText2[48] = "Single/Near/Slash/W-ACT\nFederation Army sword";

const char ethNameSWD21AGText[8] = "SWD21AG";

const char ethNameSingleNearSlashWACTFederationText3[48] = "Single/Near/Slash/W-ACT\nFederation Army axe";

const char ethNameAXE11AGText[8] = "AXE11AG";

const char ethNameSingleNearHitText[16] = "Single/Near/Hit";

const char ethNameHANDText[8] = "HAND";

const char ethNameShionSpecialWeaponManufacturedByVectorText[48] = "Shion: Special Weapon manufactured by Vector";

const char ethNameMWSText[8] = "M.W.S.";

const char ethNameJrLegendaryGunHailedAsTheText[56] = "Jr.: Legendary gun hailed as the ultimate firearm";

const char ethNameBLOOD9Text[8] = "BLOOD9";

const char ethNameJrAnOriginalGunMadeByText[48] = "Jr.: An original gun made by a private gunsmith";

const char ethNameCROSSText[8] = "CROSS";

const char ethNameJrSecondGenerationGunBySaifarText[48] = "Jr.: Second generation gun by Saifar Co.";

const char ethNameSAIFAR45Text[16] = "SAIFAR45";

const char ethNameJrAnHeirloomQualityAntiqueGunText[40] = "Jr.: An heirloom-quality antique gun";

const char ethNameMAKAROVText[8] = "MAKAROV";

const char ethNameJrStandardRookCoGunText[32] = "Jr.: Standard Rook Co. gun";

const char ethNameROOK505Text[8] = "ROOK505";

const char ethNameAllLineHitZiggySMissileText[40] = "All/Line/Hit\nZiggy's missile pod";

const char ethNameMSP89SXText[8] = "MSP89SX";

const char ethNameAllLineBeamEA135ZiggySFormat[40] = "All/Line/Beam EA135%\nZiggy's beam pod";

const char ethNameBMP55SXText[8] = "BMP55SX";

const char ethNameMOMOSlowAgainstBGTypesText[48] = "MOMO: \"Slow\" against B, G types\nPhys Atk +7";

const char ethNamePenguinRodText[16] = "Penguin Rod";

const char ethNameMOMOScepterWithConcentratedEnergyWithinText[64] = "MOMO: Scepter with concentrated energy within\nPhys Atk +20";

const char ethNameDragonRodText[16] = "Dragon Rod";

const char ethNameMOMOAHeroSScepterMadeText[56] = "MOMO: A hero's scepter made to combat evil\nPhys Atk +12";

const char ethNameSaintRodText[16] = "Saint Rod";

const char ethNameMOMOConfusionAgainstBGTypesText[56] = "MOMO: \"Confusion\" against B, G types\nPhys Atk +10";

const char ethNameWHammerRodText[16] = "W Hammer Rod";

const char ethNameMOMOScepterMadeFromRareMaterialsText[56] = "MOMO: Scepter made from rare materials\nPhys Atk +9";

const char ethNameMithrilRodText[16] = "Mithril Rod";

const char ethNameMOMOLightPowerfulScepterPhysAtkText[48] = "MOMO: Light, powerful scepter\nPhys Atk +7";

const char ethNamePlatinumRodText[16] = "Platinum Rod";

const char ethNameMOMOHardAndHeavyBattleScepterText[48] = "MOMO: Hard and heavy battle scepter\nPhys Atk +5";

const char ethNameMetalRodText[16] = "Metal Rod";

const char ethNameMOMOTypicalSelfDefenseScepterPhysText[48] = "MOMO: Typical self-defense scepter\nPhys Atk +3";

const char ethNameBattleRodText[16] = "Battle Rod";

const char ethNameAllLineNoneEA300ScytheThatFormat[56] = "All/Line/None EA300%\nScythe that emits an Ether field";

const char ethNameFSCYTHEText[16] = "F\241\246SCYTHE";

const char ethNameSingleLinePierceShortRailCannonText[40] = "Single/Line/Pierce\nShort Rail Cannon";

const char ethNameFRSHOTText[16] = "F\241\246RSHOT";

const char ethNameSingleLineBeamEA120DragonSkullFormat[40] = "Single/Line/Beam EA120%\nDragon Skull";

const char ethNameFBSHOTText[16] = "F\241\246BSHOT";

const char ethNameAllAirHitArmMissilePodText[32] = "All/Air/Hit\nArm missile pod";

const char ethNameFMSHOTText[16] = "F\241\246MSHOT";

const char ethNameSingleLinePierceTripleGatlingGunText[40] = "Single/Line/Pierce\nTriple gatling gun";

const char ethNameFGSHOTText[16] = "F\241\246GSHOT";

const char ethNameChaosWornOutGlovesWithHolesText[56] = "chaos: Worn-out gloves (with holes)\nPhys Atk +16";

const char ethNameHoleyGlovesText[16] = "Holey Gloves";

const char ethNameChaosGlovesForDoItYourselfersText[48] = "chaos: Gloves for do-it-yourselfers\nPhys Atk +9";

const char ethNameWorkGlovesText[16] = "Work Gloves";

const char ethNameChaosTypicalRidingGlovesForWinterText[64] = "chaos: Typical riding gloves (for winter use)\nPhys Atk +7";

const char ethNameRiderGlovesText[16] = "Rider Gloves";

const char ethNameChaosMilitaryIssueGlovesMadeOfText[72] = "chaos: Military-issue gloves made of special materials\nPhys Atk +5";

const char ethNameNavyGlovesText[16] = "Navy Gloves";

const char ethNameChaosGlovesUsedWhenWorkingInText[56] = "chaos: Gloves used when working in space\nPhys Atk +3";

const char ethNameSpaceGlovesText[16] = "Space Gloves";

const char ethNameReserve10Text[16] = "Reserve10";

const char ethNameReserve9Text[16] = "Reserve9";

const char ethNameReserve8Text[16] = "Reserve8";

const char ethNameReserve7Text[16] = "Reserve7";

const char ethNameReserve6Text[16] = "Reserve6";

const char ethNameReserve5Text[16] = "Reserve5";

const char ethNameReserve4Text[16] = "Reserve4";

const char ethNameReserve3Text[16] = "Reserve3";

const char ethNameReserve2Text[16] = "Reserve2";

const char ethNameReserve1Text[16] = "Reserve1";

const char ethNameLCAG5LongCannonAmmoText[24] = "LC-AG5/Long cannon ammo";

const char ethNameLCB1Text[8] = "LCB1";

const char ethNameHGGAG5GatlingGunBulletsText[32] = "HGG-AG5/Gatling gun bullets";

const char ethNameHGGB1Text[8] = "HGGB1";

const char ethNameHMPAG5HandMissilePodAmmoText[32] = "HMP-AG5/Hand missile pod ammo";

const char ethNameHMPB1Text[8] = "HMPB1";

const char ethNameBMPB1Text[8] = "BMPB1";

const char ethNameECM2VXECMPodBulletsWearText[48] = "ECM2-VX/ECM pod bullets\n\"Wear\" against M types";

const char ethNameECM2B5Text[8] = "ECM2B5";

const char ethNameECM2VXECMPodBulletsSlowText[48] = "ECM2-VX/ECM pod bullets\n\"Slow\" against M types";

const char ethNameECM2B4Text[8] = "ECM2B4";

const char ethNameECM2VXECMPodBulletsDexterityText[64] = "ECM2-VX/ECM pod bullets\n\"Dexterity Down\" against M types";

const char ethNameECM2B3Text[8] = "ECM2B3";

const char ethNameECM2VXECMPodBulletsPhysText[56] = "ECM2-VX/ECM pod bullets\n\"Phys Def Down\" against M types";

const char ethNameECM2B2Text[8] = "ECM2B2";

const char ethNameECM2VXECMPodBulletsPhysText2[56] = "ECM2-VX/ECM pod bullets\n\"Phys Atk Down\" against M types";

const char ethNameECM2B1Text[8] = "ECM2B1";

const char ethNameSMG32VXSubmachineGunBulletsText[32] = "SMG32VX/Submachine gun bullets";

const char ethNameSMG32B1Text[8] = "SMG32B1";

const char ethNameSMP53AGMissilePodAmmoText[32] = "SMP53AG/Missile pod ammo";

const char ethNameSMP53B1Text[8] = "SMP53B1";

const char ethNameAXE11B1Text[8] = "AXE11B1";

const char ethNameGLG76AGGatlingGunBulletsText[32] = "GLG76AG/Gatling gun bullets";

const char ethNameGLG76B1Text[8] = "GLG76B1";

const char ethNameHG75VXHandgunBulletsText[24] = "HG75VX/Handgun bullets";

const char ethNameHG75B1Text[8] = "HG75B1";

const char ethNameECM1VXECMPodBulletsWearText[48] = "ECM1-VX/ECM pod bullets\n\"Wear\" against M types";

const char ethNameECM1B5Text[8] = "ECM1B5";

const char ethNameECM1VXECMPodBulletsSlowText[48] = "ECM1-VX/ECM pod bullets\n\"Slow\" against M types";

const char ethNameECM1B4Text[8] = "ECM1B4";

const char ethNameECM1VXECMPodBulletsDexterityText[64] = "ECM1-VX/ECM pod bullets\n\"Dexterity Down\" against M types";

const char ethNameECM1B3Text[8] = "ECM1B3";

const char ethNameECM1VXECMPodBulletsArmorText[56] = "ECM1-VX/ECM pod bullets\n\"Armor Failure\" against M types";

const char ethNameECM1B2Text[8] = "ECM1B2";

const char ethNameECM1VXECMPodBulletsPowerText[56] = "ECM1-VX/ECM pod bullets\n\"Power Loss\" against M types";

const char ethNameECM1B1Text[8] = "ECM1B1";

const char ethNameCB85B5Text[8] = "CB85B5";

const char ethNameCB85VXChaffBoxBulletsPhysDefText[64] = "CB85VX/Chaff box bullets\n\"Phys Def Down\" against B, G types";

const char ethNameCB85B4Text[8] = "CB85B4";

const char ethNameCB85VXChaffBoxBulletsPhysAtkText[64] = "CB85VX/Chaff box bullets\n\"Phys Atk Down\" against B, G types";

const char ethNameCB85B3Text[8] = "CB85B3";

const char ethNameCB85VXChaffBoxBulletsEtherAtkText[64] = "CB85VX/Chaff box bullets\n\"Ether Atk Down\" against B, G types";

const char ethNameCB85B2Text[8] = "CB85B2";

const char ethNameCB85VXChaffBoxBulletsDexterityDownText[64] = "CB85VX/Chaff box bullets\n\"Dexterity Down\" against M types";

const char ethNameCB85B1Text[8] = "CB85B1";

const char ethNameNoBulletsText[16] = "No bullets";

const char ethNameLG100B1Text[8] = "LG100B1";

const char ethNameHMP33HandMissilePodAmmoText[32] = "HMP33/Hand missile pod ammo";

const char ethNameHMP33B1Text[8] = "HMP33B1";

const char ethNameGRD20AGGrenadeAmmoText[24] = "GRD20AG/Grenade ammo";

const char ethNameGRD20B1Text[8] = "GRD20B1";

const char ethNameSMG99AGSubmachineGunBulletsText[32] = "SMG99AG/Submachine gun bullets";

const char ethNameSMG99B1Text[8] = "SMG99B1";

const char ethNameLG10AGRifleBulletsText[24] = "LG10AG/Rifle bullets";

const char ethNameLG10B1Text[8] = "LG10B1";

const char ethNameHG45VXHandgunBulletsText[24] = "HG45VX/Handgun bullets";

const char ethNameHG45B1Text[8] = "HG45B1";

const char ethNameJrBulletsForCROSSEtherAtkText[64] = "Jr.: Bullets for \"CROSS\"\n\"Ether Atk Down\" against B, G types";

const char ethNameCS700UVLText[16] = "CS700UVL";

const char ethNameJrBulletsForCROSSPhysDefText[64] = "Jr.: Bullets for \"CROSS\"\n\"Phys Def Down\" against B, G types";

const char ethNameCS700DFDText[16] = "CS700DFD";

const char ethNameJrBulletsForCROSSPhysAtkText[64] = "Jr.: Bullets for \"CROSS\"\n\"Phys Atk Down\" against B, G types";

const char ethNameCS700PWDText[16] = "CS700PWD";

const char ethNameJrBulletsForCROSSSlowAgainstText[56] = "Jr.: Bullets for \"CROSS\"\n\"Slow\" against B, G types";

const char ethNameCS700SLWText[16] = "CS700SLW";

const char ethNameJrBulletsForBLOOD9PhysAtkText[40] = "Jr.: Bullets for \"BLOOD9\"\nPhys Atk +16";

const char ethNameBD900Text[8] = "BD900";

const char ethNameJrBulletsForCROSSPhysAtkText2[40] = "Jr.: Bullets for \"CROSS\"\nPhys Atk +10";

const char ethNameCS700Text[8] = "CS700";

const char ethNameJrBulletsForSAIFARPhysAtkText[40] = "Jr.: Bullets for \"SAIFAR\"\nPhys Atk +8";

const char ethNameSR500Text[8] = "SR500";

const char ethNameJrBulletsForROOK505PhysAtkText[40] = "Jr.: Bullets for \"ROOK505\"\nPhys Atk +6";

const char ethNameRK280Text[8] = "RK280";

const char ethNameJrValuableBulletsForMAKAROVGunText[56] = "Jr.: Valuable bullets for MAKAROV gun\nPhys Atk +5";

const char ethNameMAKAROVBulletsText[16] = "MAKAROV Bullets";

const char ethNameJrBulletsForROOK505PhysAtkText2[40] = "Jr.: Bullets for \"ROOK505\"\nPhys Atk +4";

const char ethNameRK200Text[8] = "RK200";

const char ethNameMSS580Text[8] = "MSS580";

const char ethNameMSS560Text[8] = "MSS560";

const char ethNameZiggyMissilesForMSP89SXPhysAtkText[48] = "Ziggy: Missiles for \"MSP89SX\"\nPhys Atk +12";

const char ethNameMSS540Text[8] = "MSS540";

const char ethNameMSS520Text[8] = "MSS520";

const char ethNameZiggyMissilesForMSP89SXPhysAtkText2[48] = "Ziggy: Missiles for \"MSP89SX\"\nPhys Atk +4";

const char ethNameMSS500Text[8] = "MSS500";

const char ethNameKRS54VText[8] = "KRS54V";

const char ethNameKMS38VText[8] = "KMS38V";

const char ethNameKGS16VText[8] = "KGS16V";

const char ethNameKMS36VText[8] = "KMS36V";

const char ethNameKRS52VText[8] = "KRS52V";

const char ethNameKMS34VText[8] = "KMS34V";

const char ethNameKGS14VText[8] = "KGS14V";

const char ethNameKOSMOSBulletsForFRSHOTText[48] = "KOS-MOS: Bullets for \"F\241\246RSHOT\"\nPhys Atk +20";

const char ethNameKRS50VText[8] = "KRS50V";

const char ethNameKMS32VText[8] = "KMS32V";

const char ethNameKGS12VText[8] = "KGS12V";

const char ethNameKOSMOSBulletsForFMSHOTText[48] = "KOS-MOS: Bullets for \"F\241\246MSHOT\"\nPhys Atk +4";

const char ethNameKMS30VText[8] = "KMS30V";

const char ethNameKOSMOSBulletsForFGSHOTText[48] = "KOS-MOS: Bullets for \"F\241\246GSHOT\"\nPhys Atk +12";

const char ethNameKGS10VText[8] = "KGS10V";

const char ethNameBXS015VText[8] = "BXS015V";

const char ethNameBXS014VText[8] = "BXS014V";

const char ethNameBXS013VText[8] = "BXS013V";

const char ethNameBXS012VText[8] = "BXS012V";

const char ethNameBXS011VText[8] = "BXS011V";

const char ethNameBXS010VText[8] = "BXS010V";

const char ethNameBXS009VText[8] = "BXS009V";

const char ethNameBXS008VText[8] = "BXS008V";

const char ethNameBXS007VText[8] = "BXS007V";

const char ethNameBXS006VText[8] = "BXS006V";

const char ethNameShionTechAttackShockBladePhysText[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Phys Def Down\" against B, G types";

const char ethNameBXS005VText[8] = "BXS005V";

const char ethNameShionTechAttackShockBladePhysText2[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Phys Atk Down\" against B, G types";

const char ethNameBXS004VText[8] = "BXS004V";

const char ethNameShionTechAttackShockBladeArmorText[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Armor Failure\" against M types";

const char ethNameBXS003VText[8] = "BXS003V";

const char ethNameShionTechAttackShockBladePowerText[64] = "Shion: Tech Attack \"Shock Blade\"\n\"Power Loss\" against M types";

const char ethNameBXS002VText[8] = "BXS002V";

const char ethNameShionTechAttackShockBladeSlowText[56] = "Shion: Tech Attack \"Shock Blade\"\n\"Slow\" against M types";

const char ethNameBXS001VText[8] = "BXS001V";

const char ethNameRushText[8] = "Rush";

const char ethNameRifleShotText[16] = "Rifle Shot";

const char ethNameHipShotText[16] = "Hip Shot";

const char ethNameCrossfireText[16] = "Crossfire";

const char ethNameCoinSnapText[16] = "Coin Snap";

const char ethNameTrickShotText[16] = "Trick Shot";

const char ethNameSouthpawText[16] = "Southpaw";

const char ethNameCrossShotText[16] = "Cross Shot";

const char ethNameCosmicFlipText[16] = "Cosmic Flip";

const char ethNameFlowerStormText[16] = "Flower Storm";

const char ethNameDreamWhirlText[16] = "Dream Whirl";

const char ethNameTwinStarsText[16] = "Twin Stars";

const char ethNameMelodyRayText[16] = "Melody Ray";

const char ethNameStardustText[16] = "Stardust";

const char ethNameJackBladeText[16] = "Jack Blade";

const char ethNameLaserSwingText[16] = "Laser Swing";

const char ethNameIgnitionText[16] = "Ignition";

const char ethNameJackKnifeText[16] = "Jack Knife";

const char ethNameLaserBladeText[16] = "Laser Blade";

const char ethNameHighKickText[16] = "High Kick";

const char ethNameSpinKickText[16] = "Spin Kick";

const char ethNameCherryBombText[16] = "Cherry Bomb";

const char ethNameStunShockText[16] = "Stun Shock";

const char ethNamePowerKickText[16] = "Power Kick";

const char ethNameFirecrackerText[16] = "Firecracker";

const char ethNameKnuckleText[8] = "Knuckle";

const char ethNameNEEDLEText[8] = "NEEDLE";

const char ethNameSWORDText[8] = "SWORD";

const char ethNameSSAULTText[8] = "S-SAULT";

const char ethNameBLASTERText[8] = "BLASTER";

const char ethNamePUNCHText[8] = "PUNCH";

const char ethNameStarThrustText[16] = "Star Thrust";

const char ethNameAngelShotText[16] = "Angel Shot";

const char ethNameElectroUpperText[16] = "Electro Upper";

const char ethNameSeraphimRushText[16] = "Seraphim Rush";

const char ethNameArrowText[8] = "Arrow";

const char ethNameTornadoSlashText[16] = "Tornado Slash";

const char ethNameOneEnemyLineSpiritAGunText[56] = "One enemy/Line/Spirit\nA gun that absorbs G-types";

const char ethNameSoulRhapsodyText[16] = "Soul Rhapsody";

const char ethNameAllEnemiesAirPierceSnipeFromText[72] = "All enemies/Air/Pierce\nSnipe from above surrounded by angel wings";

const char ethNameAngelicRequiemText[16] = "Angelic Requiem";

const char ethNameAllEnemiesAirSpiritAttackWithText[64] = "All enemies/Air/Spirit\nAttack with a spirit gun from above";

const char ethNameMysticNocturneText[16] = "Mystic Nocturne";

const char ethNameOneEnemyLinePierceConcentratedHeavyText[48] = "One enemy/Line/Pierce\nConcentrated heavy fire";

const char ethNameLastSymphonyText[16] = "Last Symphony";

const char ethNameAllEnemiesLinePierceCoinShotText[48] = "All enemies/Line/Pierce\nCoin shot attack";

const char ethNameStormWaltzText[16] = "Storm Waltz";

const char ethNameOneEnemyLineSpiritAttackWithText[48] = "One enemy/Line/Spirit\nAttack with a spirit gun";

const char ethNameMoonlitSerenadeText[24] = "Moonlit Serenade";

const char ethNameOneEnemyLinePierceCrossShotText[40] = "One enemy/Line/Pierce\nCross shot";

const char ethNamePreludeToBattleText[24] = "Prelude to Battle";

const char ethNameOneEnemyLineCanBeUsedText[72] = "One enemy/Line   Can be used only when transformed\nSteal rare items";

const char ethNameMagicCasterText[16] = "Magic Caster";

const char ethNameOneEnemyLineBeamCanBeText[72] = "One enemy/Line/Beam   Can be used only when transformed\nMOMO Beam";

const char ethNameMOMOSKissText[16] = "MOMO's Kiss";

const char ethNameOneEnemyNearHitTurnGText[72] = "One enemy/Near/Hit\nTurn G-types into items if used to defeat them";

const char ethNameDarkScepterText[16] = "Dark Scepter";

const char ethNameAllEnemiesAirPierceAttackWithText[56] = "All enemies/Air/Pierce\nAttack with arrow of light";

const char ethNameAngelArrowText[16] = "Angel Arrow";

const char ethNameAllEnemiesAirHitStrikeWithText[48] = "All enemies/Air/Hit\nStrike with a large star";

const char ethNameStarCannonText[16] = "Star Cannon";

const char ethNameOneEnemyLineNoneAttackFromText[56] = "One enemy/Line/None\nAttack from another dimension";

const char ethNameMeteorStormText[16] = "Meteor Storm";

const char ethNameOneEnemyNearSlashSlashWithText[48] = "One enemy/Near/Slash\nSlash with knife-like wind";

const char ethNameFloralTempestText[16] = "Floral Tempest";

const char ethNameOneEnemyNearHitStrikeWithText[48] = "One enemy/Near/Hit\nStrike with a small star";

const char ethNameStarStrikeText[16] = "Star Strike";

const char ethNameOneEnemyNearSlashFireFireText[64] = "One enemy/Near/Slash/Fire\nFire slash attack from the air";

const char ethNameHellFireText[16] = "Hell Fire";

const char ethNameAllEnemiesAirLightningLightningAttackText[56] = "All enemies/Air/Lightning\nLightning attack from the sky";

const char ethNameExecutionerText[16] = "Executioner";

const char ethNameOneEnemyNearPierceACrossedText[56] = "One enemy/Near/Pierce\nA crossed-blade slash attack";

const char ethNameCrossLancerText[16] = "Cross Lancer";

const char ethNameOneEnemyNearSlashATornadoText[56] = "One enemy/Near/Slash\nA tornado-like slash attack";

const char ethNameCycloneText[8] = "Cyclone";

const char ethNameAllEnemiesAirFireFireAttackText[48] = "All enemies/Air/Fire\nFire attack from the air";

const char ethNameMeteorShotText[16] = "Meteor Shot";

const char ethNameOneEnemyLineLightningLightningAttackText[64] = "One enemy/Line/Lightning\nLightning attack from the ground";

const char ethNameLightningFistText[16] = "Lightning Fist";

const char ethNameOneEnemyNearHitFireHundredText[48] = "One enemy/Near/Hit/Fire\nHundred kicks of fire";

const char ethNameCyberKickText[16] = "Cyber Kick";

const char ethNameAllEnemiesAirBeamRainOfText[40] = "All enemies/Air/Beam\nRain of beams";

const char ethNameRainBladeText[16] = "Rain Blade";

const char ethNameOneEnemyNearSlashBeamSliceText[56] = "One enemy/Near/Slash/Beam\nSlice with a beam blade";

const char ethNameLunarBladeText[16] = "Lunar Blade";

const char ethNameOneEnemyNearHitSSlowText[56] = "One enemy/Near/Hit/S\n\"Slow\" against B and G-types";

const char ethNameGravityWellText[16] = "Gravity Well";

const char ethNameOneEnemyNearS1TimeText[80] = "One enemy/Near/S/1 time effect\nSets Ether damage 2x bombs against B and G-types";

const char ethNameEtherAmpText[16] = "Ether Amp";

const char ethNameOneEnemyNearHitFireViciousText[48] = "One enemy/Near/Hit/Fire\nVicious fire attack";

const char ethNameThermalBlastText[16] = "Thermal Blast";

const char ethNameOneEnemyNearPierceSDifferentText[72] = "One enemy/Near/Pierce/S\nDifferent ST effects by equipping Cartridges";

const char ethNameShockBladeText[16] = "Shock Blade";

const char ethNameOneEnemyLineBeamConcentratedEtherText[48] = "One enemy/Line/Beam\nConcentrated Ether beam";

const char ethNameSpellRayText[16] = "Spell Ray";

const char ethNameOneEnemyNearHitLightningLightningText[48] = "One enemy/Near/Hit/Lightning\nLightning attack";

const char ethNameLightningBlastText[16] = "Lightning Blast";

const char ethNameOneEnemyNearHitAttackWithText[48] = "One enemy/Near/Hit\nAttack with a dragon arm";

const char ethNameRDRAGONText[16] = "R\241\246DRAGON";

const char ethNameOneEnemyNearHitAttackWithText2[48] = "One enemy/Near/Hit\nAttack with a hammer arm";

const char ethNameRHAMMERText[16] = "R\241\246HAMMER";

const char ethNameOneEnemyLineSMultipleStatusText[64] = "One enemy/Line/S\nMultiple status effects on B, G, and M-types";

const char ethNameSCHAINText[16] = "S\241\246CHAIN";

const char ethNameXText[8] = "X";

const char ethNameAllEnemiesLineBeamAbdominalSpreadText[56] = "All enemies/Line/Beam\nAbdominal spread beam weapon";

const char ethNameXBUSTERText[16] = "X\241\246BUSTER";

const char ethNameOneEnemyNearPierceAttackWithText[48] = "One enemy/Near/Pierce\nAttack with a drill arm";

const char ethNameRDRILLText[16] = "R\241\246DRILL";

const char ethNameOneEnemyLineBeamAttackWithText[48] = "One enemy/Line/Beam\nAttack with a beam arm";

const char ethNameRCANNONText[16] = "R\241\246CANNON";

const char ethNameOneEnemyNearSlashAttackWithText[48] = "One enemy/Near/Slash\nAttack with a sword arm";

const char ethNameRBLADEText[16] = "R\241\246BLADE";

const char ethNameOneEnemyNearPierceSpiritOnlyText[64] = "One enemy/Near/Pierce/Spirit\nOnly available when ally is KO'd";

const char ethNameDivineSpearText[16] = "Divine Spear";

const char ethNameAllEnemiesAirIceHurlDownText[56] = "All enemies/Air/Ice\nHurl down ice boulders from the air";

const char ethNameArcticBlastText[16] = "Arctic Blast";

const char ethNameOneEnemyLineSCurseAgainstText[48] = "One enemy/Line/S\n\"Curse\" against G-types";

const char ethNameDemonBanisherText[16] = "Demon Banisher";

const char ethNameOneEnemyNearSpiritSPilotText[56] = "One enemy/Near/Spirit/S\n\"Pilot KO\" against M-types";

const char ethNameChainedBlastText[16] = "Chained Blast";

const char ethNameAllEnemiesAirSpiritGiantChiText[48] = "All enemies/Air/Spirit\nGiant chi impact attack";

const char ethNameAngelBlowText[16] = "Angel Blow";

const char ethNameOneEnemyNearSlashLightningSlashText[80] = "One enemy/Near/Slash/Lightning\nSlash enemies by changing chi into lightning";

const char ethNameHeavenSWrathText[16] = "Heaven's Wrath";

const char ethNameOneEnemyLineSpiritAChiText[40] = "One enemy/Line/Spirit\nA chi wave attack";

const char ethNameLunarSealText[16] = "Lunar Seal";

const char ethNameOneEnemyNearHitSpiritImpactText[80] = "One enemy/Near/Hit/Spirit\nImpact created by focusing chi into one's fist";

const char ethNameAngelWingsText[16] = "Angel Wings";

const char ethNameBOSS120Text[16] = "BOSS-120";

const char ethNameAlbedoText[8] = "Albedo";

const char ethNameBOSS032Text[16] = "BOSS-032";

const char ethNameBOSS031Text[16] = "BOSS-031";

const char ethNameMintiaText[8] = "Mintia";

const char ethNameGreatJoeText[16] = "Great Joe";

const char ethNameBOSS028Text[16] = "BOSS-028";

const char ethNameBOSS027Text[16] = "BOSS-027";

const char ethNameSophiePeithosText[16] = "Sophie Peithos";

const char ethNameRatatoskText[16] = "Ratatosk";

const char ethNameJaldabaothText[16] = "Jaldabaoth";

const char ethNameBOSS023Text[16] = "BOSS-023";

const char ethNameBOSS022Text[16] = "BOSS-022";

const char ethNameSimeonText[8] = "Simeon";

const char ethNameMargulisText[16] = "Margulis";

const char ethNameDoppelwogelText[16] = "Doppelwogel";

const char ethNameProtoDoraText[16] = "Proto Dora";

const char ethNameRianonSeText[16] = "Rianon Se";

const char ethNameGigasText[8] = "Gigas";

const char ethNameDragonText[8] = "Dragon";

const char ethNameEinRugelText[16] = "Ein Rugel";

const char ethNameTiamatText[8] = "Tiamat";

const char ethNameGargoyleText[16] = "Gargoyle";

const char ethNameStribogText[8] = "Stribog";

const char ethNamePerunText[8] = "Perun";

const char ethNameSvarozicText[16] = "Svarozic";

const char ethNameBOSS008Text[16] = "BOSS-008";

const char ethNameDroneGXText[16] = "Drone GX";

const char ethNameBOSS006Text[16] = "BOSS-006";

const char ethNameDOMOCarrierText[16] = "DOMO Carrier";

const char ethNameGolemText[8] = "Golem";

const char ethNameGremlinText[8] = "Gremlin";

const char ethNameMinotaurText[16] = "Minotaur";

const char ethNameCyclopsText[8] = "Cyclops";

const char ethNameMr300000PointsText[24] = "Mr. 300000 Points";

const char ethNameMr100000PointsText[24] = "Mr. 100000 Points";

const char ethNameMr10000PointsText[24] = "Mr. 10000 Points";

const char ethNameMr1000PointsText[16] = "Mr. 1000 Points";

const char ethNameMr100PointsText[16] = "Mr. 100 Points";

const char ethNameMONS035Text[16] = "MONS-035";

const char ethNameMONS034Text[16] = "MONS-034";

const char ethNameMONS033Text[16] = "MONS-033";

const char ethNameMONS032Text[16] = "MONS-032";

const char ethNameMONS031Text[16] = "MONS-031";

const char ethNameMONS030Text[16] = "MONS-030";

const char ethNameMONS029Text[16] = "MONS-029";

const char ethNameMONS028Text[16] = "MONS-028";

const char ethNameMONS027Text[16] = "MONS-027";

const char ethNameMONS026Text[16] = "MONS-026";

const char ethNameMONS025Text[16] = "MONS-025";

const char ethNameMONS024Text[16] = "MONS-024";

const char ethNameMONS023Text[16] = "MONS-023";

const char ethNameMONS022Text[16] = "MONS-022";

const char ethNameMONS021Text[16] = "MONS-021";

const char ethNameMONS020Text[16] = "MONS-020";

const char ethNameMONS019Text[16] = "MONS-019";

const char ethNameMONS018Text[16] = "MONS-018";

const char ethNameMONS017Text[16] = "MONS-017";

const char ethNameMONS016Text[16] = "MONS-016";

const char ethNameMONS015Text[16] = "MONS-015";

const char ethNameMONS014Text[16] = "MONS-014";

const char ethNameMONS013Text[16] = "MONS-013";

const char ethNameMONS012Text[16] = "MONS-012";

const char ethNameMONS011Text[16] = "MONS-011";

const char ethNameMONS010Text[16] = "MONS-010";

const char ethNameMONS009Text[16] = "MONS-009";

const char ethNameMONS008Text[16] = "MONS-008";

const char ethNameDOMOText[8] = "DOMO-\246\302";

const char ethNameAcePilotText[16] = "Ace Pilot";

const char ethNameUTICSoldierXText[16] = "U-TIC Soldier X";

const char ethNameEinzatsText[8] = "Einzats";

const char ethNameDirlewangerText[16] = "Dirlewanger";

const char ethNameUTICSoldierText[16] = "U-TIC Soldier";

const char ethNameXanthosisText[16] = "Xanthosis";

const char ethNameIosysText[8] = "Iosys";

const char ethNameAmbixText[8] = "Ambix";

const char ethNameZolfoText[8] = "Zolfo";

const char ethNameMercurioText[16] = "Mercurio";

const char ethNameUTRE006Text[16] = "UTRE-006";

const char ethNameAthra26SeriesText[16] = "Athra 26 Series";

const char ethNameByproduct172Text[16] = "Byproduct 172";

const char ethNameByproduct145Text[16] = "Byproduct 145";

const char ethNameByproduct103Text[16] = "Byproduct 103";

const char ethNameUTRE001Text[16] = "UTRE-001";

const char ethNameUTMA014Text[16] = "UTMA-014";

const char ethNameDinGarethText[16] = "Din Gareth";

const char ethNameSchutzText[8] = "Schutz";

const char ethNameKubelText[8] = "Kubel";

const char ethNameUTMA010Text[16] = "UTMA-010";

const char ethNameGertzogUTText[16] = "Gertzog UT";

const char ethNameCaptoMortumText[16] = "Capto Mortum";

const char ethNameWorkDroidText[16] = "Work Droid";

const char ethNameGardisF10Text[16] = "Gardis F10";

const char ethNameGardisM1Text[16] = "Gardis M1";

const char ethNameShotCrabText[16] = "Shot Crab";

const char ethNameCyberCrabText[16] = "Cyber Crab";

const char ethNameDOMOBText[8] = "DOMO-B";

const char ethNameDOMOAText[8] = "DOMO-A";

const char ethNameFedSoldierText[16] = "Fed. Soldier";

const char ethNameDroneFText[8] = "Drone F";

const char ethNameDroneMText[8] = "Drone M";

const char ethNameDroneSPXText[16] = "Drone SPX";

const char ethNameFERE003Text[16] = "FERE-003";

const char ethNameViveText[8] = "Vive";

const char ethNameCalxText[8] = "Calx";

const char ethNameFEMA006Text[16] = "FEMA-006";

const char ethNameMeldGarethText[16] = "Meld Gareth";

const char ethNameFEMA004Text[16] = "FEMA-004";

const char ethNameFEMA003Text[16] = "FEMA-003";

const char ethNameGertzogFEText[16] = "Gertzog FE";

const char ethNameAttackDroneText[16] = "Attack Drone";

const char ethNameOudogogueText[16] = "Oudogogue";

const char ethNameDroneG3Text[16] = "Drone G3";

const char ethNameDroneG2Text[16] = "Drone G2";

const char ethNameDroneG1Text[16] = "Drone G1";

const char ethNameBaraqijalText[16] = "Baraqijal";

const char ethNameArmarosText[8] = "Armaros";

const char ethNameAzazelText[8] = "Azazel";

const char ethNameDelphyneText[16] = "Delphyne";

const char ethNameBasiliskText[16] = "Basilisk";

const char ethNameBugbearText[8] = "Bugbear";

const char ethNameWyrmText[8] = "Wyrm";

const char ethNameUnicornText[8] = "Unicorn";

const char ethNameLizardmanText[16] = "Lizardman";

const char ethNameHydraText[8] = "Hydra";

const char ethNameDemonText[8] = "Demon";

const char ethNameGelFishText[16] = "Gel Fish";

const char ethNameCerberusText[16] = "Cerberus";

const char ethNameLarvaFaceText[16] = "Larva Face";

const char ethNameLarvaDollText[16] = "Larva Doll";

const char ethNameFairyText[8] = "Fairy";

const char ethNameTrollText[8] = "Troll";

const char ethNameOgreText[8] = "Ogre";

const char ethNameKoboldText[8] = "Kobold";

const char ethNameSkyFishText[16] = "Sky Fish";

const char ethNameManticoreText[16] = "Manticore";

const char ethNameGoblinText[8] = "Goblin";

const char ethNameDebugsterText[16] = "Debugster";

const char ethNameAttackDisabledText[16] = "Attack Disabled";

const char ethNameHPHalfText[8] = "HP Half";

const char ethNameReverseText[8] = "Reverse";

const char ethNameEtherBombText[16] = "Ether Bomb";

const char ethNameAPHalfText[8] = "AP Half";

const char ethNamePhysDefDownText[16] = "Phys Def Down";

const char ethNamePhysAtkDownText[16] = "Phys Atk Down";

const char ethNameEvadeDownText[16] = "Evade Down";

const char ethNameDexterityDownText[16] = "Dexterity Down";

const char ethNamePoisonText[8] = "Poison";

const char ethNameSlowText[8] = "Slow";

const char ethNameStopText[8] = "Stop";

const char ethNameBlockBoostText[16] = "Block Boost";

const char ethNameEPOverconsumptionText[24] = "EP Overconsumption";

const char ethNameEtherPoisonText[16] = "Ether Poison";

const char ethNameCurseText[8] = "Curse";

const char ethNameAttackPoisonText[16] = "Attack Poison";

const char ethNameEtherAtkDownText[16] = "Ether Atk Down";

const char ethNameLostText[8] = "Lost";

const char ethNameSleepText[8] = "Sleep";

const char ethNameConfusionText[16] = "Confusion";

const char ethNameSafetyText[8] = "Safety";

const char ethNameAPMAXText[8] = "AP MAX";

const char ethNameEvadeUpText[16] = "Evade Up";

const char ethNameDexterityUpText[16] = "Dexterity Up";

const char ethNameSpeed50Text[16] = "Speed 50";

const char ethNameSpeed25Text2[16] = "Speed 25";

const char ethNameFocusPhysDefText[16] = "Focus Phys Def";

const char ethNameFocusPhysAtkText[16] = "Focus Phys Atk";

const char ethNamePhysDefUpText[16] = "Phys Def Up";

const char ethNamePhysAtkUpText[16] = "Phys Atk Up";

const char ethNameSTLockText[8] = "ST Lock";

const char ethNameAutoReviveText[16] = "Auto Revive";

const char ethNameFocusEtherDefText[16] = "Focus Ether Def";

const char ethNameFocusEtherAtkText[16] = "Focus Ether Atk";

const char ethNameEPX2Text[8] = "EP x2";

const char ethNameRecoveryX2Text[16] = "Recovery x2";

const char ethNameMOMOGuardText[16] = "MOMO Guard";

const char ethNameChivalryText[16] = "Chivalry";

const char ethNameCRTCMarkText[16] = "CRTC Mark";

const char ethNameWearText[8] = "Wear";

const char ethNameEtherCrashText[16] = "Ether Crash";

const char ethNameArmorFailureText[16] = "Armor Failure";

const char ethNamePowerLossText[16] = "Power Loss";

const char ethNameEngineStopText[16] = "Engine Stop";

const char ethNamePilotConfusionText[16] = "Pilot Confusion";

const char ethNamePilotSleepText[16] = "Pilot Sleep";

const char ethNamePilotKOText[16] = "Pilot KO";

const char ethNameEtherShieldText[16] = "Ether Shield";

const char ethNameBunnieText[8] = "Bunnie";

const char ethNameAG06Text[8] = "AG-06";

const char ethNameAG05Text[8] = "AG-05";

const char ethNameAG04Text[8] = "AG-04";

const char ethNameAG03Text[8] = "AG-03";

const char ethNameAG02Text[8] = "AG-02";

const char ethNameAG01Text[8] = "AG-01";

const char ethNameVX06Text[8] = "VX-06";

const char ethNameVX05Text[8] = "VX-05";

const char ethNameVX04Text[8] = "VX-04";

const char ethNameVX03Text[8] = "VX-03";

const char ethNameVX02Text[8] = "VX-02";

const char ethNameVX01Text[8] = "VX-01";

const char ethNameMaryText[8] = "Mary";

const char ethNameVirgilText[8] = "Virgil";

const char ethNameJrText[8] = "Jr.";

const char ethNameMOMOText[8] = "MOMO";

const char ethNameZiggurat8Text[16] = "Ziggurat 8";

const char ethNameShionText[8] = "Shion";

const char ethNameKOSMOSText[8] = "KOS-MOS";

const char ethNameChaosText[8] = "chaos";

const char ethNameZiggyText[8] = "Ziggy";

const char ethNameAG05G06Text[16] = "AG05-G06";

const char ethNameAG04G06Text[16] = "AG04-G06";

const char ethNameAG04G05Text[16] = "AG04-G05";

const char ethNameAG02G06Text[16] = "AG02-G06";

const char ethNameAG02G05Text[16] = "AG02-G05";

const char ethNameAG02G04Text[16] = "AG02-G04";

const char ethNameAG02G03Text[16] = "AG02-G03";

const char ethNameAGO2G02Text[16] = "AGO2-G02";

const char ethNameVX06G06Text[16] = "VX06-G06";

const char ethNameVX06G05Text[16] = "VX06-G05";

const char ethNameVX4000Text[8] = "VX-4000";

const char ethNameVX06G04Text[16] = "VX06-G04";

const char ethNameVX02G06Text[16] = "VX02-G06";

const char ethNameVX02G05Text[16] = "VX02-G05";

const char ethNameVX02G04Text[16] = "VX02-G04";

const char ethNameVX7000Text[8] = "VX-7000";

const char ethNameVX02G03Text[16] = "VX02-G03";

const char ethNameVX01G06Text[16] = "VX01-G06";

const char ethNameVX01G05Text[16] = "VX01-G05";

const char ethNameVX01G04Text[16] = "VX01-G04";

const char ethNameVX01G03Text[16] = "VX01-G03";

const char ethNameVX01G02Text[16] = "VX01-G02";

const char ethNameVX10000Text[16] = "VX-10000";

const char ethNameVX01G01Text[16] = "VX01-G01";

const char ethNameAG05F06Text[16] = "AG05-F06";

const char ethNameAG04F06Text[16] = "AG04-F06";

const char ethNameAG04F05Text[16] = "AG04-F05";

const char ethNameAG02F06Text[16] = "AG02-F06";

const char ethNameAG02F05Text[16] = "AG02-F05";

const char ethNameAG02F04Text[16] = "AG02-F04";

const char ethNameAG02F03Text[16] = "AG02-F03";

const char ethNameAG02F02Text[16] = "AG02-F02";

const char ethNameVX06F06Text[16] = "VX06-F06";

const char ethNameVX06F05Text[16] = "VX06-F05";

const char ethNameVX06F04Text[16] = "VX06-F04";

const char ethNameVX02F06Text[16] = "VX02-F06";

const char ethNameVX02F05Text[16] = "VX02-F05";

const char ethNameVX02F04Text[16] = "VX02-F04";

const char ethNameVX02F03Text[16] = "VX02-F03";

const char ethNameVX01F06Text[16] = "VX01-F06";

const char ethNameVX01F05Text[16] = "VX01-F05";

const char ethNameVX01F04Text[16] = "VX01-F04";

const char ethNameVX01F03Text[16] = "VX01-F03";

const char ethNameVX01F02Text[16] = "VX01-F02";

const char ethNameVX01F01Text[16] = "VX01-F01";



const char ethNameJpText[8] = "";
