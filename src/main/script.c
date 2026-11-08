#include "common.h"
#include "init_class_db.h"
#include "string_utf_get_hash.h"
#include "map_2.h"
#include "unit.h"
#include "shared.h"
#include "main/act_1.h"
#include "main/jni.h"
#include "main/xgl_cd.h"
#include "main/xgl_thread.h"
#include "main/xgl_2.h"
#include "main/Undulate.h"
#include "main/act_2.h"
#include "main/exm_2.h"
#include "main/find_native_method.h"
#include "main/fx_screen_mask.h"
#include "main/game.h"
#include "main/game_camera.h"
#include "main/game_debug_menu.h"
#include "main/game_init_camera.h"
#include "main/game_movie.h"
#include "main/game_pause_disp.h"
#include "main/jthread.h"
#include "main/map_1.h"
#include "main/map_2.h"
#include "main/msg_init.h"
#include "main/near_dir.h"
#include "main/nml_model_set.h"
#include "main/party.h"
#include "main/res_get_path.h"
#include "main/scene_1.h"
#include "main/sef_print.h"
#include "main/string_utf_get_hash.h"
#include "main/tcamera.h"
#include "main/twsys_init.h"
#include "main/xgl_font.h"
#include "main/xgl_pad.h"
#include "main/xgl_sound.h"

#define call_method_signature_void sig_2
#define call_method_signature_int sig_3
#define call_method_signature_int_int sig_4

#define SCRIPT_FADE_COLOR(red, green, blue)                                    \
    (0x7F000000 + ((int)((red) * 127.0f) << 16) +                              \
     ((int)((green) * 127.0f) << 8) + (int)((blue) * 127.0f))

/* GameLoopState.flags bits tested or toggled by the script, talk and debug code. */
#define GAME_LOOP_FLAG_DEBUG_MENU 0x1        /* a debug menu task owns the screen */
#define GAME_LOOP_FLAG_TALK_BLOCKED 0x20     /* no talk task starts, running ones abort */
#define GAME_LOOP_FLAG_ENTERED_RUNNING 0x400 /* the "entered" handler is executing */
#define GAME_LOOP_FLAG_EVENT_SCENE 0x800     /* an event scene is playing */
#define GAME_LOOP_FLAG_TALKING 0x1000        /* an actor talk command is running */
#define GAME_LOOP_FLAG_ENTERED_PENDING 0x100000 /* "entered" call queued by SCRIPT_execEntered */
#define GAME_LOOP_FLAG_FUNC_CALLED 0x200000  /* funcObserver finished a script call */

/* GameLoopState.kind values set by the script system. */
enum GameLoopKind {
    GAME_MODE_FIELD = 2,      /* free-roaming field/map play */
    GAME_MODE_EVENT = 3,      /* an event scene is running */
    GAME_MODE_DEBUG_TOOL = 7  /* "Tool" debug menu entry (cursor tool) */
};

/* Debug menu entries, in the order of the SCRIPT_debugMenu item text. */
enum ScriptDebugMenuItem {
    DEBUG_MENU_SCRIPT_EXEC,
    DEBUG_MENU_BG_CHANGE,
    DEBUG_MENU_VIRTUAL_M,
    DEBUG_MENU_TOOL,
    DEBUG_MENU_LIGHT,
    DEBUG_MENU_MEMORY_INFO,
    DEBUG_MENU_WIND
};

/* VirtualM debug switches (vmSwitchStr pairs); item 4 is the event monitor. */
#define VM_DEBUG_ITEM_MONITOR 4
#define VM_DEBUG_MASK_MONITOR 0x10

/* Scene system memory layout. */
#define SCRIPT_JNI_HEAP_SIZE 0x40000     /* heap handed to JNI_initSystem */
#define SCRIPT_EVT_FILE_BUFFER_SIZE 0x8000 /* system.evt and base.evt buffers */
#define SCRIPT_CLASS_IMAGE_SIZE 0x2D000  /* class database image of one script */
#define SCRIPT_DB0_MOTION_ADDR 0x05000000
#define SCRIPT_DB0_PDB_ADDR 0x06100000
#define SCRIPT_DB1_MOTION_ADDR 0x06500000
#define SCRIPT_DB1_PDB_ADDR 0x06900000
/* JNI class database slots. */
#define JNI_CLASS_SLOT_SYSTEM 0
#define JNI_CLASS_SLOT_BASE 1
#define JNI_CLASS_SLOT_SCRIPT0 2
#define JNI_CLASS_SLOT_SCRIPT1 3
/* JNI_createThread frame sizes and VM counts. */
#define SCRIPT_VM_STACK_WORDS 8
#define SCRIPT_DEFAULT_VM_FRAME_WORDS 0x80
#define SCRIPT_STAGE_VM_FRAME_WORDS 0x46
#define SCRIPT_EVENT_VM_FRAME_WORDS 0x40
#define SCRIPT_EVENT_VM_COUNT 2
#define SCRIPT_JS_CLASS_CAPACITY 0x40
#define SCRIPT_JS_METHOD_CAPACITY 8

/* Scene change timing (frames). */
#define SCRIPT_DEFAULT_FADE_FRAMES 30
#define SCRIPT_SCENE_CHANGE_FRAMES 25
#define SCRIPT_CAMERA_PARK_POSITION 1000000.0f /* stage camera moved far away */
#define RENDER_CLEAR_COLOR_BLACK 0x80000000    /* opaque black, alpha 0x80 */

/* Talk cancel test on PadData.button_state: the "pressed" halfword sits at bits
 * 16-31, so the mask is PAD_CIRCLE | PAD_CROSS and the value PAD_CROSS alone. */
#define SCRIPT_TALK_CANCEL_MASK 0x600000
#define SCRIPT_TALK_CANCEL_CROSS 0x400000

/* Pause screen (START pauses, TRIANGLE skips the event): frames before the
 * pause button is read again. */
#define PAUSE_INPUT_DELAY 0x20
#define PAUSE_SKIP_INPUT_DELAY 0x40

/* Slider window state meaning the window has been answered. */
#define SCRIPT_WINDOW_STATE_DONE 2

/* Debug tool layout and limits. */
#define SCRIPT_PATH_MAX 0x100
#define DEBUG_MAP_NO_MAX 2000
#define DEBUG_SLIDER_X 256.0f
#define DEBUG_SLIDER_Y 224.0f
#define DEBUG_MEMORY_TOTAL_KB 0x100
#define LIGHT_FILE_COUNT 100
#define LIGHT_FILE_LAST 99

/* PS2 sceOpen/sceLseek arguments. */
#define SCE_WRONLY_CREAT_TRUNC 0x0602 /* SCE_WRONLY | SCE_CREAT | SCE_TRUNC */
#define SCE_SEEK_SET 0
#define SCE_SEEK_END 2

/* GNU EE native TI storage type: one 16-byte store clears the debug menu storage. */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef void (*JSNativeMethod)(void);


/* TU-local observer and CallMethod declarations. */
/* One scriptDB element, indexed by currentScriptDB. The element is 0x1C bytes. */
typedef struct ScriptDbEntry {
    void *class_image;   /* +0x0: class database image read from the script file */
    void *motion_data;   /* +0x4: extra-motion pack read next to the script file (.fpk) */
    void *pdb;            /* +0x8: event data used for resource lookup */
    const char *active;   /* +0xC: first class name returned by JNI_searchClasses */
    SceneVm *thread;     /* +0x10: opaque handle of the JNI-created VM thread */
    int class_slot;      /* +0x14: class database slot the image is loaded into */
    int skip_count;      /* +0x18: class-name skip count JNI_searchClasses reported */
} ScriptDbEntry;

typedef struct ScriptPdbFile {
    u8 unmodeled_00[8];
    int data;
} ScriptPdbFile;

typedef struct ScriptEventDebugState {
    int mode;
    unsigned int flags;
    int selected;
} ScriptEventDebugState;

typedef union ScriptPathPrefix {
    char bytes[16];
    unsigned long long words[2];
} ScriptPathPrefix;

typedef union ScriptPathBuffer {
    char bytes[SCRIPT_PATH_MAX];
    unsigned long long words[SCRIPT_PATH_MAX / 8];
} ScriptPathBuffer;

enum ScriptDebugWord {
    SCRIPT_DEBUG_MODE,
    SCRIPT_DEBUG_FLAGS,
    SCRIPT_DEBUG_SELECTION
};

extern const char debugBaseDirPath[];
extern const char lightClassName[];
extern const char systemEvtPath[];
extern const char baseEvtPath[];
extern const char memoryUsageFormat[];
extern unsigned char scriptSceneStartName[];
extern const char sceneStageClassPath[];
extern char fpkExtension[];
extern char arcExtension[];
extern const ScriptPathPrefix mtnpackPathPrefix;
extern const char evtMonitorLabel[];
extern const char evtActorFormat[];
extern const char evtFlagsFormat[];
extern const char evtPositionFormat[];
extern const char evtRotationFormat[];
extern const char evtUnitFormat[];
extern char unknownBaseName[];
extern char cmdLabel[];
extern char moveLabel[];
extern char separatorLabel[];
extern const char evtFrameFormat[];
extern char classMethodFormat[];
extern const char fileOffText[24];
extern const char execOffText[24];
extern const char execOnText[24];
extern const char monitorOffText[24];
extern const char initOnText[24];
extern const char memoryOnText[24];
extern const char memoryOffText[24];
extern const char initOffText[24];
extern const char monitorOnText[24];
extern const char fileOnText[24];

extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *task),
                                       XglTaskPrefix *entry);

extern int xglTaskRemove(XglTaskPrefix *task);
extern void *PLAY_getCurrent(void);
extern void PLAY_setupDefault(void *play);
extern JavaClass *loadClass(SceneString *name, int initialize);
extern void STAGE_instance(SceneVm *vm, SceneObject stage_object);
extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

extern void JNI_initThread(SceneVm *vm);
extern void JNI_catchException(void);
extern void actTalkAfter(SceneObject object);
extern void initVM(void);
extern void nmlModelSetFadeOut(int frames, int option);
extern void nmlModelSendSignalEventFinish(void);
extern void xglRenderClearColor(u32 color);
extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern void xglCameraClipRangeDefault(StudioCamera *camera);

extern JThread *JNI_createThread(int kind, int stack_words,
                                     int frame_words);

extern void JNI_pushFrame(void);
extern void JNI_loadNativeClass(void);
void createTalkTask(void *actor, const char *method_name);
extern void TWIN_initCF(void *window);
extern void JS_init(int state, int class_capacity, int method_capacity);
extern int JS_loadClass(const char *class_name);
extern void JS_classSetup(int class_id, JSNativeMethod get_peer);

extern void JS_classAddMethod(int class_id, const char *name,
                              JSNativeMethod method);

