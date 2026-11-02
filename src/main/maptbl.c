#include "common.h"
#include "main/maptbl.h"
#include "maptbl.h"

/* MapFileNameGet indexes group.names by map ID and reads group.count. */
extern const char MapFileNameMcVok12b[];

extern const char MapFileNameMcVok14b[];
extern const char MapFileNameMcVok13b[];
extern const char MapFileNameMcVok10b[];
extern const char MapFileNameMcVok09b[];
extern const char MapFileNameMcVok08b[];
extern const char MapFileNameMcVok07b[];
extern const char MapFileNameMcVok06b[];
extern const char MapFileNameMcVok05b[];
extern const char MapFileNameMcVok04b[];
extern const char MapFileNameMcVok03b[];
extern const char MapFileNameMcEls01[];
extern const char MapFileNameMcVok26[];
extern const char MapFileNameMcVok25[];
extern const char MapFileNameMcVok24[];
extern const char MapFileNameMcVok23[];
extern const char MapFileNameMcVok22[];
extern const char MapFileNameMcVok21[];
extern const char MapFileNameMcVok20[];
extern const char MapFileNameMcVok19[];
extern const char MapFileNameMcVok18[];
extern const char MapFileNameMcVok16[];
extern const char MapFileNameMcVok15[];
extern const char MapFileNameMcVok14[];
extern const char MapFileNameMcVok13[];
extern const char MapFileNameMcVok12[];
extern const char MapFileNameMcVok11[];
extern const char MapFileNameMcVok10[];
extern const char MapFileNameMcVok09[];
extern const char MapFileNameMcVok08[];
extern const char MapFileNameMcVok07[];
extern const char MapFileNameMcVok06[];
extern const char MapFileNameMcVok05[];
extern const char MapFileNameMcVok04[];
extern const char MapFileNameMcVok03[];
extern const char MapFileNameMcVok02[];
extern const char MapFileNameMcVok01[];
extern const char MapFileNameMcBtl01[];
extern const char MapFileNameMcTst13[];
extern const char MapFileNameMcTst12[];
extern const char MapFileNameMcTst11[];
extern const char MapFileNameMcTst10[];
extern const char MapFileNameMcTst09[];
extern const char MapFileNameMcTst08[];
extern const char MapFileNameMcTst07[];
extern const char MapFileNameMcTst06[];
extern const char MapFileNameMcTst05[];
extern const char MapFileNameMcTst04[];
extern const char MapFileNameMcTst03[];
extern const char MapFileNameMcTst02[];
extern const char MapFileNameMcTst01[];
extern const char MapFileNameAraikabe[];
extern const char MapFileNameMcEls06h[];
extern const char MapFileNameMcEls01h[];
extern const char MapFileNameMcEls13b[];
extern const char MapFileNameMcEls12b[];
extern const char MapFileNameMcEls08b[];
extern const char MapFileNameMcEls07b[];
extern const char MapFileNameMcEls06b[];
extern const char MapFileNameMcEls05b[];
extern const char MapFileNameMcEls04b[];
extern const char MapFileNameMcEls02b[];
extern const char MapFileNameMcEls16[];
extern const char MapFileNameMcEls15[];
extern const char MapFileNameMcEls13[];
extern const char MapFileNameMcEls12[];
extern const char MapFileNameMcEls11[];
extern const char MapFileNameMcEls10[];
extern const char MapFileNameMcEls09[];
extern const char MapFileNameMcEls08[];
extern const char MapFileNameMcEls07[];
extern const char MapFileNameMcEls06[];
extern const char MapFileNameMcEls05[];
extern const char MapFileNameMcEls04[];
extern const char MapFileNameMcEls03[];
extern const char MapFileNameMcEls02[];
extern const char MapFileNameMcSyu01[];
extern const char MapFileNameMcUmn02[];
extern const char MapFileNameMcUmn01[];
extern const char MapFileNameMcUzu08[];
extern const char MapFileNameMcUzu07[];
extern const char MapFileNameMcUzu06[];
extern const char MapFileNameMcUzu05[];
extern const char MapFileNameMcUzu04[];
extern const char MapFileNameMcUzu03[];
extern const char MapFileNameMcUzu02[];
extern const char MapFileNameMcUzu01[];
extern const char MapFileNameMcMil02[];
extern const char MapFileNameMcMil01[];
extern const char MapFileNameMcEve49[];
extern const char MapFileNameMcEve48[];
extern const char MapFileNameMcEve47[];
extern const char MapFileNameMcEve46[];
extern const char MapFileNameMcEve45[];
extern const char MapFileNameMcEve44[];
extern const char MapFileNameMcEve43[];
extern const char MapFileNameMcEve42[];
extern const char MapFileNameMcEve41[];
extern const char MapFileNameMcEve40[];
extern const char MapFileNameMcEve39[];
extern const char MapFileNameMcEve38[];
extern const char MapFileNameMcEve37[];
extern const char MapFileNameMcEve36[];
extern const char MapFileNameMcEve35[];
extern const char MapFileNameMcEve33[];
extern const char MapFileNameMcEve32[];
extern const char MapFileNameMcEve31[];
extern const char MapFileNameMcEve30[];
extern const char MapFileNameMcEve29[];
extern const char MapFileNameMcEve28[];
extern const char MapFileNameMcEve27[];
extern const char MapFileNameMcEve26[];
extern const char MapFileNameMcEve25[];
extern const char MapFileNameMcEve24[];
extern const char MapFileNameMcEve23[];
extern const char MapFileNameMcEve22[];
extern const char MapFileNameMcEve21[];
extern const char MapFileNameMcEve20[];
extern const char MapFileNameMcEve19[];
extern const char MapFileNameMcEve18[];
extern const char MapFileNameMcEve17[];
extern const char MapFileNameMcEve16[];
extern const char MapFileNameMcEve15[];
extern const char MapFileNameMcEve14[];
extern const char MapFileNameMcEve13[];
extern const char MapFileNameMcEve12[];
extern const char MapFileNameMcEve11[];
extern const char MapMcEve10Name[];
extern const char MapMcEve09Name[];
extern const char MapMcEve08Name[];
extern const char MapMcEve07Name[];
extern const char MapMcEve06Name[];
extern const char MapMcEve05Name[];
extern const char MapMcEve04Name[];
extern const char MapMcEve03Name[];
extern const char MapMcEve02Name[];
extern const char MapMcEve01Name[];
extern const char MapMcDyu04bName[];
extern const char MapMcDyu18Name[];
extern const char MapMcDyu17Name[];
extern const char MapMcDyu16Name[];
extern const char MapMcDyu15Name[];
extern const char MapMcDyu14Name[];
extern const char MapMcDyu13Name[];
extern const char MapMcDyu12Name[];
extern const char MapMcDyu10Name[];
extern const char MapMcDyu08Name[];
extern const char MapMcDyu07Name[];
extern const char MapMcDyu06Name[];
extern const char MapMcDyu05Name[];
extern const char MapMcDyu04Name[];
extern const char MapMcDyu03Name[];
extern const char MapMcDyu02Name[];
extern const char MapMcDyu01Name[];
extern const char MapMcUta19Name[];
extern const char MapMcUta18Name[];
extern const char MapMcUta17Name[];
extern const char MapMcUta16Name[];
extern const char MapMcUta15Name[];
extern const char MapMcUta14Name[];
extern const char MapMcUta13Name[];
extern const char MapMcUta12Name[];
extern const char MapMcUta11Name[];
extern const char MapMcUta10Name[];
extern const char MapMcUta09Name[];
extern const char MapMcUta08Name[];
extern const char MapMcUta07Name[];
extern const char MapMcUta06Name[];
extern const char MapMcUta05Name[];
extern const char MapMcUta04Name[];
extern const char MapMcUta03Name[];
extern const char MapMcUta02Name[];
extern const char MapMcUta01Name[];
extern const char MapMcSyo01Name[];
extern const char MapMcPro09Name[];
extern const char MapMcPro08Name[];
extern const char MapMcPro07Name[];
extern const char MapMcPro06Name[];
extern const char MapMcPro05Name[];
extern const char MapMcPro04Name[];
extern const char MapMcPro03Name[];
extern const char MapMcPro02Name[];
extern const char MapMcPro01Name[];
extern const char MapMcKas41Name[];
extern const char MapMcKas40Name[];
extern const char MapMcKas39Name[];
extern const char MapMcKas38Name[];
extern const char MapMcKas37Name[];
extern const char MapMcKas36Name[];
extern const char MapMcKas35Name[];
extern const char MapMcKas34Name[];
extern const char MapMcKas33Name[];
extern const char MapMcKas32Name[];
extern const char MapMcKas31Name[];
extern const char MapMcKas30Name[];
extern const char MapMcKas29Name[];
extern const char MapMcKas28Name[];
extern const char MapMcKas27Name[];
extern const char MapMcKas26Name[];
extern const char MapMcKas25Name[];
extern const char MapMcKas24Name[];
extern const char MapMcKas23Name[];
extern const char MapMcKas22Name[];
extern const char MapMcKas21Name[];
extern const char MapMcKas20Name[];
extern const char MapMcKas19Name[];
extern const char MapMcKas18Name[];
extern const char MapMcKas17Name[];
extern const char MapMcKas16Name[];
extern const char MapMcKas15Name[];
extern const char MapMcKas14Name[];
extern const char MapMcKas13Name[];
extern const char MapMcKas12Name[];
extern const char MapMcKas11Name[];
extern const char MapMcKas10Name[];
extern const char MapMcKas09Name[];
extern const char MapMcKas08Name[];
extern const char MapMcKas07Name[];
extern const char MapMcKas06Name[];
extern const char MapMcKas05Name[];
extern const char MapMcKas04Name[];
extern const char MapMcKas03Name[];
extern const char MapMcKas02Name[];
extern const char MapMcKas01Name[];
extern const char MapMcKuk16Name[];
extern const char MapMcKuk10bName[];
extern const char MapMcKuk09bName[];
extern const char MapMcKuk08bName[];
extern const char MapMcKuk07bName[];
extern const char MapMcKuk06bName[];
extern const char MapMcKuk05bName[];
extern const char MapMcKuk04bName[];
extern const char MapMcKuk03bName[];
extern const char MapMcKuk15Name[];
extern const char MapMcKuk14Name[];
extern const char MapMcKuk13Name[];
extern const char MapMcKuk12Name[];
extern const char MapMcKuk11Name[];
extern const char MapMcKuk10Name[];
extern const char MapMcKuk09Name[];
extern const char MapMcKuk08Name[];
extern const char MapMcKuk07Name[];
extern const char MapMcKuk06Name[];
extern const char MapMcKuk05Name[];
extern const char MapMcKuk04Name[];
extern const char MapMcKuk03Name[];
extern const char MapMcKuk02Name[];
extern const char MapMcKuk01Name[];
extern const char MapMcGnk25Name[];
extern const char MapMcGnk24Name[];
extern const char MapMcGnk23Name[];
extern const char MapMcGnk22Name[];
extern const char MapMcGnk21Name[];
extern const char MapMcGnk20Name[];
extern const char MapMcGnk19Name[];
extern const char MapMcGnk18Name[];
extern const char MapMcGnk17Name[];
extern const char MapModelGnk16Name[];
extern const char MapModelGnk15Name[];
extern const char MapModelGnk14Name[];
extern const char MapModelGnk13Name[];
extern const char MapModelGnk12Name[];
extern const char MapModelGnk11Name[];
extern const char MapModelGnk10Name[];
extern const char MapModelGnk09Name[];
extern const char MapModelGnk08Name[];
extern const char MapModelGnk07Name[];
extern const char MapModelGnk06Name[];
extern const char MapModelGnk05Name[];
extern const char MapModelGnk04Name[];
extern const char MapModelGnk03Name[];
extern const char MapModelGnk02Name[];
extern const char MapModelGnk01Name[];
extern const char MapModelUtk03BName[];
extern const char MapModelUtk11Name[];
extern const char MapModelUtk10Name[];
extern const char MapModelUtk09Name[];
extern const char MapModelUtk08Name[];
extern const char MapModelUtk07Name[];
extern const char MapModelUtk06Name[];
extern const char MapModelUtk05Name[];
extern const char MapModelUtk04Name[];
extern const char MapModelUtk03Name[];
extern const char MapModelUtk02Name[];
extern const char MapModelUtk01Name[];
extern const char MapModelKou07Name[];
extern const char MapModelKou05Name[];
extern const char MapModelKou04Name[];
extern const char MapModelKou02Name[];
extern const char MapModelKou01Name[];
extern const char MapModelGnu14Name[];
extern const char MapModelGnu13Name[];
extern const char MapModelGnu12Name[];
extern const char MapModelGnu11Name[];
extern const char MapModelGnu10Name[];
extern const char MapModelGnu09Name[];
extern const char MapModelGnu08Name[];
extern const char MapModelGnu07Name[];
extern const char MapModelGnu06Name[];
extern const char MapModelGnu05Name[];
extern const char MapModelGnu04Name[];
extern const char MapModelGnu03Name[];
extern const char MapModelGnu02Name[];
extern const char MapModelGnu01Name[];
extern const char MapModelUdo22Name[];
extern const char MapModelUdo21Name[];
extern const char MapModelUdo15Name[];
extern const char MapModelUdo14Name[];
extern const char MapModelUdo13Name[];
extern const char MapModelUdo12Name[];
extern const char MapModelUdo11Name[];
extern const char MapModelUdo10Name[];
extern const char MapModelUdo09Name[];
extern const char MapModelUdo08Name[];
extern const char MapModelUdo07Name[];
extern const char MapModelUdo06Name[];
extern const char MapModelUdo05Name[];
extern const char MapModelUdo04Name[];
extern const char MapModelUdo03Name[];
extern const char MapModelUdo02Name[];
extern const char MapModelUdo01Name[];
extern const char GameEmptyName[];
extern const char GameMcTestName[];

