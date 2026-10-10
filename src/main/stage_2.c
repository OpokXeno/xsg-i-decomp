#include "common.h"

#include "stage_2.h"

const char D_004DC208[8] = "peer";


void nmlModelSetMapLastInit(void);

void nmlModelSetBackBufferClear(void);

void Java_xeno_Stage_start__ILjava_lang_Object_(SceneVm *thread,
                                                 StageNativeSlot *arguments,
                                                 unsigned int *failure_result)
{
    SceneClass *scene_class = classJava_xeno_Stage;
    SceneObject object = arguments[0].object;
    JavaField *peer_field;
    StagePeer *peer;
    StageStringStorage *storage;
    SceneString *name;
    SceneMethod *method;
    StageThread *peer_thread;
    StageJavaString *method_object;

    if (JNI_isInstanceOf(object, scene_class) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(scene_class,
                                  loadConstString(D_004DC208, -1), 0);
    peer = (StagePeer *)*(SceneObject *)(object + peer_field->offset);
    if (arguments[1].integer != 0) {
        if (arguments[1].integer == 1) {
            method_object = arguments[2].method_name;
            peer->stage_state = 0;
            scene_class = ((SceneObjectHeader *)object)->class_ref->scene_class;
            storage = method_object->storage;
            name = loadConstString(storage->text, storage->length);
            method = findMethod(scene_class, name, TYPE_Void);
            peer_thread = JTHREAD_get((SceneObject)peer);
            if (peer_thread != 0) {
                peer_thread->entry = JTHREAD_defaultStage;
                peer_thread->method = method;
                peer_thread->flags |= 0x10;
            }
            if (peer_thread == (StageThread *)thread) {
                peer_thread->flags |= 0x21;
            }
        }
    }
}

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

void Java_xeno_Stage_play__Ljava_lang_String_(StageThread *thread,
                                              StagePlayCall *arguments)
{
    StageJavaString *script = arguments->string;
    StageStringStorage *storage;

    thread->wait_kind = 7;
    thread->resume_frames = thread->frame_depth;
    storage = script->storage;
    thread->flags |= 0x5;
    SCRIPT_load_DBG(storage->text);
    SCRIPT_exec();
}

void Java_xeno_Stage_setPartsLast__I(StageThread *thread, StageIntCall *arguments)
{
    StageModel *model;

    model = GameLoopState.model;
    if (model != 0 && model->id != 0) {
        nmlModelSetMapLastEntry(model->id, arguments->value);
    }
}

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

void Java_xeno_Stage_setFade__IIFFF(StageThread *thread, StageFadeCall *arguments)
{
    int channel;

    GameLoopState.fade_mode = arguments->mode;
    GameLoopState.fade.components.duration = (float)arguments->duration_frames;
    GameLoopState.fade.components.color[0] = arguments->red;
    GameLoopState.fade.components.color[1] = arguments->green;
    GameLoopState.fade.components.color[2] = arguments->blue;

    EnemySound_StopAll(1);
    for (channel = 0; channel < 3; channel++) {
        float value = GameLoopState.fade.components.color[channel];
        if (value > 1.0f) {
            value = GameLoopState.fade.components.color[channel] = 1.0f;
        }
        if (value < 0.0f) {
            GameLoopState.fade.components.color[channel] = 0.0f;
        }
    }
    if (GameLoopState.fade.components.duration < 1.0f) {
        GameLoopState.fade.components.duration = 1.0f;
    }
    GameLoopState.previous_fade_mode = GameLoopState.fade_mode;
    /* Snapshot both naturally aligned doubleword lanes of the fade record. */
    GameLoopState.previous_fade.aligned_words[0] = GameLoopState.fade.aligned_words[0];
    GameLoopState.previous_fade.aligned_words[1] = GameLoopState.fade.aligned_words[1];
    SCRIPT_fade(arguments->duration_frames);
}

void Java_xeno_Stage_setEventFade__IFFFIFFF(StageThread *thread,
                                             StageNativeSlot *arguments)
{
    nmlModelSetFadeInInterrupt(arguments[0].integer,
                               arguments[1].floating,
                               arguments[2].floating,
                               arguments[3].floating);

    GameLoopState.fade_mode = 0;
    GameLoopState.fade.components.duration = (float) arguments[4].integer;
    GameLoopState.fade.components.color[0] = arguments[5].floating;
    GameLoopState.fade.components.color[1] = arguments[6].floating;
    GameLoopState.fade.components.color[2] = arguments[7].floating;
    SCRIPT_fade(arguments[4].integer);
}

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

void Java_xeno_Stage_setVisible__IZ(StageThread *thread, StageVisibleCall *arguments)
{
    StageModel *model = GameLoopState.model;
    StagePartList *parts;

    if (model == 0 || model->id == 0) {
        return;
    }
    /* The model's integer ID is also the address of its part list. */
    parts = (StagePartList *)model->id;
    if (arguments->part_index >= 0) {
        if (arguments->part_index < parts->part_count) {
            nmlModelSetPartsVisible(parts, arguments->part_index,
                                    arguments->visible);
        }
    } else {
        return nmlModelInitPartsVisible(parts, arguments->visible);
    }
}

void Java_xeno_Stage_setCFBG__II_F(StageThread *thread, StageCFBGCall *arguments)
{
    StageBackground *background = arguments->background;

    switch (arguments->draw_type) {
    case 0:
        GameLoopState.background_mode = 0;
        GameLoopState.background_parameter = 0;
        break;
    case 1:
        GameBgDrawType1Entry(background->draw_parameter, background);
        break;
    case 2:
        GameBgDrawType2Entry(background->draw_parameter, background);
        break;
    }
}

void Java_xeno_Stage_setEffectRender__I(StageThread *thread, StageIntCall *arguments)
{
    nmlModelSetEffectWrite(arguments->value);
}

void Java_xeno_Stage_renderCommand__I(StageThread *thread, StageIntCall *arguments)
{
    GameLoopState.render_command = arguments->value;
}

void Java_xeno_Stage_setBgColor__FFF(StageThread *thread, StageBgColorCall *arguments)
{
    xglRenderClearColor(
        (((unsigned int)(arguments->blue * 255.0f) & 0xffu) << 16) +
        (((unsigned int)(arguments->green * 255.0f) & 0xffu) << 8) +
        ((unsigned int)(arguments->red * 255.0f) & 0xffu));
}

void Java_xeno_Stage_setBgClip__I(StageThread *thread, StageIntCall *arguments)
{
    GameLoopState.bg_clip = arguments->value;
}

void Java_xeno_Stage_clrBackBuffer__(void)
{
    nmlModelSetBackBufferClear();
}
