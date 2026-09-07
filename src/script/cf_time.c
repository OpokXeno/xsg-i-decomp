/*
 * SCRIPT and CF are retained from the original main ELF symbols; their
 * historical expansions are not established.  The counter records CF
 * main-loop invocations between the reset sites documented in the packet.
 * s_nScriptCfTime is an external witness for the original local .sdata word,
 * not a definition recovered by this partial translation unit.
 */
extern unsigned int s_nScriptCfTime;

int SCRIPT_getCfTime(void)
{
    return (int)s_nScriptCfTime;
}

void SCRIPT_incCfTime(void)
{
    s_nScriptCfTime += 1u;
}