static char *maptbl0000[58] = {
    (char *)MapFileNameAraikabe,
    (char *)MapFileNameMcTst01,
    (char *)MapFileNameMcTst02,
    (char *)MapFileNameMcTst03,
    (char *)MapFileNameMcTst04,
    (char *)MapFileNameMcTst05,
    (char *)MapFileNameMcTst06,
    (char *)MapFileNameMcTst07,
    (char *)MapFileNameMcTst08,
    (char *)MapFileNameMcTst09,
    (char *)MapFileNameMcTst10,
    (char *)MapFileNameMcTst11,
    (char *)MapFileNameMcTst12,
    (char *)MapFileNameMcTst13,
    (char *)GameMcTestName,
    (char *)MapFileNameMcBtl01,
    (char *)MapFileNameMcVok01,
    (char *)MapFileNameMcVok02,
    (char *)MapFileNameMcVok03,
    (char *)MapFileNameMcVok04,
    (char *)MapFileNameMcVok05,
    (char *)MapFileNameMcVok06,
    (char *)MapFileNameMcVok07,
    (char *)MapFileNameMcVok08,
    (char *)MapFileNameMcVok09,
    (char *)MapFileNameMcVok10,
    (char *)MapFileNameMcVok11,
    (char *)MapFileNameMcVok12,
    (char *)MapFileNameMcVok13,
    (char *)MapFileNameMcVok14,
    (char *)MapFileNameMcVok15,
    (char *)MapFileNameMcVok16,
    (char *)GameEmptyName,
    (char *)MapFileNameMcVok18,
    (char *)MapFileNameMcVok19,
    (char *)MapFileNameMcVok20,
    (char *)MapFileNameMcVok21,
    (char *)MapFileNameMcVok22,
    (char *)MapFileNameMcVok23,
    (char *)MapFileNameMcVok24,
    (char *)MapFileNameMcVok25,
    (char *)MapFileNameMcVok26,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)MapFileNameMcEls01,
    (char *)MapFileNameMcVok03b,
    (char *)MapFileNameMcVok04b,
    (char *)MapFileNameMcVok05b,
    (char *)MapFileNameMcVok06b,
    (char *)MapFileNameMcVok07b,
    (char *)MapFileNameMcVok08b,
    (char *)MapFileNameMcVok09b,
    (char *)MapFileNameMcVok10b,
    (char *)MapFileNameMcVok13b,
    (char *)MapFileNameMcVok14b,
    (char *)MapFileNameMcVok12b
};