extern void JS_classLight_getPeer(void);
extern void JS_classLight_setColor(void);
extern void JS_classLight_setDirection2(void);
extern ScriptPdbFile *PDB_findFile(void *pdb, const char *path);

int loadScriptCD(ScriptDbEntry *entry, const char *path);

int loadScriptCD2(ScriptDbEntry *entry, const char *path);
static int PauseSkipCheck(void);
extern char *strcpy(char *destination, const char *source);
extern unsigned int strlen(const char *string);
extern void TWSYS_setGRPStatus(int group, int status);
extern void initCursor(void);
extern int sprintf(char *buffer, const char *format, ...);
extern void DB_lightWrite(int descriptor);
extern void RSRC_init(int rc);
extern char *RSRC_loadFile2(int rc, const char *directory, const char *name);
extern int *RSRC_getItem(int rc, char *handle);
extern void JS_exec(char *begin, char *end);
extern int strncmp(const char *a, const char *b, unsigned int length);
extern void LoadMap2(int map_no, int entrance);
static char *getStrIndex(char *string, unsigned int length, int ch);
extern int sceOpen(const char *path, int flags, ...);
extern int sceClose(int descriptor);
extern char *strrchr(const char *string, int ch);
extern unsigned char xglFontLoad(int font_id, int flags);
const char *loadScript(ScriptDbEntry *entry, const char *path_base, const char *path);
extern void PartyTimePauseStart(void);
extern void xglRenderCopyDisp2Draw(void);
extern u32 xheap_info(u8 *heap_info);
extern void drawBox2(Matrix4 *matrix, int color);
extern void drawRect2(Matrix4 *matrix, int color);
extern void ACT_init(void);
extern void nmlModelInit(void);
void sceneObserver(XglTaskPrefix *task_prefix);
extern void GameDefocusSet(int a, int b, int c);
extern void initLight2(void);
extern void initLight3(void);
extern int updateLight(int mode);
extern const char *JNI_searchClasses(void *, const char *, int, int *);
extern void *RES_loadFile(int, int, const char *, int);
extern int sceRead(int descriptor, void *buffer, unsigned int size);
extern int sceLseek(int descriptor, int offset, int whence);
extern void DB_cmode(int mode);
extern void JNI_initSystem(void *heap_base, int heap_size);
extern void JNI_loadClassDB(int slot, void *image);
const char lightSetColorMethodName[] = "setColor";

const char lightSetDirection2MethodName[] = "setDirection2";

/* EUC-JP text meaning "event in progress". */
const char func_observer_debug_text[16] =
    "\xa5\xa4\xa5\xd9\xa5\xf3\xa5\xc8\xbc\xc2\xb9\xd4\xc3\xe6";

static char sig0_0[16] ;

static char sig1_1[34] ;

static const char *debugBasePath = debugBaseDirPath;

static unsigned int s_nScriptFadeOutTime = SCRIPT_DEFAULT_FADE_FRAMES;

static unsigned int s_nScriptFadeRequest = 0;

static unsigned char s_nScriptChangeTime = 0;

static unsigned int s_nScriptSequenceReset = 0;

static unsigned int s_nScriptFrameLockEntry = 0;

static unsigned int s_nScriptEventSkipEntry = 0;

static unsigned int s_nScriptCfTime = 0;

static unsigned int s_nScriptEventFin = 0;

static unsigned int s_nScriptEventActive = 0;

static unsigned int s_nScriptTalkLock = 0;

const char lightClassName[8] = "light";

/* JNI method signatures; the original ELF symbols are sig.2, sig.3 and sig.4. */
static const char sig_2[4] = "()V";

static const char sig_3[5] = "(I)V";

static const char sig_4[6] = "(II)V";

static void *windowOwner = 0;

static const char * vmSwitchStr[12];

static char sig0_0[16] = "(Lxeno/Enepc;)V";

static char sig1_1[34] = "(Lxeno/Enepc;Lxeno/util/Window;)V";

static const char * vmSwitchStr[12] = { memoryOffText, memoryOnText, initOffText, initOnText, fileOffText, fileOnText, execOffText, execOnText, monitorOffText, monitorOnText, 0, 0 };

JThread *defaultVM;

int UseVMFlag;

JThread *stageVM;

JThread *evtVM[SCRIPT_EVENT_VM_COUNT];

static int currentScriptDB;

static int resourceID;

static ScriptDbEntry scriptDB[2];

int rcFile;

static int lightFileNum;

static ScriptEventDebugState evtDebug;

static unsigned char heapInfo[136];

static int lightActFlags[4];

/* Declared here, not at the top: moving it up changes the original code. */
#include "script.h"
#include "main/script.h"

void talkCancel(ScriptObserverTask *task);
void sceneObserver2(ScriptSceneTask *task);
extern void Before_Talk(Actor *actor);
void talktoObserver(ScriptObserverTask *task);
void funcObserver(ScriptObserverTask *task);
int getEmptyVM(ScriptObserverTask *observer);

extern XglPadRecord PadData[2];

/* Game loop state, defined in another file. */
extern ScriptGameLoopState GameLoopState;

extern ScriptRenderState sRender;

static ScriptFileSelect * fileSelect;

extern Actor actor[ACTOR_COUNT];

extern MenuNativeScriptView *TMENU_create(int component_id);
extern MenuNativeScriptView *TSLIDER_create(int component_id);
extern void TMENU_addItem(MenuNativeScriptView *menu, const char *text);
extern int xglCdFileSelect(ScriptFileSelect *state);
extern void TWIN_dispose(TComponent *window);
void attrObserver(ScriptObserverTask *task);

static u32 heapUsed;

static int lightSave;

/* Initializes the JS runtime and registers the light class with its native methods. */
static void initJS(int state)
{
    int class_id;

    JS_init(state, SCRIPT_JS_CLASS_CAPACITY, SCRIPT_JS_METHOD_CAPACITY);
    class_id = JS_loadClass(lightClassName);
    JS_classSetup(class_id, JS_classLight_getPeer);
    JS_classAddMethod(class_id, lightSetColorMethodName, JS_classLight_setColor);
    JS_classAddMethod(class_id, lightSetDirection2MethodName, JS_classLight_setDirection2);
}

/* Calls method on the stage VM with argument, when method is not null. */
void SCRIPT_test(SceneObject argument, JavaMethod *method)
{
    JNI_initThread(stageVM);
    if (method != 0) {
        SceneObject arguments[1];

        arguments[0] = argument;
        JNI_callMethod(stageVM, method, arguments, 0);
    }
}

void SCRIPT_talkIgnoreSet(void)
{
    s_nScriptTalkLock = 1;
}

void SCRIPT_talkIgnoreClr(void)
{
    s_nScriptTalkLock = 0;
}

int SCRIPT_talkIgnoreGet(void)
{
    return s_nScriptTalkLock;
}

void SCRIPT_sceneChangeTimeInit(void)
{
    s_nScriptChangeTime = 0;
}

int SCRIPT_sceneChangeTimeGet(void)
{
    return s_nScriptChangeTime;
}

void SCRIPT_sceneChangeTimeSet(int value)
{
    s_nScriptChangeTime = value;
}

void SCRIPT_sceneChangeTimeDec(void)
{
    if (s_nScriptChangeTime != 0)
        s_nScriptChangeTime -= 1;
    else
        s_nScriptChangeTime = 0;
}

void SCRIPT_sendMovieSkipSignal(void)
{
    ScriptGameLoopState *game_state = &GameLoopState;

    if (game_state->kind == GAME_MODE_EVENT)
        s_nScriptEventFin = 1;
}

void SCRIPT_clrEventActiveFlag(void)
{
    s_nScriptEventActive = 0;
}

void SCRIPT_setEventActiveFlag(void)
{
    s_nScriptEventActive = 1;
}

int SCRIPT_getEventActiveFlag(void)
{
    return s_nScriptEventActive;
}

void SCRIPT_eventFinish(void)
{
    s_nScriptEventFin = 1;
}

void SCRIPT_methodClearSet(void)
{
    s_nScriptSequenceReset = 1;
}

int SCRIPT_methodClearGet(void)
{
    return s_nScriptSequenceReset;
}

void SCRIPT_fade(int time)
{
    s_nScriptFadeRequest = 1;
    if (time < 0)
        time = 0;
    s_nScriptFadeOutTime = time;
}

int SCRIPT_getFadeTime(void)
{
    return s_nScriptFadeOutTime;
}

int SCRIPT_fadeGet(void)
{
    if (s_nScriptFadeRequest == 0)
        return 0;
    return s_nScriptFadeOutTime;
}

int SCRIPT_getCfTime(void)
{
    return (int)s_nScriptCfTime;
}

void SCRIPT_incCfTime(void)
{
    s_nScriptCfTime += 1u;
}

void SCRIPT_frameLock2Battle(void)
{
    s_nScriptFrameLockEntry = 1;
}

/* Recreates the default, stage and event VMs and loads the native class. */
void SCRIPT_reset(void)
{
    JThread **event_vm;
    int remaining_vms;

    event_vm = evtVM;
    remaining_vms = 1;
    initVM();
    defaultVM = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_DEFAULT_VM_FRAME_WORDS);
    stageVM = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_STAGE_VM_FRAME_WORDS);
    do {
        --remaining_vms;
        *event_vm = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_EVENT_VM_FRAME_WORDS);
        ++event_vm;
    } while (remaining_vms >= 0);
    UseVMFlag = 0;
    JNI_pushFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
}

/*
 * Sets up the JNI heap, the two event file buffers and both script databases
 * in heap_base, creates the VMs and returns the address after the class images.
 */
void *SCRIPT_init(void *heap_base)
{
    unsigned char *cursor = heap_base;
    unsigned char *system_file_buffer;
    unsigned char *base_file_buffer;
    JThread **event_thread = &evtVM[0];
    ScriptDbEntry *entry = scriptDB;
    int remaining_vms = 1;

    evtDebug.flags = 0;
    cursor += SCRIPT_JNI_HEAP_SIZE;
    system_file_buffer = cursor;
    cursor += SCRIPT_EVT_FILE_BUFFER_SIZE;
    base_file_buffer = cursor;
    cursor += SCRIPT_EVT_FILE_BUFFER_SIZE;
    entry->motion_data = (void *)SCRIPT_DB0_MOTION_ADDR;
    entry->pdb = (void *)SCRIPT_DB0_PDB_ADDR;
    entry->class_slot = JNI_CLASS_SLOT_SCRIPT0;
    entry->skip_count = 0;
    entry->class_image = cursor;
    cursor += SCRIPT_CLASS_IMAGE_SIZE;
    entry->active = 0;
    entry->thread = 0;
    ++entry;
    {
    int *current_db = &currentScriptDB;
    *current_db = 0;
    }
    entry->active = 0;
    entry->thread = 0;
    entry->skip_count = 0;
    entry->class_image = cursor;
    cursor += SCRIPT_CLASS_IMAGE_SIZE;
    entry->motion_data = (void *)SCRIPT_DB1_MOTION_ADDR;
    entry->pdb = (void *)SCRIPT_DB1_PDB_ADDR;
    entry->class_slot = JNI_CLASS_SLOT_SCRIPT1;

    xglCdReadFile(systemEvtPath, system_file_buffer, 0, 0);
    xglCdReadFile(baseEvtPath, base_file_buffer, 0, 0);
    JNI_initSystem(heap_base, SCRIPT_JNI_HEAP_SIZE);
    JNI_loadClassDB(JNI_CLASS_SLOT_SYSTEM, system_file_buffer);
    JNI_loadClassDB(JNI_CLASS_SLOT_BASE, base_file_buffer);
    initVM();
    defaultVM = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_DEFAULT_VM_FRAME_WORDS);
    stageVM = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_STAGE_VM_FRAME_WORDS);
    while (remaining_vms >= 0) {
        --remaining_vms;
        event_thread[0] = JNI_createThread(0, SCRIPT_VM_STACK_WORDS, SCRIPT_EVENT_VM_FRAME_WORDS);
        ++event_thread;
    }
    UseVMFlag = 0;
    JNI_pushFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
    return cursor;
}

