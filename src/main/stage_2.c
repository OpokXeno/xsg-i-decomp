#include "common.h"
#include "stage_2.h"

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_start__ILjava_lang_Object_);

void Java_xeno_Stage_stop__(StageThread *thread, StageObjectCall *arguments,
                            u32 *failure_result)
{
    SceneObject object;
    JavaField *peer_field;
    SceneObject peer;
    StageThread *peer_thread;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Stage) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Stage,
                                  loadConstString(D_004DC208, -1), 0);
    peer = *(SceneObject *)(object + peer_field->offset);
    peer_thread = JTHREAD_get(peer);
    if (peer_thread != 0) {
        peer_thread->flags &= ~STAGE_THREAD_RUNNING;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_play__Ljava_lang_String_);

void Java_xeno_Stage_setPartsLast__I(StageThread *thread, StageIntCall *arguments)
{
    StageModel *model;

    model = GameLoopState.model;
    if (model != 0 && model->id != 0) {
        nmlModelSetMapLastEntry(model->id, arguments->value);
    }
}

/*
 * nmlModelSetMapLastInit is src/main/nml_model_set.c's own INCLUDE_ASM
 * function (main/tu... , still unresolved there); this TU only tail-calls
 * it, so it stays a local extern declaration until that TU converts it.
 */
void nmlModelSetMapLastInit(void);

void Java_xeno_Stage_setPartsLastReset__(void)
{
    nmlModelSetMapLastInit();
}

void Java_xeno_Stage_setColor__FFF(StageThread *thread, StageColorCall *arguments)
{
    GameLoopState.color_r = arguments->r;
    GameLoopState.color_g = arguments->g;
    GameLoopState.color_b = arguments->b;
}

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setFade__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setEventFade__IFFFIFFF);

void Java_xeno_Stage_setFrameRender__II(StageThread *thread, StageFrameRenderCall *arguments)
{
    nmlModelSetBackBuffer(arguments->model_id, arguments->count, -1, 0);
}

void Java_xeno_Stage_setFadeCancel__I(StageThread *thread, StageIntCall *arguments)
{
    switch (arguments->value) {
    case 0:
        nmlModelSetFadeInCancel(60);
        nmlModelSetFadeOutCancel(60);
        break;
    case 1:
        nmlModelSetFadeOutCancel(60);
        break;
    case 2:
        nmlModelSetFadeInCancel(60);
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setVisible__IZ);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setCFBG__II_F);

void Java_xeno_Stage_setEffectRender__I(StageThread *thread, StageIntCall *arguments)
{
    nmlModelSetEffectWrite(arguments->value);
}

void Java_xeno_Stage_renderCommand__I(StageThread *thread, StageIntCall *arguments)
{
    GameLoopState.render_command = arguments->value;
}

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setBgColor__FFF);

void Java_xeno_Stage_setBgClip__I(StageThread *thread, StageIntCall *arguments)
{
    GameLoopState.bg_clip = arguments->value;
}

/*
 * nmlModelSetBackBufferClear is src/main/nml_model_set.c's own INCLUDE_ASM
 * function, still unresolved there; this TU only tail-calls it.
 */
void nmlModelSetBackBufferClear(void);

void Java_xeno_Stage_clrBackBuffer__(void)
{
    nmlModelSetBackBufferClear();
}