static char *maptbl0100[32] = {
    (char *)MapFileNameMcEls01,
    (char *)MapFileNameMcEls02,
    (char *)MapFileNameMcEls03,
    (char *)MapFileNameMcEls04,
    (char *)MapFileNameMcEls05,
    (char *)MapFileNameMcEls06,
    (char *)MapFileNameMcEls07,
    (char *)MapFileNameMcEls08,
    (char *)MapFileNameMcEls09,
    (char *)MapFileNameMcEls10,
    (char *)MapFileNameMcEls11,
    (char *)MapFileNameMcEls12,
    (char *)MapFileNameMcEls13,
    (char *)GameEmptyName,
    (char *)MapFileNameMcEls15,
    (char *)MapFileNameMcEls16,
    (char *)MapFileNameMcEls02b,
    (char *)MapFileNameMcEls04b,
    (char *)MapFileNameMcEls05b,
    (char *)MapFileNameMcEls06b,
    (char *)MapFileNameMcEls07b,
    (char *)MapFileNameMcEls08b,
    (char *)MapFileNameMcEls12b,
    (char *)MapFileNameMcEls13b,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)MapFileNameMcEls01h,
    (char *)MapFileNameMcEls06h
};

/* Original group 02 has count zero and a genuine zero-size LOCAL OBJECT.
 * Its empty table shares the following group03 table address; no storage
 * or new DATA credit is assigned to this declaration. */