/* Starts the actor's talk method when talk is not locked, the scene is active and the actor is a Chr. */
void SCRIPT_execTalkto(void *actor)
{
    Actor *record = (Actor *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (record->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(record->java_object, classJava_xeno_Chr) != 0) {
        do {
            JThread *thread = entry->thread;

            if (!JNI_isInstanceOf(thread->object, classJava_xeno_Stage))
                break;
            createTalkTask(actor, record->talk_method_name);
        } while (0);
    }
}

/* Starts the actor's touch method under the same conditions as SCRIPT_execTalkto. */
void SCRIPT_execTouchto(void *actor)
{
    Actor *record = (Actor *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (record->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(record->java_object, classJava_xeno_Chr) != 0) {
        do {
            JThread *thread = entry->thread;

            if (!JNI_isInstanceOf(thread->object, classJava_xeno_Stage))
                break;
            createTalkTask(actor, record->touch_method_name);
        } while (0);
    }
}

/*
 * Creates the observer task that runs method_name on the actor. It uses the
 * (Enepc) signature when the class has it, else the (Enepc, Window) one with a new window.
 */
void createTalkTask(void *actor, const char *method_name)
{
    JThread *thread;
    SceneObject receiver;
    JavaClass *scene_class;
    JavaMethod *method;
    ScriptObserverTask *task;
    XglTaskScheduler *scheduler;

    if (GameLoopState.flags & GAME_LOOP_FLAG_TALK_BLOCKED)
        return;
    thread = scriptDB[currentScriptDB].thread;
    receiver = thread->object;
    scene_class = ((ObjectHeader *)receiver)->classPointer->clazz;
    method = findMethod(scene_class, loadConstString(method_name, -1),
                        loadConstString(sig0_0, -1));
    if (method != 0) {
        scheduler = GameLoopState.task_scheduler;
        task = (ScriptObserverTask *)xglTaskEntryNext(
            scheduler, (int (*)(XglTaskPrefix *))talktoObserver,
            scheduler != 0 ? scheduler->active_tail : 0);
        if (task != 0) {
            task->state_flags = 0;
            task->owner = &GameLoopState;
            task->object = 0;
        }
        task->argument2 = 0;
        task->method_signature = sig0_0;
    } else {
        TComponent *window;

        method = findMethod(scene_class, loadConstString(method_name, -1),
                            loadConstString(sig1_1, -1));
        if (method == 0)
            return;
        scheduler = GameLoopState.task_scheduler;
        task = (ScriptObserverTask *)xglTaskEntryNext(
            scheduler, (int (*)(XglTaskPrefix *))talktoObserver,
            scheduler != 0 ? scheduler->active_tail : 0);
        if (task != 0) {
            task->state_flags = 0;
            task->owner = &GameLoopState;
            task->object = 0;
        }
        task->method_signature = sig1_1;
        window = TWIN_create2(-1);
        window->class_ref = classJava_xeno_util_Window->classPointer;
        TWIN_initCF(window);
        window->flags &= ~0x40;
        window->flags &= ~0x80;
        task->argument2 = (int)window;
    }
    task->receiver = receiver;
    task->method_name = method_name;
    task->method = method;
    task->object = actor;
    Before_Talk(actor);
    GameLoopState.flags |= GAME_LOOP_FLAG_TALKING | GAME_LOOP_FLAG_PLAYER_LOCK;
    XTK_setWindowOwner(actor);
    JNI_initThread(stageVM);
    talktoObserver(task);
}

/*
 * Observer task step that calls the talk method on the stage VM. The task is
 * removed when the call finishes or the talk is cancelled.
 */
void talktoObserver(ScriptObserverTask *task)
{
    unsigned int owner_flags = task->owner->flags;
    SceneObject receiver;
    void *object;
    JavaMethod *method;
    int script_flags;
    SceneObject arguments[3];

    if (owner_flags & GAME_LOOP_FLAG_EVENT_SCENE) {
        xglTaskRemove(&task->task);
        return;
    }
    if (owner_flags & GAME_LOOP_FLAG_TALK_BLOCKED) {
        xglTaskRemove(&task->task);
        return;
    }

    receiver = task->receiver;
    {
        ObjectHeader *header = (ObjectHeader *)receiver;
        JavaClass *scene_class = header->classPointer->clazz;
        SceneString *name = loadConstString(task->method_name, -1);
        SceneString *signature = loadConstString(task->method_signature, -1);

        task->method = findMethod(scene_class, name, signature);
    }
    if (task->method == 0) {
        xglTaskRemove(&task->task);
        return;
    }

    object = task->object;
    script_flags = getScriptFlag(object);
    if (task->argument2 != 0 && (script_flags & 8)) {
        if (UseVMFlag != 0 || (PadData[0].button_state & SCRIPT_TALK_CANCEL_MASK) == SCRIPT_TALK_CANCEL_CROSS) {
            XTK_setWindowOwner(0);
            talkCancel(task);
            return;
        }
    }

    /* The method is read from the task before the argument array is filled; the original keeps this order. */
    method = task->method;
    arguments[0] = receiver;
    arguments[1] = ((Actor *)object)->java_object;
    if (task->argument2 != 0)
        arguments[2] = (SceneObject)task->argument2;
    JNI_callMethod(stageVM, method, arguments, 0);
    if (stageVM->flags & SCENE_THREAD_CALL_PENDING) {
        XTK_setWindowOwner(0);
        actTalkAfter(object);
        GameLoopState.flags &= ~GAME_LOOP_FLAG_TALKING;
        GameLoopState.flags &= ~GAME_LOOP_FLAG_PLAYER_LOCK;
        GameLoopState.flags &= ~0x10000;
        xglTaskRemove(&task->task);
    }
}

/* Ends a talk early: closes its window, clears the talk flags and removes the observer task. */
void talkCancel(ScriptObserverTask *task)
{
    TComponent *window;
    ScriptGameLoopState *game_state;

    actTalkAfter(task->object);
    window = (TComponent *)task->argument2;
    game_state = &GameLoopState;
    window->flags |= 0x80;
    window->flags &= ~2;
    game_state->flags &= ~GAME_LOOP_FLAG_TALKING;
    game_state->flags &= ~GAME_LOOP_FLAG_PLAYER_LOCK;
    game_state->flags &= ~0x10000;
    if (window != 0)
        window->state = 2;
    TWIN_dispose(window);
    xglTaskRemove(&task->task);
}

/* Queues an attrObserver task that calls the stage's entered method with attribute, unless one is running. */
void SCRIPT_execEntered(int attribute)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (!(GameLoopState.flags & GAME_LOOP_FLAG_ENTERED_RUNNING) &&
            JNI_isInstanceOf(object, classJava_xeno_Stage)) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))attrObserver,
                scheduler != 0 ? scheduler->active_tail : 0);

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->argument1 = attribute;
            GameLoopState.flags |= GAME_LOOP_FLAG_ENTERED_PENDING;
        }
    }
}

/* Calls method_name with no arguments on the stage object, through a func observer task. */
void CallMethod(const char *method_name)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_void;
            task->invocation_state = 0;
            funcObserver(task);
            /* The load after the call keeps funcObserver out of tail position, as in the original. */
            started = task->invocation_state;
        }
    }
}

/* Like CallMethod, passing argument1 as one int argument. */
void CallMethod_I(const char *method_name, int argument1)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int;
            task->argument1 = argument1;
            task->invocation_state = 0;
            funcObserver(task);
            /* The load after the call keeps funcObserver out of tail position, as in the original. */
            started = task->invocation_state;
        }
    }
}

/* Like CallMethod, passing argument1 and argument2 as two int arguments. */
void CallMethod_II(const char *method_name, int argument1, int argument2)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int_int;
            task->argument1 = argument1;
            task->argument2 = argument2;
            task->invocation_state = 0;
            funcObserver(task);
            /* The load after the call keeps funcObserver out of tail position, as in the original. */
            started = task->invocation_state;
        }
    }
}

/*
 * Observer task step that calls a script method on an event VM. Each early
 * exit removes the task; the do { } while (0) exits are kept as in the original.
 */
void funcObserver(ScriptObserverTask *task)
{
    unsigned int owner_flags;
    ScriptDbEntry *entry;
    JThread *thread;
    signed char vm_slot;
    SceneObject object;

    xglFontDebugPrintf(8, 24, func_observer_debug_text);

    owner_flags = task->owner->flags;
    if (owner_flags & GAME_LOOP_FLAG_EVENT_SCENE)
        do
        {
            xglTaskRemove(&(task)->task);
            return;
        }
        while (0);
    if ((task->state_flags & 1) == 0) {
        if (owner_flags & GAME_LOOP_FLAG_ENTERED_RUNNING)
            do
            {
                xglTaskRemove(&(task)->task);
                return;
            }
            while (0);
        task->state_flags |= 1;
        if (getEmptyVM(task) == 0)
            do
            {
                xglTaskRemove(&(task)->task);
                return;
            }
            while (0);
    }

    entry = &scriptDB[currentScriptDB];
    vm_slot = task->vm_slot;
    if (entry->active == 0)
        return;

    /* The Stage object changed since CallMethod queued the task. */
    thread = entry->thread;
    object = thread->object;
    if (object != task->object)
        do
        {
            xglTaskRemove(&(task)->task);
            return;
        }
        while (0);

    {
        ObjectHeader *header = (ObjectHeader *)task->object;
        JavaClass *scene_class = header->classPointer->clazz;
        SceneString *name = loadConstString(task->method_name, -1);
        SceneString *signature = loadConstString(task->method_signature, -1);
        JavaMethod *method = findMethod(scene_class, name, signature);
        SceneObject arguments[3];

        task->method = method;
        if (method == 0) {
            UseVMFlag &= ~(1 << vm_slot);
            xglTaskRemove(&task->task);
            return;
        }

        arguments[0] = object;
        arguments[1] = (SceneObject)task->argument1;
        arguments[2] = (SceneObject)task->argument2;
        JNI_callMethod(evtVM[vm_slot], method, arguments, 0);
        if (evtVM[vm_slot]->flags & SCENE_THREAD_CALL_PENDING) {
            JNI_catchException();
            UseVMFlag &= ~(1 << vm_slot);
            xglTaskRemove(&task->task);
            return;
        }
    }
    GameLoopState.flags |= GAME_LOOP_FLAG_FUNC_CALLED;
}

