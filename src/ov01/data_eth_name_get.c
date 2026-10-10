/*
 * OV01 original TU 11: 0x00a2c5a8..0x00a2c9b8 (13 functions)
 */
#include "common.h"

extern const char D_00A46C28[];
extern const char D_00A46C40[];
extern const char D_00A46C50[];
extern const char D_00A46C90[];
extern const char D_00A46CA0[];
extern const char D_00A46CC8[];
extern const char D_00A46CD8[];
extern const char D_00A46CF0[];
extern const char D_00A46D00[];
extern const char D_00A46D18[];
extern const char D_00A46D28[];
extern const char D_00A46D40[];
extern const char D_00A46D50[];
extern const char D_00A46D80[];
extern const char D_00A46D90[];
extern const char D_00A46DA8[];
extern const char D_00A46DB8[];
extern const char D_00A46DC0[];
extern const char D_00A46DF0[];
extern const char D_00A46E00[];
extern const char D_00A46E30[];
extern const char D_00A46E40[];
extern const char D_00A46E50[];
extern const char D_00A46E80[];
extern const char D_00A46E90[];
extern const char D_00A46EB8[];
extern const char D_00A46EC0[];
extern const char D_00A46EE8[];
extern const char D_00A46EF0[];
extern const char D_00A46F08[];
extern const char D_00A46F18[];
extern const char D_00A46F50[];
extern const char D_00A46F60[];
extern const char D_00A46F90[];
extern const char D_00A46FA0[];
extern const char D_00A46FB8[];
extern const char D_00A46FC8[];
extern const char D_00A47000[];
extern const char D_00A47010[];
extern const char D_00A47048[];
extern const char D_00A47058[];
extern const char D_00A47090[];
extern const char D_00A470A0[];
extern const char D_00A470C8[];
extern const char D_00A470D8[];
extern const char D_00A47110[];
extern const char D_00A47120[];
extern const char D_00A47158[];
extern const char D_00A47168[];
extern const char D_00A47190[];
extern const char D_00A471A0[];
extern const char D_00A471C8[];
extern const char D_00A471D0[];
extern const char D_00A471E8[];
extern const char D_00A471F0[];
extern const char D_00A47218[];
extern const char D_00A47228[];
extern const char D_00A47250[];
extern const char D_00A47260[];
extern const char D_00A47280[];
extern const char D_00A47290[];
extern const char D_00A472C0[];
extern const char D_00A472D0[];
extern const char D_00A472D8[];
extern const char D_00A47300[];
extern const char D_00A47310[];
extern const char D_00A47338[];
extern const char D_00A47350[];
extern const char D_00A47370[];
extern const char D_00A47380[];
extern const char D_00A47388[];
extern const char D_00A473B0[];
extern const char D_00A473C0[];
extern const char D_00A473C8[];
extern const char D_00A473E8[];
extern const char D_00A473F8[];
extern const char D_00A47420[];
extern const char D_00A47430[];
extern const char D_00A47438[];
extern const char D_00A47458[];
extern const char D_00A47468[];
extern const char D_00A47470[];
extern const char D_00A47490[];
extern const char D_00A474A0[];
extern const char D_00A474A8[];
extern const char D_00A474D0[];
extern const char D_00A474E0[];
extern const char D_00A47510[];
extern const char D_00A47520[];
extern const char D_00A47540[];
extern const char D_00A47550[];
extern const char D_00A47570[];
extern const char D_00A47580[];
extern const char D_00A475B8[];
extern const char D_00A475C8[];
extern const char D_00A475F0[];
extern const char D_00A47600[];
extern const char D_00A47648[];
extern const char D_00A47650[];
extern const char D_00A47678[];
extern const char D_00A47688[];
extern const char D_00A476B8[];
extern const char D_00A476C8[];
extern const char D_00A476F8[];
extern const char D_00A47708[];
extern const char D_00A47720[];
extern const char D_00A47728[];
extern const char D_00A47730[];
extern const char D_00A47778[];
extern const char D_00A47788[];
extern const char D_00A47790[];
extern const char D_00A477C8[];
extern const char D_00A477D8[];
extern const char D_00A477E0[];
extern const char D_00A47818[];
extern const char D_00A47828[];
extern const char D_00A47860[];
extern const char D_00A47870[];
extern const char D_00A478A0[];
extern const char D_00A478B0[];
extern const char D_00A478B8[];
extern const char D_00A478D8[];
extern const char D_00A478E0[];
extern const char D_00A478E8[];
extern const char D_00A47908[];
extern const char D_00A47918[];
extern const char D_00A47940[];
extern const char D_00A47948[];
extern const char D_00A47968[];
extern const char D_00A47978[];
extern const char D_00A47980[];
extern const char D_00A479A8[];
extern const char D_00A479B0[];
extern const char D_00A479B8[];
extern const char D_00A479D8[];
extern const char D_00A479E0[];
extern const char D_00A479E8[];
extern const char D_00A47A10[];
extern const char D_00A47A18[];
extern const char D_00A47A38[];
extern const char D_00A47A48[];
extern const char D_00A47A50[];
extern const char D_00A47A68[];
extern const char D_00A47A70[];
extern const char D_00A47A78[];
extern const char D_00A47A98[];
extern const char D_00A47AA0[];
extern const char D_00A47AA8[];
extern const char D_00A47AC8[];
extern const char D_00A47AD0[];
extern const char D_00A47AD8[];
extern const char D_00A47AE0[];
extern const char D_00A47AE8[];
extern const char D_00A47AF8[];
extern const char D_00A47B00[];
extern const char D_00A47B18[];
extern const char D_00A47B28[];
extern const char D_00A47B30[];
extern const char D_00A47B40[];
extern const char D_00A47B48[];
extern const char D_00A47B58[];
extern const char D_00A47B70[];
extern const char D_00A47B80[];
extern const char D_00A47B98[];
extern const char D_00A47BA8[];
extern const char D_00A47BC0[];
extern const char D_00A47BD0[];
extern const char D_00A47BE8[];
extern const char D_00A47BF8[];
extern const char D_00A47C10[];
extern const char D_00A47C20[];
extern const char D_00A47C38[];
extern const char D_00A47C48[];
extern const char D_00A47C60[];
extern const char D_00A47C70[];
extern const char D_00A47C88[];
extern const char D_00A47C98[];
extern const char D_00A47CB0[];
extern const char D_00A47CC0[];
extern const char D_00A47CD8[];
extern const char D_00A47CE8[];
extern const char D_00A47D00[];
extern const char D_00A47D10[];
extern const char D_00A47D20[];
extern const char D_00A47D48[];
extern const char D_00A47D58[];
extern const char D_00A47D80[];
extern const char D_00A47D90[];
extern const char D_00A47DC8[];
extern const char D_00A47DD8[];
extern const char D_00A47DE8[];
extern const char D_00A47DF0[];
extern const char D_00A47E30[];
extern const char D_00A47E38[];
extern const char D_00A47E70[];
extern const char D_00A47E80[];
extern const char D_00A47EB8[];
extern const char D_00A47EC8[];
extern const char D_00A47EE8[];
extern const char D_00A47EF8[];
extern const char D_00A47F30[];
extern const char D_00A47F40[];
extern const char D_00A47F50[];
extern const char D_00A47F58[];
extern const char D_00A47F90[];
extern const char D_00A47F98[];
extern const char D_00A47FD0[];
extern const char D_00A47FD8[];
extern const char D_00A47FF8[];
extern const char D_00A48008[];
extern const char D_00A48020[];
extern const char D_00A48030[];
extern const char D_00A48038[];
extern const char D_00A48070[];
extern const char D_00A48080[];
extern const char D_00A480B0[];
extern const char D_00A480C0[];
extern const char D_00A480F0[];
extern const char D_00A48100[];
extern const char D_00A48148[];
extern const char D_00A48158[];
extern const char D_00A481A0[];
extern const char D_00A481A8[];
extern const char D_00A481E0[];
extern const char D_00A481F0[];
extern const char D_00A48220[];
extern const char D_00A48230[];
extern const char D_00A48260[];
extern const char D_00A48270[];
extern const char D_00A482A0[];
extern const char D_00A482B0[];
extern const char D_00A482E0[];
extern const char D_00A482F0[];
extern const char D_00A48328[];
extern const char D_00A48338[];
extern const char D_00A48370[];
extern const char D_00A48380[];
extern const char D_00A483B8[];
extern const char D_00A483C8[];
extern const char D_00A48400[];
extern const char D_00A48408[];
extern const char D_00A48418[];
extern const char D_00A48428[];
extern const char D_00A48440[];
extern const char D_00A48450[];
extern const char D_00A48470[];
extern const char D_00A48480[];
extern const char D_00A484B8[];
extern const char D_00A484C0[];
extern const char D_00A484E8[];
extern const char D_00A484F8[];
extern const char D_00A48520[];
extern const char D_00A48530[];
extern const char D_00A48568[];
extern const char D_00A48570[];
extern const char D_00A485A8[];
extern const char D_00A485B0[];
extern const char D_00A48600[];
extern const char D_00A48610[];
extern const char D_00A48658[];
extern const char D_00A48660[];
extern const char D_00A486A8[];
extern const char D_00A486B0[];
extern const char D_00A486F8[];
extern const char D_00A48708[];
extern const char D_00A48750[];
extern const char D_00A48758[];
extern const char D_00A48778[];
extern const char D_00A48788[];
extern const char D_00A487A8[];
extern const char D_00A487B8[];
extern const char D_00A48800[];
extern const char D_00A48810[];
extern const char D_00A48858[];
extern const char D_00A48868[];
extern const char D_00A488B0[];
extern const char D_00A488C0[];
extern const char D_00A48910[];
extern const char D_00A48920[];
extern const char D_00A48958[];
extern const char D_00A48968[];
extern const char D_00A48988[];
extern const char D_00A48998[];
extern const char D_00A489A0[];
extern const char D_00A489C8[];
extern const char D_00A489D8[];
extern const char D_00A48A00[];
extern const char D_00A48A10[];
extern const char D_00A48A38[];
extern const char D_00A48A40[];
extern const char D_00A48A98[];
extern const char D_00A48AA0[];
extern const char D_00A48AF8[];
extern const char D_00A48B00[];
extern const char D_00A48B40[];
extern const char D_00A48B50[];
extern const char D_00A48B90[];
extern const char D_00A48BA0[];
extern const char D_00A48BE0[];
extern const char D_00A48BF0[];
extern const char D_00A48C38[];
extern const char D_00A48C48[];
extern const char D_00A48C98[];
extern const char D_00A48CA0[];
extern const char D_00A48CD8[];
extern const char D_00A48CE0[];
extern const char D_00A48D18[];
extern const char D_00A48D20[];
extern const char D_00A48D58[];
extern const char D_00A48D60[];
extern const char D_00A48D98[];
extern const char D_00A48DA0[];
extern const char D_00A48DD8[];
extern const char D_00A48DE0[];
extern const char D_00A48E18[];
extern const char D_00A48E20[];
extern const char D_00A48E58[];
extern const char D_00A48E60[];
extern const char D_00A48E80[];
extern const char D_00A48E90[];
extern const char D_00A48EA8[];
extern const char D_00A48EB8[];
extern const char D_00A48ED0[];
extern const char D_00A48EE0[];
extern const char D_00A48EF8[];
extern const char D_00A48F08[];
extern const char D_00A48F20[];
extern const char D_00A48F28[];
extern const char D_00A48F68[];
extern const char D_00A48F78[];
extern const char D_00A48F90[];
extern const char D_00A48FA0[];
extern const char D_00A48FD8[];
extern const char D_00A48FE8[];
extern const char D_00A49008[];
extern const char D_00A49018[];
extern const char D_00A49030[];
extern const char D_00A49040[];
extern const char D_00A49058[];
extern const char D_00A49068[];
extern const char D_00A49078[];
extern const char D_00A49088[];
extern const char D_00A490A0[];
extern const char D_00A490B0[];
extern const char D_00A490D0[];
extern const char D_00A490E0[];
extern const char D_00A490F0[];
extern const char D_00A49100[];
extern const char D_00A49118[];
extern const char D_00A49128[];
extern const char D_00A49148[];
extern const char D_00A49158[];
extern const char D_00A49178[];
extern const char D_00A49188[];
extern const char D_00A491A0[];
extern const char D_00A491B0[];
extern const char D_00A491D0[];
extern const char D_00A491E0[];
extern const char D_00A49200[];
extern const char D_00A49210[];
extern const char D_00A49220[];
extern const char D_00A49230[];
extern const char D_00A49250[];
extern const char D_00A49260[];
extern const char D_00A49278[];
extern const char D_00A49288[];
extern const char D_00A492A8[];
extern const char D_00A492B8[];
extern const char D_00A492D8[];
extern const char D_00A492F0[];
extern const char D_00A49310[];
extern const char D_00A49320[];
extern const char D_00A49340[];
extern const char D_00A49350[];
extern const char D_00A49360[];
extern const char D_00A49370[];
extern const char D_00A49380[];
extern const char D_00A49390[];
extern const char D_00A493A0[];
extern const char D_00A493B8[];
extern const char D_00A493C8[];
extern const char D_00A493E0[];
extern const char D_00A493F0[];
extern const char D_00A49408[];
extern const char D_00A49430[];
extern const char D_00A49440[];
extern const char D_00A49470[];
extern const char D_00A49480[];
extern const char D_00A49490[];
extern const char D_00A494B8[];
extern const char D_00A494C8[];
extern const char D_00A494E0[];
extern const char D_00A494F0[];
extern const char D_00A49500[];
extern const char D_00A49510[];
extern const char D_00A49528[];
extern const char D_00A49540[];
extern const char D_00A49550[];
extern const char D_00A49568[];
extern const char D_00A49578[];
extern const char D_00A49590[];
extern const char D_00A495A0[];
extern const char D_00A495C8[];
extern const char D_00A495D8[];
extern const char D_00A495F8[];
extern const char D_00A49610[];
extern const char D_00A49628[];
extern const char D_00A49638[];
extern const char D_00A49650[];
extern const char D_00A49660[];
extern const char D_00A49670[];
extern const char D_00A49680[];
extern const char D_00A496B8[];
extern const char D_00A496C8[];
extern const char D_00A496D8[];
extern const char D_00A496E8[];
extern const char D_00A496F0[];
extern const char D_00A49700[];
extern const char D_00A49710[];
extern const char D_00A49720[];
extern const char D_00A49730[];
extern const char D_00A49740[];
extern const char D_00A49750[];
extern const char D_00A49760[];
extern const char D_00A49770[];
extern const char D_00A49780[];
extern const char D_00A49790[];
extern const char D_00A497A0[];
extern const char D_00A497B0[];
extern const char D_00A497D0[];
extern const char D_00A497E0[];
extern const char D_00A497F0[];
extern const char D_00A49810[];
extern const char D_00A49820[];
extern const char D_00A49840[];
extern const char D_00A49858[];
extern const char D_00A49868[];
extern const char D_00A49878[];
extern const char D_00A49888[];
extern const char D_00A498B8[];
extern const char D_00A498C8[];
extern const char D_00A498D8[];
extern const char D_00A498F0[];
extern const char D_00A49900[];
extern const char D_00A49910[];
extern const char D_00A49938[];
extern const char D_00A49950[];
extern const char D_00A49980[];
extern const char D_00A49998[];
extern const char D_00A499C8[];
extern const char D_00A499D8[];
extern const char D_00A499F0[];
extern const char D_00A49A00[];
extern const char D_00A49A08[];
extern const char D_00A49A20[];
extern const char D_00A49A30[];
extern const char D_00A49A40[];
extern const char D_00A49A48[];
extern const char D_00A49A88[];
extern const char D_00A49A98[];
extern const char D_00A49AB8[];
extern const char D_00A49AC8[];
extern const char D_00A49AD8[];
extern const char D_00A49AE8[];
extern const char D_00A49AF0[];
extern const char D_00A49B00[];
extern const char D_00A49B10[];
extern const char D_00A49B20[];
extern const char D_00A49B30[];
extern const char D_00A49B40[];
extern const char D_00A49B50[];
extern const char D_00A49B60[];
extern const char D_00A49B70[];
extern const char D_00A49B80[];
extern const char D_00A49B90[];
extern const char D_00A49BA0[];
extern const char D_00A49BB0[];
extern const char D_00A49BC0[];
extern const char D_00A49BE0[];
extern const char D_00A49BF0[];
extern const char D_00A49C08[];
extern const char D_00A49C20[];
extern const char D_00A49C38[];
extern const char D_00A49C48[];
extern const char D_00A49C60[];
extern const char D_00A49C70[];
extern const char D_00A49C88[];
extern const char D_00A49C98[];
extern const char D_00A49CB8[];
extern const char D_00A49CC8[];
extern const char D_00A49CE8[];
extern const char D_00A49CF8[];
extern const char D_00A49D18[];
extern const char D_00A49D28[];
extern const char D_00A49D40[];
extern const char D_00A49D50[];
extern const char D_00A49D70[];
extern const char D_00A49D80[];
extern const char D_00A49DA0[];
extern const char D_00A49DB0[];
extern const char D_00A49DC8[];
extern const char D_00A49DD8[];
extern const char D_00A49DE8[];
extern const char D_00A49E08[];
extern const char D_00A49E18[];
extern const char D_00A49E40[];
extern const char D_00A49E50[];
extern const char D_00A49E78[];
extern const char D_00A49E88[];
extern const char D_00A49EA8[];
extern const char D_00A49EB8[];
extern const char D_00A49ED8[];
extern const char D_00A49EE8[];
extern const char D_00A49EF8[];
extern const char D_00A49F28[];
extern const char D_00A49F38[];
extern const char D_00A49F60[];
extern const char D_00A49F70[];
extern const char D_00A49F90[];
extern const char D_00A49FA0[];
extern const char D_00A49FD0[];
extern const char D_00A49FE0[];
extern const char D_00A4A008[];
extern const char D_00A4A018[];
extern const char D_00A4A040[];
extern const char D_00A4A050[];
extern const char D_00A4A070[];
extern const char D_00A4A0A8[];
extern const char D_00A4A0B0[];
extern const char D_00A4A0E8[];
extern const char D_00A4A0F0[];
extern const char D_00A4A120[];
extern const char D_00A4A130[];
extern const char D_00A4A158[];
extern const char D_00A4A168[];
extern const char D_00A4A190[];
extern const char D_00A4A1A0[];
extern const char D_00A4A1C8[];
extern const char D_00A4A1D8[];
extern const char D_00A4A200[];
extern const char D_00A4A210[];
extern const char D_00A4A238[];
extern const char D_00A4A248[];
extern const char D_00A4A280[];
extern const char D_00A4A290[];
extern const char D_00A4A2B0[];
extern const char D_00A4A2B8[];
extern const char D_00A4A2D8[];
extern const char D_00A4A2E0[];
extern const char D_00A4A300[];
extern const char D_00A4A308[];
extern const char D_00A4A328[];
extern const char D_00A4A330[];
extern const char D_00A4A350[];
extern const char D_00A4A358[];
extern const char D_00A4A390[];
extern const char D_00A4A3A0[];
extern const char D_00A4A3A8[];
extern const char D_00A4A3C8[];
extern const char D_00A4A3E8[];
extern const char D_00A4A3F8[];
extern const char D_00A4A418[];
extern const char D_00A4A428[];
extern const char D_00A4A448[];
extern const char D_00A4A458[];
extern const char D_00A4A478[];
extern const char D_00A4A488[];
extern const char D_00A4A4A8[];
extern const char D_00A4A4B8[];
extern const char D_00A4A4D8[];
extern const char D_00A4A4E8[];
extern const char D_00A4A520[];
extern const char D_00A4A538[];
extern const char D_00A4A580[];
extern const char D_00A4A590[];
extern const char D_00A4A5B0[];
extern const char D_00A4A5C0[];
extern const char D_00A4A5E0[];
extern const char D_00A4A5F0[];
extern const char D_00A4A630[];
extern const char D_00A4A640[];
extern const char D_00A4A690[];
extern const char D_00A4A6A0[];
extern const char D_00A4A6D8[];
extern const char D_00A4A6E8[];
extern const char D_00A4A720[];
extern const char D_00A4A730[];
extern const char D_00A4A760[];
extern const char D_00A4A768[];
extern const char D_00A4A798[];
extern const char D_00A4A7D8[];
extern const char D_00A4A7E0[];
extern const char D_00A4A818[];
extern const char D_00A4A820[];
extern const char D_00A4A860[];
extern const char D_00A4A868[];
extern const char D_00A4A890[];
extern const char D_00A4A898[];
extern const char D_00A4A8C8[];
extern const char D_00A4A8D0[];
extern const char D_00A4A900[];
extern const char D_00A4A908[];
extern const char D_00A4A938[];
extern const char D_00A4A948[];
extern const char D_00A4A988[];
extern const char D_00A4A990[];
extern const char D_00A4A9B8[];
extern const char D_00A4A9C0[];
extern const char D_00A4A9E0[];
extern const char D_00A4A9E8[];
extern const char D_00A4AA30[];
extern const char D_00A4AA38[];
extern const char D_00A4AA68[];
extern const char D_00A4AA70[];
extern const char D_00A4AAA0[];
extern const char D_00A4AAA8[];
extern const char D_00A4AAE8[];
extern const char D_00A4AAF0[];
extern const char D_00A4AB00[];
extern const char D_00A4AB40[];
extern const char D_00A4AB48[];
extern const char D_00A4AB80[];
extern const char D_00A4AB88[];
extern const char D_00A4AB90[];
extern const char D_00A4ABC0[];
extern const char D_00A4ABC8[];
extern const char D_00A4ABD0[];
extern const char D_00A4AC08[];
extern const char D_00A4AC10[];
extern const char D_00A4AC48[];
extern const char D_00A4AC50[];
extern const char D_00A4AC90[];
extern const char D_00A4AC98[];
extern const char D_00A4ACD0[];
extern const char D_00A4ACD8[];
extern const char D_00A4AD18[];
extern const char D_00A4AD20[];
extern const char D_00A4AD58[];
extern const char D_00A4AD60[];
extern const char D_00A4ADA0[];
extern const char D_00A4ADA8[];
extern const char D_00A4ADE8[];
extern const char D_00A4ADF0[];
extern const char D_00A4AE30[];
extern const char D_00A4AE38[];
extern const char D_00A4AE68[];
extern const char D_00A4AE70[];
extern const char D_00A4AEB0[];
extern const char D_00A4AEB8[];
extern const char D_00A4AEF0[];
extern const char D_00A4AEF8[];
extern const char D_00A4AF28[];
extern const char D_00A4AF30[];
extern const char D_00A4AF68[];
extern const char D_00A4AF78[];
extern const char D_00A4AFA8[];
extern const char D_00A4AFB0[];
extern const char D_00A4AFB8[];
extern const char D_00A4AFF0[];
extern const char D_00A4AFF8[];
extern const char D_00A4B030[];
extern const char D_00A4B040[];
extern const char D_00A4B068[];
extern const char D_00A4B070[];
extern const char D_00A4B0A0[];
extern const char D_00A4B0A8[];
extern const char D_00A4B0D8[];
extern const char D_00A4B0E0[];
extern const char D_00A4B0F0[];
extern const char D_00A4B0F8[];
extern const char D_00A4B128[];
extern const char D_00A4B130[];
extern const char D_00A4B168[];
extern const char D_00A4B170[];
extern const char D_00A4B1A0[];
extern const char D_00A4B1A8[];
extern const char D_00A4B1D8[];
extern const char D_00A4B1E8[];
extern const char D_00A4B210[];
extern const char D_00A4B218[];
extern const char D_00A4B238[];
extern const char D_00A4B240[];
extern const char D_00A4B268[];
extern const char D_00A4B270[];
extern const char D_00A4B298[];
extern const char D_00A4B2A0[];
extern const char D_00A4B2D0[];
extern const char D_00A4B2E0[];
extern const char D_00A4B320[];
extern const char D_00A4B330[];
extern const char D_00A4B368[];
extern const char D_00A4B378[];
extern const char D_00A4B3B0[];
extern const char D_00A4B3C0[];
extern const char D_00A4B3F8[];
extern const char D_00A4B408[];
extern const char D_00A4B438[];
extern const char D_00A4B448[];
extern const char D_00A4B478[];
extern const char D_00A4B488[];
extern const char D_00A4B4B8[];
extern const char D_00A4B4C8[];
extern const char D_00A4B500[];
extern const char D_00A4B510[];
extern const char D_00A4B538[];
extern const char D_00A4B548[];
extern const char D_00A4B570[];
extern const char D_00A4B580[];
extern const char D_00A4B5A0[];
extern const char D_00A4B5B0[];
extern const char D_00A4B5D8[];
extern const char D_00A4B5E8[];
extern const char D_00A4B620[];
extern const char D_00A4B630[];
extern const char D_00A4B660[];
extern const char D_00A4B670[];
extern const char D_00A4B6B0[];
extern const char D_00A4B6C0[];
extern const char D_00A4B708[];
extern const char D_00A4B718[];
extern const char D_00A4B750[];
extern const char D_00A4B760[];
extern const char D_00A4B770[];
extern const char D_00A4B780[];
extern const char D_00A4B790[];
extern const char D_00A4B7A0[];
extern const char D_00A4B7B0[];
extern const char D_00A4B7C0[];
extern const char D_00A4B7D0[];
extern const char D_00A4B7E0[];
extern const char D_00A4B7F0[];
extern const char D_00A4B800[];
extern const char D_00A4B818[];
extern const char D_00A4B820[];
extern const char D_00A4B840[];
extern const char D_00A4B848[];
extern const char D_00A4B868[];
extern const char D_00A4B870[];
extern const char D_00A4B878[];
extern const char D_00A4B8A8[];
extern const char D_00A4B8B0[];
extern const char D_00A4B8E0[];
extern const char D_00A4B8E8[];
extern const char D_00A4B928[];
extern const char D_00A4B930[];
extern const char D_00A4B968[];
extern const char D_00A4B970[];
extern const char D_00A4B9A8[];
extern const char D_00A4B9B0[];
extern const char D_00A4B9D0[];
extern const char D_00A4B9D8[];
extern const char D_00A4B9F8[];
extern const char D_00A4BA00[];
extern const char D_00A4BA08[];
extern const char D_00A4BA28[];
extern const char D_00A4BA30[];
extern const char D_00A4BA48[];
extern const char D_00A4BA50[];
extern const char D_00A4BA80[];
extern const char D_00A4BA88[];
extern const char D_00A4BAB8[];
extern const char D_00A4BAC0[];
extern const char D_00A4BB00[];
extern const char D_00A4BB08[];
extern const char D_00A4BB40[];
extern const char D_00A4BB48[];
extern const char D_00A4BB80[];
extern const char D_00A4BB88[];
extern const char D_00A4BB90[];
extern const char D_00A4BBD0[];
extern const char D_00A4BBD8[];
extern const char D_00A4BC18[];
extern const char D_00A4BC20[];
extern const char D_00A4BC60[];
extern const char D_00A4BC68[];
extern const char D_00A4BCA8[];
extern const char D_00A4BCB0[];
extern const char D_00A4BCC0[];
extern const char D_00A4BCC8[];
extern const char D_00A4BCE8[];
extern const char D_00A4BCF0[];
extern const char D_00A4BD08[];
extern const char D_00A4BD10[];
extern const char D_00A4BD30[];
extern const char D_00A4BD38[];
extern const char D_00A4BD50[];
extern const char D_00A4BD58[];
extern const char D_00A4BD70[];
extern const char D_00A4BD78[];
extern const char D_00A4BDB8[];
extern const char D_00A4BDC8[];
extern const char D_00A4BE08[];
extern const char D_00A4BE18[];
extern const char D_00A4BE58[];
extern const char D_00A4BE68[];
extern const char D_00A4BEA0[];
extern const char D_00A4BEB0[];
extern const char D_00A4BED8[];
extern const char D_00A4BEE0[];
extern const char D_00A4BF08[];
extern const char D_00A4BF10[];
extern const char D_00A4BF38[];
extern const char D_00A4BF40[];
extern const char D_00A4BF68[];
extern const char D_00A4BF70[];
extern const char D_00A4BFA8[];
extern const char D_00A4BFB8[];
extern const char D_00A4BFE0[];
extern const char D_00A4BFE8[];
extern const char D_00A4BFF0[];
extern const char D_00A4BFF8[];
extern const char D_00A4C028[];
extern const char D_00A4C030[];
extern const char D_00A4C038[];
extern const char D_00A4C068[];
extern const char D_00A4C070[];
extern const char D_00A4C078[];
extern const char D_00A4C080[];
extern const char D_00A4C088[];
extern const char D_00A4C090[];
extern const char D_00A4C098[];
extern const char D_00A4C0A0[];
extern const char D_00A4C0A8[];
extern const char D_00A4C0D8[];
extern const char D_00A4C0E0[];
extern const char D_00A4C0E8[];
extern const char D_00A4C0F0[];
extern const char D_00A4C120[];
extern const char D_00A4C128[];
extern const char D_00A4C158[];
extern const char D_00A4C160[];
extern const char D_00A4C168[];
extern const char D_00A4C170[];
extern const char D_00A4C178[];
extern const char D_00A4C180[];
extern const char D_00A4C188[];
extern const char D_00A4C190[];
extern const char D_00A4C198[];
extern const char D_00A4C1A0[];
extern const char D_00A4C1A8[];
extern const char D_00A4C1B0[];
extern const char D_00A4C1F8[];
extern const char D_00A4C200[];
extern const char D_00A4C248[];
extern const char D_00A4C250[];
extern const char D_00A4C298[];
extern const char D_00A4C2A0[];
extern const char D_00A4C2E0[];
extern const char D_00A4C2E8[];
extern const char D_00A4C320[];
extern const char D_00A4C328[];
extern const char D_00A4C330[];
extern const char D_00A4C340[];
extern const char D_00A4C350[];
extern const char D_00A4C360[];
extern const char D_00A4C370[];
extern const char D_00A4C380[];
extern const char D_00A4C390[];
extern const char D_00A4C3A0[];
extern const char D_00A4C3B0[];
extern const char D_00A4C3C0[];
extern const char D_00A4C3D0[];
extern const char D_00A4C3E0[];
extern const char D_00A4C3F0[];
extern const char D_00A4C400[];
extern const char D_00A4C410[];
extern const char D_00A4C420[];
extern const char D_00A4C430[];
extern const char D_00A4C440[];
extern const char D_00A4C450[];
extern const char D_00A4C460[];
extern const char D_00A4C470[];
extern const char D_00A4C480[];
extern const char D_00A4C490[];
extern const char D_00A4C4A0[];
extern const char D_00A4C4B0[];
extern const char D_00A4C4B8[];
extern const char D_00A4C4C0[];
extern const char D_00A4C4C8[];
extern const char D_00A4C4D0[];
extern const char D_00A4C4D8[];
extern const char D_00A4C4E0[];
extern const char D_00A4C4F0[];
extern const char D_00A4C500[];
extern const char D_00A4C510[];
extern const char D_00A4C520[];
extern const char D_00A4C528[];
extern const char D_00A4C538[];
extern const char D_00A4C570[];
extern const char D_00A4C580[];
extern const char D_00A4C5C8[];
extern const char D_00A4C5D8[];
extern const char D_00A4C618[];
extern const char D_00A4C628[];
extern const char D_00A4C658[];
extern const char D_00A4C668[];
extern const char D_00A4C698[];
extern const char D_00A4C6A8[];
extern const char D_00A4C6D8[];
extern const char D_00A4C6F0[];
extern const char D_00A4C718[];
extern const char D_00A4C730[];
extern const char D_00A4C778[];
extern const char D_00A4C788[];
extern const char D_00A4C7D0[];
extern const char D_00A4C7E0[];
extern const char D_00A4C828[];
extern const char D_00A4C838[];
extern const char D_00A4C870[];
extern const char D_00A4C880[];
extern const char D_00A4C8B0[];
extern const char D_00A4C8C0[];
extern const char D_00A4C8F8[];
extern const char D_00A4C908[];
extern const char D_00A4C938[];
extern const char D_00A4C948[];
extern const char D_00A4C978[];
extern const char D_00A4C988[];
extern const char D_00A4C9C8[];
extern const char D_00A4C9D8[];
extern const char D_00A4CA10[];
extern const char D_00A4CA20[];
extern const char D_00A4CA58[];
extern const char D_00A4CA68[];
extern const char D_00A4CAA0[];
extern const char D_00A4CAA8[];
extern const char D_00A4CAD8[];
extern const char D_00A4CAE8[];
extern const char D_00A4CB28[];
extern const char D_00A4CB38[];
extern const char D_00A4CB68[];
extern const char D_00A4CB78[];
extern const char D_00A4CBA0[];
extern const char D_00A4CBB0[];
extern const char D_00A4CBE8[];
extern const char D_00A4CBF8[];
extern const char D_00A4CC30[];
extern const char D_00A4CC40[];
extern const char D_00A4CC90[];
extern const char D_00A4CCA0[];
extern const char D_00A4CCD0[];
extern const char D_00A4CCE0[];
extern const char D_00A4CD28[];
extern const char D_00A4CD38[];
extern const char D_00A4CD68[];
extern const char D_00A4CD78[];
extern const char D_00A4CDA8[];
extern const char D_00A4CDB8[];
extern const char D_00A4CDE8[];
extern const char D_00A4CDF8[];
extern const char D_00A4CE28[];
extern const char D_00A4CE38[];
extern const char D_00A4CE78[];
extern const char D_00A4CE88[];
extern const char D_00A4CE90[];
extern const char D_00A4CEC8[];
extern const char D_00A4CED8[];
extern const char D_00A4CF08[];
extern const char D_00A4CF18[];
extern const char D_00A4CF48[];
extern const char D_00A4CF58[];
extern const char D_00A4CF88[];
extern const char D_00A4CF98[];
extern const char D_00A4CFD8[];
extern const char D_00A4CFE8[];
extern const char D_00A4D020[];
extern const char D_00A4D030[];
extern const char D_00A4D060[];
extern const char D_00A4D070[];
extern const char D_00A4D0A8[];
extern const char D_00A4D0B8[];
extern const char D_00A4D0E8[];
extern const char D_00A4D0F8[];
extern const char D_00A4D148[];
extern const char D_00A4D158[];
extern const char D_00A4D180[];
extern const char D_00A4D190[];
extern const char D_00A4D1E0[];
extern const char D_00A4D1F0[];
extern const char D_00A4D200[];
extern const char D_00A4D208[];
extern const char D_00A4D218[];
extern const char D_00A4D228[];
extern const char D_00A4D230[];
extern const char D_00A4D240[];
extern const char D_00A4D250[];
extern const char D_00A4D260[];
extern const char D_00A4D270[];
extern const char D_00A4D280[];
extern const char D_00A4D290[];
extern const char D_00A4D2A0[];
extern const char D_00A4D2B0[];
extern const char D_00A4D2B8[];
extern const char D_00A4D2C8[];
extern const char D_00A4D2D8[];
extern const char D_00A4D2E8[];
extern const char D_00A4D2F8[];
extern const char D_00A4D300[];
extern const char D_00A4D308[];
extern const char D_00A4D318[];
extern const char D_00A4D320[];
extern const char D_00A4D330[];
extern const char D_00A4D338[];
extern const char D_00A4D340[];
extern const char D_00A4D350[];
extern const char D_00A4D360[];
extern const char D_00A4D370[];
extern const char D_00A4D380[];
extern const char D_00A4D390[];
extern const char D_00A4D398[];
extern const char D_00A4D3A0[];
extern const char D_00A4D3B0[];
extern const char D_00A4D3B8[];
extern const char D_00A4D3D0[];
extern const char D_00A4D3E8[];
extern const char D_00A4D400[];
extern const char D_00A4D410[];
extern const char D_00A4D420[];
extern const char D_00A4D430[];
extern const char D_00A4D440[];
extern const char D_00A4D450[];
extern const char D_00A4D460[];
extern const char D_00A4D470[];
extern const char D_00A4D480[];
extern const char D_00A4D490[];
extern const char D_00A4D4A0[];
extern const char D_00A4D4B0[];
extern const char D_00A4D4C0[];
extern const char D_00A4D4D0[];
extern const char D_00A4D4E0[];
extern const char D_00A4D4F0[];
extern const char D_00A4D500[];
extern const char D_00A4D510[];
extern const char D_00A4D520[];
extern const char D_00A4D530[];
extern const char D_00A4D540[];
extern const char D_00A4D550[];
extern const char D_00A4D560[];
extern const char D_00A4D570[];
extern const char D_00A4D580[];
extern const char D_00A4D590[];
extern const char D_00A4D5A0[];
extern const char D_00A4D5B0[];
extern const char D_00A4D5C0[];
extern const char D_00A4D5D0[];
extern const char D_00A4D5E0[];
extern const char D_00A4D5E8[];
extern const char D_00A4D5F8[];
extern const char D_00A4D608[];
extern const char D_00A4D610[];
extern const char D_00A4D620[];
extern const char D_00A4D630[];
extern const char D_00A4D640[];
extern const char D_00A4D648[];
extern const char D_00A4D650[];
extern const char D_00A4D658[];
extern const char D_00A4D668[];
extern const char D_00A4D678[];
extern const char D_00A4D688[];
extern const char D_00A4D698[];
extern const char D_00A4D6A8[];
extern const char D_00A4D6B8[];
extern const char D_00A4D6C8[];
extern const char D_00A4D6D8[];
extern const char D_00A4D6E8[];
extern const char D_00A4D6F0[];
extern const char D_00A4D6F8[];
extern const char D_00A4D708[];
extern const char D_00A4D718[];
extern const char D_00A4D728[];
extern const char D_00A4D738[];
extern const char D_00A4D748[];
extern const char D_00A4D758[];
extern const char D_00A4D768[];
extern const char D_00A4D778[];
extern const char D_00A4D780[];
extern const char D_00A4D788[];
extern const char D_00A4D798[];
extern const char D_00A4D7A0[];
extern const char D_00A4D7A8[];
extern const char D_00A4D7B8[];
extern const char D_00A4D7C8[];
extern const char D_00A4D7D0[];
extern const char D_00A4D7D8[];
extern const char D_00A4D7E8[];
extern const char D_00A4D7F8[];
extern const char D_00A4D808[];
extern const char D_00A4D818[];
extern const char D_00A4D828[];
extern const char D_00A4D838[];
extern const char D_00A4D848[];
extern const char D_00A4D858[];
extern const char D_00A4D868[];
extern const char D_00A4D878[];
extern const char D_00A4D888[];
extern const char D_00A4D890[];
extern const char D_00A4D898[];
extern const char D_00A4D8A8[];
extern const char D_00A4D8B8[];
extern const char D_00A4D8C0[];
extern const char D_00A4D8C8[];
extern const char D_00A4D8D0[];
extern const char D_00A4D8E0[];
extern const char D_00A4D8E8[];
extern const char D_00A4D8F0[];
extern const char D_00A4D900[];
extern const char D_00A4D910[];
extern const char D_00A4D920[];
extern const char D_00A4D930[];
extern const char D_00A4D938[];
extern const char D_00A4D940[];
extern const char D_00A4D948[];
extern const char D_00A4D950[];
extern const char D_00A4D960[];
extern const char D_00A4D970[];
extern const char D_00A4D978[];
extern const char D_00A4DC30[];
extern const char D_00A4DC38[];
extern const char D_00A4DC40[];
extern const char D_00A4DC48[];
extern const char D_00A4DC50[];
extern const char D_00A4DC58[];
extern const char D_00A4DC60[];
extern const char D_00A4DC68[];
extern const char D_00A4DC70[];
extern const char D_00A4DC78[];
extern const char D_00A4DC80[];
extern const char D_00A4DC88[];
extern const char D_00A4DC90[];
extern const char D_00A4DC98[];
extern const char D_00A4DCA0[];
extern const char D_00A4DCA8[];
extern const char D_00A4DCB0[];
extern const char D_00A4DCB8[];
extern const char D_00A4DCC8[];
extern const char D_00A4DCD0[];
extern const char D_00A4DCD8[];
extern const char D_00A4DCE0[];
extern const char D_00A4DCE8[];
extern const char D_00A4DCF8[];
extern const char D_00A4DD08[];
extern const char D_00A4DD18[];
extern const char D_00A4DD28[];
extern const char D_00A4DD38[];
extern const char D_00A4DD48[];
extern const char D_00A4DD58[];
extern const char D_00A4DD68[];
extern const char D_00A4DD78[];
extern const char D_00A4DD88[];
extern const char D_00A4DD90[];
extern const char D_00A4DDA0[];
extern const char D_00A4DDB0[];
extern const char D_00A4DDC0[];
extern const char D_00A4DDD0[];
extern const char D_00A4DDD8[];
extern const char D_00A4DDE8[];
extern const char D_00A4DDF8[];
extern const char D_00A4DE08[];
extern const char D_00A4DE18[];
extern const char D_00A4DE28[];
extern const char D_00A4DE38[];
extern const char D_00A4DE48[];
extern const char D_00A4DE58[];
extern const char D_00A4DE68[];
extern const char D_00A4DE78[];
extern const char D_00A4DE88[];
extern const char D_00A4DE98[];
extern const char D_00A4DEA8[];
extern const char D_00A4DEB8[];
extern const char D_00A4DEC8[];
extern const char D_00A4DED8[];
extern const char D_00A4DEE8[];
extern const char D_00A4DEF8[];
extern const char D_00A4DF08[];
extern const char D_00A4DF18[];
extern const char D_00A4DF28[];
extern const char D_00A4DF38[];
extern const char D_00A4DF48[];
extern const char D_00A4DF58[];
extern const char D_00A4DF68[];
extern const char D_00A4DF78[];
extern const char D_00A4DF88[];
extern const char D_00A4DF98[];
static const char * aa1[13];
static const char * aa2[3];
static const char * ab[4];
static const char * ba[14];
static const char * bb[12];
static const char * ma[11];
static const char * mb[8];