static char *maptbl0200[0] = {};

static char *maptbl0300[1] = {
    (char *)MapFileNameMcSyu01
};

static char *maptbl0400[2] = {
    (char *)MapFileNameMcUmn01,
    (char *)MapFileNameMcUmn02
};

static char *maptbl0500[8] = {
    (char *)MapFileNameMcUzu01,
    (char *)MapFileNameMcUzu02,
    (char *)MapFileNameMcUzu03,
    (char *)MapFileNameMcUzu04,
    (char *)MapFileNameMcUzu05,
    (char *)MapFileNameMcUzu06,
    (char *)MapFileNameMcUzu07,
    (char *)MapFileNameMcUzu08
};

static char *maptbl0600[2] = {
    (char *)MapFileNameMcMil01,
    (char *)MapFileNameMcMil02
};

static char *maptbl0700[48] = {
    (char *)MapMcEve01Name,
    (char *)MapMcEve02Name,
    (char *)MapMcEve03Name,
    (char *)MapMcEve04Name,
    (char *)MapMcEve05Name,
    (char *)MapMcEve06Name,
    (char *)MapMcEve07Name,
    (char *)MapMcEve08Name,
    (char *)MapMcEve09Name,
    (char *)MapMcEve10Name,
    (char *)MapFileNameMcEve11,
    (char *)MapFileNameMcEve12,
    (char *)MapFileNameMcEve13,
    (char *)MapFileNameMcEve14,
    (char *)MapFileNameMcEve15,
    (char *)MapFileNameMcEve16,
    (char *)MapFileNameMcEve17,
    (char *)MapFileNameMcEve18,
    (char *)MapFileNameMcEve19,
    (char *)MapFileNameMcEve20,
    (char *)MapFileNameMcEve21,
    (char *)MapFileNameMcEve22,
    (char *)MapFileNameMcEve23,
    (char *)MapFileNameMcEve24,
    (char *)MapFileNameMcEve25,
    (char *)MapFileNameMcEve26,
    (char *)MapFileNameMcEve27,
    (char *)MapFileNameMcEve28,
    (char *)MapFileNameMcEve29,
    (char *)MapFileNameMcEve30,
    (char *)MapFileNameMcEve31,
    (char *)MapFileNameMcEve32,
    (char *)MapFileNameMcEve33,
    (char *)MapFileNameMcEve35,
    (char *)MapFileNameMcEve36,
    (char *)MapFileNameMcEve37,
    (char *)MapFileNameMcEve38,
    (char *)MapFileNameMcEve39,
    (char *)MapFileNameMcEve40,
    (char *)MapFileNameMcEve41,
    (char *)MapFileNameMcEve42,
    (char *)MapFileNameMcEve43,
    (char *)MapFileNameMcEve44,
    (char *)MapFileNameMcEve45,
    (char *)MapFileNameMcEve46,
    (char *)MapFileNameMcEve47,
    (char *)MapFileNameMcEve48,
    (char *)MapFileNameMcEve49
};