/* Assigns a free event VM to the observer and marks it in use. Returns 0 when none is free. */
int getEmptyVM(ScriptObserverTask *observer)
{
    int slot;

    observer->vm_slot = -1;
    for (slot = 0; slot < SCRIPT_EVENT_VM_COUNT; ++slot) {
        if (((UseVMFlag >> slot) & 1) == 0) {
            JNI_initThread(evtVM[slot]);
            observer->state_flags |= 1;
            UseVMFlag |= 1 << slot;
            observer->vm_slot = (signed char)slot;
            return 1;
        }
    }
    return observer->vm_slot != -1;
}

int SCRIPT_load(const char *path)
{
    return loadScriptCD(&scriptDB[(currentScriptDB + 1) & 1], path);
}

int SCRIPT_load2(const char *path)
{
    return loadScriptCD2(&scriptDB[(currentScriptDB + 1) & 1], path);
}

/* Sets the debug database addresses and loads the script at path into the inactive database. */
void SCRIPT_load_DBG(const char *path)
{
    ScriptDbEntry *entry;
    ScriptDbEntry *nextEntry;
    const char *base;
    const char **path_slot;

    xglFontLoad(0, 0);
    path_slot = &debugBasePath;
    entry = &scriptDB[0];
    entry->motion_data = (void *)SCRIPT_DB0_MOTION_ADDR;
    entry->pdb = (void *)SCRIPT_DB0_PDB_ADDR;
    entry = scriptDB + 1;
    entry->motion_data = (void *)SCRIPT_DB1_MOTION_ADDR;
    entry->pdb = (void *)SCRIPT_DB1_PDB_ADDR;
    nextEntry = &scriptDB[(currentScriptDB + 1) & 1];
    base = *path_slot;
    loadScript(nextEntry, base, path);
}

/* Returns the last occurrence of ch in the first length bytes of string, or 0. */
static char *getStrIndex(char *string, unsigned int length, int ch)
{
    int index = (int)(length - 1U);

    while (index >= 0) {
        char *cursor = &string[index];
        long current_character = *cursor;

        --index;
        if (current_character == ch)
            return cursor;
    }
    return 0;
}

/* Replaces the text from the last '.' of path onward with ext. */
static char *replacePathExt(char *path, const char *ext)
{
    return strcpy(getStrIndex(path, strlen(path), '.'), ext);
}

/* Observer task step that calls the stage's entered method with the task's attribute. */
void attrObserver(ScriptObserverTask *task)
{
    SceneObject object = task->object;
    JavaMethod *method;
    int vm_slot;
    ScriptMethodArgument arguments[2];

    if ((task->state_flags & 1) == 0) {
        if (task->owner->flags & GAME_LOOP_FLAG_ENTERED_RUNNING) {
            xglTaskRemove(&task->task);
            return;
        }
        task->owner->flags |= GAME_LOOP_FLAG_ENTERED_RUNNING;
        if (getEmptyVM(task) == 0) {
            xglTaskRemove(&task->task);
            return;
        }
        task->state_flags |= 1;
    }
    {
        ObjectHeader *header = (ObjectHeader *)object;
        JavaClass *scene_class = header->classPointer->clazz;
        SceneString *name = loadConstString("entered", -1);
        SceneString *signature = loadConstString("(I)V", -1);

        method = findMethod(scene_class, name, signature);
    }
    if (method == 0) {
        task->owner->flags &= ~GAME_LOOP_FLAG_ENTERED_RUNNING;
        xglTaskRemove(&task->task);
        return;
    }
    arguments[0].reference = object;
    vm_slot = task->vm_slot;
    arguments[1].integer = task->argument1;
    JNI_callMethod(evtVM[vm_slot], method, (SceneObject *)arguments, 0);
    if (evtVM[vm_slot]->flags & SCENE_THREAD_CALL_PENDING) {
        JNI_catchException();
        task->owner->flags &= ~GAME_LOOP_FLAG_ENTERED_RUNNING;
        UseVMFlag &= ~(1 << vm_slot);
        xglTaskRemove(&task->task);
        GameLoopState.flags &= ~GAME_LOOP_FLAG_ENTERED_PENDING;
    }
}

/* Runs the pause screen when START is pressed. Returns 1 when TRIANGLE skips the event, else 0. */
static int PauseSkipCheck(void)
{
    if ((GameLoopState.flags & 0x90000000) != 0)
        return 1;
    if (GameLoopState.input_delay != 0 ||
        (PadData[0].pressed & PAD_START) == 0)
        return 0;

    GameLoopState.input_delay = PAUSE_INPUT_DELAY;
    xglSoundEffectNormalDirect(4);
    sRender.scene_disabled = 1;
    xglSleep();
    xglRenderCopyDisp2Draw();
    GamePauseDispEvent();
    xglSleep();
    GamePauseDispEvent();
    xglSleep();
    PartyTimePauseStart();

    /* Waits until START resumes the event or TRIANGLE skips it. */
    if (!(PadData[0].pressed & PAD_TRIANGLE)) {
        for (;;) {
            if (GameLoopState.input_delay != 0) {
                GameLoopState.input_delay--;
            } else if ((PadData[0].pressed & PAD_START) != 0) {
                break;
            }
            xglSleep();
            if (PadData[0].pressed & PAD_TRIANGLE)
                break;
        }
    }

    PartyTimePauseEnd();
    xglSoundEffectNormalDirect(4);
    sRender.scene_disabled = 0;
    GameLoopState.input_delay = PAUSE_INPUT_DELAY;
    if (PadData[0].pressed & PAD_TRIANGLE) {
        GameLoopState.input_delay = PAUSE_SKIP_INPUT_DELAY;
        if (s_nScriptFrameLockEntry == 0)
            s_nScriptEventSkipEntry = 1;
        xglSoundStreamStop(0);
        return 1;
    }
    xglSoundStreamMute();
    return 0;
}

/*
 * Scene observer step: fades out once the event is pending, then resets the
 * scene state and cameras and removes the task.
 */
void sceneObserver2(ScriptSceneTask *task)
{
    JThread *thread;
    StudioCamera *camera;
    int camera_index;

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        task->fade_started = 0;
        task->elapsed_frames = 0;
    }
    thread = task->thread;
    if (PauseSkipCheck() != 0 || s_nScriptEventFin != 0) {
        thread->flags = (thread->flags & ~SCENE_THREAD_HAS_METHOD) | SCENE_THREAD_CALL_PENDING;
        s_nScriptEventFin = 0;
    }
    if (!(thread->flags & SCENE_THREAD_CALL_PENDING))
        return;
    if (task->fade_started == 0) {
        task->fade_started = 1;
        if (s_nScriptFrameLockEntry == 0) {
            if ((int)GameLoopState.fade_out_frames > 0) {
                nmlModelSetFadeOut((int)GameLoopState.fade_out_frames, 0);
                FX_ScreenMask(0,
                              SCRIPT_FADE_COLOR(GameLoopState.fade_out_red,
                                                GameLoopState.fade_out_green,
                                                GameLoopState.fade_out_blue),
                              (int)GameLoopState.fade_out_frames, 2);
            }
        } else {
            GameLoopState.frame_lock_wait = s_nScriptFadeOutTime + 4;
        }
        if (s_nScriptEventSkipEntry != 0 && s_nScriptFrameLockEntry == 0)
            nmlModelSetBackBufferToBattle(sRender.owner_index);
        s_nScriptChangeTime = (int)GameLoopState.fade_out_frames + 1;
    }
    task->elapsed_frames++;
    if ((int)s_nScriptFadeOutTime >= task->elapsed_frames)
        return;
    if (s_nScriptFrameLockEntry != 0) {
        nmlModelSetBackBufferToBattle(sRender.owner_index);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
    }
    if (s_nScriptEventSkipEntry != 0) {
        nmlModelSendSignalEventFinish();
        s_nScriptEventSkipEntry = 0;
    }
    s_nScriptFadeOutTime = SCRIPT_DEFAULT_FADE_FRAMES;
    s_nScriptChangeTime = SCRIPT_SCENE_CHANGE_FRAMES;
    camera_index = 0;
    s_nScriptEventActive = 0;
    s_nScriptFrameLockEntry = 0;
    s_nScriptFadeRequest = 0;
    s_nScriptTalkLock = 0;
    MAP_initUnit();
    ACT_initScene();
    ACT_initMTNResource();
    TWSYS_init();
    MSG_init();
    s_nScriptSequenceReset = 0;
    xglRenderClearColor(RENDER_CLEAR_COLOR_BLACK);
    GameLoopState.kind = GAME_MODE_FIELD;
    s_nScriptCfTime = 0;
    nmlModelSetGlobalPointLightReset();
    SCENE_cleanup(stageVM, thread->object);
    JNI_catchException();
    task->owner->flags &= ~GAME_LOOP_FLAG_EVENT_SCENE;
    do {
        xglStudioGetCamera(&camera, camera_index);
        camera_index++;
        xglCameraClipRangeDefault(camera);
    } while (camera_index < STUDIO_CAMERA_COUNT);
    nmlModelSetGlobalPointLightReset();
    nmlModelSetEffectWrite(1);
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    xglTaskRemove(&task->task);
}

/*
 * Scene observer step. The first run creates the stage object and its VM. Later
 * runs fade out once the event is pending, then reset the scene and remove the task.
 */