extern int printf(const char *format, ...);
const char D_00A4DFA8[] = "** dataEthnameGet: err %d\n";

/*
 * Sibling of the dataItmNameGet/dataWepNameGet/dataBltNameGet/dataAccNameGet
 * family this TU also defines (forward-declared, with this same
 * `const char **` return, in src/ov01/battle_init.c): each entry's first
 * member is the display name string this family returns the address of.
 * ethNameTbl is 0x3B4 bytes for a stride of 0xC, i.e. 79 entries.
 */
typedef struct EthName {
    const char *name;
    const char *unmodeled_04[2];
} EthName;

static EthName ethNameTbl[79] = {
    {D_00A47AC8, {D_00A47AA8, D_00A47AA0}},
    {D_00A47A98, {D_00A47A78, D_00A47A70}},
    {D_00A47A68, {D_00A47A50, D_00A47A48}},
    {D_00A47A38, {D_00A47A18, D_00A47AA0}},
    {D_00A47A10, {D_00A479E8, D_00A479E0}},
    {D_00A479D8, {D_00A479B8, D_00A479B0}},
    {D_00A479A8, {D_00A47980, D_00A47978}},
    {D_00A47968, {D_00A47948, D_00A47AA0}},
    {D_00A47940, {D_00A47918, D_00A479E0}},
    {D_00A47908, {D_00A478E8, D_00A478E0}},
    {D_00A478D8, {D_00A478B8, D_00A478B0}},
    {D_00A478A0, {D_00A47870, D_00A478B0}},
    {D_00A47860, {D_00A47828, D_00A478E0}},
    {D_00A47818, {D_00A477E0, D_00A477D8}},
    {D_00A477C8, {D_00A47790, D_00A47788}},
    {D_00A47778, {D_00A47730, D_00A47728}},
    {D_00A47720, {D_00A47708, D_00A47AA0}},
    {D_00A476F8, {D_00A476C8, D_00A47788}},
    {D_00A476B8, {D_00A47688, D_00A47788}},
    {D_00A47678, {D_00A47650, D_00A47788}},
    {D_00A47648, {D_00A47600, D_00A47A48}},
    {D_00A475F0, {D_00A475C8, D_00A47728}},
    {D_00A475B8, {D_00A47580, D_00A478E0}},
    {D_00A47570, {D_00A47550, D_00A47788}},
    {D_00A47540, {D_00A47520, D_00A47788}},
    {D_00A47510, {D_00A474E0, D_00A47788}},
    {D_00A474D0, {D_00A474A8, D_00A474A0}},
    {D_00A47490, {D_00A47470, D_00A47468}},
    {D_00A47458, {D_00A47438, D_00A47430}},
    {D_00A47420, {D_00A473F8, D_00A474A0}},
    {D_00A473E8, {D_00A473C8, D_00A473C0}},
    {D_00A473B0, {D_00A47388, D_00A47380}},
    {D_00A47370, {D_00A47350, D_00A479B0}},
    {D_00A47338, {D_00A47310, D_00A478E0}},
    {D_00A47300, {D_00A472D8, D_00A472D0}},
    {D_00A472C0, {D_00A47290, D_00A47380}},
    {D_00A47280, {D_00A47260, D_00A47380}},
    {D_00A47250, {D_00A47228, D_00A47AA0}},
    {D_00A47218, {D_00A471F0, D_00A478E0}},
    {D_00A471E8, {D_00A471D0, D_00A474A0}},
    {D_00A471C8, {D_00A471A0, D_00A479E0}},
    {D_00A47190, {D_00A47168, D_00A47978}},
    {D_00A47158, {D_00A47120, D_00A478E0}},
    {D_00A47110, {D_00A470D8, D_00A478E0}},
    {D_00A470C8, {D_00A470A0, D_00A478E0}},
    {D_00A47090, {D_00A47058, D_00A478E0}},
    {D_00A47048, {D_00A47010, D_00A478E0}},
    {D_00A47000, {D_00A46FC8, D_00A478E0}},
    {D_00A46FB8, {D_00A46FA0, D_00A474A0}},
    {D_00A46F90, {D_00A46F60, D_00A47728}},
    {D_00A46F50, {D_00A46F18, D_00A47788}},
    {D_00A46F08, {D_00A46EF0, D_00A474A0}},
    {D_00A46EE8, {D_00A46EC0, D_00A472D0}},
    {D_00A46EB8, {D_00A46E90, D_00A47AA0}},
    {D_00A46E80, {D_00A46E50, D_00A472D0}},
    {D_00A46E40, {D_00A478B8, D_00A478E0}},
    {D_00A46E30, {D_00A46E00, D_00A47AA0}},
    {D_00A46DF0, {D_00A46DC0, D_00A479B0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DA8, {D_00A46D90, D_00A47AA0}},
    {D_00A46D80, {D_00A46D50, D_00A479B0}},
    {D_00A46D40, {D_00A46D28, D_00A479E0}},
    {D_00A46D18, {D_00A46D00, D_00A47728}},
    {D_00A46CF0, {D_00A46CD8, D_00A47728}},
    {D_00A46CC8, {D_00A46CA0, D_00A472D0}},
    {D_00A46C90, {D_00A46C50, D_00A479E0}},
    {D_00A46C40, {D_00A46C28, D_00A479B0}}
};

static EthName itmNameTbl[72] = {
    {D_00A48400, {D_00A483C8, D_00A47AA0}},
    {D_00A483B8, {D_00A48380, D_00A47AA0}},
    {D_00A48370, {D_00A48338, D_00A47AA0}},
    {D_00A48328, {D_00A482F0, D_00A47AA0}},
    {D_00A482E0, {D_00A482B0, D_00A47728}},
    {D_00A482A0, {D_00A48270, D_00A47728}},
    {D_00A48260, {D_00A48230, D_00A47728}},
    {D_00A48220, {D_00A481F0, D_00A47728}},
    {D_00A481E0, {D_00A481A8, D_00A479E0}},
    {D_00A481A0, {D_00A48158, D_00A479E0}},
    {D_00A48148, {D_00A48100, D_00A479E0}},
    {D_00A480F0, {D_00A480C0, D_00A472D0}},
    {D_00A480B0, {D_00A48080, D_00A47A70}},
    {D_00A48070, {D_00A48038, D_00A48030}},
    {D_00A48020, {D_00A48008, D_00A479B0}},
    {D_00A47FF8, {D_00A47FD8, D_00A47728}},
    {D_00A47FD0, {D_00A47F98, D_00A478E0}},
    {D_00A47F90, {D_00A47F58, D_00A478E0}},
    {D_00A47F50, {D_00A47F40, D_00A47468}},
    {D_00A47F30, {D_00A47EF8, D_00A478E0}},
    {D_00A47EE8, {D_00A47EC8, D_00A478E0}},
    {D_00A47EB8, {D_00A47E80, D_00A47788}},
    {D_00A47E70, {D_00A47E38, D_00A478E0}},
    {D_00A47E30, {D_00A47DF0, D_00A47DE8}},
    {D_00A47DD8, {D_00A47DF0, D_00A47A70}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A47DC8, {D_00A47D90, D_00A479B0}},
    {D_00A47D80, {D_00A47D58, D_00A473C0}},
    {D_00A47D48, {D_00A47D20, D_00A473C0}},
    {D_00A47D10, {D_00A47D00, D_00A478E0}},
    {D_00A47CE8, {D_00A47D00, D_00A47978}},
    {D_00A47CD8, {D_00A47CC0, D_00A478E0}},
    {D_00A47CB0, {D_00A47C98, D_00A477D8}},
    {D_00A47C88, {D_00A47C70, D_00A477D8}},
    {D_00A47C60, {D_00A47C48, D_00A477D8}},
    {D_00A47C38, {D_00A47C20, D_00A47728}},
    {D_00A47C10, {D_00A47BF8, D_00A47728}},
    {D_00A47BE8, {D_00A47BD0, D_00A47728}},
    {D_00A47BC0, {D_00A47BA8, D_00A478E0}},
    {D_00A47B98, {D_00A47B80, D_00A478E0}},
    {D_00A47B70, {D_00A47B58, D_00A478E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A47B48, {D_00A47D00, D_00A47B40}},
    {D_00A47B30, {D_00A47D00, D_00A47B28}},
    {D_00A47B18, {D_00A47D00, D_00A474A0}},
    {D_00A47B00, {D_00A47D00, D_00A47978}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A47AF8, {D_00A47D00, D_00A47A48}},
    {D_00A47AE8, {D_00A47D00, D_00A478E0}},
    {D_00A47AE0, {D_00A47D00, D_00A479E0}},
    {D_00A47AD8, {D_00A47D00, D_00A47728}},
    {D_00A47AD0, {D_00A47D00, D_00A47788}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}}
};

static EthName sklNameTbl[67] = {
    {D_00A49278, {D_00A49260, D_00A472D0}},
    {D_00A49250, {D_00A49230, D_00A47A70}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49220, {D_00A49210, D_00A479B0}},
    {D_00A49200, {D_00A491E0, D_00A47728}},
    {D_00A491D0, {D_00A491B0, D_00A47788}},
    {D_00A491A0, {D_00A49188, D_00A47728}},
    {D_00A49178, {D_00A49158, D_00A474A0}},
    {D_00A49148, {D_00A49128, D_00A474A0}},
    {D_00A49118, {D_00A49100, D_00A478E0}},
    {D_00A490F0, {D_00A490E0, D_00A47380}},
    {D_00A490D0, {D_00A490B0, D_00A47728}},
    {D_00A490A0, {D_00A49088, D_00A47A70}},
    {D_00A49078, {D_00A49068, D_00A478E0}},
    {D_00A49058, {D_00A49040, D_00A474A0}},
    {D_00A49030, {D_00A49018, D_00A47B28}},
    {D_00A49008, {D_00A48FE8, D_00A478E0}},
    {D_00A48FD8, {D_00A48FA0, D_00A47A70}},
    {D_00A48F90, {D_00A48F78, D_00A47728}},
    {D_00A48F68, {D_00A48F28, D_00A478E0}},
    {D_00A48F20, {D_00A48F08, D_00A47728}},
    {D_00A48EF8, {D_00A48EE0, D_00A478E0}},
    {D_00A48ED0, {D_00A48EB8, D_00A479E0}},
    {D_00A48EA8, {D_00A48E90, D_00A47728}},
    {D_00A48E80, {D_00A48E60, D_00A47728}},
    {D_00A48E58, {D_00A48E20, D_00A474A0}},
    {D_00A48E18, {D_00A48DE0, D_00A474A0}},
    {D_00A48DD8, {D_00A48DA0, D_00A47788}},
    {D_00A48D98, {D_00A48D60, D_00A47728}},
    {D_00A48D58, {D_00A48D20, D_00A47728}},
    {D_00A48D18, {D_00A48CE0, D_00A47728}},
    {D_00A48CD8, {D_00A48CA0, D_00A47A70}},
    {D_00A48C98, {D_00A48C48, D_00A472D0}},
    {D_00A48C38, {D_00A48BF0, D_00A472D0}},
    {D_00A48BE0, {D_00A48BA0, D_00A47788}},
    {D_00A48B90, {D_00A48B50, D_00A47468}},
    {D_00A48B40, {D_00A48B00, D_00A47728}},
    {D_00A48AF8, {D_00A48AA0, D_00A479E0}},
    {D_00A48A98, {D_00A48A40, D_00A479E0}},
    {D_00A48A38, {D_00A48A10, D_00A47A70}},
    {D_00A48A00, {D_00A489D8, D_00A47788}},
    {D_00A489C8, {D_00A489A0, D_00A48998}},
    {D_00A48988, {D_00A48968, D_00A477D8}},
    {D_00A48958, {D_00A48920, D_00A477D8}},
    {D_00A48910, {D_00A488C0, D_00A47728}},
    {D_00A488B0, {D_00A48868, D_00A477D8}},
    {D_00A48858, {D_00A48810, D_00A47728}},
    {D_00A48800, {D_00A487B8, D_00A478E0}},
    {D_00A487A8, {D_00A48788, D_00A47468}},
    {D_00A48778, {D_00A48758, D_00A479B0}},
    {D_00A48750, {D_00A48708, D_00A473C0}},
    {D_00A486F8, {D_00A486B0, D_00A47380}},
    {D_00A486A8, {D_00A48660, D_00A47430}},
    {D_00A48658, {D_00A48610, D_00A479B0}},
    {D_00A48600, {D_00A485B0, D_00A478E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A485A8, {D_00A48570, D_00A474A0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A48568, {D_00A48530, D_00A47728}},
    {D_00A48520, {D_00A484F8, D_00A47380}},
    {D_00A484E8, {D_00A484C0, D_00A47380}},
    {D_00A484B8, {D_00A48480, D_00A472D0}},
    {D_00A48470, {D_00A48450, D_00A478E0}},
    {D_00A48440, {D_00A48428, D_00A472D0}},
    {D_00A48418, {D_00A48408, D_00A478E0}}
};

static EthName accNameTbl[180] = {
    {D_00A4A760, {D_00A4A730, D_00A47B40}},
    {D_00A4A720, {D_00A4A6E8, D_00A47AA0}},
    {D_00A4A6D8, {D_00A4A6A0, D_00A473C0}},
    {D_00A4A690, {D_00A4A640, D_00A478E0}},
    {D_00A4A630, {D_00A4A5F0, D_00A479B0}},
    {D_00A4A5E0, {D_00A4A5C0, D_00A48030}},
    {D_00A4A5B0, {D_00A4A590, D_00A48030}},
    {D_00A4A580, {D_00A4A538, D_00A479E0}},
    {D_00A4A520, {D_00A4A4E8, D_00A477D8}},
    {D_00A4A4D8, {D_00A4A4B8, D_00A47788}},
    {D_00A4A4A8, {D_00A4A488, D_00A47788}},
    {D_00A4A478, {D_00A4A458, D_00A47788}},
    {D_00A4A448, {D_00A4A428, D_00A47788}},
    {D_00A4A418, {D_00A4A3F8, D_00A47788}},
    {D_00A4A3E8, {D_00A4A3C8, D_00A47788}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A390, {D_00A4A358, D_00A478E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A350, {D_00A4A330, D_00A47DE8}},
    {D_00A4A328, {D_00A4A308, D_00A47DE8}},
    {D_00A4A300, {D_00A4A2E0, D_00A47DE8}},
    {D_00A4A2D8, {D_00A4A2B8, D_00A47DE8}},
    {D_00A4A2B0, {D_00A4A290, D_00A47DE8}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A3A8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4A280, {D_00A4A248, D_00A478E0}},
    {D_00A4A238, {D_00A4A210, D_00A472D0}},
    {D_00A4A200, {D_00A4A1D8, D_00A472D0}},
    {D_00A4A1C8, {D_00A4A1A0, D_00A472D0}},
    {D_00A4A190, {D_00A4A168, D_00A472D0}},
    {D_00A4A158, {D_00A4A130, D_00A472D0}},
    {D_00A4A120, {D_00A4A0F0, D_00A472D0}},
    {D_00A4A0E8, {D_00A4A0B0, D_00A478E0}},
    {D_00A4A0A8, {D_00A4A070, D_00A478E0}},
    {D_00A4A050, {D_00A4A3A0, D_00A478E0}},
    {D_00A4A050, {D_00A4A3A0, D_00A478E0}},
    {D_00A4A050, {D_00A4A3A0, D_00A478E0}},
    {D_00A4A050, {D_00A4A3A0, D_00A478E0}},
    {D_00A4A040, {D_00A4A018, D_00A478E0}},
    {D_00A4A008, {D_00A49FE0, D_00A47AA0}},
    {D_00A49FD0, {D_00A49FA0, D_00A472D0}},
    {D_00A49F90, {D_00A49F70, D_00A478E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49F60, {D_00A49F38, D_00A479E0}},
    {D_00A49F28, {D_00A49EF8, D_00A477D8}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49ED8, {D_00A49EB8, D_00A48030}},
    {D_00A49EA8, {D_00A49E88, D_00A47AA0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49E78, {D_00A49E50, D_00A478E0}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49EE8, {D_00A4A3A0, D_00A47468}},
    {D_00A49E40, {D_00A49E18, D_00A474A0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49E08, {D_00A49DE8, D_00A477D8}},
    {D_00A49DD8, {D_00A49230, D_00A47DE8}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49DC8, {D_00A49DB0, D_00A48998}},
    {D_00A49DA0, {D_00A49D80, D_00A479E0}},
    {D_00A49D70, {D_00A49D50, D_00A478E0}},
    {D_00A49D40, {D_00A49D28, D_00A47468}},
    {D_00A49D18, {D_00A49CF8, D_00A474A0}},
    {D_00A49CE8, {D_00A49CC8, D_00A47A48}},
    {D_00A49CB8, {D_00A49C98, D_00A478E0}},
    {D_00A49C88, {D_00A49C70, D_00A47978}},
    {D_00A49C60, {D_00A49C48, D_00A472D0}},
    {D_00A49C38, {D_00A49C20, D_00A478E0}},
    {D_00A49C08, {D_00A49BF0, D_00A47788}},
    {D_00A49BE0, {D_00A49BC0, D_00A478E0}},
    {D_00A49BB0, {D_00A48F78, D_00A472D0}},
    {D_00A49BA0, {D_00A49B90, D_00A479E0}},
    {D_00A49B80, {D_00A49B70, D_00A47A48}},
    {D_00A49B60, {D_00A49B50, D_00A479B0}},
    {D_00A49B40, {D_00A49B30, D_00A47B28}},
    {D_00A49B20, {D_00A49B10, D_00A474A0}},
    {D_00A49B00, {D_00A49AF0, D_00A49AE8}},
    {D_00A49AD8, {D_00A49AC8, D_00A48998}},
    {D_00A49AB8, {D_00A49A98, D_00A47468}},
    {D_00A49A88, {D_00A49A48, D_00A478E0}},
    {D_00A49A40, {D_00A49A30, D_00A472D0}},
    {D_00A49A20, {D_00A49A08, D_00A49A00}},
    {D_00A499F0, {D_00A499D8, D_00A478E0}},
    {D_00A499C8, {D_00A49998, D_00A477D8}},
    {D_00A49980, {D_00A49950, D_00A472D0}},
    {D_00A49938, {D_00A49910, D_00A472D0}},
    {D_00A49900, {D_00A489D8, D_00A47A48}},
    {D_00A498F0, {D_00A498D8, D_00A478E0}},
    {D_00A498C8, {D_00A489A0, D_00A47788}},
    {D_00A498B8, {D_00A49888, D_00A479B0}},
    {D_00A49878, {D_00A48F08, D_00A47A70}},
    {D_00A49868, {D_00A48EE0, D_00A47A48}},
    {D_00A49858, {D_00A48EB8, D_00A47380}},
    {D_00A49840, {D_00A49820, D_00A47AA0}},
    {D_00A46DB8, {D_00A46DB8, D_00A479E0}},
    {D_00A49810, {D_00A497F0, D_00A479B0}},
    {D_00A497E0, {D_00A48E90, D_00A479B0}},
    {D_00A497D0, {D_00A497B0, D_00A479E0}},
    {D_00A497A0, {D_00A49790, D_00A479E0}},
    {D_00A49780, {D_00A49770, D_00A47A48}},
    {D_00A49760, {D_00A49750, D_00A479B0}},
    {D_00A49740, {D_00A49730, D_00A472D0}},
    {D_00A49720, {D_00A49710, D_00A474A0}},
    {D_00A49700, {D_00A496F0, D_00A49AE8}},
    {D_00A496E8, {D_00A48450, D_00A478E0}},
    {D_00A496D8, {D_00A48428, D_00A479B0}},
    {D_00A496C8, {D_00A48408, D_00A473C0}},
    {D_00A496B8, {D_00A49680, D_00A479E0}},
    {D_00A49670, {D_00A48758, D_00A479B0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A49660, {D_00A498D8, D_00A473C0}},
    {D_00A49650, {D_00A49638, D_00A473C0}},
    {D_00A49628, {D_00A49610, D_00A479B0}},
    {D_00A495F8, {D_00A495D8, D_00A47A70}},
    {D_00A495C8, {D_00A495A0, D_00A478E0}},
    {D_00A49590, {D_00A49578, D_00A474A0}},
    {D_00A49568, {D_00A49550, D_00A47728}},
    {D_00A49540, {D_00A49068, D_00A47A48}},
    {D_00A49528, {D_00A49510, D_00A47728}},
    {D_00A49500, {D_00A494F0, D_00A47AA0}},
    {D_00A494E0, {D_00A49088, D_00A47A70}},
    {D_00A494C8, {D_00A48FE8, D_00A48998}},
    {D_00A494B8, {D_00A49490, D_00A472D0}},
    {D_00A49480, {D_00A49AC8, D_00A477D8}},
    {D_00A49470, {D_00A49440, D_00A47A48}},
    {D_00A49430, {D_00A49408, D_00A47A48}},
    {D_00A493F0, {D_00A493E0, D_00A47A70}},
    {D_00A493C8, {D_00A493B8, D_00A47A70}},
    {D_00A493A0, {D_00A49390, D_00A47A70}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A49380, {D_00A49AF0, D_00A47728}},
    {D_00A49370, {D_00A496F0, D_00A47728}},
    {D_00A49360, {D_00A49350, D_00A47728}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A49340, {D_00A49320, D_00A47A70}},
    {D_00A49310, {D_00A492F0, D_00A47A70}},
    {D_00A492D8, {D_00A492B8, D_00A47A70}},
    {D_00A492A8, {D_00A49288, D_00A47A70}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}}
};

static EthName wepNameTbl[114] = {
    {D_00A4B750, {D_00A4B718, D_00A478E0}},
    {D_00A4B708, {D_00A4B6C0, D_00A48030}},
    {D_00A4B6B0, {D_00A4B670, D_00A479E0}},
    {D_00A4B660, {D_00A4B630, D_00A48998}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B620, {D_00A4B5E8, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B5D8, {D_00A4B5B0, D_00A473C0}},
    {D_00A4B5A0, {D_00A4B580, D_00A473C0}},
    {D_00A4B570, {D_00A4B548, D_00A473C0}},
    {D_00A4B538, {D_00A4B510, D_00A473C0}},
    {D_00A4B500, {D_00A4B4C8, D_00A473C0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B4B8, {D_00A4B488, D_00A479B0}},
    {D_00A4B478, {D_00A4B448, D_00A47AA0}},
    {D_00A4B438, {D_00A4B408, D_00A474A0}},
    {D_00A4B3F8, {D_00A4B3C0, D_00A47AA0}},
    {D_00A4B3B0, {D_00A4B378, D_00A48998}},
    {D_00A4B368, {D_00A4B330, D_00A478E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B320, {D_00A4B2E0, D_00A47788}},
    {D_00A4B2D0, {D_00A4B2A0, D_00A474A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B298, {D_00A4B270, D_00A479B0}},
    {D_00A4B268, {D_00A4B240, D_00A47AA0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B238, {D_00A4B218, D_00A479E0}},
    {D_00A4B210, {D_00A4B1E8, D_00A47AA0}},
    {D_00A4B1D8, {D_00A4B1A8, D_00A478E0}},
    {D_00A4B1A0, {D_00A4B170, D_00A472D0}},
    {D_00A4B168, {D_00A4B130, D_00A479B0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B128, {D_00A4B0F8, D_00A47AA0}},
    {D_00A4B0F0, {D_00A4B0E0, D_00A47468}},
    {D_00A4B0D8, {D_00A4B0A8, D_00A47A70}},
    {D_00A4B0A0, {D_00A4B070, D_00A478E0}},
    {D_00A4B068, {D_00A4B040, D_00A478E0}},
    {D_00A4B030, {D_00A4AFF8, D_00A47788}},
    {D_00A4AFF0, {D_00A4AFB8, D_00A47468}},
    {D_00A4AFB0, {D_00A4AFB8, D_00A47468}},
    {D_00A4AFA8, {D_00A4AF78, D_00A47468}},
    {D_00A4AF68, {D_00A4AF30, D_00A48998}},
    {D_00A4AF28, {D_00A4AEF8, D_00A47468}},
    {D_00A4AEF0, {D_00A4AEB8, D_00A478E0}},
    {D_00A4AEB0, {D_00A4AE70, D_00A478E0}},
    {D_00A4AE68, {D_00A4AE38, D_00A47380}},
    {D_00A4AE30, {D_00A4ADF0, D_00A47380}},
    {D_00A4ADE8, {D_00A4ADA8, D_00A47380}},
    {D_00A4ADA0, {D_00A4AD60, D_00A479B0}},
    {D_00A4AD58, {D_00A4AD20, D_00A479B0}},
    {D_00A4AD18, {D_00A4ACD8, D_00A47380}},
    {D_00A4ACD0, {D_00A4AC98, D_00A474A0}},
    {D_00A4AC90, {D_00A4AC50, D_00A473C0}},
    {D_00A4AC48, {D_00A4AC10, D_00A47A48}},
    {D_00A4AC08, {D_00A4ABD0, D_00A47A48}},
    {D_00A4ABC8, {D_00A4ABD0, D_00A47468}},
    {D_00A4ABC0, {D_00A4AB90, D_00A47468}},
    {D_00A4AB88, {D_00A4AB90, D_00A47468}},
    {D_00A4AB80, {D_00A4AB48, D_00A479B0}},
    {D_00A4AB40, {D_00A4AB00, D_00A472D0}},
    {D_00A4AAF0, {D_00A4A3A0, D_00A47A70}},
    {D_00A4AAE8, {D_00A4AAA8, D_00A479B0}},
    {D_00A4AAA0, {D_00A4AA70, D_00A47380}},
    {D_00A4AA68, {D_00A4AA38, D_00A479B0}},
    {D_00A4AA30, {D_00A4A9E8, D_00A478E0}},
    {D_00A4A9E0, {D_00A4A9C0, D_00A478E0}},
    {D_00A4A9B8, {D_00A4A990, D_00A478E0}},
    {D_00A4A988, {D_00A4A948, D_00A479B0}},
    {D_00A4A938, {D_00A4A908, D_00A47A70}},
    {D_00A4A900, {D_00A4A8D0, D_00A47728}},
    {D_00A4A8C8, {D_00A4A898, D_00A47728}},
    {D_00A4A890, {D_00A4A868, D_00A478E0}},
    {D_00A4A860, {D_00A4A820, D_00A47728}},
    {D_00A4A818, {D_00A4A7E0, D_00A47380}},
    {D_00A4A7D8, {D_00A4A798, D_00A47788}},
    {D_00A4AC08, {D_00A4ABD0, D_00A47A48}},
    {D_00A4ABC8, {D_00A4ABD0, D_00A47468}},
    {D_00A4AB88, {D_00A4A768, D_00A47468}}
};

static EthName bltNameTbl[178] = {
    {D_00A4C320, {D_00A4C2E8, D_00A479B0}},
    {D_00A4C2E0, {D_00A4C2A0, D_00A479B0}},
    {D_00A4C298, {D_00A4C250, D_00A479B0}},
    {D_00A4C248, {D_00A4C200, D_00A479B0}},
    {D_00A4C1F8, {D_00A4C1B0, D_00A479B0}},
    {D_00A4C1A8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C1A0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C198, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C190, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C188, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C180, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C178, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C170, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C168, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C160, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C158, {D_00A4C128, D_00A47B28}},
    {D_00A4C120, {D_00A4C0F0, D_00A47B28}},
    {D_00A4C0E8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C0E0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C0D8, {D_00A4C0A8, D_00A47B28}},
    {D_00A4C0A0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C098, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C090, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C088, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C080, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C078, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C070, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4C068, {D_00A4C038, D_00A47AA0}},
    {D_00A4C030, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C028, {D_00A4BFF8, D_00A47AA0}},
    {D_00A4BFF0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4BFE8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BFE0, {D_00A4BFB8, D_00A479E0}},
    {D_00A4BFA8, {D_00A4BF70, D_00A47AA0}},
    {D_00A4BF68, {D_00A4BF40, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BF38, {D_00A4BF10, D_00A478E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BF08, {D_00A4BEE0, D_00A472D0}},
    {D_00A4BED8, {D_00A4BEB0, D_00A479B0}},
    {D_00A4BEA0, {D_00A4BE68, D_00A472D0}},
    {D_00A4BE58, {D_00A4BE18, D_00A472D0}},
    {D_00A4BE08, {D_00A4BDC8, D_00A472D0}},
    {D_00A4BDB8, {D_00A4BD78, D_00A472D0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BD70, {D_00A4BD58, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BD50, {D_00A4BD38, D_00A47380}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BD30, {D_00A4BD10, D_00A478E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BD08, {D_00A4BCF0, D_00A47A48}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BCE8, {D_00A4BCC8, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BCC0, {D_00A4BCB0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BCA8, {D_00A4BC68, D_00A472D0}},
    {D_00A4BC60, {D_00A4BC20, D_00A472D0}},
    {D_00A4BC18, {D_00A4BBD8, D_00A472D0}},
    {D_00A4BBD0, {D_00A4BB90, D_00A472D0}},
    {D_00A4BB88, {D_00A4A3A0, D_00A472D0}},
    {D_00A4BB80, {D_00A4BB48, D_00A47728}},
    {D_00A4BB40, {D_00A4BB08, D_00A47728}},
    {D_00A4BB00, {D_00A4BAC0, D_00A47728}},
    {D_00A4BAB8, {D_00A4BA88, D_00A47728}},
    {D_00A4BA80, {D_00A4BA50, D_00A47728}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BA48, {D_00A4BA30, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BA28, {D_00A4BA08, D_00A47A48}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4BA00, {D_00A4BCB0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B9F8, {D_00A4B9D8, D_00A478E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B9D0, {D_00A4B9B0, D_00A478E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B9A8, {D_00A4B970, D_00A47728}},
    {D_00A4B968, {D_00A4B930, D_00A47728}},
    {D_00A4B928, {D_00A4B8E8, D_00A47728}},
    {D_00A4B8E0, {D_00A4B8B0, D_00A47728}},
    {D_00A4B8A8, {D_00A4B878, D_00A47728}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B870, {D_00A4BCB0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B868, {D_00A4B848, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B840, {D_00A4B820, D_00A47468}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B818, {D_00A4B800, D_00A47380}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4B7F0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B7E0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B7D0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B7C0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B7B0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B7A0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B790, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B780, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B770, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4B760, {D_00A4A3A0, D_00A4A3A0}}
};

static EthName nrmNameTbl[44] = {
    {D_00A4C528, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C520, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C510, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C500, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4F0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4E0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4D8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4D0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4C8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4C0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4D0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4B8, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4B0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C4A0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C490, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C480, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C470, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C460, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4C450, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C440, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C430, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C420, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C410, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C400, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3F0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3E0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3D0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3C0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3B0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C3A0, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C390, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C380, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C370, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C360, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C350, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C340, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C330, {D_00A4A3A0, D_00A4A3A0}},
    {D_00A4C328, {D_00A4A3A0, D_00A4A3A0}}
};

static EthName spcNameTbl[53] = {
    {D_00A4D1E0, {D_00A4D190, D_00A47A70}},
    {D_00A4D180, {D_00A4D158, D_00A47380}},
    {D_00A4D148, {D_00A4D0F8, D_00A47468}},
    {D_00A4D0E8, {D_00A4D0B8, D_00A47A70}},
    {D_00A4D0A8, {D_00A4D070, D_00A472D0}},
    {D_00A4D060, {D_00A4D030, D_00A47788}},
    {D_00A4D020, {D_00A4CFE8, D_00A47A70}},
    {D_00A4CFD8, {D_00A4CF98, D_00A47788}},
    {D_00A4CF88, {D_00A4CF58, D_00A479E0}},
    {D_00A4CF48, {D_00A4CF18, D_00A479E0}},
    {D_00A4CF08, {D_00A4CED8, D_00A479E0}},
    {D_00A4CEC8, {D_00A4CE90, D_00A4CE88}},
    {D_00A4CE78, {D_00A4CE38, D_00A478E0}},
    {D_00A4CE28, {D_00A4CDF8, D_00A479E0}},
    {D_00A4CDE8, {D_00A4CDB8, D_00A479E0}},
    {D_00A4CDA8, {D_00A4CD78, D_00A47380}},
    {D_00A4CD68, {D_00A4CD38, D_00A478E0}},
    {D_00A4CD28, {D_00A4CCE0, D_00A478E0}},
    {D_00A4CCD0, {D_00A4CCA0, D_00A477D8}},
    {D_00A4CC90, {D_00A4CC40, D_00A47728}},
    {D_00A4CC30, {D_00A4CBF8, D_00A47A48}},
    {D_00A4CBE8, {D_00A4CBB0, D_00A47380}},
    {D_00A4CBA0, {D_00A4CB78, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4CB68, {D_00A4CB38, D_00A472D0}},
    {D_00A4CB28, {D_00A4CAE8, D_00A47380}},
    {D_00A4CAD8, {D_00A4CAA8, D_00A47AA0}},
    {D_00A4CAA0, {D_00A4CA68, D_00A472D0}},
    {D_00A4CA58, {D_00A4CA20, D_00A472D0}},
    {D_00A4CA10, {D_00A4C9D8, D_00A47728}},
    {D_00A4C9C8, {D_00A4C988, D_00A47468}},
    {D_00A4C978, {D_00A4C948, D_00A478E0}},
    {D_00A4C938, {D_00A4C908, D_00A473C0}},
    {D_00A4C8F8, {D_00A4C8C0, D_00A47AA0}},
    {D_00A4C8B0, {D_00A4C880, D_00A478E0}},
    {D_00A4C870, {D_00A4C838, D_00A47A70}},
    {D_00A4C828, {D_00A4C7E0, D_00A47788}},
    {D_00A4C7D0, {D_00A4C788, D_00A47AA0}},
    {D_00A4C778, {D_00A4C730, D_00A47AA0}},
    {D_00A4C718, {D_00A4C6F0, D_00A474A0}},
    {D_00A4C6D8, {D_00A4C6A8, D_00A47AA0}},
    {D_00A4C698, {D_00A4C668, D_00A478E0}},
    {D_00A4C658, {D_00A4C628, D_00A47380}},
    {D_00A4C618, {D_00A4C5D8, D_00A47AA0}},
    {D_00A4C5C8, {D_00A4C580, D_00A47A70}},
    {D_00A4C570, {D_00A4C538, D_00A478E0}}
};

static const char *enemyNameTbl[154] = {
    D_00A4D978, D_00A4D970, D_00A4D960, D_00A4D950, D_00A4D948, D_00A4D940, D_00A4D938, D_00A4D930, D_00A4D920, D_00A4D910, D_00A4D900, D_00A4D8F0, D_00A4D8E8, D_00A4D8E0, D_00A4D8D0, D_00A4D8C8, D_00A4D8C0, D_00A4D8B8, D_00A4D8A8, D_00A4D898, D_00A4D890, D_00A4D888, D_00A4D878, D_00A4D868, D_00A4D858, D_00A4D848, D_00A4D970, D_00A4D960, D_00A4D950, D_00A4D838, D_00A4D828, D_00A4D818, D_00A4D808, D_00A4D7F8, D_00A4D7E8, D_00A4D7D8, D_00A4D7D0, D_00A4D7C8, D_00A4D7B8, D_00A4D7A8, D_00A4D7A0, D_00A4D798, D_00A4D788, D_00A4D788, D_00A4D780, D_00A4D778, D_00A4D768, D_00A4D758, D_00A4D748, D_00A4D738, D_00A4D728, D_00A4D718, D_00A4D708, D_00A4D6F8, D_00A4D6F0, D_00A4D6E8, D_00A4D6D8, D_00A4D6C8, D_00A4D6B8, D_00A4D6A8, D_00A4D698, D_00A4D688, D_00A4D678, D_00A4D668, D_00A4D658, D_00A4D650, D_00A4D658, D_00A4D650, D_00A4D648, D_00A4D640, D_00A4D630, D_00A4D620, D_00A4D620, D_00A4D610, D_00A4D620, D_00A4D608, D_00A4D5F8, D_00A4D5E8, D_00A4D5E0, D_00A4D5E0, D_00A4D738, D_00A4D938, D_00A4D828, D_00A4D7A0, D_00A4D798, D_00A4D5D0, D_00A4D5C0, D_00A4D5B0, D_00A4D5A0, D_00A4D590, D_00A4D580, D_00A4D570, D_00A4D560, D_00A4D550, D_00A4D540, D_00A4D530, D_00A4D520, D_00A4D510, D_00A4D500, D_00A4D4F0, D_00A4D4E0, D_00A4D4D0, D_00A4D4C0, D_00A4D4B0, D_00A4D4A0, D_00A4D490, D_00A4D480, D_00A4D470, D_00A4D460, D_00A4D450, D_00A4D440, D_00A4D430, D_00A4D420, D_00A4D410, D_00A4D400, D_00A4D3E8, D_00A4D3D0, D_00A4D3B8, D_00A4D3B0, D_00A4D3A0, D_00A4D398, D_00A4D390, D_00A4D380, D_00A4D370, D_00A4D360, D_00A4D350, D_00A4D340, D_00A4D338, D_00A4D330, D_00A4D320, D_00A4D318, D_00A4D308, D_00A4D300, D_00A4D2F8, D_00A4D2E8, D_00A4D2D8, D_00A4D2C8, D_00A4D2B8, D_00A4D2B0, D_00A4D2A0, D_00A4D290, D_00A4D280, D_00A4D270, D_00A4D260, D_00A4D250, D_00A4D240, D_00A4D230, D_00A4D228, D_00A4D218, D_00A4D208, D_00A4D200, D_00A4D398, D_00A4D390, D_00A4D1F0
};

static const char **statNameTbl[8] = {
    ba, ma, bb, mb, aa1, aa2, ab, 0x00000000
};

static const char *playerNameTbl[32] = {
    D_00A4DCD8, D_00A4DCD0, D_00A4DCC8, D_00A4A3A0, D_00A4DCB8, D_00A4DCB0, D_00A4DCA8, D_00A4A3A0, D_00A4A3A0, D_00A4DCA0, D_00A4DC98, D_00A4A3A0, D_00A4A3A0, D_00A4A3A0, D_00A4A3A0, D_00A4A3A0, D_00A4DC90, D_00A4DC88, D_00A4DC80, D_00A4DC78, D_00A4DC70, D_00A4DC68, D_00A4DC60, D_00A4DC58, D_00A4DC50, D_00A4DC48, D_00A4DC40, D_00A4DC38, D_00A4DC30, D_00A4A3A0, D_00A4A3A0, D_00A4A3A0
};

static const char *ziggyName = D_00A4DCE0;

static EthName engNameTbl[40] = {
    {D_00A4DE48, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DE28, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DE18, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DE08, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DE08, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DDF8, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DDE8, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DDD8, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DDC0, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DDC0, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DDB0, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DDA0, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DD90, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DD90, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DD78, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DD68, {D_00A4DD88, D_00A47DE8}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4DD58, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD48, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD38, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD38, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD28, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD18, {D_00A4DC58, D_00A47A70}},
    {D_00A4DD08, {D_00A4DC48, D_00A47A70}},
    {D_00A4DCF8, {D_00A4DC48, D_00A47A70}},
    {D_00A4DCE8, {D_00A4DC40, D_00A47A70}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}}
};

static EthName frmNameTbl[40] = {
    {D_00A4DF98, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF88, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF78, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF68, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF68, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF58, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF48, {D_00A4DE38, D_00A47DE8}},
    {D_00A4DF38, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DF28, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DF28, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DF18, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DF08, {D_00A4DDD0, D_00A47DE8}},
    {D_00A4DEF8, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DEF8, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DEE8, {D_00A4DD88, D_00A47DE8}},
    {D_00A4DED8, {D_00A4DD88, D_00A47DE8}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A4DEC8, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DEB8, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DEA8, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DEA8, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DE98, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DE88, {D_00A4DC58, D_00A47DE8}},
    {D_00A4DE78, {D_00A4DC48, D_00A47DE8}},
    {D_00A4DE68, {D_00A4DC48, D_00A47DE8}},
    {D_00A4DE58, {D_00A4DC40, D_00A47DE8}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}},
    {D_00A46DB8, {D_00A4A3A0, D_00A479E0}}
};


const char **dataEthNameGet(int ethId)
{
    if (ethId <= 0)
    {
        printf(D_00A4DFA8, ethId);
        return 0;
    }
    return &ethNameTbl[ethId - 1].name;
}

const char D_00A4DFC8[] = "** dataItmNameGet: err %d\n";

const char **dataItmNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4DFC8, index);
        return 0;
    }
    return &itmNameTbl[index - 1].name;
}

const char D_00A4DFE8[] = "** dataSklNameGet: err %d\n";

const char **dataSklNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4DFE8, index);
        return 0;
    }
    return &sklNameTbl[index - 1].name;
}

const char D_00A4E008[] = "** dataAccNameGet: err %d\n";

const char **dataAccNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E008, index);
        return 0;
    }
    return &accNameTbl[index - 1].name;
}

const char D_00A4E028[] = "** dataWepNameGet: err %d\n";

const char **dataWepNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E028, index);
        return 0;
    }
    return &wepNameTbl[index - 1].name;
}

const char D_00A4E048[] = "** dataBltNameGet: err %d\n";

const char **dataBltNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E048, index);
        return 0;
    }
    return &bltNameTbl[index - 1].name;
}

const char D_00A4E068[] = "** dataNrmNameGet: err %d\n";

const char **dataNrmNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E068, index);
        return 0;
    }
    return &nrmNameTbl[index - 1].name;
}

const char D_00A4E088[] = "** dataSpcNameGet: err %d\n";
/*
 * spcNameTbl (0x27C bytes, 53 entries) is laid out right after nrmNameTbl
 * (0x210 bytes, 44 entries): special item ids continue the normal item id
 * space, so the table index here is (index - 45).
 */

const char **dataSpcNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E088, index);
        return 0;
    }
    return &spcNameTbl[index - 45].name;
}

const char D_00A4E0A8[] = "** dataEnemyNameGet: err %d\n";
/* enemyNameTbl holds 154 name pointers (0x268 bytes, stride 4); enemy ids start at 33. */

const char **dataEnemyNameGet(int index)
{
    if (index < 33)
    {
        printf(D_00A4E0A8, index);
        return 0;
    }
    return &enemyNameTbl[index - 33];
}

/* statNameTbl holds one name-table pointer per stat type; each is indexed by `index`. */

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

extern int MenuScenarioNoGet(void);
const char D_00A4E0C8[] = "** dataPlayerNameGet: err %d\n";

const char **dataPlayerNameGet(int id)
{
    int no;

    if (id >= 33)
    {
        printf(D_00A4E0C8, id);
        return 0;
    }
    if (id == 5)
    {
        no = MenuScenarioNoGet();
        if (no >= 108)
        {
            return &ziggyName;
        }
    }
    return &playerNameTbl[id - 1];
}

const char D_00A4E0E8[] = "** dataEngNameGet: err %d\n";

const char **dataEngNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E0E8, index);
        return 0;
    }
    return &engNameTbl[index - 1].name;
}

const char D_00A4E108[40] = "** dataFrmNameGet: err %d\n";

const char **dataFrmNameGet(int index)
{
    if (index <= 0)
    {
        printf(D_00A4E108, index);
        return 0;
    }
    return &frmNameTbl[index - 1].name;
}

extern const char D_00A4D9A0[];
extern const char D_00A4D998[];
extern const char D_00A4D9B8[];
extern const char D_00A4D988[];
extern const char D_00A4DA00[];
extern const char D_00A4D9F0[];
extern const char D_00A4DA10[];
extern const char D_00A4DA08[];
extern const char D_00A4D9D0[];
extern const char D_00A4D9E0[];
extern const char D_00A4D9C0[];
extern const char D_00A4D9A8[];

extern const char D_00A4DA50[];
extern const char D_00A4DA28[];

extern const char D_00A4DA40[];
extern const char D_00A4DA88[];
extern const char D_00A4DA80[];
extern const char D_00A4DA68[];
extern const char D_00A4DA78[];
extern const char D_00A4DA18[];
extern const char D_00A4DA58[];
extern const char D_00A4DAF8[];
extern const char D_00A4DB28[];
extern const char D_00A4DAC8[];
extern const char D_00A4DA98[];
extern const char D_00A4DB30[];
extern const char D_00A4DB08[];
extern const char D_00A4DAA0[];
extern const char D_00A4DAA8[];
extern const char D_00A4DAB8[];
extern const char D_00A4DAD8[];
extern const char D_00A4DB18[];
extern const char D_00A4DAE8[];
extern const char D_00A4DB50[];
extern const char D_00A4DB40[];
extern const char D_00A4DB78[];


extern const char D_00A4DB68[];
extern const char D_00A4DB88[];
extern const char D_00A4DB60[];
extern const char D_00A4DBE0[];
extern const char D_00A4DBD0[];
extern const char D_00A4DB98[];
extern const char D_00A4DBC0[];
extern const char D_00A4DBA8[];
extern const char D_00A4DBB0[];
extern const char D_00A4DC10[];
extern const char D_00A4DBF0[];
extern const char D_00A4DC00[];
extern const char D_00A4DC20[];

static const char * ba[14] = { D_00A4DA10, D_00A4DA08, D_00A4DA00, D_00A4D9F0, D_00A4D9E0, D_00A4D9D0, D_00A4D9C0, D_00A4D9B8, D_00A4D9A8, D_00A4D9A0, D_00A4D998, D_00A4D988, D_00A4D988, D_00A4D988 };

static const char * ma[11] = { D_00A4DA88, D_00A4DA80, D_00A4DA78, D_00A4DA68, D_00A4DA58, D_00A4DA50, D_00A48F20, D_00A4DA40, D_00A4DA28, D_00A48EF8, D_00A4DA18 };

static const char * bb[12] = { D_00A4DB30, D_00A4DB28, D_00A4DB18, D_00A4DB08, D_00A4DAF8, D_00A4DAE8, D_00A4DAD8, D_00A4DAC8, D_00A4DAB8, D_00A4DAA8, D_00A4DAA0, D_00A4DA98 };

static const char * mb[8] = { D_00A4DB88, D_00A4DB78, D_00A4DB68, D_00A4DB60, D_00A4DB50, D_00A4DB40, D_00A47E30, D_00A47DD8 };

static const char * aa1[13] = { D_00A4DBE0, D_00A4DBD0, D_00A4DA08, D_00A4DBC0, D_00A4D9F0, D_00A4D9E0, D_00A4DBB0, D_00A4DBA8, D_00A4D9B8, D_00A4DB98, D_00A4D988, D_00A4D988, D_00A4D988 };

static const char * aa2[3] = { D_00A4DC10, D_00A4DC00, D_00A4DBF0 };

static const char * ab[4] = { D_00A4DAD8, D_00A4DAC8, D_00A4DAA0, D_00A4DC20 };

const char D_00A46C28[24] = "Self\nIncrease Speed 50%";

const char D_00A46C40[16] = "Speed Boost";

const char D_00A46C50[64] = "One Enemy/1x only\n80% Critical Hit rate against M enemies";

const char D_00A46C90[16] = "Red Mark";

const char D_00A46CA0[40] = "Self/1x only\nHP & EP recovery x2";

const char D_00A46CC8[16] = "Charge X";

const char D_00A46CD8[24] = "Self\nFocus Ether Atk";

const char D_00A46CF0[16] = "Ether Shift B";

const char D_00A46D00[24] = "Self\nFocus Ether Def";

const char D_00A46D18[16] = "Ether Shift A";

const char D_00A46D28[24] = "Self\nHP recovery/Mid";

const char D_00A46D40[16] = "Recharge";

const char D_00A46D50[48] = "Self\nIncrease Str/Def when MOMO is behind";

const char D_00A46D80[16] = "Bodyguard";

const char D_00A46D90[24] = "Self\nFocus Phys Def";

const char D_00A46DA8[16] = "My Guard";

const char D_00A46DB8[8] = "Reserve";

const char D_00A46DC0[48] = "All enemies/1x per battle\nSummon Great Joe";

const char D_00A46DF0[16] = "Buster Joe";

const char D_00A46E00[48] = "One enemy/1x per battle\nSummon Great Joe";

const char D_00A46E30[16] = "Magnum Joe";

const char D_00A46E40[16] = "Speed Machine";

const char D_00A46E50[48] = "One ally\nBlock stat. changes & support effects";

const char D_00A46E80[16] = "Coin Lock";

const char D_00A46E90[40] = "One enemy\nDisable B & G enemies' spells";

const char D_00A46EB8[8] = "Misty";

const char D_00A46EC0[40] = "One enemy\nEvade down for B & G enemies";

const char D_00A46EE8[8] = "Chain";

const char D_00A46EF0[24] = "One enemy\nSteal Items";

const char D_00A46F08[16] = "Psycho Pocket";

const char D_00A46F18[56] = "Self/1x only\nRevive when female character is behind";

const char D_00A46F50[16] = "Dandyism";

const char D_00A46F60[48] = "One enemy/ally\nIncrease Ether effect 25% ";

const char D_00A46F90[16] = "Ether Flare";

const char D_00A46FA0[24] = "Self\nIncrease Phys Atk";

const char D_00A46FB8[16] = "Psycho Arm";

const char D_00A46FC8[56] = "All enemies/1x per Starlight\nNon-elemental Ether Atk";

const char D_00A47000[16] = "Star Bunnie";

const char D_00A47010[56] = "One enemy/Only during Starlight\nNon-elemental Ether Atk";

const char D_00A47048[16] = "Star Bell";

const char D_00A47058[56] = "Self/1x per battle\nUse starlight power to transform";

const char D_00A47090[16] = "Starlight";

const char D_00A470A0[40] = "One ally/1x per Star Wind\nMax AP";

const char D_00A470C8[16] = "Star Action";

const char D_00A470D8[56] = "All allies/Only during Star Wind\nEther effect down 25%";

const char D_00A47110[16] = "Star Veil";

const char D_00A47120[56] = "Self/1x per battle\nUse star wind power to transform";

const char D_00A47158[16] = "Star Wind";

const char D_00A47168[40] = "One enemy\nReduce M enemy action 50%";

const char D_00A47190[16] = "Junk Beam";

const char D_00A471A0[40] = "One ally\nRevive + HP recovery/Low";

const char D_00A471C8[8] = "Refine";

const char D_00A471D0[24] = "Self\nA heavenly gift";

const char D_00A471E8[8] = "Prayer";

const char D_00A471F0[40] = "One enemy\nPut M enemy pilots to sleep";

const char D_00A47218[16] = "Sheep Beam";

const char D_00A47228[40] = "One enemy\nNon-elemental Ether Atk";

const char D_00A47250[16] = "Miracle Star";

const char D_00A47260[32] = "One enemy/ally\nHP Recovery/Mid";

const char D_00A47280[16] = "Life Shot";

const char D_00A47290[48] = "All enemies\nAttack + clear all St changes";

const char D_00A472C0[16] = "Light & Wings";

const char D_00A472D0[8] = "C";

const char D_00A472D8[40] = "All enemies\nReset attack & wait time";

const char D_00A47300[16] = "Cataclysm";

const char D_00A47310[40] = "One enemy\nHP recovery down (1/2)";

const char D_00A47338[24] = "Supreme Judgment";

const char D_00A47350[32] = "All allies/1x per battle\nRevive";

const char D_00A47370[16] = "Best Ally";

const char D_00A47380[8] = "L";

const char D_00A47388[40] = "One enemy/Lightning\nLightning Ether Atk";

const char D_00A473B0[16] = "Lightning Wings";

const char D_00A473C0[8] = "F";

const char D_00A473C8[32] = "One enemy/Fire\nFire Ether Atk";

const char D_00A473E8[16] = "Flame Wings";

const char D_00A473F8[40] = "All allies\nClear all status changes";

const char D_00A47420[16] = "Purifying Storm";

const char D_00A47430[8] = "I";

const char D_00A47438[32] = "One enemy/Ice\nIce Ether Atk";

const char D_00A47458[16] = "Ice Wings";

const char D_00A47468[8] = "H";

const char D_00A47470[32] = "All allies\nHP recovery/Low";

const char D_00A47490[16] = "Healing Dew";

const char D_00A474A0[8] = "P";

const char D_00A474A8[40] = "One enemy/ally\nEther effect down 25%";

const char D_00A474D0[16] = "Protective Wear";

const char D_00A474E0[48] = "All enemies/Lightning\nLightning Ether Atk";

const char D_00A47510[16] = "Dex Ether Le";

const char D_00A47520[32] = "All enemies/Fire\nFire Ether Atk";

const char D_00A47540[16] = "Dex Ether Fa";

const char D_00A47550[32] = "All enemies/Ice\nIce Ether Atk";

const char D_00A47570[16] = "Dex Ether Ra";

const char D_00A47580[56] = "All enemies/Beam/1x per battle\nSatellite beam weapon";

const char D_00A475B8[16] = "Satellite";

const char D_00A475C8[40] = "Self\nEther Atk damage and cost x2";

const char D_00A475F0[16] = "Ether Limit";

const char D_00A47600[72] = "One enemy/Occasionally \"Instant KO\"\nHP/4 damage on B & G enemies";

const char D_00A47648[8] = "Gate";

const char D_00A47650[40] = "One enemy\nEther down for B & G enemies";

const char D_00A47678[16] = "Down Ether";

const char D_00A47688[48] = "One enemy\nPhys Atk down for B & G enemies";

const char D_00A476B8[16] = "Down Force";

const char D_00A476C8[48] = "One enemy\nDexterity down for B & G enemies";

const char D_00A476F8[16] = "Down Dex";

const char D_00A47708[24] = "Self\nFocus Phys Atk";

const char D_00A47720[8] = "Mode A7";

const char D_00A47728[8] = "E";

const char D_00A47730[72] = "All enemies/Slash/1x per battle\nUltimate attack using combined form";

const char D_00A47778[16] = "Erde Kaiser";

const char D_00A47788[8] = "D";

const char D_00A47790[56] = "All enemies/Hit/1x per battle\nUltra heavy weight attack";

const char D_00A477C8[16] = "Dominion Tank";

const char D_00A477D8[8] = "T";

const char D_00A477E0[56] = "All enemies/Slash/1x per battle\nGiant top attack";

const char D_00A47818[16] = "Throni Blade";

const char D_00A47828[56] = "All enemies/Beam/1x per battle\nSpread beam attack";

const char D_00A47860[16] = "Seraphim Bird";

const char D_00A47870[48] = "One enemy/Occasionally \"Instant KO\"\nSteal items";

const char D_00A478A0[16] = "Queen's Kiss";

const char D_00A478B0[8] = "Q";

const char D_00A478B8[32] = "One ally\nIncrease speed 25%";

const char D_00A478D8[8] = "Quick";

const char D_00A478E0[8] = "S";

const char D_00A478E8[32] = "1x per ally\nSurvive with 1 HP";

const char D_00A47908[16] = "Safety Level";

const char D_00A47918[40] = "One ally\nRevive & HP recovery/Mid";

const char D_00A47940[8] = "Revert";

const char D_00A47948[32] = "One enemy/ally\nHP recovery/Max";

const char D_00A47968[16] = "Medica Rest";

const char D_00A47978[8] = "J";

const char D_00A47980[40] = "One enemy\nDexterity down for M enemies";

const char D_00A479A8[8] = "Jamming";

const char D_00A479B0[8] = "B";

const char D_00A479B8[32] = "One ally\nIncrease Boost by 1";

const char D_00A479D8[8] = "Boost 1";

const char D_00A479E0[8] = "R";

const char D_00A479E8[40] = "One ally\nClear all status changes";

const char D_00A47A10[8] = "Refresh";

const char D_00A47A18[32] = "All allies\nHP recovery/Mid";

const char D_00A47A38[16] = "Medica All";

const char D_00A47A48[8] = "G";

const char D_00A47A50[24] = "Ally\nEscape from battle";

const char D_00A47A68[8] = "Goodbye";

const char D_00A47A70[8] = "A";

const char D_00A47A78[32] = "One enemy\nAnalyze items and HP";

const char D_00A47A98[8] = "Analyze";

const char D_00A47AA0[8] = "M";

const char D_00A47AA8[32] = "One enemy/ally\nHP recovery/Low";

const char D_00A47AC8[8] = "Medica";

const char D_00A47AD0[8] = "Diamond";

const char D_00A47AD8[8] = "Emerald";

const char D_00A47AE0[8] = "Ruby";

const char D_00A47AE8[16] = "Sapphire";

const char D_00A47AF8[8] = "Garnet";

const char D_00A47B00[24] = "Junked Circuit B";

const char D_00A47B18[16] = "Precious Stone";

const char D_00A47B28[8] = "K";

const char D_00A47B30[16] = "Kobold Blade";

const char D_00A47B40[8] = "U";

const char D_00A47B48[16] = "Unicorn Horn";

const char D_00A47B58[24] = "One ally\nS.Pts +100";

const char D_00A47B70[16] = "Skill Upgrade Z";

const char D_00A47B80[24] = "One ally\nS.Pts +50";

const char D_00A47B98[16] = "Skill Upgrade S";

const char D_00A47BA8[24] = "One ally\nS.Pts +10";

const char D_00A47BC0[16] = "Skill Upgrade A";

const char D_00A47BD0[24] = "One ally\nE.Pts +100";

const char D_00A47BE8[16] = "Ether Upgrade Z";

const char D_00A47BF8[24] = "One ally\nE.Pts +50";

const char D_00A47C10[16] = "Ether Upgrade S";

const char D_00A47C20[24] = "One ally\nE.Pts +10";

const char D_00A47C38[16] = "Ether Upgrade A";

const char D_00A47C48[24] = "One ally\nT.Pts +100";

const char D_00A47C60[16] = "Tech Upgrade Z";

const char D_00A47C70[24] = "One ally\nT.Pts +50";

const char D_00A47C88[16] = "Tech Upgrade S";

const char D_00A47C98[24] = "One ally\nT.Pts +10";

const char D_00A47CB0[16] = "Tech Upgrade A";

const char D_00A47CC0[24] = "One ally\nSkill level +1";

const char D_00A47CD8[16] = "Skill Upgrade";

const char D_00A47CE8[24] = "Junked Circuit A";

const char D_00A47D00[16] = "Barter item";

const char D_00A47D10[16] = "Scrap Iron";

const char D_00A47D20[40] = "A.G.W.S.\nFHP recovery/50% recovery";

const char D_00A47D48[16] = "Frame Repair Z";

const char D_00A47D58[40] = "A.G.W.S.\nFHP recovery/25% recovery";

const char D_00A47D80[16] = "Frame Repair A";

const char D_00A47D90[56] = "All allies/Save Points only\nHP & EP full recovery";

const char D_00A47DC8[16] = "Bio Sphere";

const char D_00A47DD8[16] = "Anti-Veil";

const char D_00A47DE8[8] = "V";

const char D_00A47DF0[64] = "One enemy/ally/W (Press \242\244 at AP6) \nEther effects down 25%";

const char D_00A47E30[8] = "Veil";

const char D_00A47E38[56] = "One ally/W (Press \242\244 at AP6) \nIncreases speed 50%";

const char D_00A47E70[16] = "Speed Stim DX";

const char D_00A47E80[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Def";

const char D_00A47EB8[16] = "Defense Shield";

const char D_00A47EC8[32] = "One ally\nIncreases speed 50%";

const char D_00A47EE8[16] = "Speed Stim SX";

const char D_00A47EF8[56] = "One ally/W (Press \242\244 at AP6) \nIncreases speed 25%";

const char D_00A47F30[16] = "Speed Stim";

const char D_00A47F40[16] = "One ally\nHP=1";

const char D_00A47F50[8] = "Hemlock";

const char D_00A47F58[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Atk 50%";

const char D_00A47F90[8] = "Stim DX";

const char D_00A47F98[56] = "One ally/W (Press \242\244 at AP6) \nIncreases Phys Atk 25%";

const char D_00A47FD0[8] = "Stim";

const char D_00A47FD8[32] = "One ally\nEscape from battle";

const char D_00A47FF8[16] = "Escape Pack";

const char D_00A48008[24] = "One ally\nBoost+1";

const char D_00A48020[16] = "Booster Pack";

const char D_00A48030[8] = "N";

const char D_00A48038[56] = "One ally/W (Press \242\244 at AP6) \nPsych Status Clear";

const char D_00A48070[16] = "Neuro Stim";

const char D_00A48080[48] = "One ally/W (Press \242\244 at AP6) \nPhys Status Clear";

const char D_00A480B0[16] = "Antidote";

const char D_00A480C0[48] = "One ally/W (Press \242\244 at AP6) \nAll Status Clear";

const char D_00A480F0[16] = "Cure-All";

const char D_00A48100[72] = "One ally/W (Press \242\244 at AP6) \nRevives a KO'd ally & HP recovery/Max";

const char D_00A48148[16] = "Revive DX";

const char D_00A48158[72] = "One ally/W (Press \242\244 at AP6) \nRevives a KO'd ally & HP recovery/Low";

const char D_00A481A0[8] = "Revive";

const char D_00A481A8[56] = "One ally/W (Press \242\244 at AP6) \nHP & EP recovery/Max";

const char D_00A481E0[16] = "Rejuvenator";

const char D_00A481F0[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Max";

const char D_00A48220[16] = "Ether Pack MAX";

const char D_00A48230[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/High";

const char D_00A48260[16] = "Ether Pack DX";

const char D_00A48270[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Mid";

const char D_00A482A0[16] = "Ether Pack S";

const char D_00A482B0[48] = "One ally/W (Press \242\244 at AP6) \nEP recovery/Low";

const char D_00A482E0[16] = "Ether Pack";

const char D_00A482F0[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Max";

const char D_00A48328[16] = "Med Kit MAX";

const char D_00A48338[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/High";

const char D_00A48370[16] = "Med Kit DX";

const char D_00A48380[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Mid";

const char D_00A483B8[16] = "Med Kit S";

const char D_00A483C8[56] = "One enemy/ally/W (Press \242\244 at AP6) \nHP recovery/Low";

const char D_00A48400[8] = "Med Kit";

const char D_00A48408[16] = "Prevents \"Stop\"";

const char D_00A48418[16] = "Stop Guard";

const char D_00A48428[24] = "Prevents \"Confusion\"";

const char D_00A48440[16] = "Confusion Guard";

const char D_00A48450[32] = "Displays enemy HP information";

const char D_00A48470[16] = "Search Eyes";

const char D_00A48480[56] = "Auto-Boost when attacked \n\"Counter+10\" skill necessary";

const char D_00A484B8[8] = "CB On";

const char D_00A484C0[40] = "Increases Ether Def as allies are KO'd";

const char D_00A484E8[16] = "Lonely EDUP";

const char D_00A484F8[40] = "Increases Phys Def as allies are KO'd";

const char D_00A48520[16] = "Lonely PDUP";

const char D_00A48530[56] = "Ether Def +4\n2x effect when combined with an accessory";

const char D_00A48568[8] = "EDEF+4";

const char D_00A48570[56] = "Phys Def +4\n2x effect when combined with an accessory";

const char D_00A485A8[8] = "PDEF+4";

const char D_00A485B0[80] = "Reduces Slsh/Prce-type damage 20%\n2x effect when combined with an accessory";

const char D_00A48600[16] = "Sword-20";

const char D_00A48610[72] = "Reduces B-type damage 20%\n2x effect when combined with an accessory";

const char D_00A48658[8] = "Beam-20";

const char D_00A48660[72] = "Reduces I-type damage 20%\n2x effect when combined with an accessory";

const char D_00A486A8[8] = "Ice-20";

const char D_00A486B0[72] = "Reduces L-type damage 20%\n2x effect when combined with an accessory";

const char D_00A486F8[16] = "Lightning-20";

const char D_00A48708[72] = "Reduces F-type damage 20%\n2x effect when combined with an accessory";

const char D_00A48750[8] = "Fire-20";

const char D_00A48758[32] = "Boost +1 at start of battle";

const char D_00A48778[16] = "Battle BC+1";

const char D_00A48788[32] = "Str increases as HP decreases";

const char D_00A487A8[16] = "HP Strong";

const char D_00A487B8[72] = "Increase S.Pts earned 25%\n2x effect when combined with an accessory";

const char D_00A48800[16] = "Skill P+25";

const char D_00A48810[72] = "Increases E.Pts earned 25%\n2x effect when combined with an accessory";

const char D_00A48858[16] = "Ether P+25";

const char D_00A48868[72] = "Increases T.Pts earned 25%\n2x effect when combined with an accessory";

const char D_00A488B0[16] = "Tech P+25";

const char D_00A488C0[80] = "Increases experience points 25%\n2x effect when combined with an accessory";

const char D_00A48910[16] = "Experience P+25";

const char D_00A48920[56] = "HI Slot Tech Attack increases in Critical Hit rate";

const char D_00A48958[16] = "Tech Attack AC";

const char D_00A48968[32] = "HI Slot Tech Attack EP=0";

const char D_00A48988[16] = "Tech Attack AE";

const char D_00A48998[8] = "W";

const char D_00A489A0[40] = "\242\242\241\373\241\373, \242\244\241\373\241\373 2x Tech Attack possible";

const char D_00A489C8[16] = "W Special";

const char D_00A489D8[40] = "Fluctuating damage depending on HP";

const char D_00A48A00[16] = "Damage UD";

const char D_00A48A10[40] = "Each turn AP +1\n (Character only) ";

const char D_00A48A38[8] = "AP+1";

const char D_00A48A40[88] = "Increases rare item acquisition rate 30%\n2x effect when combined with an accessory";

const char D_00A48A98[8] = "Rare+30";

const char D_00A48AA0[88] = "Increases rare item acquisition rate 10%\n2x effect when combined with an accessory";

const char D_00A48AF8[8] = "Rare+10";

const char D_00A48B00[64] = "Increases Max EP 15%\n2x effect when combined with an accessory";

const char D_00A48B40[16] = "EPMAX+15";

const char D_00A48B50[64] = "Increases Max HP 15%\n2x effect when combined with an accessory";

const char D_00A48B90[16] = "HPMAX+15";

const char D_00A48BA0[64] = "Reduces damage 10%\n2x effect when combined with an accessory";

const char D_00A48BE0[16] = "Damage-10";

const char D_00A48BF0[72] = "Increases counter rate 10%\n2x effect when combined with an accessory";

const char D_00A48C38[16] = "Counter+10";

const char D_00A48C48[80] = "Increases Critical Hit rate 5%\n2x effect when combined with an accessory";

const char D_00A48C98[8] = "CRTC+5";

const char D_00A48CA0[56] = "Agility +1\n2x effect when combined with an accessory";

const char D_00A48CD8[8] = "AGL+1";

const char D_00A48CE0[56] = "Ether Def +2\n2x effect when combined with an accessory";

const char D_00A48D18[8] = "EDEF+2";

const char D_00A48D20[56] = "Ether Atk +2\n2x effect when combined with an accessory";

const char D_00A48D58[8] = "EATK+2";

const char D_00A48D60[56] = "Evade +2\n2x effect when combined with an accessory";

const char D_00A48D98[8] = "EVA+2";

const char D_00A48DA0[56] = "Dexterity +2\n2x effect when combined with an accessory";

const char D_00A48DD8[8] = "DEX+2";

const char D_00A48DE0[56] = "Phys Def +2\n2x effect when combined with an accessory";

const char D_00A48E18[8] = "PDEF+2";

const char D_00A48E20[56] = "Phys Atk +2\n2x effect when combined with an accessory";

const char D_00A48E58[8] = "PATK+2";

const char D_00A48E60[32] = "\"Anti-Veil\" support effect";

const char D_00A48E80[16] = "Ether Power U";

const char D_00A48E90[24] = "\"Veil\" support effect";

const char D_00A48EA8[16] = "Ether Power D";

const char D_00A48EB8[24] = "Recovery amount x2";

const char D_00A48ED0[16] = "Recovery Double";

const char D_00A48EE0[24] = "Status effect time x2";

const char D_00A48EF8[16] = "ST Double";

const char D_00A48F08[24] = "EP consumption 1/2";

const char D_00A48F20[8] = "EP Half";

const char D_00A48F28[64] = "Increases speed 25%\n2x effect when combined with an accessory";

const char D_00A48F68[16] = "Speed +25";

const char D_00A48F78[24] = "Prevents EP damage";

const char D_00A48F90[16] = "EP Guard";

const char D_00A48FA0[56] = "Prevents all status abnormalities & support effects";

const char D_00A48FD8[16] = "All Guard";

const char D_00A48FE8[32] = "Prevents \"Attack Disable\"";

const char D_00A49008[16] = "Special Guard";

const char D_00A49018[24] = "Prevents \"Instant KO\"";

const char D_00A49030[16] = "KO Guard";

const char D_00A49040[24] = "Prevents \"Poison\"";

const char D_00A49058[16] = "Poison Guard";

const char D_00A49068[16] = "Prevents \"Slow\"";

const char D_00A49078[16] = "Slow Guard";

const char D_00A49088[24] = "Prevents \"AP Half\"";

const char D_00A490A0[16] = "AP Guard";

const char D_00A490B0[32] = "Prevents \"Ether Atk Down\"";

const char D_00A490D0[16] = "EATK Guard";

const char D_00A490E0[16] = "Prevents \"Lost\"";

const char D_00A490F0[16] = "Lost Guard";

const char D_00A49100[24] = "Prevents \"Sleep\"";

const char D_00A49118[16] = "Sleep Guard";

const char D_00A49128[32] = "Prevents \"Phys Def Down\"";

const char D_00A49148[16] = "PDEF Guard";

const char D_00A49158[32] = "Prevents \"Phys Atk Down\"";

const char D_00A49178[16] = "PATK Guard";

const char D_00A49188[24] = "Prevents \"Evade Down\"";

const char D_00A491A0[16] = "EVA Guard";

const char D_00A491B0[32] = "Prevents \"Dexterity Down\"";

const char D_00A491D0[16] = "DEX Guard";

const char D_00A491E0[32] = "Prevents \"EP Overconsumption\"";

const char D_00A49200[16] = "EP Double Guard";

const char D_00A49210[16] = "Prevents \"Bind\"";

const char D_00A49220[16] = "Bind Guard";

const char D_00A49230[32] = "Prevents \"Attack Poison\"";

const char D_00A49250[16] = "A-Poison Guard";

const char D_00A49260[24] = "Prevents \"Critical Hit\"";

const char D_00A49278[16] = "CRTC Guard";

const char D_00A49288[32] = "Reduces B-type damage 25%";

const char D_00A492A8[16] = "Anti-Beam Armor";

const char D_00A492B8[32] = "Reduces L-type damage 25%";

const char D_00A492D8[24] = "Anti-Lightning Armor";

const char D_00A492F0[32] = "Reduces I-type damage 25%";

const char D_00A49310[16] = "Anti-Ice Armor";

const char D_00A49320[32] = "Reduces F-type damage 25%";

const char D_00A49340[16] = "Anti-Fire Armor";

const char D_00A49350[16] = "Ether Def +6";

const char D_00A49360[16] = "EF Circuit C";

const char D_00A49370[16] = "EF Circuit B";

const char D_00A49380[16] = "EF Circuit A";

const char D_00A49390[16] = "Armor +6";

const char D_00A493A0[24] = "Auxiliary Armor C";

const char D_00A493B8[16] = "Armor +4";

const char D_00A493C8[24] = "Auxiliary Armor B";

const char D_00A493E0[16] = "Armor +2";

const char D_00A493F0[24] = "Auxiliary Armor A";

const char D_00A49408[40] = "Recovers 10% of Max FHP when guarding";

const char D_00A49430[16] = "Guard Recovery";

const char D_00A49440[48] = "Clears status abnormalities when guarding";

const char D_00A49470[16] = "Guard Cleaner";

const char D_00A49480[16] = "Tuned Circuit";

const char D_00A49490[40] = "Prevents \"Pilot Confusion & Sleep\"";

const char D_00A494B8[16] = "Cockpit Guard";

const char D_00A494C8[24] = "W Circuit Shield";

const char D_00A494E0[16] = "AP Shield";

const char D_00A494F0[16] = "Prevents \"Wear\"";

const char D_00A49500[16] = "M Chip Guard";

const char D_00A49510[24] = "Prevents \"Ether Crash\"";

const char D_00A49528[24] = "E Circuit Shield";

const char D_00A49540[16] = "Gear Shield";

const char D_00A49550[24] = "Prevents \"Engine Stop\"";

const char D_00A49568[16] = "Engine Shield";

const char D_00A49578[24] = "Prevents \"Power Loss\"";

const char D_00A49590[16] = "Power Shield";

const char D_00A495A0[40] = "Prevents \"Dexterity Down\" \"Evade Down\"";

const char D_00A495C8[16] = "Scope Shield";

const char D_00A495D8[32] = "Prevents \"Armor Failure\"";

const char D_00A495F8[24] = "Armor Protect Unit";

const char D_00A49610[24] = "Enables use of Boost";

const char D_00A49628[16] = "B-MAX Circuit";

const char D_00A49638[24] = "Increases speed 50%";

const char D_00A49650[16] = "Fast Circuit 50";

const char D_00A49660[16] = "Fast Circuit 25";

const char D_00A49670[16] = "Boost Pack";

const char D_00A49680[56] = "Auto-Boost when attacked\n\"Counter+10\" skill necessary";

const char D_00A496B8[16] = "Revenge Power";

const char D_00A496C8[16] = "Field Ring";

const char D_00A496D8[16] = "Blade Soul";

const char D_00A496E8[8] = "Scope";

const char D_00A496F0[16] = "Ether Def +4";

const char D_00A49700[16] = "Orange Ring S";

const char D_00A49710[16] = "Ether Atk +4";

const char D_00A49720[16] = "Purple Ring S";

const char D_00A49730[16] = "Evade +4";

const char D_00A49740[16] = "Cobalt Ring S";

const char D_00A49750[16] = "Dexterity +4";

const char D_00A49760[16] = "Blue Ring S";

const char D_00A49770[16] = "Phys Def +4";

const char D_00A49780[16] = "Green Ring S";

const char D_00A49790[16] = "Phys Atk +4";

const char D_00A497A0[16] = "Red Ring S";

const char D_00A497B0[32] = " \"Anti-Veil\" support effect";

const char D_00A497D0[16] = "Red Topaz";

const char D_00A497E0[16] = "Blue Topaz";

const char D_00A497F0[32] = "Increases Str as HP decreases";

const char D_00A49810[16] = "Bravesoul";

const char D_00A49820[32] = "Increases experience points 25%";

const char D_00A49840[24] = "Master's Pendant";

const char D_00A49858[16] = "Life Stone";

const char D_00A49868[16] = "Gemini Clock";

const char D_00A49878[16] = "Angel Ring";

const char D_00A49888[48] = "Increases HI Slot Tech Attack Critical Hit rate";

const char D_00A498B8[16] = "Battle Mask";

const char D_00A498C8[16] = "Double Buster";

const char D_00A498D8[24] = "Increases speed 25%";

const char D_00A498F0[16] = "Speed Shoes";

const char D_00A49900[16] = "Golden Dice";

const char D_00A49910[40] = "Each turn AP +1\n(characters only) ";

const char D_00A49938[24] = "Commander's Crest";

const char D_00A49950[48] = "Increases rare item acquisition rate 30%";

const char D_00A49980[24] = "Cat Burglar Gloves";

const char D_00A49998[48] = "Increases rare item acquisition rate 10%";

const char D_00A499C8[16] = "Thief Ring";

const char D_00A499D8[24] = "Increases Max EP 15%";

const char D_00A499F0[16] = "Silver Crown";

const char D_00A49A00[8] = "Y";

const char D_00A49A08[24] = "Increases Max HP 15%";

const char D_00A49A20[16] = "Yamato Belt";

const char D_00A49A30[16] = "Damage down 10%";

const char D_00A49A40[8] = "Cross";

const char D_00A49A48[64] = "Increases counter rate 10%\n1/2 BG accumulation during counter";

const char D_00A49A88[16] = "Samurai Heart";

const char D_00A49A98[32] = "Increases Critical Hit rate 5%";

const char D_00A49AB8[16] = "Hunter Goggle";

const char D_00A49AC8[16] = "Agility +1";

const char D_00A49AD8[16] = "White Ring";

const char D_00A49AE8[8] = "O";

const char D_00A49AF0[16] = "Ether Def +2";

const char D_00A49B00[16] = "Orange Ring";

const char D_00A49B10[16] = "Ether Atk +2";

const char D_00A49B20[16] = "Purple Ring";

const char D_00A49B30[16] = "Evade +2";

const char D_00A49B40[16] = "Kobold Ring";

const char D_00A49B50[16] = "Dexterity +2";

const char D_00A49B60[16] = "Blue Ring";

const char D_00A49B70[16] = "Phys Def +2";

const char D_00A49B80[16] = "Green Ring";

const char D_00A49B90[16] = "Phys Atk +2";

const char D_00A49BA0[16] = "Red Ring";

const char D_00A49BB0[16] = "Chakra Shield";

const char D_00A49BC0[32] = "Prevents \"Attack Disable\" ";

const char D_00A49BE0[16] = "Soldier's Honor";

const char D_00A49BF0[24] = "Prevents \"Instant KO\" ";

const char D_00A49C08[24] = "Defibrillator Vest";

const char D_00A49C20[24] = "Prevents \"Poison\" ";

const char D_00A49C38[16] = "Snake Hunter";

const char D_00A49C48[24] = "Prevents \"Slow\" ";

const char D_00A49C60[16] = "Clock Shield";

const char D_00A49C70[24] = "Prevents \"AP Half\" ";

const char D_00A49C88[16] = "Jade Mask";

const char D_00A49C98[32] = "Prevents \"Ether Atk Down\" ";

const char D_00A49CB8[16] = "Spirit Pendant";

const char D_00A49CC8[32] = "Prevents \"Phys Def Down\" ";

const char D_00A49CE8[16] = "Guard Pendant";

const char D_00A49CF8[32] = "Prevents \"Phys Atk Down\" ";

const char D_00A49D18[16] = "Power Brace";

const char D_00A49D28[24] = "Prevents \"Evade Down\" ";

const char D_00A49D40[16] = "Hyper Shoes";

const char D_00A49D50[32] = "Prevents \"Dexterity Down\" ";

const char D_00A49D70[16] = "Sniper Goggles";

const char D_00A49D80[32] = "Prevents \"EP Overconsumption\" ";

const char D_00A49DA0[16] = "Rune Crystal";

const char D_00A49DB0[24] = "Prevents \"Curse\" ";

const char D_00A49DC8[16] = "Wooden Idol";

const char D_00A49DD8[16] = "Venom Block";

const char D_00A49DE8[32] = "Prevents \"Critical Hits\"";

const char D_00A49E08[16] = "Trauma Plate";

const char D_00A49E18[40] = "Auxiliary armor for arms\nPhys Def +2";

const char D_00A49E40[16] = "Protector";

const char D_00A49E50[40] = "Increases E.Pts earned 25%\nPhys Def +4";

const char D_00A49E78[16] = "Star Hat";

const char D_00A49E88[32] = "Prevents \"Lost\" \nPhys Def +4";

const char D_00A49EA8[16] = "Magical Hat";

const char D_00A49EB8[32] = "Prevents \"Sleep\"\nPhys Def +4";

const char D_00A49ED8[16] = "Nightwalker";

const char D_00A49EE8[16] = "Hat Reserve";

const char D_00A49EF8[48] = "Matches Techtron Clothes outfit\nPhys Def +8";

const char D_00A49F28[16] = "Techtron Helmet";

const char D_00A49F38[40] = "Matches Ruby Suit outfit\nPhys Def +6";

const char D_00A49F60[16] = "Ruby Helmet";

const char D_00A49F70[32] = "Space-use helmet\nPhys Def +4";

const char D_00A49F90[16] = "Space Helmet";

const char D_00A49FA0[48] = "Matches Survival Wear outfit\nPhys Def +2";

const char D_00A49FD0[16] = "Cowboy Hat";

const char D_00A49FE0[40] = "Matches Metal Wear outfit\nPhys Def +2";

const char D_00A4A008[16] = "Metal Helmet";

const char D_00A4A018[40] = "Increases T.Pts earned 25%\nPhys Def +1";

const char D_00A4A040[16] = "Swimsuit";

const char D_00A4A050[32] = "Special Clothing Reserve";

const char D_00A4A070[56] = "Increases Ether Def as allies are KO'd\nPhys Def +20";

const char D_00A4A0A8[8] = "Spirit";

const char D_00A4A0B0[56] = "Increases Phys Def as allies are KO'd\nPhys Def +20";

const char D_00A4A0E8[8] = "Soul";

const char D_00A4A0F0[48] = "Reduces Slsh/Prce-type damage 20%\nPhys Def +2";

const char D_00A4A120[16] = "Coat\241\246Sword";

const char D_00A4A130[40] = "Reduces B-type damage 20%\nPhys Def +2";

const char D_00A4A158[16] = "Coat\241\246Beam";

const char D_00A4A168[40] = "Reduces I-type damage 20%\nPhys Def +2";

const char D_00A4A190[16] = "Coat\241\246Ice";

const char D_00A4A1A0[40] = "Reduces L-type damage 20%\nPhys Def +2";

const char D_00A4A1C8[16] = "Coat\241\246Lightning";

const char D_00A4A1D8[40] = "Reduces F-type damage 20%\nPhys Def +2";

const char D_00A4A200[16] = "Coat\241\246Fire";

const char D_00A4A210[40] = "Increases S.Pts earned 25%\nPhys Def +2";

const char D_00A4A238[16] = "Craft Apron";

const char D_00A4A248[56] = "Prevents all status effects/abnormalities\nPhys Def +36";

const char D_00A4A280[16] = "Shield Armor";

const char D_00A4A290[32] = "Cyborg Armor Unit\nPhys Def +44";

const char D_00A4A2B0[8] = "VOLG50";

const char D_00A4A2B8[32] = "Cyborg Armor Unit\nPhys Def +36";

const char D_00A4A2D8[8] = "VOLG40";

const char D_00A4A2E0[32] = "Cyborg Armor Unit\nPhys Def +28";

const char D_00A4A300[8] = "VOLG30";

const char D_00A4A308[32] = "Cyborg Armor Unit\nPhys Def +16";

const char D_00A4A328[8] = "VOLG20";

const char D_00A4A330[32] = "Cyborg Armor Unit\nPhys Def +3";

const char D_00A4A350[8] = "VOLG10";

const char D_00A4A358[56] = "Clothing made for adventure enthusiasts\nPhys Def +12";

const char D_00A4A390[16] = "Survival Wear";

const char D_00A4A3A8[32] = "Regular Clothing Reserve";

const char D_00A4A3C8[32] = "KOS-MOS Armor Unit\nPhys Def +48";

const char D_00A4A3E8[16] = "D Unit V6";

const char D_00A4A3F8[32] = "KOS-MOS Armor Unit\nPhys Def +38";

const char D_00A4A418[16] = "D Unit V5";

const char D_00A4A428[32] = "KOS-MOS Armor Unit\nPhys Def +30";

const char D_00A4A448[16] = "D Unit V4";

const char D_00A4A458[32] = "KOS-MOS Armor Unit\nPhys Def +20";

const char D_00A4A478[16] = "D Unit V3";

const char D_00A4A488[32] = "KOS-MOS Armor Unit\nPhys Def +14";

const char D_00A4A4A8[16] = "D Unit V2";

const char D_00A4A4B8[32] = "KOS-MOS Armor Unit\nPhys Def +4";

const char D_00A4A4D8[16] = "D Unit V1";

const char D_00A4A4E8[56] = "The latest top-of-the-line combat wear\nPhys Def +38";

const char D_00A4A520[24] = "Techtron Clothes";

const char D_00A4A538[72] = "Reinforced clothing that generates a defensive force-field\nPhys Def +30";

const char D_00A4A580[16] = "Ruby Suit";

const char D_00A4A590[32] = "Has \"EDEF+4\" skill\nPhys Def +30";

const char D_00A4A5B0[16] = "Neo Armor \246\302";

const char D_00A4A5C0[32] = "Has \"PDEF+4\" skill\nPhys Def +30";

const char D_00A4A5E0[16] = "Neo Armor \246\301";

const char D_00A4A5F0[64] = "Reinforced clothing made for battles in the arctic\nPhys Def +24";

const char D_00A4A630[16] = "Battle Gear";

const char D_00A4A640[80] = "Reinforced clothing that auto-adjusts to people's body shape\nPhys Def +18";

const char D_00A4A690[16] = "Stylish Armor";

const char D_00A4A6A0[56] = "Clothing woven out of high-strength fibers\nPhys Def +12";

const char D_00A4A6D8[16] = "Fiber Suit";

const char D_00A4A6E8[56] = "Clothing woven with special alloy threads\nPhys Def +8";

const char D_00A4A720[16] = "Metal Wear";

const char D_00A4A730[48] = "Uniform for female Vector employees\nPhys Def +3";

const char D_00A4A760[8] = "Uniform";

const char D_00A4A768[48] = "All/Air/Pierce\nFederation Army hand missile pod";

const char D_00A4A798[64] = "All Allies/Ether Shield\nDefensive shield manufactured by Vector";

const char D_00A4A7D8[8] = "DEF-VX";

const char D_00A4A7E0[56] = "All/Air/Beam EA300%\nLance manufactured by Vector";

const char D_00A4A818[8] = "LW-VX2";

const char D_00A4A820[64] = "Single/Near/Slash/W-ACT\nElectro shooter manufactured by Vector";

const char D_00A4A860[8] = "ER-VX";

const char D_00A4A868[40] = "All/Air/Hit\nFederation Army missile pod";

const char D_00A4A890[8] = "SMP53AG";

const char D_00A4A898[48] = "All/Air/S\nECM pod manufactured by Vector";

const char D_00A4A8C8[8] = "ECM2-VX";

const char D_00A4A8D0[48] = "Single/Line/S\nECM pod manufactured by Vector";

const char D_00A4A900[8] = "ECM1-VX";

const char D_00A4A908[48] = "All/Air/Beam EA250%\nFederation Army aerials";

const char D_00A4A938[16] = "AIRD-AG2";

const char D_00A4A948[64] = "Single/Line/Beam EA600%\nFederation Army large beam cannon";

const char D_00A4A988[8] = "BBC-AG5";

const char D_00A4A990[40] = "Defense\nShield manufactured by Vector";

const char D_00A4A9B8[8] = "SHD12VX";

const char D_00A4A9C0[32] = "Defense\nFederation Army shield";

const char D_00A4A9E0[8] = "SHD02AG";

const char D_00A4A9E8[72] = "Single/Air/Beam EA130%/Defense\nFederation Army shield beam rifle";

const char D_00A4AA30[8] = "SHB67AG";

const char D_00A4AA38[48] = "All/Air/Hit\nFederation Army large missile pod";

const char D_00A4AA68[8] = "BMP-AG5";

const char D_00A4AA70[48] = "Single/Line/Pierce\nFederation Army long cannon";

const char D_00A4AAA0[8] = "LC-AG5";

const char D_00A4AAA8[64] = "All/Air/Beam EA130%\nWide beam pod manufactured by Vector";

const char D_00A4AAE8[8] = "BMP45VX";

const char D_00A4AAF0[16] = "AIRC-AG2";

const char D_00A4AB00[64] = "Enemies & All Allies/Air/S\nChaff box manufactured by Vector";

const char D_00A4AB40[8] = "CB85VX";

const char D_00A4AB48[56] = "Single/Line/Beam EA200%\nFederation Army beam launcher";

const char D_00A4AB80[8] = "BL24AG";

const char D_00A4AB88[8] = "HMP-AG5";

const char D_00A4AB90[48] = "All/Air/Hit\nFederation Army hand missile pod";

const char D_00A4ABC0[8] = "HMP33AG";

const char D_00A4ABC8[8] = "HGG-AG5";

const char D_00A4ABD0[56] = "Single/Line/Pierce/W-ACT\nFederation Army gatling gun";

const char D_00A4AC08[8] = "GLG76AG";

const char D_00A4AC10[56] = "Single/Line/Fire/W-ACT\nFederation Army grenade launcher";

const char D_00A4AC48[8] = "GRD20AG";

const char D_00A4AC50[64] = "Single/Line/Fire EA150%/W-ACT\nFederation Army flame launcher";

const char D_00A4AC90[8] = "FLM64AG";

const char D_00A4AC98[56] = "Single/Near/Pierce\nFederation Army bunker buster";

const char D_00A4ACD0[8] = "PB55AG";

const char D_00A4ACD8[64] = "Single/Near/Slash/W-ACT\nLancer arm manufactured by Vector";

const char D_00A4AD18[8] = "LM11VX";

const char D_00A4AD20[56] = "Single/Near/Slash/Beam/W-ACT\nFederation Army beam sword";

const char D_00A4AD58[8] = "BSW13AG";

const char D_00A4AD60[64] = "Single/Line/Beam EA185%/W-ACT\nBeam arm manufactured by Vector";

const char D_00A4ADA0[8] = "BA15VX";

const char D_00A4ADA8[64] = "Single/Line/Beam EA300%\nLong beam rifle manufactured by Vector";

const char D_00A4ADE8[8] = "LG100VX";

const char D_00A4ADF0[64] = "Single/Line/Beam EA120%/W-ACT\nBeam rifle manufactured by Vector";

const char D_00A4AE30[8] = "LG24VX";

const char D_00A4AE38[48] = "Single/Line/Pierce/W-ACT\nFederation Army rifle";

const char D_00A4AE68[8] = "LG10AG";

const char D_00A4AE70[64] = "Single/Line/Pierce/W-ACT\nSubmachine gun manufactured by Vector";

const char D_00A4AEB0[8] = "SMG32VX";

const char D_00A4AEB8[56] = "Single/Line/Pierce/W-ACT\nFederation Army submachine gun";

const char D_00A4AEF0[8] = "SMG99AG";

const char D_00A4AEF8[48] = "Single/Near/Hit\nFederation Army heavy hammer";

const char D_00A4AF28[8] = "HMR-AG5";

const char D_00A4AF30[56] = "Single/Near/Slash/W-ACT\nFederation Army gyro saucer";

const char D_00A4AF68[16] = "WCT02AG4";

const char D_00A4AF78[48] = "Single/Near/Hit/W-ACT\nFederation Army hammer";

const char D_00A4AFA8[8] = "HMR55AG";

const char D_00A4AFB0[8] = "HG75VX";

const char D_00A4AFB8[56] = "Single/Line/Pierce/W-ACT\nHandgun manufactured by Vector";

const char D_00A4AFF0[8] = "HG45VX";

const char D_00A4AFF8[56] = "Single/Near/Pierce/W-ACT\nFederation Army drill claw";

const char D_00A4B030[16] = "DLCO2AG4";

const char D_00A4B040[40] = "Single/Near/Slash/W-ACT\nVector Sword";

const char D_00A4B068[8] = "SWD34VX";

const char D_00A4B070[48] = "Single/Near/Slash/W-ACT\nFederation Army sword";

const char D_00A4B0A0[8] = "SWD21AG";

const char D_00A4B0A8[48] = "Single/Near/Slash/W-ACT\nFederation Army axe";

const char D_00A4B0D8[8] = "AXE11AG";

const char D_00A4B0E0[16] = "Single/Near/Hit";

const char D_00A4B0F0[8] = "HAND";

const char D_00A4B0F8[48] = "Shion: Special Weapon manufactured by Vector";

const char D_00A4B128[8] = "M.W.S.";

const char D_00A4B130[56] = "Jr.: Legendary gun hailed as the ultimate firearm";

const char D_00A4B168[8] = "BLOOD9";

const char D_00A4B170[48] = "Jr.: An original gun made by a private gunsmith";

const char D_00A4B1A0[8] = "CROSS";

const char D_00A4B1A8[48] = "Jr.: Second generation gun by Saifar Co.";

const char D_00A4B1D8[16] = "SAIFAR45";

const char D_00A4B1E8[40] = "Jr.: An heirloom-quality antique gun";

const char D_00A4B210[8] = "MAKAROV";

const char D_00A4B218[32] = "Jr.: Standard Rook Co. gun";

const char D_00A4B238[8] = "ROOK505";

const char D_00A4B240[40] = "All/Line/Hit\nZiggy's missile pod";

const char D_00A4B268[8] = "MSP89SX";

const char D_00A4B270[40] = "All/Line/Beam EA135%\nZiggy's beam pod";

const char D_00A4B298[8] = "BMP55SX";

const char D_00A4B2A0[48] = "MOMO: \"Slow\" against B, G types\nPhys Atk +7";

const char D_00A4B2D0[16] = "Penguin Rod";

const char D_00A4B2E0[64] = "MOMO: Scepter with concentrated energy within\nPhys Atk +20";

const char D_00A4B320[16] = "Dragon Rod";

const char D_00A4B330[56] = "MOMO: A hero's scepter made to combat evil\nPhys Atk +12";

const char D_00A4B368[16] = "Saint Rod";

const char D_00A4B378[56] = "MOMO: \"Confusion\" against B, G types\nPhys Atk +10";

const char D_00A4B3B0[16] = "W Hammer Rod";

const char D_00A4B3C0[56] = "MOMO: Scepter made from rare materials\nPhys Atk +9";

const char D_00A4B3F8[16] = "Mithril Rod";

const char D_00A4B408[48] = "MOMO: Light, powerful scepter\nPhys Atk +7";

const char D_00A4B438[16] = "Platinum Rod";

const char D_00A4B448[48] = "MOMO: Hard and heavy battle scepter\nPhys Atk +5";

const char D_00A4B478[16] = "Metal Rod";

const char D_00A4B488[48] = "MOMO: Typical self-defense scepter\nPhys Atk +3";

const char D_00A4B4B8[16] = "Battle Rod";

const char D_00A4B4C8[56] = "All/Line/None EA300%\nScythe that emits an Ether field";

const char D_00A4B500[16] = "F\241\246SCYTHE";

const char D_00A4B510[40] = "Single/Line/Pierce\nShort Rail Cannon";

const char D_00A4B538[16] = "F\241\246RSHOT";

const char D_00A4B548[40] = "Single/Line/Beam EA120%\nDragon Skull";

const char D_00A4B570[16] = "F\241\246BSHOT";

const char D_00A4B580[32] = "All/Air/Hit\nArm missile pod";

const char D_00A4B5A0[16] = "F\241\246MSHOT";

const char D_00A4B5B0[40] = "Single/Line/Pierce\nTriple gatling gun";

const char D_00A4B5D8[16] = "F\241\246GSHOT";

const char D_00A4B5E8[56] = "chaos: Worn-out gloves (with holes)\nPhys Atk +16";

const char D_00A4B620[16] = "Holey Gloves";

const char D_00A4B630[48] = "chaos: Gloves for do-it-yourselfers\nPhys Atk +9";

const char D_00A4B660[16] = "Work Gloves";

const char D_00A4B670[64] = "chaos: Typical riding gloves (for winter use)\nPhys Atk +7";

const char D_00A4B6B0[16] = "Rider Gloves";

const char D_00A4B6C0[72] = "chaos: Military-issue gloves made of special materials\nPhys Atk +5";

const char D_00A4B708[16] = "Navy Gloves";

const char D_00A4B718[56] = "chaos: Gloves used when working in space\nPhys Atk +3";

const char D_00A4B750[16] = "Space Gloves";

const char D_00A4B760[16] = "Reserve10";

const char D_00A4B770[16] = "Reserve9";

const char D_00A4B780[16] = "Reserve8";

const char D_00A4B790[16] = "Reserve7";

const char D_00A4B7A0[16] = "Reserve6";

const char D_00A4B7B0[16] = "Reserve5";

const char D_00A4B7C0[16] = "Reserve4";

const char D_00A4B7D0[16] = "Reserve3";

const char D_00A4B7E0[16] = "Reserve2";

const char D_00A4B7F0[16] = "Reserve1";

const char D_00A4B800[24] = "LC-AG5/Long cannon ammo";

const char D_00A4B818[8] = "LCB1";

const char D_00A4B820[32] = "HGG-AG5/Gatling gun bullets";

const char D_00A4B840[8] = "HGGB1";

const char D_00A4B848[32] = "HMP-AG5/Hand missile pod ammo";

const char D_00A4B868[8] = "HMPB1";

const char D_00A4B870[8] = "BMPB1";

const char D_00A4B878[48] = "ECM2-VX/ECM pod bullets\n\"Wear\" against M types";

const char D_00A4B8A8[8] = "ECM2B5";

const char D_00A4B8B0[48] = "ECM2-VX/ECM pod bullets\n\"Slow\" against M types";

const char D_00A4B8E0[8] = "ECM2B4";

const char D_00A4B8E8[64] = "ECM2-VX/ECM pod bullets\n\"Dexterity Down\" against M types";

const char D_00A4B928[8] = "ECM2B3";

const char D_00A4B930[56] = "ECM2-VX/ECM pod bullets\n\"Phys Def Down\" against M types";

const char D_00A4B968[8] = "ECM2B2";

const char D_00A4B970[56] = "ECM2-VX/ECM pod bullets\n\"Phys Atk Down\" against M types";

const char D_00A4B9A8[8] = "ECM2B1";

const char D_00A4B9B0[32] = "SMG32VX/Submachine gun bullets";

const char D_00A4B9D0[8] = "SMG32B1";

const char D_00A4B9D8[32] = "SMP53AG/Missile pod ammo";

const char D_00A4B9F8[8] = "SMP53B1";

const char D_00A4BA00[8] = "AXE11B1";

const char D_00A4BA08[32] = "GLG76AG/Gatling gun bullets";

const char D_00A4BA28[8] = "GLG76B1";

const char D_00A4BA30[24] = "HG75VX/Handgun bullets";

const char D_00A4BA48[8] = "HG75B1";

const char D_00A4BA50[48] = "ECM1-VX/ECM pod bullets\n\"Wear\" against M types";

const char D_00A4BA80[8] = "ECM1B5";

const char D_00A4BA88[48] = "ECM1-VX/ECM pod bullets\n\"Slow\" against M types";

const char D_00A4BAB8[8] = "ECM1B4";

const char D_00A4BAC0[64] = "ECM1-VX/ECM pod bullets\n\"Dexterity Down\" against M types";

const char D_00A4BB00[8] = "ECM1B3";

const char D_00A4BB08[56] = "ECM1-VX/ECM pod bullets\n\"Armor Failure\" against M types";

const char D_00A4BB40[8] = "ECM1B2";

const char D_00A4BB48[56] = "ECM1-VX/ECM pod bullets\n\"Power Loss\" against M types";

const char D_00A4BB80[8] = "ECM1B1";

const char D_00A4BB88[8] = "CB85B5";

const char D_00A4BB90[64] = "CB85VX/Chaff box bullets\n\"Phys Def Down\" against B, G types";

const char D_00A4BBD0[8] = "CB85B4";

const char D_00A4BBD8[64] = "CB85VX/Chaff box bullets\n\"Phys Atk Down\" against B, G types";

const char D_00A4BC18[8] = "CB85B3";

const char D_00A4BC20[64] = "CB85VX/Chaff box bullets\n\"Ether Atk Down\" against B, G types";

const char D_00A4BC60[8] = "CB85B2";

const char D_00A4BC68[64] = "CB85VX/Chaff box bullets\n\"Dexterity Down\" against M types";

const char D_00A4BCA8[8] = "CB85B1";

const char D_00A4BCB0[16] = "No bullets";

const char D_00A4BCC0[8] = "LG100B1";

const char D_00A4BCC8[32] = "HMP33/Hand missile pod ammo";

const char D_00A4BCE8[8] = "HMP33B1";

const char D_00A4BCF0[24] = "GRD20AG/Grenade ammo";

const char D_00A4BD08[8] = "GRD20B1";

const char D_00A4BD10[32] = "SMG99AG/Submachine gun bullets";

const char D_00A4BD30[8] = "SMG99B1";

const char D_00A4BD38[24] = "LG10AG/Rifle bullets";

const char D_00A4BD50[8] = "LG10B1";

const char D_00A4BD58[24] = "HG45VX/Handgun bullets";

const char D_00A4BD70[8] = "HG45B1";

const char D_00A4BD78[64] = "Jr.: Bullets for \"CROSS\"\n\"Ether Atk Down\" against B, G types";

const char D_00A4BDB8[16] = "CS700UVL";

const char D_00A4BDC8[64] = "Jr.: Bullets for \"CROSS\"\n\"Phys Def Down\" against B, G types";

const char D_00A4BE08[16] = "CS700DFD";

const char D_00A4BE18[64] = "Jr.: Bullets for \"CROSS\"\n\"Phys Atk Down\" against B, G types";

const char D_00A4BE58[16] = "CS700PWD";

const char D_00A4BE68[56] = "Jr.: Bullets for \"CROSS\"\n\"Slow\" against B, G types";

const char D_00A4BEA0[16] = "CS700SLW";

const char D_00A4BEB0[40] = "Jr.: Bullets for \"BLOOD9\"\nPhys Atk +16";

const char D_00A4BED8[8] = "BD900";

const char D_00A4BEE0[40] = "Jr.: Bullets for \"CROSS\"\nPhys Atk +10";

const char D_00A4BF08[8] = "CS700";

const char D_00A4BF10[40] = "Jr.: Bullets for \"SAIFAR\"\nPhys Atk +8";

const char D_00A4BF38[8] = "SR500";

const char D_00A4BF40[40] = "Jr.: Bullets for \"ROOK505\"\nPhys Atk +6";

const char D_00A4BF68[8] = "RK280";

const char D_00A4BF70[56] = "Jr.: Valuable bullets for MAKAROV gun\nPhys Atk +5";

const char D_00A4BFA8[16] = "MAKAROV Bullets";

const char D_00A4BFB8[40] = "Jr.: Bullets for \"ROOK505\"\nPhys Atk +4";

const char D_00A4BFE0[8] = "RK200";

const char D_00A4BFE8[8] = "MSS580";

const char D_00A4BFF0[8] = "MSS560";

const char D_00A4BFF8[48] = "Ziggy: Missiles for \"MSP89SX\"\nPhys Atk +12";

const char D_00A4C028[8] = "MSS540";

const char D_00A4C030[8] = "MSS520";

const char D_00A4C038[48] = "Ziggy: Missiles for \"MSP89SX\"\nPhys Atk +4";

const char D_00A4C068[8] = "MSS500";

const char D_00A4C070[8] = "KRS54V";

const char D_00A4C078[8] = "KMS38V";

const char D_00A4C080[8] = "KGS16V";

const char D_00A4C088[8] = "KMS36V";

const char D_00A4C090[8] = "KRS52V";

const char D_00A4C098[8] = "KMS34V";

const char D_00A4C0A0[8] = "KGS14V";

const char D_00A4C0A8[48] = "KOS-MOS: Bullets for \"F\241\246RSHOT\"\nPhys Atk +20";

const char D_00A4C0D8[8] = "KRS50V";

const char D_00A4C0E0[8] = "KMS32V";

const char D_00A4C0E8[8] = "KGS12V";

const char D_00A4C0F0[48] = "KOS-MOS: Bullets for \"F\241\246MSHOT\"\nPhys Atk +4";

const char D_00A4C120[8] = "KMS30V";

const char D_00A4C128[48] = "KOS-MOS: Bullets for \"F\241\246GSHOT\"\nPhys Atk +12";

const char D_00A4C158[8] = "KGS10V";

const char D_00A4C160[8] = "BXS015V";

const char D_00A4C168[8] = "BXS014V";

const char D_00A4C170[8] = "BXS013V";

const char D_00A4C178[8] = "BXS012V";

const char D_00A4C180[8] = "BXS011V";

const char D_00A4C188[8] = "BXS010V";

const char D_00A4C190[8] = "BXS009V";

const char D_00A4C198[8] = "BXS008V";

const char D_00A4C1A0[8] = "BXS007V";

const char D_00A4C1A8[8] = "BXS006V";

const char D_00A4C1B0[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Phys Def Down\" against B, G types";

const char D_00A4C1F8[8] = "BXS005V";

const char D_00A4C200[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Phys Atk Down\" against B, G types";

const char D_00A4C248[8] = "BXS004V";

const char D_00A4C250[72] = "Shion: Tech Attack \"Shock Blade\"\n\"Armor Failure\" against M types";

const char D_00A4C298[8] = "BXS003V";

const char D_00A4C2A0[64] = "Shion: Tech Attack \"Shock Blade\"\n\"Power Loss\" against M types";

const char D_00A4C2E0[8] = "BXS002V";

const char D_00A4C2E8[56] = "Shion: Tech Attack \"Shock Blade\"\n\"Slow\" against M types";

const char D_00A4C320[8] = "BXS001V";

const char D_00A4C328[8] = "Rush";

const char D_00A4C330[16] = "Rifle Shot";

const char D_00A4C340[16] = "Hip Shot";

const char D_00A4C350[16] = "Crossfire";

const char D_00A4C360[16] = "Coin Snap";

const char D_00A4C370[16] = "Trick Shot";

const char D_00A4C380[16] = "Southpaw";

const char D_00A4C390[16] = "Cross Shot";

const char D_00A4C3A0[16] = "Cosmic Flip";

const char D_00A4C3B0[16] = "Flower Storm";

const char D_00A4C3C0[16] = "Dream Whirl";

const char D_00A4C3D0[16] = "Twin Stars";

const char D_00A4C3E0[16] = "Melody Ray";

const char D_00A4C3F0[16] = "Stardust";

const char D_00A4C400[16] = "Jack Blade";

const char D_00A4C410[16] = "Laser Swing";

const char D_00A4C420[16] = "Ignition";

const char D_00A4C430[16] = "Jack Knife";

const char D_00A4C440[16] = "Laser Blade";

const char D_00A4C450[16] = "High Kick";

const char D_00A4C460[16] = "Spin Kick";

const char D_00A4C470[16] = "Cherry Bomb";

const char D_00A4C480[16] = "Stun Shock";

const char D_00A4C490[16] = "Power Kick";

const char D_00A4C4A0[16] = "Firecracker";

const char D_00A4C4B0[8] = "Knuckle";

const char D_00A4C4B8[8] = "NEEDLE";

const char D_00A4C4C0[8] = "SWORD";

const char D_00A4C4C8[8] = "S-SAULT";

const char D_00A4C4D0[8] = "BLASTER";

const char D_00A4C4D8[8] = "PUNCH";

const char D_00A4C4E0[16] = "Star Thrust";

const char D_00A4C4F0[16] = "Angel Shot";

const char D_00A4C500[16] = "Electro Upper";

const char D_00A4C510[16] = "Seraphim Rush";

const char D_00A4C520[8] = "Arrow";

const char D_00A4C528[16] = "Tornado Slash";

const char D_00A4C538[56] = "One enemy/Line/Spirit\nA gun that absorbs G-types";

const char D_00A4C570[16] = "Soul Rhapsody";

const char D_00A4C580[72] = "All enemies/Air/Pierce\nSnipe from above surrounded by angel wings";

const char D_00A4C5C8[16] = "Angelic Requiem";

const char D_00A4C5D8[64] = "All enemies/Air/Spirit\nAttack with a spirit gun from above";

const char D_00A4C618[16] = "Mystic Nocturne";

const char D_00A4C628[48] = "One enemy/Line/Pierce\nConcentrated heavy fire";

const char D_00A4C658[16] = "Last Symphony";

const char D_00A4C668[48] = "All enemies/Line/Pierce\nCoin shot attack";

const char D_00A4C698[16] = "Storm Waltz";

const char D_00A4C6A8[48] = "One enemy/Line/Spirit\nAttack with a spirit gun";

const char D_00A4C6D8[24] = "Moonlit Serenade";

const char D_00A4C6F0[40] = "One enemy/Line/Pierce\nCross shot";

const char D_00A4C718[24] = "Prelude to Battle";

const char D_00A4C730[72] = "One enemy/Line   Can be used only when transformed\nSteal rare items";

const char D_00A4C778[16] = "Magic Caster";

const char D_00A4C788[72] = "One enemy/Line/Beam   Can be used only when transformed\nMOMO Beam";

const char D_00A4C7D0[16] = "MOMO's Kiss";

const char D_00A4C7E0[72] = "One enemy/Near/Hit\nTurn G-types into items if used to defeat them";

const char D_00A4C828[16] = "Dark Scepter";

const char D_00A4C838[56] = "All enemies/Air/Pierce\nAttack with arrow of light";

const char D_00A4C870[16] = "Angel Arrow";

const char D_00A4C880[48] = "All enemies/Air/Hit\nStrike with a large star";

const char D_00A4C8B0[16] = "Star Cannon";

const char D_00A4C8C0[56] = "One enemy/Line/None\nAttack from another dimension";

const char D_00A4C8F8[16] = "Meteor Storm";

const char D_00A4C908[48] = "One enemy/Near/Slash\nSlash with knife-like wind";

const char D_00A4C938[16] = "Floral Tempest";

const char D_00A4C948[48] = "One enemy/Near/Hit\nStrike with a small star";

const char D_00A4C978[16] = "Star Strike";

const char D_00A4C988[64] = "One enemy/Near/Slash/Fire\nFire slash attack from the air";

const char D_00A4C9C8[16] = "Hell Fire";

const char D_00A4C9D8[56] = "All enemies/Air/Lightning\nLightning attack from the sky";

const char D_00A4CA10[16] = "Executioner";

const char D_00A4CA20[56] = "One enemy/Near/Pierce\nA crossed-blade slash attack";

const char D_00A4CA58[16] = "Cross Lancer";

const char D_00A4CA68[56] = "One enemy/Near/Slash\nA tornado-like slash attack";

const char D_00A4CAA0[8] = "Cyclone";

const char D_00A4CAA8[48] = "All enemies/Air/Fire\nFire attack from the air";

const char D_00A4CAD8[16] = "Meteor Shot";

const char D_00A4CAE8[64] = "One enemy/Line/Lightning\nLightning attack from the ground";

const char D_00A4CB28[16] = "Lightning Fist";

const char D_00A4CB38[48] = "One enemy/Near/Hit/Fire\nHundred kicks of fire";

const char D_00A4CB68[16] = "Cyber Kick";

const char D_00A4CB78[40] = "All enemies/Air/Beam\nRain of beams";

const char D_00A4CBA0[16] = "Rain Blade";

const char D_00A4CBB0[56] = "One enemy/Near/Slash/Beam\nSlice with a beam blade";

const char D_00A4CBE8[16] = "Lunar Blade";

const char D_00A4CBF8[56] = "One enemy/Near/Hit/S\n\"Slow\" against B and G-types";

const char D_00A4CC30[16] = "Gravity Well";

const char D_00A4CC40[80] = "One enemy/Near/S/1 time effect\nSets Ether damage 2x bombs against B and G-types";

const char D_00A4CC90[16] = "Ether Amp";

const char D_00A4CCA0[48] = "One enemy/Near/Hit/Fire\nVicious fire attack";

const char D_00A4CCD0[16] = "Thermal Blast";

const char D_00A4CCE0[72] = "One enemy/Near/Pierce/S\nDifferent ST effects by equipping Cartridges";

const char D_00A4CD28[16] = "Shock Blade";

const char D_00A4CD38[48] = "One enemy/Line/Beam\nConcentrated Ether beam";

const char D_00A4CD68[16] = "Spell Ray";

const char D_00A4CD78[48] = "One enemy/Near/Hit/Lightning\nLightning attack";

const char D_00A4CDA8[16] = "Lightning Blast";

const char D_00A4CDB8[48] = "One enemy/Near/Hit\nAttack with a dragon arm";

const char D_00A4CDE8[16] = "R\241\246DRAGON";

const char D_00A4CDF8[48] = "One enemy/Near/Hit\nAttack with a hammer arm";

const char D_00A4CE28[16] = "R\241\246HAMMER";

const char D_00A4CE38[64] = "One enemy/Line/S\nMultiple status effects on B, G, and M-types";

const char D_00A4CE78[16] = "S\241\246CHAIN";

const char D_00A4CE88[8] = "X";

const char D_00A4CE90[56] = "All enemies/Line/Beam\nAbdominal spread beam weapon";

const char D_00A4CEC8[16] = "X\241\246BUSTER";

const char D_00A4CED8[48] = "One enemy/Near/Pierce\nAttack with a drill arm";

const char D_00A4CF08[16] = "R\241\246DRILL";

const char D_00A4CF18[48] = "One enemy/Line/Beam\nAttack with a beam arm";

const char D_00A4CF48[16] = "R\241\246CANNON";

const char D_00A4CF58[48] = "One enemy/Near/Slash\nAttack with a sword arm";

const char D_00A4CF88[16] = "R\241\246BLADE";

const char D_00A4CF98[64] = "One enemy/Near/Pierce/Spirit\nOnly available when ally is KO'd";

const char D_00A4CFD8[16] = "Divine Spear";

const char D_00A4CFE8[56] = "All enemies/Air/Ice\nHurl down ice boulders from the air";

const char D_00A4D020[16] = "Arctic Blast";

const char D_00A4D030[48] = "One enemy/Line/S\n\"Curse\" against G-types";

const char D_00A4D060[16] = "Demon Banisher";

const char D_00A4D070[56] = "One enemy/Near/Spirit/S\n\"Pilot KO\" against M-types";

const char D_00A4D0A8[16] = "Chained Blast";

const char D_00A4D0B8[48] = "All enemies/Air/Spirit\nGiant chi impact attack";

const char D_00A4D0E8[16] = "Angel Blow";

const char D_00A4D0F8[80] = "One enemy/Near/Slash/Lightning\nSlash enemies by changing chi into lightning";

const char D_00A4D148[16] = "Heaven's Wrath";

const char D_00A4D158[40] = "One enemy/Line/Spirit\nA chi wave attack";

const char D_00A4D180[16] = "Lunar Seal";

const char D_00A4D190[80] = "One enemy/Near/Hit/Spirit\nImpact created by focusing chi into one's fist";

const char D_00A4D1E0[16] = "Angel Wings";

const char D_00A4D1F0[16] = "BOSS-120";

const char D_00A4D200[8] = "Albedo";

const char D_00A4D208[16] = "BOSS-032";

const char D_00A4D218[16] = "BOSS-031";

const char D_00A4D228[8] = "Mintia";

const char D_00A4D230[16] = "Great Joe";

const char D_00A4D240[16] = "BOSS-028";

const char D_00A4D250[16] = "BOSS-027";

const char D_00A4D260[16] = "Sophie Peithos";

const char D_00A4D270[16] = "Ratatosk";

const char D_00A4D280[16] = "Jaldabaoth";

const char D_00A4D290[16] = "BOSS-023";

const char D_00A4D2A0[16] = "BOSS-022";

const char D_00A4D2B0[8] = "Simeon";

const char D_00A4D2B8[16] = "Margulis";

const char D_00A4D2C8[16] = "Doppelwogel";

const char D_00A4D2D8[16] = "Proto Dora";

const char D_00A4D2E8[16] = "Rianon Se";

const char D_00A4D2F8[8] = "Gigas";

const char D_00A4D300[8] = "Dragon";

const char D_00A4D308[16] = "Ein Rugel";

const char D_00A4D318[8] = "Tiamat";

const char D_00A4D320[16] = "Gargoyle";

const char D_00A4D330[8] = "Stribog";

const char D_00A4D338[8] = "Perun";

const char D_00A4D340[16] = "Svarozic";

const char D_00A4D350[16] = "BOSS-008";

const char D_00A4D360[16] = "Drone GX";

const char D_00A4D370[16] = "BOSS-006";

const char D_00A4D380[16] = "DOMO Carrier";

const char D_00A4D390[8] = "Golem";

const char D_00A4D398[8] = "Gremlin";

const char D_00A4D3A0[16] = "Minotaur";

const char D_00A4D3B0[8] = "Cyclops";

const char D_00A4D3B8[24] = "Mr. 300000 Points";

const char D_00A4D3D0[24] = "Mr. 100000 Points";

const char D_00A4D3E8[24] = "Mr. 10000 Points";

const char D_00A4D400[16] = "Mr. 1000 Points";

const char D_00A4D410[16] = "Mr. 100 Points";

const char D_00A4D420[16] = "MONS-035";

const char D_00A4D430[16] = "MONS-034";

const char D_00A4D440[16] = "MONS-033";

const char D_00A4D450[16] = "MONS-032";

const char D_00A4D460[16] = "MONS-031";

const char D_00A4D470[16] = "MONS-030";

const char D_00A4D480[16] = "MONS-029";

const char D_00A4D490[16] = "MONS-028";

const char D_00A4D4A0[16] = "MONS-027";

const char D_00A4D4B0[16] = "MONS-026";

const char D_00A4D4C0[16] = "MONS-025";

const char D_00A4D4D0[16] = "MONS-024";

const char D_00A4D4E0[16] = "MONS-023";

const char D_00A4D4F0[16] = "MONS-022";

const char D_00A4D500[16] = "MONS-021";

const char D_00A4D510[16] = "MONS-020";

const char D_00A4D520[16] = "MONS-019";

const char D_00A4D530[16] = "MONS-018";

const char D_00A4D540[16] = "MONS-017";

const char D_00A4D550[16] = "MONS-016";

const char D_00A4D560[16] = "MONS-015";

const char D_00A4D570[16] = "MONS-014";

const char D_00A4D580[16] = "MONS-013";

const char D_00A4D590[16] = "MONS-012";

const char D_00A4D5A0[16] = "MONS-011";

const char D_00A4D5B0[16] = "MONS-010";

const char D_00A4D5C0[16] = "MONS-009";

const char D_00A4D5D0[16] = "MONS-008";

const char D_00A4D5E0[8] = "DOMO-\246\302";

const char D_00A4D5E8[16] = "Ace Pilot";

const char D_00A4D5F8[16] = "U-TIC Soldier X";

const char D_00A4D608[8] = "Einzats";

const char D_00A4D610[16] = "Dirlewanger";

const char D_00A4D620[16] = "U-TIC Soldier";

const char D_00A4D630[16] = "Xanthosis";

const char D_00A4D640[8] = "Iosys";

const char D_00A4D648[8] = "Ambix";

const char D_00A4D650[8] = "Zolfo";

const char D_00A4D658[16] = "Mercurio";

const char D_00A4D668[16] = "UTRE-006";

const char D_00A4D678[16] = "Athra 26 Series";

const char D_00A4D688[16] = "Byproduct 172";

const char D_00A4D698[16] = "Byproduct 145";

const char D_00A4D6A8[16] = "Byproduct 103";

const char D_00A4D6B8[16] = "UTRE-001";

const char D_00A4D6C8[16] = "UTMA-014";

const char D_00A4D6D8[16] = "Din Gareth";

const char D_00A4D6E8[8] = "Schutz";

const char D_00A4D6F0[8] = "Kubel";

const char D_00A4D6F8[16] = "UTMA-010";

const char D_00A4D708[16] = "Gertzog UT";

const char D_00A4D718[16] = "Capto Mortum";

const char D_00A4D728[16] = "Work Droid";

const char D_00A4D738[16] = "Gardis F10";

const char D_00A4D748[16] = "Gardis M1";

const char D_00A4D758[16] = "Shot Crab";

const char D_00A4D768[16] = "Cyber Crab";

const char D_00A4D778[8] = "DOMO-B";

const char D_00A4D780[8] = "DOMO-A";

const char D_00A4D788[16] = "Fed. Soldier";

const char D_00A4D798[8] = "Drone F";

const char D_00A4D7A0[8] = "Drone M";

const char D_00A4D7A8[16] = "Drone SPX";

const char D_00A4D7B8[16] = "FERE-003";

const char D_00A4D7C8[8] = "Vive";

const char D_00A4D7D0[8] = "Calx";

const char D_00A4D7D8[16] = "FEMA-006";

const char D_00A4D7E8[16] = "Meld Gareth";

const char D_00A4D7F8[16] = "FEMA-004";

const char D_00A4D808[16] = "FEMA-003";

const char D_00A4D818[16] = "Gertzog FE";

const char D_00A4D828[16] = "Attack Drone";

const char D_00A4D838[16] = "Oudogogue";

const char D_00A4D848[16] = "Drone G3";

const char D_00A4D858[16] = "Drone G2";

const char D_00A4D868[16] = "Drone G1";

const char D_00A4D878[16] = "Baraqijal";

const char D_00A4D888[8] = "Armaros";

const char D_00A4D890[8] = "Azazel";

const char D_00A4D898[16] = "Delphyne";

const char D_00A4D8A8[16] = "Basilisk";

const char D_00A4D8B8[8] = "Bugbear";

const char D_00A4D8C0[8] = "Wyrm";

const char D_00A4D8C8[8] = "Unicorn";

const char D_00A4D8D0[16] = "Lizardman";

const char D_00A4D8E0[8] = "Hydra";

const char D_00A4D8E8[8] = "Demon";

const char D_00A4D8F0[16] = "Gel Fish";

const char D_00A4D900[16] = "Cerberus";

const char D_00A4D910[16] = "Larva Face";

const char D_00A4D920[16] = "Larva Doll";

const char D_00A4D930[8] = "Fairy";

const char D_00A4D938[8] = "Troll";

const char D_00A4D940[8] = "Ogre";

const char D_00A4D948[8] = "Kobold";

const char D_00A4D950[16] = "Sky Fish";

const char D_00A4D960[16] = "Manticore";

const char D_00A4D970[8] = "Goblin";

const char D_00A4D978[16] = "Debugster";

const char D_00A4D988[16] = "Attack Disabled";

const char D_00A4D998[8] = "HP Half";

const char D_00A4D9A0[8] = "Reverse";

const char D_00A4D9A8[16] = "Ether Bomb";

const char D_00A4D9B8[8] = "AP Half";

const char D_00A4D9C0[16] = "Phys Def Down";

const char D_00A4D9D0[16] = "Phys Atk Down";

const char D_00A4D9E0[16] = "Evade Down";

const char D_00A4D9F0[16] = "Dexterity Down";

const char D_00A4DA00[8] = "Poison";

const char D_00A4DA08[8] = "Slow";

const char D_00A4DA10[8] = "Stop";

const char D_00A4DA18[16] = "Block Boost";

const char D_00A4DA28[24] = "EP Overconsumption";

const char D_00A4DA40[16] = "Ether Poison";

const char D_00A4DA50[8] = "Curse";

const char D_00A4DA58[16] = "Attack Poison";

const char D_00A4DA68[16] = "Ether Atk Down";

const char D_00A4DA78[8] = "Lost";

const char D_00A4DA80[8] = "Sleep";

const char D_00A4DA88[16] = "Confusion";

const char D_00A4DA98[8] = "Safety";

const char D_00A4DAA0[8] = "AP MAX";

const char D_00A4DAA8[16] = "Evade Up";

const char D_00A4DAB8[16] = "Dexterity Up";

const char D_00A4DAC8[16] = "Speed 50";

const char D_00A4DAD8[16] = "Speed 25";

const char D_00A4DAE8[16] = "Focus Phys Def";

const char D_00A4DAF8[16] = "Focus Phys Atk";

const char D_00A4DB08[16] = "Phys Def Up";

const char D_00A4DB18[16] = "Phys Atk Up";

const char D_00A4DB28[8] = "ST Lock";

const char D_00A4DB30[16] = "Auto Revive";

const char D_00A4DB40[16] = "Focus Ether Def";

const char D_00A4DB50[16] = "Focus Ether Atk";

const char D_00A4DB60[8] = "EP x2";

const char D_00A4DB68[16] = "Recovery x2";

const char D_00A4DB78[16] = "MOMO Guard";

const char D_00A4DB88[16] = "Chivalry";

const char D_00A4DB98[16] = "CRTC Mark";

const char D_00A4DBA8[8] = "Wear";

const char D_00A4DBB0[16] = "Ether Crash";

const char D_00A4DBC0[16] = "Armor Failure";

const char D_00A4DBD0[16] = "Power Loss";

const char D_00A4DBE0[16] = "Engine Stop";

const char D_00A4DBF0[16] = "Pilot Confusion";

const char D_00A4DC00[16] = "Pilot Sleep";

const char D_00A4DC10[16] = "Pilot KO";

const char D_00A4DC20[16] = "Ether Shield";

const char D_00A4DC30[8] = "Bunnie";

const char D_00A4DC38[8] = "AG-06";

const char D_00A4DC40[8] = "AG-05";

const char D_00A4DC48[8] = "AG-04";

const char D_00A4DC50[8] = "AG-03";

const char D_00A4DC58[8] = "AG-02";

const char D_00A4DC60[8] = "AG-01";

const char D_00A4DC68[8] = "VX-06";

const char D_00A4DC70[8] = "VX-05";

const char D_00A4DC78[8] = "VX-04";

const char D_00A4DC80[8] = "VX-03";

const char D_00A4DC88[8] = "VX-02";

const char D_00A4DC90[8] = "VX-01";

const char D_00A4DC98[8] = "Mary";

const char D_00A4DCA0[8] = "Virgil";

const char D_00A4DCA8[8] = "Jr.";

const char D_00A4DCB0[8] = "MOMO";

const char D_00A4DCB8[16] = "Ziggurat 8";

const char D_00A4DCC8[8] = "Shion";

const char D_00A4DCD0[8] = "KOS-MOS";

const char D_00A4DCD8[8] = "chaos";

const char D_00A4DCE0[8] = "Ziggy";

const char D_00A4DCE8[16] = "AG05-G06";

const char D_00A4DCF8[16] = "AG04-G06";

const char D_00A4DD08[16] = "AG04-G05";

const char D_00A4DD18[16] = "AG02-G06";

const char D_00A4DD28[16] = "AG02-G05";

const char D_00A4DD38[16] = "AG02-G04";

const char D_00A4DD48[16] = "AG02-G03";

const char D_00A4DD58[16] = "AGO2-G02";

const char D_00A4DD68[16] = "VX06-G06";

const char D_00A4DD78[16] = "VX06-G05";

const char D_00A4DD88[8] = "VX-4000";

const char D_00A4DD90[16] = "VX06-G04";

const char D_00A4DDA0[16] = "VX02-G06";

const char D_00A4DDB0[16] = "VX02-G05";

const char D_00A4DDC0[16] = "VX02-G04";

const char D_00A4DDD0[8] = "VX-7000";

const char D_00A4DDD8[16] = "VX02-G03";

const char D_00A4DDE8[16] = "VX01-G06";

const char D_00A4DDF8[16] = "VX01-G05";

const char D_00A4DE08[16] = "VX01-G04";

const char D_00A4DE18[16] = "VX01-G03";

const char D_00A4DE28[16] = "VX01-G02";

const char D_00A4DE38[16] = "VX-10000";

const char D_00A4DE48[16] = "VX01-G01";

const char D_00A4DE58[16] = "AG05-F06";

const char D_00A4DE68[16] = "AG04-F06";

const char D_00A4DE78[16] = "AG04-F05";

const char D_00A4DE88[16] = "AG02-F06";

const char D_00A4DE98[16] = "AG02-F05";

const char D_00A4DEA8[16] = "AG02-F04";

const char D_00A4DEB8[16] = "AG02-F03";

const char D_00A4DEC8[16] = "AG02-F02";

const char D_00A4DED8[16] = "VX06-F06";

const char D_00A4DEE8[16] = "VX06-F05";

const char D_00A4DEF8[16] = "VX06-F04";

const char D_00A4DF08[16] = "VX02-F06";

const char D_00A4DF18[16] = "VX02-F05";

const char D_00A4DF28[16] = "VX02-F04";

const char D_00A4DF38[16] = "VX02-F03";

const char D_00A4DF48[16] = "VX01-F06";

const char D_00A4DF58[16] = "VX01-F05";

const char D_00A4DF68[16] = "VX01-F04";

const char D_00A4DF78[16] = "VX01-F03";

const char D_00A4DF88[16] = "VX01-F02";

const char D_00A4DF98[16] = "VX01-F01";



const char D_00A4A3A0[8] = "";