static char *maptbl0800[19] = {
    (char *)MapMcDyu01Name,
    (char *)MapMcDyu02Name,
    (char *)MapMcDyu03Name,
    (char *)MapMcDyu04Name,
    (char *)MapMcDyu05Name,
    (char *)MapMcDyu06Name,
    (char *)MapMcDyu07Name,
    (char *)MapMcDyu08Name,
    (char *)GameEmptyName,
    (char *)MapMcDyu10Name,
    (char *)GameEmptyName,
    (char *)MapMcDyu12Name,
    (char *)MapMcDyu13Name,
    (char *)MapMcDyu14Name,
    (char *)MapMcDyu15Name,
    (char *)MapMcDyu16Name,
    (char *)MapMcDyu17Name,
    (char *)MapMcDyu18Name,
    (char *)MapMcDyu04bName
};

static char *maptbl0900[19] = {
    (char *)MapMcUta01Name,
    (char *)MapMcUta02Name,
    (char *)MapMcUta03Name,
    (char *)MapMcUta04Name,
    (char *)MapMcUta05Name,
    (char *)MapMcUta06Name,
    (char *)MapMcUta07Name,
    (char *)MapMcUta08Name,
    (char *)MapMcUta09Name,
    (char *)MapMcUta10Name,
    (char *)MapMcUta11Name,
    (char *)MapMcUta12Name,
    (char *)MapMcUta13Name,
    (char *)MapMcUta14Name,
    (char *)MapMcUta15Name,
    (char *)MapMcUta16Name,
    (char *)MapMcUta17Name,
    (char *)MapMcUta18Name,
    (char *)MapMcUta19Name
};