void sceneObserver(XglTaskPrefix *task_prefix)
{
    ScriptSceneTask *task = (ScriptSceneTask *)task_prefix;
    JThread *thread;
    ScriptDbEntry *entry;
    StudioCamera *camera;
    int camera_index;

    if (!(task->state_flags & 1)) {
        SceneObject object;

        task->state_flags |= 1;
        task->fade_started = 0;
        task->elapsed_frames = 0;
        entry = &scriptDB[currentScriptDB];
        object = newObject(loadClass(loadConstString(entry->active,
                                                     -1), 0));
        thread = JNI_createThread(2, 6, SCRIPT_STAGE_VM_FRAME_WORDS);
        {
            SceneVm *instance_vm = defaultVM;

            thread->object = object;
            entry->thread = thread;
            task->thread = entry->thread;
            SCENE_instance(instance_vm, thread->object);
            SCENE_start(defaultVM, thread->object, 8,
                        scriptSceneStartName);
        }
    }

    thread = task->thread;
    if (thread->stack == 0)
        xglTaskRemove(task_prefix);
    if (PauseSkipCheck() != 0 || s_nScriptEventFin != 0) {
        thread->flags = (thread->flags & ~SCENE_THREAD_HAS_METHOD) | SCENE_THREAD_CALL_PENDING;
        s_nScriptEventFin = 0;
    }
    if (!(thread->flags & SCENE_THREAD_CALL_PENDING))
        return;

    /* Starts the fade out once, then waits until the fade time has elapsed. */
    if (task->fade_started == 0) {
        task->fade_started = 1;
        if (s_nScriptFrameLockEntry == 0) {
            if ((int)GameLoopState.fade_out_frames > 0) {
                nmlModelSetFadeOut((int)GameLoopState.fade_out_frames, 0);
                FX_ScreenMask(0,
                              SCRIPT_FADE_COLOR(GameLoopState.fade_out_red,
                                                GameLoopState.fade_out_green,
                                                GameLoopState.fade_out_blue),
                              (int)GameLoopState.fade_out_frames, 2);
            }
        } else {
            GameLoopState.frame_lock_wait = s_nScriptFadeOutTime + 4;
        }
        if (s_nScriptEventSkipEntry != 0 && s_nScriptFrameLockEntry == 0)
            nmlModelSetBackBufferToBattle(sRender.owner_index);
        s_nScriptChangeTime = (int)GameLoopState.fade_out_frames + 1;
    }
    task->elapsed_frames++;
    if ((int)s_nScriptFadeOutTime >= task->elapsed_frames)
        return;

    if (s_nScriptFrameLockEntry != 0) {
        nmlModelSetBackBufferToBattle(sRender.owner_index);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
    }
    if (s_nScriptEventSkipEntry != 0) {
        nmlModelSendSignalEventFinish();
        s_nScriptEventSkipEntry = 0;
    }
    /* Resets the scene flags, then reloads the map, actor and message systems. */
    s_nScriptFadeOutTime = SCRIPT_DEFAULT_FADE_FRAMES;
    s_nScriptChangeTime = SCRIPT_SCENE_CHANGE_FRAMES;
    s_nScriptFrameLockEntry = 0;
    s_nScriptFadeRequest = 0;
    s_nScriptEventActive = 0;
    s_nScriptTalkLock = 0;
    xglSoundStreamStop(0);
    GameMovieStop();
    GameDefocusSet(-1, 0, 0);
    MAP_initUnit();
    ACT_init();
    ACT_initMTNResource();
    TWSYS_init();
    MSG_init();
    sefInitEffectCf();
    JNI_popFrame();
    GameLoopState.kind = GAME_MODE_FIELD;
    entry = &scriptDB[currentScriptDB ^ 1];
    currentScriptDB ^= 1;
    nmlModelSetGlobalPointLightReset();
    s_nScriptCfTime = 0;
    s_nScriptSequenceReset = 0;
    xglRenderClearColor(RENDER_CLEAR_COLOR_BLACK);
    nmlModelInit();
    GameResourceReset(0);
    GameLoopState.scene_reset_counter = 0;
    MapChange2(MapGetNo());
    for (camera_index = 0; camera_index < STUDIO_CAMERA_COUNT; camera_index++) {
        StudioCamera *configured_camera;

        xglStudioGetCamera(&camera, camera_index);
        xglCameraClipRangeDefault(camera);
        configured_camera = camera;
        if (camera_index == 0) {
            configured_camera->state = 4;
            configured_camera->active = 1;
        } else {
            configured_camera->active = 0;
        }
    }
    if (entry->thread == 0) {
        task->owner->flags &= ~GAME_LOOP_FLAG_EVENT_SCENE;
        JNI_loadClassDB(entry->class_slot, 0);
        entry = &scriptDB[(currentScriptDB + 1) & 1];
        entry->thread = 0;
    } else {
        JNI_loadClassDB(entry->class_slot, entry->class_image);
        task->owner->flags &= ~GAME_LOOP_FLAG_EVENT_SCENE;
        entry = &scriptDB[(currentScriptDB + 1) & 1];
        JNI_loadClassDB(entry->class_slot, 0);
    }
    nmlModelSetGlobalPointLightReset();
    nmlModelSetEffectWrite(1);
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    xglTaskRemove(task_prefix);
}

/* Declared here, not at the top: moving it up changes the original code. */
extern void TMENU_setItem(MenuNativeScriptView *menu, int index, const char *text);

int DB_lightSave(ScriptFileSelect *state);
void DB_evtMonitor(ScriptMenuTask *task);

/*
 * Debug light editor step. The first run picks the light set (SQUARE selects
 * the second) and saves and clears the actor light flags. TRIANGLE starts a
 * save; CROSS restores the flags and exits.
 */
static void DB_light(ScriptMenuTask *task)
{
    XglPadRecord *pad = PadData;
    unsigned int state_flags;

    state_flags = task->state_flags;
    if (!(state_flags & 1)) {
        int i;
        unsigned short buttons = pad->buttons;

        task->state_flags = state_flags | 1;
        if (buttons & PAD_SQUARE)
            initLight3();
        else
            initLight2();

        for (i = 0; i < ACTOR_COUNT; i++) {
            int word = i >> 5;
            int shift = i & 31;
            if (actor[i].flags & 0x8000)
                lightActFlags[word] |= 1U << shift;
            else
                lightActFlags[word] &= ~(1U << shift);
            actor[i].flags &= ~0x8000;
        }
    }

    if (lightSave != 0) {
        int result = DB_lightSave(fileSelect);

        lightSave = result != 0 ? 0 : lightSave;
        pad->pressed = 0;
    } else {
        if (pad->pressed & PAD_TRIANGLE)
            lightSave = 1;
        updateLight(0);
    }

    if (pad->pressed & PAD_CROSS) {
        int i;

        for (i = 0; i < ACTOR_COUNT; i++) {
            if ((lightActFlags[i >> 5] >> (i & 31)) & 1)
                actor[i].flags |= 0x8000;
        }
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
    }
}

/*
 * Debug menu step for the VirtualM switches: builds the item list on the first
 * run and toggles the selected switch. Cancel closes the menu.
 */
static void DB_vmSwitch(ScriptMenuTask *task)
{
    MenuNativeScriptView *menu = task->window;
    int item;
    int on;
    int i;
    int bit;
    int item_bit;

    if (!(task->state_flags & 1)) {
        menu = TMENU_create(-1);
        menu->options = 0;
        menu->mode |= 0x10;
        for (i = 0, bit = 1; i < 8 && vmSwitchStr[i * 2] != 0; i++, bit = 1 << (i & 0x1f)) {
            on = (vmDebugMask & bit) != 0;
            TMENU_setItem(menu, -1, vmSwitchStr[i * 2 + on]);
        }
        task->window = menu;
        task->state_flags |= 1;
    }
    if (menu->state != 0)
        return;
    item = menu->selected_item;
    if (menu->cancel_item < 0) {
        menu->state = 2;
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
        return;
    }
    item_bit = 1 << (item & 0x1f);
    if (vmDebugMask & item_bit) {
        vmDebugMask &= ~item_bit;
        on = 0;
    } else {
        vmDebugMask |= item_bit;
        on = 1;
    }
    TMENU_setItem(menu, item, vmSwitchStr[item * 2 + on]);
    menu->state = 0xe;
    if (item == VM_DEBUG_ITEM_MONITOR) {
        ScriptMenuTask *monitor;

        do {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            monitor = (ScriptMenuTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))DB_evtMonitor,
                scheduler != 0 ? scheduler->active_tail : 0);
            if (monitor != 0) {
                monitor->state_flags = 0;
                monitor->window = 0;
                monitor->owner = &GameLoopState;
            }
        } while (0);
    }
}

/*
 * Debug stage select: a map slider, then an entrance slider. Confirming the
 * entrance loads that map at it. Cancelling a slider ends the step.
 */
static int DB_stageSelect(ScriptMenuTask *task)
{
    MenuNativeScriptView *slider;
    const char *map_name;
    int value;

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        slider = TSLIDER_create(-1);
        slider->max_value = DEBUG_MAP_NO_MAX;
        slider->mode |= 0x10;
        slider->x = DEBUG_SLIDER_X;
        slider->y = DEBUG_SLIDER_Y;
        slider->value = MapGetNo();
        task->window = slider;
        task->phase = 0;
        task->loaded_map = 0;
    }
    slider = task->window;
    switch (task->phase) {
    case 0:
        if (slider->value >= 0) {
            map_name = MapGetName(slider->value);
            if (map_name != 0)
                xglFontDebugPrintf(8, 32, map_name);
        }
        if (slider->state == SCRIPT_WINDOW_STATE_DONE) {
            if (slider->value >= 0) {
                task->phase = 1;
                task->map_no = slider->value;
                slider = TSLIDER_create(-1);
                task->window = slider;
                slider->x = DEBUG_SLIDER_X;
                slider->y = DEBUG_SLIDER_Y;
                slider->mode |= 0x10;
                break;
            }
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
            return xglTaskRemove(&task->task);
        }
        break;
    case 1:
        xglFontDebugPrintf(8, 32, "Entrance:");
        if (slider->state == SCRIPT_WINDOW_STATE_DONE) {
            value = slider->value;
            if (value >= 0) {
                task->loaded_map = value;
                LoadMap2(task->map_no, value);
            }
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
            return xglTaskRemove(&task->task);
        }
    }
}

/* Debug memory display: draws the heap usage bar and label each frame. CROSS closes it. */
void DB_memory(ScriptMenuTask *task)
{
    XglPadRecord *pad = PadData;
    char text[0x100];
    float matrix[4][4];
    float scale[4];
    float translation[4];
    float heap_bytes;

    if ((task->state_flags & 1) == 0) {
        task->state_flags = task->state_flags | 1;
        heapUsed = xheap_info(heapInfo);
    }

    sprintf(text, memoryUsageFormat, heapUsed >> 10, DEBUG_MEMORY_TOTAL_KB);
    xglFontDebugPrintf(0x10, 0x40, text);
    heap_bytes = (float)heapUsed;
    scale[0] = heap_bytes * 128.0f * 0.0000038146973f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    scale[1] = 24.0f;
    translation[0] = -64.0f;
    translation[1] = 64.0f;
    translation[2] = 0.0f;
    translation[3] = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(translation);
    xglMatrixStackScale(scale);
    xglMatrixStackSave(matrix);
    if (heapUsed <= 0x2aaa9)
        drawRect2(&matrix, 1);
    else
        drawRect2(&matrix, 0);

    scale[0] = 132.0f;
    scale[1] = 28.0f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    translation[0] = 0.0f;
    translation[1] = 52.0f;
    translation[2] = 0.0f;
    translation[3] = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(translation);
    xglMatrixStackScale(scale);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    if (pad->pressed & PAD_CROSS) {
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
    }
}