static char *maptbl1000[1] = {
    (char *)MapMcSyo01Name
};

static char *maptbl1100[9] = {
    (char *)MapMcPro01Name,
    (char *)MapMcPro02Name,
    (char *)MapMcPro03Name,
    (char *)MapMcPro04Name,
    (char *)MapMcPro05Name,
    (char *)MapMcPro06Name,
    (char *)MapMcPro07Name,
    (char *)MapMcPro08Name,
    (char *)MapMcPro09Name
};

static char *maptbl1200[41] = {
    (char *)MapMcKas01Name,
    (char *)MapMcKas02Name,
    (char *)MapMcKas03Name,
    (char *)MapMcKas04Name,
    (char *)MapMcKas05Name,
    (char *)MapMcKas06Name,
    (char *)MapMcKas07Name,
    (char *)MapMcKas08Name,
    (char *)MapMcKas09Name,
    (char *)MapMcKas10Name,
    (char *)MapMcKas11Name,
    (char *)MapMcKas12Name,
    (char *)MapMcKas13Name,
    (char *)MapMcKas14Name,
    (char *)MapMcKas15Name,
    (char *)MapMcKas16Name,
    (char *)MapMcKas17Name,
    (char *)MapMcKas18Name,
    (char *)MapMcKas19Name,
    (char *)MapMcKas20Name,
    (char *)MapMcKas21Name,
    (char *)MapMcKas22Name,
    (char *)MapMcKas23Name,
    (char *)MapMcKas24Name,
    (char *)MapMcKas25Name,
    (char *)MapMcKas26Name,
    (char *)MapMcKas27Name,
    (char *)MapMcKas28Name,
    (char *)MapMcKas29Name,
    (char *)MapMcKas30Name,
    (char *)MapMcKas31Name,
    (char *)MapMcKas32Name,
    (char *)MapMcKas33Name,
    (char *)MapMcKas34Name,
    (char *)MapMcKas35Name,
    (char *)MapMcKas36Name,
    (char *)MapMcKas37Name,
    (char *)MapMcKas38Name,
    (char *)MapMcKas39Name,
    (char *)MapMcKas40Name,
    (char *)MapMcKas41Name
};

static char *maptbl1300[24] = {
    (char *)MapMcKuk01Name,
    (char *)MapMcKuk02Name,
    (char *)MapMcKuk03Name,
    (char *)MapMcKuk04Name,
    (char *)MapMcKuk05Name,
    (char *)MapMcKuk06Name,
    (char *)MapMcKuk07Name,
    (char *)MapMcKuk08Name,
    (char *)MapMcKuk09Name,
    (char *)MapMcKuk10Name,
    (char *)MapMcKuk11Name,
    (char *)MapMcKuk12Name,
    (char *)MapMcKuk13Name,
    (char *)MapMcKuk14Name,
    (char *)MapMcKuk15Name,
    (char *)MapMcKuk03bName,
    (char *)MapMcKuk04bName,
    (char *)MapMcKuk05bName,
    (char *)MapMcKuk06bName,
    (char *)MapMcKuk07bName,
    (char *)MapMcKuk08bName,
    (char *)MapMcKuk09bName,
    (char *)MapMcKuk10bName,
    (char *)MapMcKuk16Name
};

static char *maptbl1400[25] = {
    (char *)MapModelGnk01Name,
    (char *)MapModelGnk02Name,
    (char *)MapModelGnk03Name,
    (char *)MapModelGnk04Name,
    (char *)MapModelGnk05Name,
    (char *)MapModelGnk06Name,
    (char *)MapModelGnk07Name,
    (char *)MapModelGnk08Name,
    (char *)MapModelGnk09Name,
    (char *)MapModelGnk10Name,
    (char *)MapModelGnk11Name,
    (char *)MapModelGnk12Name,
    (char *)MapModelGnk13Name,
    (char *)MapModelGnk14Name,
    (char *)MapModelGnk15Name,
    (char *)MapModelGnk16Name,
    (char *)MapMcGnk17Name,
    (char *)MapMcGnk18Name,
    (char *)MapMcGnk19Name,
    (char *)MapMcGnk20Name,
    (char *)MapMcGnk21Name,
    (char *)MapMcGnk22Name,
    (char *)MapMcGnk23Name,
    (char *)MapMcGnk24Name,
    (char *)MapMcGnk25Name
};

static char *maptbl1500[12] = {
    (char *)MapModelUtk01Name,
    (char *)MapModelUtk02Name,
    (char *)MapModelUtk03Name,
    (char *)MapModelUtk04Name,
    (char *)MapModelUtk05Name,
    (char *)MapModelUtk06Name,
    (char *)MapModelUtk07Name,
    (char *)MapModelUtk08Name,
    (char *)MapModelUtk09Name,
    (char *)MapModelUtk10Name,
    (char *)MapModelUtk11Name,
    (char *)MapModelUtk03BName
};

static char *maptbl1600[7] = {
    (char *)MapModelKou01Name,
    (char *)MapModelKou02Name,
    (char *)GameEmptyName,
    (char *)MapModelKou04Name,
    (char *)MapModelKou05Name,
    (char *)GameEmptyName,
    (char *)MapModelKou07Name
};

static char *maptbl1700[14] = {
    (char *)MapModelGnu01Name,
    (char *)MapModelGnu02Name,
    (char *)MapModelGnu03Name,
    (char *)MapModelGnu04Name,
    (char *)MapModelGnu05Name,
    (char *)MapModelGnu06Name,
    (char *)MapModelGnu07Name,
    (char *)MapModelGnu08Name,
    (char *)MapModelGnu09Name,
    (char *)MapModelGnu10Name,
    (char *)MapModelGnu11Name,
    (char *)MapModelGnu12Name,
    (char *)MapModelGnu13Name,
    (char *)MapModelGnu14Name
};

static char *maptbl1800[22] = {
    (char *)MapModelUdo01Name,
    (char *)MapModelUdo02Name,
    (char *)MapModelUdo03Name,
    (char *)MapModelUdo04Name,
    (char *)MapModelUdo05Name,
    (char *)MapModelUdo06Name,
    (char *)MapModelUdo07Name,
    (char *)MapModelUdo08Name,
    (char *)MapModelUdo09Name,
    (char *)MapModelUdo10Name,
    (char *)MapModelUdo11Name,
    (char *)MapModelUdo12Name,
    (char *)MapModelUdo13Name,
    (char *)MapModelUdo14Name,
    (char *)MapModelUdo15Name,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)GameEmptyName,
    (char *)MapModelUdo21Name,
    (char *)MapModelUdo22Name
};

MapNameGroup maptbl[20] = {
    {maptbl0000, 58},
    {maptbl0100, 32},
    {maptbl0200, 0},
    {maptbl0300, 1},
    {maptbl0400, 2},
    {maptbl0500, 8},
    {maptbl0600, 2},
    {maptbl0700, 48},
    {maptbl0800, 19},
    {maptbl0900, 19},
    {maptbl1000, 1},
    {maptbl1100, 9},
    {maptbl1200, 41},
    {maptbl1300, 24},
    {maptbl1400, 25},
    {maptbl1500, 12},
    {maptbl1600, 7},
    {maptbl1700, 14},
    {maptbl1800, 22},
    {0, 0}
};

/* The retail initialized camera table contains 33 zeroed records. */
/* Existing Java camera setters establish 33 records of 160 bytes.
 * Both zero-initialized objects are genuine retail .data OBJECTs, not BSS. */
CfCameraDefinition CfCameraDefine[33] = {{0}};

/* GameCameraStateSave/Restore copies the whole studio camera 0 here. */
StudioCamera save_cam = {0};