/*
 * Light file dialog: LEFT and RIGHT pick the lgtNN.rc number, CIRCLE loads it
 * and SQUARE saves the current lights into it. Returns 1 when the dialog closes.
 */
int DB_lightSave(ScriptFileSelect *state)
{
    char path[512];
    int result;
    char *handle;
    int file;
    XglPadRecord *pad = PadData;

    result = xglCdFileSelect(state);
    switch (result) {
    case 0:
        break;
    case 1:
        RSRC_init(rcFile);
        handle = RSRC_loadFile2(rcFile, state->directory, state->file_path);
        JS_exec(handle, handle + *RSRC_getItem(rcFile, handle));
        return 0;
    case 2:
        return 1;
    }
    if (pad->repeated & PAD_LEFT)
        lightFileNum--;
    if (pad->repeated & PAD_RIGHT)
        lightFileNum++;
    if (lightFileNum < 0)
        lightFileNum = LIGHT_FILE_LAST;
    if (lightFileNum >= LIGHT_FILE_COUNT)
        lightFileNum = 0;
    DB_reset(32, 160);
    /* Circle button: load.
     * EN: Filled circle button: Load */
    DB_println("\xa1\xfc\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xa5\xed\xa1\xbc\xa5\xc9");
    /* Square button: save.
     * EN: Filled square button: Save */
    DB_println("\xa2\xa3\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xa5\xbb\xa1\xbc\xa5\xd6");
    DB_println("lgt%02d.rc", lightFileNum);
    if (pad->pressed & PAD_SQUARE) {
        sprintf(path, "%s%slgt%02d.rc", state->directory, state->file_path, lightFileNum);
        file = sceOpen(path, SCE_WRONLY_CREAT_TRUNC);
        if (file >= 0) {
            DB_lightWrite(file);
            sceClose(file);
        }
        return 1;
    }
    return 0;
}

/* Debug file dialog: runs the chosen .evt file through SCRIPT_load_DBG and SCRIPT_exec, or the chosen .rc file through JS_exec. */
static int DB_scriptFileSelect(ScriptMenuTask *task)
{
    ScriptFileSelect *browser;
    char *extension;
    char *handle;

    if (!(task->state_flags & 1)) {
        fileSelect->selection = 0;
        task->window = fileSelect;
        task->state_flags |= 1;
    }
    browser = task->window;
    switch (xglCdFileSelect(browser)) {
    case 0:
        break;
    case 1:
        extension = strrchr(browser->file_path, '.');
        if (extension != 0) {
            if (strncmp(extension, ".evt", 4) == 0) {
                SCRIPT_load_DBG(browser->file_path);
                SCRIPT_exec();
            } else if (strncmp(extension, ".rc", 3) == 0) {
                RSRC_init(rcFile);
                handle = RSRC_loadFile2(rcFile, browser->directory, browser->file_path);
                JS_exec(handle, handle + *RSRC_getItem(rcFile, handle));
                task->owner->flags &= ~1;
                task->owner->kind = task->owner->saved_kind;
            }
        }
        task->owner->flags &= ~1;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    case 2:
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    }
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
static EXM_WindState dbgWind;

extern void updateWind(EXM_WindState *params);
extern void EXM_GetWindPara(EXM_WindState *params);
extern void EXM_SetWindStruct(EXM_WindState *params);
extern void EXM_SetWindPara(EXM_WindState *params);

/* Debug wind editor: loads the wind parameters on first run and updates them each frame. CROSS applies them and exits. */
static int DB_wind(ScriptMenuTask *task)
{
    XglPadRecord *pad = &PadData[1];

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        EXM_GetWindPara(&dbgWind);
        EXM_SetWindStruct(&dbgWind);
        GameLoopState.flags &= ~1;
        GameLoopState.debug_menu_storage = 0;
        GameLoopState.kind = GameLoopState.saved_kind;
    }
    updateWind(&dbgWind);
    if (pad->pressed & PAD_CROSS) {
        EXM_ClearWindStruct();
        EXM_SetWindPara(&dbgWind);
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    }
}

/*
 * Debug menu step: builds the item list on the first run. The selected item
 * queues its debug task or enters the cursor tool, then this task ends.
 */
void SCRIPT_debugMenu(ScriptMenuTask *task)
{
    MenuNativeScriptView *menu = task->window;
    ScriptMenuTask *created;

    if (!(task->state_flags & 1)) {
        TWSYS_setGRPStatus(0, 3);
        menu = TMENU_create(-1);
        menu->options = 0;
        menu->mode |= 0x10;
        TMENU_addItem(menu, "ScriptExec\nBgChange\nVirtualM\nTool\nLight\nMemoryInfo\nWind");
        menu->flags &= ~0x40;
        task->window = menu;
        task->state_flags |= 1;
    }
    if (menu->state != 0)
        return;
    menu->state = 2;
    switch (menu->selected_item) {
    case DEBUG_MENU_SCRIPT_EXEC:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_scriptFileSelect), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case DEBUG_MENU_BG_CHANGE:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_stageSelect), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case DEBUG_MENU_VIRTUAL_M:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_vmSwitch), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case DEBUG_MENU_TOOL:
        initCursor();
        task->owner->kind = GAME_MODE_DEBUG_TOOL;
        break;
    case DEBUG_MENU_LIGHT:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_light), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case DEBUG_MENU_MEMORY_INFO:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_memory), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        created->owner->saved_kind = task->owner->saved_kind;
        break;
    case DEBUG_MENU_WIND:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_wind), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    default:
        if (menu->cancel_item < 0) {
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
        }
        break;
    }
    xglTaskRemove(&task->task);
}

/*
 * Loads a script's class image from path, then its motion pack (.fpk) and PDB
 * (.arc) beside it. Returns the skip count, or the negative read result when
 * the class image cannot be read.
 */
int loadScriptCD(ScriptDbEntry *entry, const char *path)
{
    char full_path[SCRIPT_PATH_MAX];
    int result;
    int resource_read_size;
    const char *active;
    int skip_count;

    DebugMenuResourceSizeAdd = 0;
    strcpy(full_path, MAP_getPath());
    strcat(full_path, path);
    result = xglCdReadFile(full_path, entry->class_image, 0, 1);
    if (result < 0) {
        entry->skip_count = (skip_count = 0);
        entry->active = 0;
    } else {
        active = JNI_searchClasses(entry->class_image, sceneStageClassPath,
                                   2, &skip_count);
        replacePathExt(full_path, fpkExtension);
        resource_read_size = xglCdReadFile(full_path, entry->motion_data, 0, 1);
        if (resource_read_size < 0) {
            ACT_initExMotion(0, 1, 0);
        } else {
            DebugMenuResourceSizeAdd += resource_read_size;
            ACT_initExMotion(0, 1, entry->motion_data);
        }

        replacePathExt(full_path, arcExtension);
        resource_read_size = xglCdReadFile(full_path, entry->pdb, 0, 1);
        if (resource_read_size >= 0)
            DebugMenuResourceSizeAdd += resource_read_size;
        result = skip_count;
        entry->active = active;
        entry->skip_count = result;
    }
    return result;
}

/*
 * Like loadScriptCD, but the motion pack and PDB are loaded from the mtnpack
 * path using the script's base name. Returns the PDB address, or the negative
 * read size when the class image cannot be read.
 */
int loadScriptCD2(ScriptDbEntry *entry, const char *path)
{
    ScriptPathBuffer resource_path;
    const char *base_name;
    int read_size;
    int skip_count;
    const char *active;

    DebugMenuResourceSizeAdd = 0;
    strcpy(resource_path.bytes, path);
    if ((read_size = xglCdReadFile(resource_path.bytes, entry->class_image, 0, 1)) < 0) {
        entry->skip_count = (skip_count = 0);
        entry->active = 0;
        return read_size;
    }
    active = JNI_searchClasses(entry->class_image, sceneStageClassPath, 2, &skip_count);
    base_name = strrchr(path, '\\');
    if (base_name == 0)
        base_name = strrchr(path, '/');
    resource_path.words[0] = mtnpackPathPrefix.words[0];
    resource_path.bytes[8] = mtnpackPathPrefix.bytes[8];
    if (base_name != 0)
        strcat(resource_path.bytes, base_name + 1);
    replacePathExt(resource_path.bytes, fpkExtension);
    entry->motion_data = RES_loadFile(RES_LOAD_ANY_BLOCK, RES_TYPE_FILE, resource_path.bytes, 0);
    ACT_initExMotion(0, 1, entry->motion_data);
    replacePathExt(resource_path.bytes, arcExtension);
    entry->pdb = RES_loadFile(RES_LOAD_ANY_BLOCK, RES_TYPE_FILE, resource_path.bytes, 0);
    entry->active = active;
    entry->skip_count = skip_count;
    return (int)entry->pdb;
}

/*
 * Reads the class image of the script at path_base + path into the entry, then
 * its motion pack and PDB. Returns the class search result, or 0 when a file
 * cannot be opened or read.
 */
const char *loadScript(ScriptDbEntry *entry, const char *path_base, const char *path)
{
    struct ScriptImageHeaderBytes {
        unsigned char bytes[12];
    };
    union ScriptClassImageHeader {
        struct {
            unsigned int unmodeled_00[2];
            unsigned int image_size;
        } fields;
        struct ScriptImageHeaderBytes raw;
    };
    char full_path[SCRIPT_PATH_MAX];
    union ScriptClassImageHeader header_storage;
    int fd;
    int resource_fd;
    const char *result = 0;
    int class_size;
    int skip_count;

    DebugMenuResourceSizeAdd = 0;
    if (path_base != 0) {
        strcpy(full_path, path_base);
        strcat(full_path, path);
        fd = sceOpen(full_path, SCE_RDONLY);
        if (fd < 0)
            return 0;
        {
            union ScriptClassImageHeader *header = &header_storage;

            sceRead(fd, header, sizeof(*header));
            {
                union ScriptClassImageHeader *image_header = entry->class_image;
                image_header->raw = header->raw;
            }
            class_size = (int)header->fields.image_size - sizeof(*header);
            if (class_size != sceRead(fd, (unsigned char *)entry->class_image + sizeof(*header),
                                     (unsigned int)class_size))
                return 0;
        }
        sceClose(fd);
        replacePathExt(full_path, fpkExtension);
        resource_fd = sceOpen(full_path, SCE_RDONLY);
        if (resource_fd >= 0) {
            int file_size = sceLseek(resource_fd, 0, SCE_SEEK_END);

            sceLseek(resource_fd, 0, SCE_SEEK_SET);
            sceRead(resource_fd, entry->motion_data, (unsigned int)file_size);
            sceClose(resource_fd);
            ACT_initExMotion(0, 1, entry->motion_data);
            DebugMenuResourceSizeAdd += file_size;
        } else {
            ACT_initExMotion(0, 1, 0);
        }
        replacePathExt(full_path, arcExtension);
        resource_fd = sceOpen(full_path, SCE_RDONLY);
        if (resource_fd >= 0) {
            int file_size = sceLseek(resource_fd, 0, SCE_SEEK_END);

            sceLseek(resource_fd, 0, SCE_SEEK_SET);
            sceRead(resource_fd, entry->pdb, (unsigned int)file_size);
            sceClose(resource_fd);
            DebugMenuResourceSizeAdd += file_size;
        }
    }

    result = JNI_searchClasses(entry->class_image,
                               sceneStageClassPath,
                               2, &skip_count);
    entry->active = result;
    entry->skip_count = skip_count;
    return result;
}

/*
 * Event monitor step, drawn while the monitor switch is on: prints the current
 * VM's method and frames, then the selected actor (mode 0) or map unit. R2
 * switches between the two lists; LEFT and RIGHT change the selection.
 */
void DB_evtMonitor(ScriptMenuTask *task)
{
    char model_path[256];
    ScriptEventDebugState *debug = &evtDebug;
    XglPadRecord *pad;
    unsigned short repeated;
    JThread *thread;
    int mode;
    int selected;

    DB_cmode(1);
    if (!(vmDebugMask & VM_DEBUG_MASK_MONITOR)) {
        debug->flags &= ~1;
        xglTaskRemove(&task->task);
        return;
    }
    if (!(task->state_flags & 1)) {
        if (debug->flags & 1) {
            xglTaskRemove(&task->task);
            return;
        }
        task->state_flags |= 1;
        debug->flags |= 1;
    }
    DB_reset(0, 0);
    DB_println(evtMonitorLabel);
    thread = scriptDB[currentScriptDB].thread;
    {
    JThread *debug_thread = ((thread));
    if (debug_thread != 0 && debug_thread->object != 0) {
        {
        JavaClass *debug_class =
            ((ObjectHeader *)((debug_thread->object)))->classPointer->clazz;
        JavaMethod *debug_method = ((debug_thread))->method;
        if ((((debug_thread))->flags & SCENE_THREAD_HAS_METHOD) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->bytes,
                       debug_method->name->bytes);
            if (((debug_thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = (((0)));
                     frame_index < ((debug_thread))->frame_depth;
                     ++frame_index) {
                    VMFrame *frame =
                        &((debug_thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->pc,
                               frame->callee->name->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->bytes,
                           debug_method->name->bytes);
            }
        }
};
    }
};

    /* Mode 0 lists actors; any other mode lists map units. */
    mode = debug->mode;
    switch (mode) {
    case 0: {
        Actor *actor_record;
        const char *base_name;
        float half_turn;
        SceneObject object;

        pad = PadData;
        if (pad->pressed & PAD_R2)
            debug->mode = 1;
        repeated = pad->repeated;
        if (repeated & PAD_LEFT)
            --debug->selected;
        if (repeated & PAD_RIGHT)
            ++debug->selected;
        if (debug->selected < 0)
            debug->selected = ACTOR_COUNT - 1;
        if (debug->selected >= ACTOR_COUNT)
            debug->selected = 0;
        selected = debug->selected;
        actor_record = &actor[selected];
        RES_GetMdlFileName(model_path, actor_record->inUseId);
        base_name = strrchr(model_path, '\\');
        if (base_name == 0)
            base_name = unknownBaseName;
        ++base_name;
        DB_reset(0, 10);
        DB_println(evtActorFormat, debug->selected, actor_record->inUseId,
                   base_name);
        half_turn = 3.1415927f;
        DB_println(evtFlagsFormat, actor_record->flags);
        DB_println(evtPositionFormat, actor_record->position.x,
                   actor_record->position.y, actor_record->position.z);
        DB_println(evtRotationFormat,
                   actor_record->rotation.x / half_turn * 180.0f,
                   actor_record->rotation.y / half_turn * 180.0f,
                   actor_record->rotation.z / half_turn * 180.0f);
        object = actor_record->java_object;
        if (object != 0) {
            thread = JTHREAD_get(object);
            if (thread != 0) {
                object = thread->object;
                if (object != 0) {
                    {
        JavaClass *debug_class =
            ((ObjectHeader *)((object)))->classPointer->clazz;
        JavaMethod *debug_method = ((thread))->method;
        if ((((thread))->flags & SCENE_THREAD_HAS_METHOD) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->bytes,
                       debug_method->name->bytes);
            if (((thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = ((mode));
                     frame_index < ((thread))->frame_depth;
                     ++frame_index) {
                    VMFrame *frame =
                        &((thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->pc,
                               frame->callee->name->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->bytes,
                           debug_method->name->bytes);
            }
        }
};
                }
            }
        }
        break;
    }
    default: {
        MapUnitRecord *unit;
        UnitSequenceEntry *sequence;
        SceneObject object;
        float half_turn;

        pad = PadData;
        if (pad->pressed & PAD_R2)
            debug->mode = (mode + 1) & 1;
        repeated = pad->repeated;
        if (repeated & PAD_LEFT)
            --debug->selected;
        if (repeated & PAD_RIGHT)
            ++debug->selected;
        if (debug->selected < 0)
            debug->selected = MAP_UNIT_COUNT - 1;
        if (debug->selected >= MAP_UNIT_COUNT)
            debug->selected = 0;
        selected = debug->selected;
        unit = &MapUnit[selected];
        DB_reset(0, 10);
        sequence = &unitSequence[unit->slot];
        DB_println(evtUnitFormat, debug->selected, unit->serial);
        DB_println(evtFlagsFormat, unit->flags);
        DB_println(cmdLabel);
        if (sequence->state & 1)
            DB_println(moveLabel);
        else
            DB_println(separatorLabel);
        half_turn = 3.1415927f;
        DB_println(evtPositionFormat, unit->position.x, unit->position.y, unit->position.z);
        DB_println(evtRotationFormat, unit->rotation.x / half_turn * 180.0f,
                   unit->rotation.y / half_turn * 180.0f,
                   unit->rotation.z / half_turn * 180.0f);
        object = unit->sequence;
        if (object != 0) {
            thread = JTHREAD_get(object);
            if (thread != 0) {
                object = thread->object;
                if (object != 0) {
                    {
        JavaClass *debug_class =
            ((ObjectHeader *)((object)))->classPointer->clazz;
        JavaMethod *debug_method = ((thread))->method;
        if ((((thread))->flags & SCENE_THREAD_HAS_METHOD) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->bytes,
                       debug_method->name->bytes);
            if (((thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = ((0));
                     frame_index < ((thread))->frame_depth;
                     ++frame_index) {
                    VMFrame *frame =
                        &((thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->pc,
                               frame->callee->name->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->bytes,
                           debug_method->name->bytes);
            }
        }
};
                }
            }
        }
        break;
    }
    }
    DB_cmode(0);
}

/*
 * Switches to the script loaded into the other database and starts its scene.
 * Returns 0 when that script has no active class.
 */
ScriptDbEntry *SCRIPT_exec(void)
{
    ScriptDbEntry *entry;
    SceneObject object;
    JThread *thread;
    ScriptSceneTask *task;
    XglTaskScheduler *scheduler;
    JThread *vm;
    StudioCamera *stage_camera;
    StudioCamera *scene_camera;
    int camera_index;

    XTK_setWindowOwner(0);
    currentScriptDB ^= 1;
    entry = &scriptDB[(currentScriptDB + 1) & 1];
    JNI_loadClassDB(entry->class_slot, 0);
    entry = &scriptDB[currentScriptDB];
    PLAY_setupDefault(PLAY_getCurrent());
    /* Without an active class, only the main actor is created. */
    if (entry->active == 0) {
        Actor *actor;

        JNI_popFrame();
        XTK_setResourceID(1);
        GameLoopState.kind = GAME_MODE_FIELD;
        nmlModelSetGlobalPointLightReset();
        s_nScriptCfTime = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        ACT_init();
        MAP_initUnit();
        InitCfSystem();
        actor = ACT_create(0, 1);
        ACT_initMotion(actor);
        ACT_loadMotion(actor, actor->inUseId, 1);
        ACT_loadResource(actor, actor->inUseId);
        GameLoopState.main_actor = actor;
        entry->skip_count = 1;
        entry->thread = 0;
        JNI_loadClassDB(entry->class_slot, 0);
        JNI_pushFrame();
        JNI_loadNativeClass();
        return 0;
    }
    if (GameLoopState.flags & GAME_LOOP_FLAG_EVENT_SCENE) {
        JNI_popFrame();
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        ACT_init();
        MAP_initUnit();
        JNI_pushFrame();
        JNI_loadNativeClass();
    }
    /* A skip count of 1 parks the cameras and starts the stage from the class image. */
    if (entry->skip_count != 0) {
        if (entry->skip_count == 1) {
            Actor *actor;
            int camera_index;

            camera_index = 0;
            JNI_popFrame();
            XTK_setResourceID(1);
            GameLoopState.kind = GAME_MODE_FIELD;
            nmlModelSetGlobalPointLightReset();
            s_nScriptCfTime = 0;
            while (camera_index < STUDIO_CAMERA_COUNT) {
                StudioCamera *configured_camera;

                xglStudioGetCamera(&stage_camera, camera_index);
                xglCameraClipRangeDefault(stage_camera);
                configured_camera = stage_camera;
                if (camera_index == 0) {
                    configured_camera->state = 4;
                    configured_camera->active = 1;
                } else {
                    configured_camera->active = 0;
                }
                ++camera_index;
                configured_camera->position.vector.z = SCRIPT_CAMERA_PARK_POSITION;
                configured_camera->position.vector.y = SCRIPT_CAMERA_PARK_POSITION;
                configured_camera->position.vector.x = SCRIPT_CAMERA_PARK_POSITION;
                configured_camera->rotation.vector.y = 1.5707964f;
                configured_camera->rotation.vector.x = 0.0f;
                configured_camera->rotation.vector.z = 0.0f;
            }
            s_nScriptTalkLock = 0;
            TCAMERA_init();
            TWSYS_init();
            MSG_init();
            nmlModelInit();
            ACT_init();
            MAP_initUnit();
            InitCfSystem();
            s_nScriptSequenceReset = 0;
            xglRenderClearColor(RENDER_CLEAR_COLOR_BLACK);
            actor = ACT_create(0, 1);
            ACT_initMotion(actor);
            ACT_loadMotion(actor, actor->inUseId, 1);
            ACT_loadResource(actor, actor->inUseId);
            GameLoopState.main_actor = actor;
            JNI_pushFrame();
            JNI_loadNativeClass();
            JNI_loadClassDB(entry->class_slot, entry->class_image);
            object = newObject(loadClass(
                loadConstString(entry->active, -1), 0));
            thread = JNI_createThread(2, 6, SCRIPT_STAGE_VM_FRAME_WORDS);
            *&thread->object = object;
            vm = defaultVM;
            entry->thread = thread;
            STAGE_instance(vm, object);
        }
        return entry;
    }

    /* Otherwise the scene fades in and its cameras are reset. */
    XTK_setResourceID(2);
    nmlModelSetGlobalPointLightReset();
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    {
        int camera_index = 0;

        while (camera_index < STUDIO_CAMERA_COUNT) {
            StudioCamera *configured_camera;

            xglStudioGetCamera(&scene_camera, camera_index);
            xglCameraClipRangeDefault(scene_camera);
            configured_camera = scene_camera;
            if (camera_index == 0) {
                configured_camera->state = 4;
                configured_camera->active = 1;
            } else {
                configured_camera->active = 0;
            }
            ++camera_index;
            configured_camera->position.vector.z = SCRIPT_CAMERA_PARK_POSITION;
            configured_camera->position.vector.y = SCRIPT_CAMERA_PARK_POSITION;
            configured_camera->position.vector.x = SCRIPT_CAMERA_PARK_POSITION;
            configured_camera->rotation.vector.y = 1.5707964f;
            configured_camera->rotation.vector.x = 0.0f;
            configured_camera->rotation.vector.z = 0.0f;
        }
    }
    s_nScriptTalkLock = 0;
    TCAMERA_init();
    TWSYS_init();
    MSG_init();
    {
    unsigned int *event_active = &s_nScriptEventActive;
    unsigned int *sequence_reset = &s_nScriptSequenceReset;
    GameLoopState.kind = GAME_MODE_EVENT;
    *event_active = 1;
    GameLoopState.flags |= GAME_LOOP_FLAG_EVENT_SCENE;
    *sequence_reset = 0;
    }
    xglRenderClearColor(RENDER_CLEAR_COLOR_BLACK);
    nmlModelInit();
    GameResourceReset(0);
    GameLoopState.scene_reset_counter = 0;
    UnduInit(0);
    ACT_matrixInit();
    ACT_initScene();
    MAP_initUnit();
    JNI_pushFrame();
    JNI_loadNativeClass();
    JNI_loadClassDB(entry->class_slot, entry->class_image);
    scheduler = GameLoopState.task_scheduler;
    task = (ScriptSceneTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))sceneObserver,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->thread = 0;
        task->owner = &GameLoopState;
        task->state_flags = 0;
    }
    return entry;
}

/*
 * Like SCRIPT_exec, but the scene that is not skipped runs under a
 * sceneObserver2 task.
 */
ScriptDbEntry *SCRIPT_exec2(void)
{
    ScriptDbEntry *entry;
    SceneObject object;
    JThread *thread;
    ScriptSceneTask *task;
    XglTaskScheduler *scheduler;
    StudioCamera *camera;
    StudioCamera *target;
    int camera_index;
    JThread *vm;
    int (*callback)(XglTaskPrefix *);

    XTK_setWindowOwner(0);
    currentScriptDB ^= 1;
    entry = &scriptDB[(currentScriptDB + 1) & 1];
    JNI_loadClassDB(entry->class_slot, 0);
    entry = &scriptDB[currentScriptDB];
    PLAY_setupDefault(PLAY_getCurrent());
    if (entry->active == 0) {
        s_nScriptChangeTime = SCRIPT_SCENE_CHANGE_FRAMES;
        JNI_popFrame();
        XTK_setResourceID(1);
        GameLoopState.kind = GAME_MODE_FIELD;
        nmlModelSetGlobalPointLightReset();
        s_nScriptCfTime = 0;
        s_nScriptTalkLock = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        entry->thread = 0;
        JNI_loadClassDB(entry->class_slot, 0);
        JNI_pushFrame();
        JNI_loadNativeClass();
        return entry;
    }
    if (GameLoopState.flags & GAME_LOOP_FLAG_EVENT_SCENE) {
        s_nScriptTalkLock = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
    }
    if (entry->skip_count != 0) {
        if (entry->skip_count == 1) {
            camera_index = 0;
            JNI_popFrame();
            XTK_setResourceID(1);
            GameLoopState.kind = GAME_MODE_FIELD;
            nmlModelSetGlobalPointLightReset();
            s_nScriptCfTime = 0;
            do {
                xglStudioGetCamera(&camera, camera_index);
                target = camera;
                if (camera_index == 0) {
                    target->state = 4;
                    target->active = 1;
                } else {
                    target->active = 0;
                }
                camera_index++;
            } while (camera_index < STUDIO_CAMERA_COUNT);
            s_nScriptTalkLock = 0;
            TCAMERA_init();
            TWSYS_init();
            MSG_init();
            s_nScriptChangeTime = SCRIPT_SCENE_CHANGE_FRAMES;
            JNI_pushFrame();
            JNI_loadNativeClass();
            JNI_loadClassDB(entry->class_slot, entry->class_image);
            object = newObject(
                loadClass(loadConstString(entry->active, -1), 0));
            thread = JNI_createThread(2, 6, SCRIPT_STAGE_VM_FRAME_WORDS);
            thread->object = object;
            entry->thread = thread;
            STAGE_instance(defaultVM, object);
        }
        return entry;
    }
    /* Otherwise the event scene fades in and starts under a sceneObserver2 task. */
    XTK_setResourceID(2);
    GameLoopState.kind = GAME_MODE_EVENT;
    GameLoopState.flags |= GAME_LOOP_FLAG_EVENT_SCENE;
    s_nScriptEventActive = 1;
    nmlModelSetGlobalPointLightReset();
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    s_nScriptChangeTime = SCRIPT_SCENE_CHANGE_FRAMES;
    s_nScriptTalkLock = 0;
    TCAMERA_init();
    TWSYS_init();
    MSG_init();
    JNI_popFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
    JNI_loadClassDB(entry->class_slot, entry->class_image);
    object = newObject(
        loadClass(loadConstString(entry->active, -1), 0));
    thread = JNI_createThread(2, 6, SCRIPT_STAGE_VM_FRAME_WORDS);
    callback = (int (*)(XglTaskPrefix *))sceneObserver2;
    scheduler = GameLoopState.task_scheduler;
    thread->object = object;
    entry->thread = thread;
    task = (ScriptSceneTask *)xglTaskEntryNext(
        scheduler, callback,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->state_flags = 0;
        task->owner = &GameLoopState;
        task->thread = 0;
    }
    vm = defaultVM;
    task->thread = thread;
    SCENE_instance(vm, object);
    SCENE_start(defaultVM, object, 8, "play");
    return entry;
}

void XTK_setResourceID(int id)
{
    resourceID = id;
}

int XTK_getResourceID(void)
{
    return resourceID;
}

void XTK_setWindowOwner(void *owner)
{
    windowOwner = owner;
}

void *XTK_getWindowOwner(void)
{
    return windowOwner;
}

/* Returns the data value of the file at path in the current script's PDB, or 0 when it is missing. */
int XTK_findFile(const char *path)
{
    ScriptPdbFile *file;

    file = PDB_findFile(scriptDB[currentScriptDB].pdb, path);
    if (file != 0)
        return file->data;
    return 0;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char systemEvtPath[16] = "data\\system.evt";

const char baseEvtPath[16] = "data\\base.evt";

/* JP: ＭＯＮＩＴＯＲ　ＯＮ
 * EN: MONITOR ON */
const char monitorOnText[24] = "\243\315\243\317\243\316\243\311\243\324\243\317\243\322\241\241\243\317\243\316";

/* JP: ＭＯＮＩＴＯＲ　ＯＦＦ
 * EN: MONITOR OFF */
const char monitorOffText[24] = "\243\315\243\317\243\316\243\311\243\324\243\317\243\322\241\241\243\317\243\306\243\306";

/* JP: ＥＸＥＣ  ＯＮ
 * EN: EXEC  ON */
const char execOnText[24] = "\243\305\243\330\243\305\243\303\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＥＸＥＣ  ＯＦＦ
 * EN: EXEC  OFF */
const char execOffText[24] = "\243\305\243\330\243\305\243\303\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＦＩＬＥ  ＯＮ
 * EN: FILE  ON */
const char fileOnText[24] = "\243\306\243\311\243\314\243\305\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＦＩＬＥ  ＯＦＦ
 * EN: FILE  OFF */
const char fileOffText[24] = "\243\306\243\311\243\314\243\305\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＩＮＩＴ  ＯＮ
 * EN: INIT  ON */
const char initOnText[24] = "\243\311\243\316\243\311\243\324\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＩＮＩＴ  ＯＦＦ
 * EN: INIT  OFF */
const char initOffText[24] = "\243\311\243\316\243\311\243\324\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＭＥＭＯＲＹ  ＯＮ
 * EN: MEMORY  ON */
const char memoryOnText[24] = "\243\315\243\305\243\315\243\317\243\322\243\331\241\241\241\241\243\317\243\316";

/* JP: ＭＥＭＯＲＹ  ＯＦＦ
 * EN: MEMORY  OFF */
const char memoryOffText[24] = "\243\315\243\305\243\315\243\317\243\322\243\331\241\241\241\241\243\317\243\306\243\306";

const char memoryUsageFormat[16] = "MEM %3d / %d K\n";

const char sceneStageClassPath[24] = "xeno/Scene:xeno/Stage";

const ScriptPathPrefix mtnpackPathPrefix = { .bytes = "mtnpack\\" };

const char evtMonitorLabel[16] = "evtMonitor";

const char evtFrameFormat[16] = "%04x: %s ";

const char evtActorFormat[24] = "CHR[%02x] %04x:%s";

const char evtFlagsFormat[16] = "flags %08x";

const char evtPositionFormat[32] = "pos(%6.3f, %6.3f, %6.3f)";

const char evtRotationFormat[32] = "rot(%6.3f, %6.3f, %6.3f)";

const char evtUnitFormat[16] = "UNIT[%02x] %04x";

static int lightSave = 0;

char fpkExtension[8] = ".fpk";

char arcExtension[8] = ".arc";

char classMethodFormat[8] = "%s.%s";

char unknownBaseName[8] = "\\\?\?\?\?";

char cmdLabel[8] = "cmd";

char moveLabel[8] = "move";

char separatorLabel[8] = "----";

const char DBCursorModeFcvLabel[8] = "FCV ";

const char DBCursorModeXyLabel[8] = "XY  ";

const char DBCursorModeXzLabel[8] = "XZ  ";

const char DBCursorModeYzLabel[8] = "YZ  ";

const char DBCursorModeFreeLabel[8] = "FREE";
