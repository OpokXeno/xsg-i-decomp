# Glossary

This glossary explains the terms used across the pages of the Xenosaga Episode I architecture documentation. Each entry gives a short definition and links to the place where the term is described in full.

[← Back to the overview](README.md)

[Symbols](#symbols-and-numbers) · [A](#a) · [B](#b) · [C](#c) · [D](#d) · [E](#e) · [F](#f) · [G](#g) · [H](#h) · [I](#i) · [J](#j) · [K](#k) · [L](#l) · [M](#m) · [N](#n) · [O](#o) · [P](#p) · [Q](#q) · [R](#r) · [S](#s) · [T](#t) · [U](#u) · [V](#v) · [W](#w) · [X](#x)

## Symbols and numbers

**`0x00A00000`**: The address of the overlay slot. Every overlay is linked to run there, so only one overlay is in memory at a time. See [Overview: overlays](README.md#2-overlays) and **Overlay**.

**`0x00C00000` and `0x00E00000`**: The start addresses of the two 2 MiB packet buffers, which together end at the 16 MiB mark of main memory. See [Rendering pipeline: packets](architecture/rendering.md#packets) and **Packet**.

**4s matrix**: A matrix format, marked with the suffix `4s`, whose three axes hold a pure rotation while the scale of each axis is kept apart in that axis's w lane. It lets the joint code rotate without changing the scale. `MATRIX_convert4` turns it into an ordinary matrix. See [Math helpers: the 4s matrix format](libraries/math.md#the-4s-matrix-format).

**`.a`**: See **Scene archive**.

**`.cam`**: See **Camera motion pack (CAM)**.

**`.esd`**: An effect file in `data\simajiri\esd\`, one per effect, made of effect chunks: a header chunk, an effect script table and often its own images and models. Most of them are also stored inside the effect packs, and the game loads the pack copy. See [Effect data: the .esd files](formats/effects.md#the-esd-files).

**`.evt`**: See **Script archive**.

**`.fpk`**: See **Motion pack**.

**`.id`**: See **Undulation data (`.id`)**.

**`.jnt`**: See **Skeleton**.

**`.vdm` and `.vds`**: See **VAG stream**.

## A

**A.G.W.S.**: Anti-Gnosis Weapon System, the combat mechs that some characters can board. They are units 17 to 32 in the unit records, and each carries up to three weapons on its mounts. In battle the pilot is a second, linked unit. Six of them can be used in the A.G.W.S. Battle minigame. See [Character growth: weapons and mounts](game/character-growth.md#weapons-and-mounts), [Battle system: A.G.W.S. in battle](game/battle.md#agws-in-battle) and [A.G.W.S. Battle: the six A.G.W.S.](game/minigames/agws-battle.md#the-six-agws).

**A.G.W.S. Battle**: The robot fighting minigame in OV12, started with the Battle Passport. It runs on its own engine, the rg and xrg layers, on top of the main executable's libraries. See [A.G.W.S. Battle](game/minigames/agws-battle.md), [A.G.W.S. Battle engine](game/minigames/agws-battle-engine.md) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**A.G.W.S. Battle stages**: The five backgrounds of A.G.W.S. Battle: `BG_TOWN`, `BG_AIRCAR`, `BG_COL`, `BG_BLOCK` and `BG_VS1`. `_GetStageData` sets the background and the start positions of both robots. See [A.G.W.S. Battle: stages](game/minigames/agws-battle.md#stages).

**Action stock (`ActStock`)**: The turn order of a battle, shown as the Group Turn Window (GTW), the row of portraits at the lower right. `actStockMake` fills it as the wait counts of the units reach 0, and `actStockPick` serves the boost slots first, then up to four units in order. See [Battle system: turn order and the action stock](game/battle.md#turn-order-and-the-action-stock).

**Actor**: The engine record for one skeleton-animated object: a character, an enemy, a face, a weapon, a menu model or a battle unit. All actors live in the fixed table `actor[64]` at `0x0043C1E0`, `0xA70` bytes each, and a slot is free when its `inUseId` is 0. See [Actors: what an actor is](libraries/actors.md#what-an-actor-is).

**Actor flags**: The bits that describe an actor's state, for example `0x2` busy, `0x8` hidden, `0x20` shadow, `0x40` follow ground and `0x1000` culled. See [Actors: actor flags](libraries/actors.md#actor-flags).

**Actor id (`inUseId`)**: See **Model id**.

**Actor sequence (`actSequence`)**: A per-actor interpolation record that moves, rotates, scales or animates an actor over time for a script. See [Scenes, stages and time charts: moving actors](scripting/scenes-and-timecharts.md#moving-actors).

**Actor wait**: A script wait on an actor. A `Chr` native called with `wait` set to true marks the actor busy with flag `0x2`, and the script thread waits until the actor is idle again. See [Scenes, stages and time charts: waiting for an actor](scripting/scenes-and-timecharts.md#waiting-for-an-actor).

**ADC bit**: The bit of an `XYZF2` value that makes the GS store the vertex without drawing the triangle it closes. The VU1 microcode sets it to drop the triangles that leave the clip volume or face the wrong way. See [nml: the vertex loop](libraries/nml.md#the-vertex-loop).

**ADSR**: The volume envelope of an SPU2 voice: attack, decay, sustain and release. A program in the wave bank gives default ADSR words, and sequence commands can change them. See [IOP sound driver: MIDI devices, channels and voices](architecture/iop-sound-driver.md#10-midi-devices-channels-and-voices).

**Alpha group**: The group that collects a model's transparent blocks. Each model opens one alpha group and one opaque group, and the alpha groups are sorted far to near before they are drawn. See [nml: block groups and sorting](libraries/nml.md#block-groups-and-sorting).

**Alter**: A ribbon trail of the effect system. It samples two end points over time and draws them as a Hermite strip. Up to 16 alters exist. See [sef: alters (trails)](libraries/sef.md#alters-trails).

**Alter event**: An 18-word row of an effect script table that describes one trail (an alter). See [Effect data: alter events](formats/effects.md#alter-events).

**Alter mode**: The texture mode of a ribbon trail of the effect system: 1 plain, 2 scrolling, 3 and 4 with a phase per sample. See [Effects: alters (trails)](libraries/sef.md#alters-trails).

**Ambient bank**: Sound effect bank 3 of the field, loaded from the second name of the scene command `loadse` (an `ENV_*` SED and SWD pair). See [The field game: field sound banks](game/field.md#field-sound-banks).

**Ambient dimming**: The fade of the battle light and the map colour to half over 20 frames while a big battle effect plays. See [sef: ambient dimming](libraries/sef.md#ambient-dimming).

**ANM**: See **Motion clock**.

**AP**: Action points in battle. A unit gains 4 per turn up to a cap of 6, and techniques and items cost AP. See [Battle system: AP](game/battle.md#ap).

**`archeader`**: The static 64-word copy of a scene archive's end-offset table. Archives with more than 63 entries overflow it. See [Scripts and scene archives: header overflow](formats/scripts-and-archives.md#header-overflow).

**Archive directory**: The list of packed records in the first sectors of `XENOSAGA.n0`. Each record gives a path, a sector offset, a stored size and an unpacked size, and the records form a depth-first walk of a directory tree whose root is `data\`. See [Files and resources: the archive directory](architecture/files-and-resources.md#the-archive-directory).

**Arms**: The weapon a character holds, attached to hand joint `0x42` through `JC_copyChain`. See [Actors: relations: face, arms and accessories](libraries/actors.md#relations-face-arms-and-accessories).

**ARX**: The compression format of packed files, marked by the signature `"ARX"` (`0x00585241`). It replaces each of 30 frequent 32-bit words with a short code and stores every other word as a literal. `xglArxExtract` expands a packed file in place after it has been read. See [Files and resources: packed files (ARX)](architecture/files-and-resources.md#packed-files-arx) and [xgl: packed files and ARX](libraries/xgl.md#packed-files-and-arx).

**ATB window**: The battle HUD panel that draws the Event Slot ring and the radar map of the units. See [Battle system: battle HUD and map](game/battle.md#battle-hud-and-map).

**Attachment (`ActorAttachment`)**: A record that ties a child actor's skeleton to a joint of its parent actor. `ACT_setParent` creates it, and a child can have up to eight. See [Actors: ACT_setParent and attachments](libraries/actors.md#act_setparent-and-attachments).

**Attribute slot**: The word that a format-3 motion pack keeps for each entry. Its value selects the matching range record of a curve set that several entries share. See [Skeletons and motion: the entry and slot tables](formats/skeletons-and-motion.md#format-3-the-entry-and-slot-tables).

**Attribute table (`.id`)**: The shared table of 8-byte attributes in an undulation file. Each triangle indexes it, and its codes mark walls, camera, sound and light behaviour. See [Maps and field data: the attribute table](formats/maps-and-field-data.md#the-attribute-table).

## B

**B-spline weight table**: The file `spline.dat`, a table of fixed-point cubic B-spline weights that the field enemies use to follow their routes. See [Maps and field data: the B-spline weight table](formats/maps-and-field-data.md#the-b-spline-weight-table).

**Back buffer**: A frame captured into the flip buffer and drawn over the following frames, either to freeze the picture or to cross-fade from the old picture into the new scene. Its state is `s_inBackBuffer[10]`. See [nml: back buffer](libraries/nml.md#back-buffer).

**Back clear**: A way of drawing a model group with the `BACK_CLEAR` status. The group is drawn into the cleared flip buffer at full opacity and then laid over the frame as one flat layer, so the whole model fades without its parts showing through each other. See [nml: filters](libraries/nml.md#filters).

**Back row**: Battle line 2. A party member in the back row takes 20 percent less damage and can only use techniques of range class 2. See [Battle system: critical hits, elements and rows](game/battle.md#critical-hits-elements-and-rows).

**Background draw type**: The way a scene without a map, such as a space background, draws its background. Types 1 and 2 replace the screen clear with their own drawing and skip the map pass. Scripts choose the type with `Stage.setCFBG`. See [The field game: background draw types](game/field.md#background-draw-types).

**Batch buffer**: One of the two halves of the lower VU1 data memory, at `0x000` and `0x16E`, that VIF1 fills with alternate batches of a model block. Each microcode packet sets them up with `BASE` and `OFFSET`, and the program finds the current one with `XTOP`. See [nml: how a program runs](libraries/nml.md#how-a-program-runs).

**`batdat.bin`**: The file of battle data tables, loaded at `0x00426F40`. It has 18 sections, among them the unit records, the technique records and the gain records. See [Battle system: battle data and files](game/battle.md#battle-data-and-files) and [Game data tables: battle data](formats/game-tables.md#battle-data-batdatbin).

**Battle configuration (`debugBattleConfig`)**: The OV01 developer screen for setting up test battles, with the invincibility (`muteki`) and one-hit (`ichigeki`) cheats. See [Debug tools: YamamotoTest and the battle configuration](game/debug-tools.md#yamamototest-and-the-battle-configuration).

**Battle font**: The PSMT8 grid of 10 by 18 pixel glyphs in `stat.xtx` that the OV01 battle menus draw numbers and labels with. See [Text, fonts and U.M.N. data: battle font](formats/text-and-fonts.md#battle-font).

**Battle model pack**: A `pc` or `wpn` file in `data\yamamoto` that holds the LEX model, the XTX texture set and the JNT skeleton of a battle character or weapon behind an offset header. See [Game data tables: battle model packs](formats/game-tables.md#battle-model-packs-pc-and-wpn).

**Battle motion file**: A file `data\yamamoto\mot\*.bin`, a container of two parts: a format-3 motion pack and an MTD trigger table. See [Skeletons and motion: battle motion files](formats/skeletons-and-motion.md#battle-motion-files).

**Battle object**: An OV01 task from `obj.c` with the callbacks `objExec` and `objExec2`. Battle units, the HUD, the cursors, the fades and the encounter transition are battle objects. See [Battle effects: the battle object system](game/battle-effects.md#the-battle-object-system).

**Battle Passport**: The event item that starts A.G.W.S. Battle, whose entry is `RobotGameMain` at `0x00A01EA8` in OV12. See [A.G.W.S. Battle: getting in and out](game/minigames/agws-battle.md#getting-in-and-out) and **Passport**.

**Battle sound container**: A `.bin` file in `data\yamamoto\snd\sed\` that packs one to five SED and SWD pairs behind a header of counts, sizes and offsets. See [Sound files: battle sound containers](formats/sound.md#battle-sound-containers).

**Battle stage file**: A file `data\yamamoto\map\mapNNN.bin` with the model, the textures and the camera scripts of one battle arena. See [Maps and field data: battle stage files](formats/maps-and-field-data.md#battle-stage-files).

**Battle unit (`BattleUnit`)**: One combatant in a battle: a party member, an enemy or an A.G.W.S. The unit table holds eight units, with slots 0 to 2 for the party and slots 3 to 7 for the enemies. See [Battle system: battle units](game/battle.md#battle-units).

**`battleProc`**: The main function of a battle in OV01. It does not return until the battle is over, so it replaces the field loop for the whole battle. The main executable knows it only by its address (`func_A22668`). See [Battle system: the battle loop](game/battle.md#the-battle-loop) and [The field game: battles started from the field](game/field.md#battles-started-from-the-field).

**Block**: One drawable piece of an nml model with a single material, usually one mesh of one part. Blocks are the unit that nml sorts, clips, hides and draws. See [nml: blocks](libraries/nml.md#blocks) and [Models and textures: blocks](formats/models-and-textures.md#blocks).

**Block sphere table**: The bounding spheres of the blocks of a LEX model, one per block, at header offset `+0xA0`. Only 13 maps have one. See [Models and textures: the block sphere table](formats/models-and-textures.md#the-block-sphere-table).

**Boost**: Letting a party member act at once, ahead of the turn order. Every 50 points of a member's boost gauge give one boost, and using it puts the unit into a boost slot of the action stock. See [Battle system: boost](game/battle.md#boost).

**Bunnie**: See **Procurator**.

## C

**`CalcUnitParam`**: The record that holds the numbers of a battle unit, a battle copy of the character record. See [Battle system: battle units](game/battle.md#battle-units).

**`CallMethod`**: The way C code calls a named method of the current Stage script. The call runs on a free event VM, and the task `funcObserver` follows it until it ends. See [Events and cutscenes: calling Stage methods from C](scripting/events.md#calling-stage-methods-from-c).

**Camera floor**: A mesh in the second list of an undulation file. The floor under the player selects the field camera. See [Maps and field data: the direct and second lists](formats/maps-and-field-data.md#the-direct-and-second-lists).

**Camera motion pack (CAM)**: A camera curve file (`.cam`) with the format tag `"CAM\0"`, which the source compares as the multi-character constant `'MAC'`. It holds eight curves: position x, y and z, look-at x, y and z, roll and field of view. The 307 files on the disc are members of the resource volumes in `data\mtnpack`. The camera code reads them with `TCAMERA_transMPack` and `TCAMERA_transMPack2`. See [Scenes, stages and time charts: event cameras](scripting/scenes-and-timecharts.md#event-cameras) and [Skeletons and motion: camera motion packs](formats/skeletons-and-motion.md#camera-motion-packs-cam).

**Camera table**: The part of an effect script table that lists four weighted camera candidates and one more for bosses. See [Effect data: camera tables](formats/effects.md#camera-tables).

**Camera travel state**: The way an xgl studio camera moves its viewpoint (`xglCameraTravelProc`): a manual debug mode driven by the second controller, or a chase mode that keeps the camera near a target. See [xgl: the camera update](libraries/xgl.md#the-camera-update).

**Camera zone**: A floor area whose attribute has bits 27 to 29 equal to 4. When the player walks into it, it selects one of the map's camera definitions. See [The field game: the field camera](game/field.md#the-field-camera).

**Caption file (`.cap`)**: A subtitle file that `GameMpeg2Play` looks for beside each movie. No such file exists on the disc. See [Movies, images and icons: caption files](formats/movies-images-and-icons.md#27-caption-files).

**Card command**: The effect of one Xeno Card card, run through `CardPlayCommandPlay`. See [Xeno Card: card effects and the command system](game/minigames/xeno-card.md#8-card-effects-and-the-command-system).

**Card game modes**: The three ways to play Xeno Card: Exhibition, Tournament and 2P Battle. See [Xeno Card: game modes and prizes](game/minigames/xeno-card.md#10-game-modes-and-prizes).

**Card phase**: One step of a Xeno Card turn: Draw, Move, Event, Set, Block, Battle or Adjust. A turn runs through 14 phases. See [Xeno Card: the turn and its phases](game/minigames/xeno-card.md#the-turn-and-its-phases).

**Card zones**: The areas of the Xeno Card play mat: the Standby Area, the Battlefield, the Situation Area, the Junk Pile and the Lost Pile. See [Xeno Card: the play mat](game/minigames/xeno-card.md#6-the-play-mat).

**`card.dat` (`CardDefinition`)**: The Xeno Card card file, with the version text "CardData1.10" and records of `0x24` bytes for 150 card ids. See [Xeno Card: card.dat and CardDefinition](game/minigames/xeno-card.md#carddat-and-carddefinition) and [Minigame data files: card.dat](formats/minigame-data.md#carddat).

**`carddata.new`**: The disc file that holds the two memory card icon files, `icon.sys` and `xenosaga.ico`, used as the template for every new save folder. It is not Xeno Card data. See [Save data: the icon files](architecture/save-data.md#the-icon-files) and [Movies, images and icons: carddata.new](formats/movies-images-and-icons.md#6-carddatanew-the-memory-card-icon-template).

**`CardGameWork`**: The single work structure that holds the state of the card game. See [Xeno Card: the card game work area](game/minigames/xeno-card.md#3-the-card-game-work-area).

**`CARDGRAP.BIN`**: The Xeno Card file with the 148 card pictures, one XTX texture set of `0xBAB0` bytes per card id. See [Minigame data files: the .bin textures](formats/minigame-data.md#the-bin-textures).

**`CardSaveData`**: The Xeno Card block of the save data at `SaveData+0x15090`. See [Xeno Card: the save area](game/minigames/xeno-card.md#the-save-area).

**`CASINO.res`**: The tuning file of the Network Casino. Its `debug_mode` field grants money, coins and flags at start-up. See [Network Casino: the resource file](game/minigames/network-casino.md#the-resource-file-casinores), [Debug tools: casino debug start-up and test mode](game/debug-tools.md#casino-debug-start-up-and-test-mode) and [Minigame data files: CASINO.res](formats/minigame-data.md#casinores).

**`CasinoSaveWork`**: The Network Casino block of the save data at `SaveData+0x15190`, which holds the coins (at most 9,999,999) and the prizes. See [Network Casino: save data](game/minigames/network-casino.md#save-data).

**CD thread**: System thread 1, with the entry point `xglCdControlThread`. On each turn it advances the current file read, refills the stream ring buffers from disc and handles power-off. See [Overview: the four system threads](README.md#5-the-four-system-threads).

**CF**: The field side of the game. Scene ids below `0x10000000` are field scenes with scripts named `cfNNNN`, as opposed to event scenes (`evNNNN`) and battles. The code never spells the abbreviation out. See [The field game: what "CF" means](game/field.md#what-cf-means).

**CF effect**: A visual effect created on the field with `sefCreateEffectCf`, without an effect script. See [sef: on the field](libraries/sef.md#on-the-field).

**CF event**: The game mode `GameModeCfEvent`, in which the field runs under a script for a full cutscene. See [The field game: game modes](game/field.md#game-modes) and **Event frame**.

**CF main**: The game mode `GameModeCfMain` of free field control, where the player moves. Talks, small scripted moments and battles against field enemies also run inside it. See [The field game: game modes](game/field.md#game-modes) and [Overview: the field game loop](README.md#the-field-game-loop).

**CF window**: The field conversation window of TWSYS, 460 by 80 pixels with three lines of text, typed out at two characters per frame. See [User interface and windows: text windows (TWIN)](libraries/ui-windows.md#text-windows-twin).

**`CfCameraDefine`**: The 33 camera definitions of a map that drive the data-driven field camera. See [The field game: the field camera](game/field.md#the-field-camera).

**`cfEncount`**: The trigger flags of the encounter record, which record how the enemy was touched: `0x01` for an electric stun, `0x02` for a stunned enemy and `0x04` for a seal. In battle `calcCfEncount` turns them into a first strike for the party. The electric flag also sets the Event Slot to 1, the stun flag gives each party member an extra boost, and the seal flag has no effect there. See [Battle system: encounter data](game/battle.md#encounter-data) and [Field enemies: trap damage](game/field-enemies.md#trap-damage).

**`cfEvent`**: The event flags of the encounter record, copied from the enemy's parameters. They configure the battle, for example as a boss battle, a battle without escape or without a result screen, or a battle the party may lose without a game over (`0x40`). See [Battle system: encounter data](game/battle.md#encounter-data) and [Field enemies: the encounter record](game/field-enemies.md#the-encounter-record).

**Character stand-in (`dummy`)**: The model `data\char\dummy`, a copy of Shion, loaded for model id 0 when a script's model key matches nothing. See [Files and resources: model keys and model ids](architecture/files-and-resources.md#model-keys-and-model-ids).

**`Check_Actor`**: The body contact test in `check.c`, which checks whether the body radii of two actors overlap. See [Actors: body contact (Check_Actor)](libraries/actors.md#body-contact-check_actor).

**`Chr.start` mode**: The selector that a script passes to `Chr.start(mode, method)` to choose an actor's update hook: 0 default, 1 a script method with a sequence, 2 player pad control, 4 a sequence alone and 5 PlayControl time (`ACT_updateMPack`). See [Scenes, stages and time charts: actor update modes](scripting/scenes-and-timecharts.md#actor-update-modes) and [Actors: update hooks](libraries/actors.md#update-hooks).

**Class database (`classDB`)**: The table of eight slots of loaded script archives that `findClass` searches for a class file. Slot 0 holds `system.evt`, slot 1 `base.evt`, and slots 2 and 3 the double-buffered archives of the current scene. See [Java VM: the class database](scripting/java-vm.md#the-class-database).

**`classPointer`**: The first word of every Java object. It points to the class's dispatch table (`DispatchTable`), whose first word points back to the class. See [Java VM: linking and initialization](scripting/java-vm.md#linking-and-initialization).

**`closeState`**: The value of the last `/[waitkey(n)]` tag that the player confirmed in a window. A script reads it with `getSignal()`, which returns 255 once the window has closed. See [User interface and windows: waiting for a window](libraries/ui-windows.md#waiting-for-a-window).

**CNS**: A mode of an event camera channel (`CAMERA_MODE_CONSTANT`, `0x14`) that follows the position of an actor or unit plus an offset. See [Scenes, stages and time charts: event cameras](scripting/scenes-and-timecharts.md#event-cameras).

**Column-major matrix layout**: The way a `Matrix4` is stored. Its four groups of four floats are mathematically columns, as in OpenGL: the X, Y and Z axes, then the translation. A vector is transformed as `v' = M v`. See [Math helpers: matrix layout](libraries/math.md#matrix-layout).

**Command list**: A list of 6-word records in an effect script table. Each record starts one line of the effect. See [Effect data: command lists](formats/effects.md#command-lists).

**Compulsory download**: One of 13 U.M.N. mails whose attachment is taken automatically when the mail is read. See [U.M.N.: the mail screen](game/umn.md#the-mail-screen).

**Constant string**: A string stored once in `constStringTable` (512 buckets), so the VM can compare names by pointer. See [Java VM: constant strings](scripting/java-vm.md#constant-strings).

**Control code**: A byte below 32 inside a text string that carries a command, with a fixed number of parameter bytes, such as a position or a colour. See [xgl: control codes](libraries/xgl.md#control-codes) and [Text, fonts and U.M.N. data: font control codes](formats/text-and-fonts.md#font-control-codes).

**Control thread**: System thread 3, whose body is `ControlEntry`. It runs the game itself: the title screen, the field, the menus, the battles and the minigames. It has a 64 KiB stack, while the other three system threads have 4 KiB each. See [Overview: the four system threads](README.md#5-the-four-system-threads) and **`ControlEntry`**.

**`ControlEntry`**: The body of the control thread. It runs the device checks and the logo movies once, then loops: it loads OV02 when needed, calls `Title()`, loads the overlay named by the returned value and calls the returned function. See [Boot and title screen: the control loop and the title return value](game/title-and-boot.md#3-the-control-loop-and-the-title-return-value) and [Overview: from the title screen to the game](README.md#6-from-the-title-screen-to-the-game).

**`CpuDeckRecord`**: The `0x80`-byte record of one computer opponent's deck in `deckdata.dat`: a name, the minimum gene values, 40 card ids and 40 class bytes. See [Minigame data files: deckdata.dat](formats/minigame-data.md#deckdatadat).

**crt0**: The start-up code of the executable (`_start`, `_root` and `_exit`). The EE kernel starts the program at `_start`, which appears to set up the kernel services, run `main` and finally exit. See [Runtime libraries: start-up](architecture/runtime-libraries.md#start-up).

**Culling map**: The table `s_aCullingMap` of hand-placed occluder boxes built into the executable and keyed by the map model. A model hidden behind such a box is left out of the draw passes. A separate view test checks each model against the camera windows. See [Rendering pipeline: culling](architecture/rendering.md#culling).

**`CUR_MATRIX_` functions**: Math functions that work on a second VU0 current matrix kept in `vf27` to `vf30`, with no stack and no microprogram routine. The joint code builds joint matrices with them. See [Math helpers: CUR_MATRIX_ functions](libraries/math.md#cur_matrix_-functions).

## D

**Data archive (`XENOSAGA.nn`)**: One of the large archive files that hold the game data on disc. There are three groups, 00, 10 and 20. Each group is one logical archive split into pieces of at most 1 GiB, and its first piece `XENOSAGA.n0` holds only the archive directory. The file library `xgl_cd` keeps four archive slots and finds files by name. See [Files and resources: the disc](architecture/files-and-resources.md#the-disc), [xgl: archives and lookup](libraries/xgl.md#archives-and-lookup) and [Overview: programs on the disc](README.md#1-programs-on-the-disc).

**Data boxes**: See **Inventory boxes**.

**Debug print gate**: The reason debug text never appears. `xglFontSetFlags` always clears `FONT_STATE_DEBUG_PRINT`, so `xglFontDebugPrintf` never draws. See [Debug tools: why the debug code is invisible](game/debug-tools.md#1-why-the-debug-code-is-invisible).

**DECI2**: The debug link of a development kit to the host PC. The output of `printf` reaches it through `sceTtyWrite`, so on a retail console, where no debugger is connected, that output probably goes nowhere. See [Runtime libraries: printf and the debug output](architecture/runtime-libraries.md#printf-and-the-debug-output).

**Defocus layer**: One of the 16 full-screen filter layers (types 1 to 11) that `DefocusMain` draws. The special timeline of an effect drives them. See [sef: defocus layers](libraries/sef.md#defocus-layers).

**Descriptor setting**: The third word of an effect slot descriptor. A value of 0 or more links to another descriptor, whose category becomes the loop target. The values -0x100, -0x200 and -0x201 have special meanings. See [Effects: effect slots](libraries/sef.md#effect-slots).

**Design sketch parts**: The textures `NN_Y1`, `NN_Y2` or `NN_W1` to `NN_W4` that the casino data viewer shows. See [Minigame data files: the design sketches](formats/minigame-data.md#the-design-sketches).

**Developer test programs**: Functions named after their programmers, `<Surname>Test` (for example for Endou, Yajima and Karakama), that test parts of the game. See [Debug tools: the developer test programs](game/debug-tools.md#4-the-developer-test-programs).

**Direct data**: GS packets that the caller builds itself and queues with `nmlModelDirectSend`. The model flush inserts them at a fixed point between its passes, so they are drawn in the right place relative to the models. See [nml: direct data](libraries/nml.md#direct-data).

**Draw environment (`drawEnv`)**: The render state of one actor: fog, three point lights, translucency, the render level and the alpha of up to four parts. It lets one actor be drawn differently from the rest of the scene. See [Actors: the draw environment](libraries/actors.md#the-draw-environment).

**Draw hook**: The `Actor.draw` callback that sets render state before an actor is drawn. Scripts choose filters such as `ACT_filterGuno` and `ACT_filterStealth` with `Chr.setFilter`. See [Actors: draw hooks and filters](libraries/actors.md#draw-hooks-and-filters).

**Drill phase**: A state of the drill in the Driller Game: `POWER_OFF`, `STANDBY`, `Z_MOVE`, `Z_STOP`, `X_MOVE`, `CRASH` or `RETURN`. See [Driller Game: the drill's phases](game/minigames/driller-game.md#3-the-drills-phases).

**Driller Game**: The drilling minigame, number 15. It is not an overlay but a place on the field in the main executable (event 160), where the drill is a map unit driven by that map's script. `Runtime.evsExit` brings the player back to the stored return point. See [Driller Game](game/minigames/driller-game.md) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

## E

**E-widget**: A `0x28`-byte drawing node in the table `ewComponent[64]`, the drawing part of the window toolkit. The e-widgets eWindow, eCursol, eMessage, eNumber, eRibbon and eTagFont build the menu and battle windows. See [User interface and windows: EW widgets](libraries/ui-windows.md#ew-widgets).

**E-widget mode**: The one-byte mode of an e-widget, with the phases INIT, SHOW, OPEN, RUN, HIDE and END. Each phase has a `START` value that sets it up and a `STEP` value that runs every frame after that. See [User interface and windows: mode convention](libraries/ui-windows.md#mode-convention).

**E.Pts**: Ether points, one of the three point pools of a character, at most 9999. They pay for evolving and transferring Ether. See [Character growth: points and experience](game/character-growth.md#points-and-experience).

**`eBattleWin`**: Four ready-made windows, `BW` to `BW4`, for the battle code and the U.M.N. database. Each has an `Open`, a `Main` and a `Close` function. See [User interface and windows: battle windows (eBattleWin)](libraries/ui-windows.md#battle-windows-ebattlewin).

**EE**: The Emotion Engine, the main MIPS processor of the PlayStation 2. All game code runs on it. It has two vector units, VU0 and VU1, and sends graphics to the GS. See [Overview: programs on the disc](README.md#1-programs-on-the-disc).

**EE kernel API**: The functions of the EE kernel in the console's BIOS, such as `CreateThread`, `WakeupThread` and `FlushCache`. The executable reaches them through 138 `syscall` stubs, which have no `sce` prefix. See [Runtime libraries: the EE kernel API](architecture/runtime-libraries.md#the-ee-kernel-api).

**Effect bank**: One of the 16 EE sound slots that hold a pair of files with the same name: an SWD file with the wave samples and an SED file with the effect programs. A sound ID selects the bank in its high half. See [Audio: sound banks](architecture/audio.md#5-sound-banks).

**Effect chunk**: The unit of the `.esd` and `.esp` files: a 64-byte header with a kind, a name and a size, followed by its data. The kinds hold the file header, scripts, images, colour tables, models, texture sets and whole embedded `.esd` files. See [Effect data: the chunk container](formats/effects.md#the-chunk-container).

**Effect note**: A 3-byte note of a SED track: a key and a 16-bit little-endian duration that also sets the wait before the next event. See [Sound files: effect tracks](formats/sound.md#effect-tracks).

**Effect number**: The id of a visual effect, from 1 to `0xBFF`. Its range selects the data file and the resource category. See [sef: effect numbers](libraries/sef.md#effect-numbers) and [Effect data: effect numbers and files](formats/effects.md#effect-numbers-and-files).

**Effect pack (`.esp`)**: A file in `data\simajiri\esp\` that bundles the images, colour tables, models and embedded `.esd` files of one character, enemy group or weapon. There are 8 character packs, 112 enemy and boss packs and 47 weapon packs. See [Effect data: the .esp effect packs](formats/effects.md#the-esp-effect-packs).

**Effect scheduler (`SchedulerState`)**: One running instance of a visual effect, `0xAB0` bytes, from a pool of 128. It is the peer of a Java `xeno.Effect`, and effect scripts create schedulers with their EFFECT commands. See [sef: schedulers](libraries/sef.md#schedulers).

**Effect script**: The small bytecode language of the effect system, called sc after its interpreter `src/main/sc_get.c`. It is unrelated to the Java scripts. Each effect number can have a script of 16-bit commands that picks camera shots, waits for hits, starts effect schedulers with its EFFECT commands, plays movies and fires missiles. It has 73 opcodes and 16 registers, and runs as 16 records of 8 tasks. See [sef: effect scripts](libraries/sef.md#effect-scripts), [Events and cutscenes: effect scripts](scripting/events.md#effect-scripts) and [Effect data: the effect script table](formats/effects.md#the-effect-script-table).

**Effect script table**: The graph of 16-bit words inside a script chunk of an effect file. It holds the bytecode, the scheduler records, the command lists, the line headers and the key lists of one effect. See [Effect data: the effect script table](formats/effects.md#the-effect-script-table).

**Effect sequence**: The single SSD sequence that plays every sound effect. It has the id `0xF000` and 44 tracks, and channel n of its MIDI device is bound to voice n. See [IOP sound driver: the effect sequence](architecture/iop-sound-driver.md#the-effect-sequence).

**Effect slot (`EffectData`)**: One of the 32 tracks of an effect scheduler. It holds a command list that starts lines, and it carries the hit, sound and finish signal bits. See [sef: effect slots](libraries/sef.md#effect-slots).

**Effect strip**: A run of XTX frames of the same size in a Xeno Card `effect\*.bin` file. Frame n starts at the base offset plus n times the frame size. See [Minigame data files: the .bin textures](formats/minigame-data.md#the-bin-textures).

**Effect viewer (`SimajiriTest`)**: A developer test program that browses sef effects and reads its files from `host0:/home/xeno/work/`. No caller of it remains. See [Debug tools: SimajiriTest, the effect viewer](game/debug-tools.md#simajiritest-the-effect-viewer).

**Embedded `.esd`**: A kind-10 chunk of an effect pack that holds a whole `.esd` file. 68 of the 352 embedded copies differ from the loose `.esd` file of the same name, and the game loads the pack copy. See [Effect data: embedded .esd chunks](formats/effects.md#embedded-esd-chunks).

**Embedded OV02**: The copy of OV02 that is linked into the main executable as a third LOAD segment at `0x00A00000`. The console loads it together with the executable, and the file library starts with overlay 2 marked as loaded, so the boot logo and the first title screen need no disc read. Later visits to the title screen load `OV02.OVL` from disc. See [Boot and title screen: OV02 is already in memory at boot](game/title-and-boot.md#2-ov02-is-already-in-memory-at-boot) and [Overview: overlays](README.md#2-overlays).

**eMessage**: A paged text widget with typewriter output, icons and a page cursor. See [User interface and windows: eMessage](libraries/ui-windows.md#emessage).

**Encoded item id**: An item id with the item category in the high halfword and the id within the category in the low halfword. See [Main menu: encoded item ids and categories](game/main-menu.md#encoded-item-ids-and-categories).

**Encounter record**: The 16-byte record `encount` at `GameLoopState+0x29F90` that carries a battle request from the field into OV01. It holds the monster set, the battle map, the encounter side, `cfEncount`, `cfEvent` and the enemy that started the battle. See [Battle system: encounter data](game/battle.md#encounter-data) and [Field enemies: the encounter record](game/field-enemies.md#the-encounter-record).

**Encounter side**: A value from 0 to 4 computed from the facing of the player and of the enemy at contact. Sides 1 and 2 probably favour the enemy and sides 3 and 4 the party. See [Field enemies: the encounter side](game/field-enemies.md#the-encounter-side).

**Encounter transition**: The opening effect of a battle that breaks a snapshot of the field into a grid of 64 by 64 quads. See [Battle effects: other battle visuals](game/battle-effects.md#other-battle-visuals).

**`endPrint`**: The layer that draws the primitives the e-widgets describe. It keeps a deferred queue of up to 512 primitives (`PrintFunc[512]`) and runs it later on the render thread, inside one font callback. See [User interface and windows: the endPrint layer](libraries/ui-windows.md#the-endprint-layer).

**Enemy action**: The AI state of a field enemy in `EnemyWorkEntry.action` (`ENEMY_ACT_*`), started by `Enemy_ActionReady`. See [Field enemies: actions](game/field-enemies.md#actions).

**Enemy command**: A queued script command (types 0 to 19) in the argument area of an enemy actor, run by `Start_Enemy_Command`. The freeze states and the ladder set-up are such commands. See [Field enemies: enemy commands](game/field-enemies.md#enemy-commands).

**Enemy preset**: A `0x50`-byte record of behaviour values from `data\matumoto\*.dat`, loaded by the resource command `loadene`. See [Field enemies: loading enemy data](game/field-enemies.md#loading-enemy-data) and [Maps and field data: field enemy presets](formats/maps-and-field-data.md#field-enemy-presets).

**`Enepc`**: The Java class `xeno.Enepc`, a subclass of `xeno.Chr` for enemies and NPCs on the field, whose actor `ACT_updateEnemy` drives. The name probably stands for "enemy and NPC". See [Field enemies: enemies and NPCs](game/field-enemies.md#enemies-and-npcs).

**`enepc[16]`**: The 16 enemy work records (`EnemyWorkEntry`, `0x38B0` bytes each), one per actor number. They hold the AI state, the look target, the encounter lock and the footsteps. See [Field enemies: the enemy work record](game/field-enemies.md#the-enemy-work-record) and [Actors: what an actor is](libraries/actors.md#what-an-actor-is).

**Entered attribute**: A ground attribute id under the player. Walking onto an area with such an attribute calls the Stage method `entered(int)` with it. See [Events and cutscenes: entered attributes](scripting/events.md#entered-attributes).

**Entry phase**: One step of the battle sequence `battleSeq`, from `entryPhase00` to `entryPhase100`. See [Battle system: entry phases](game/battle.md#entry-phases).

**eNumber**: A widget that draws a number from digit sprites. See [User interface and windows: eNumber](libraries/ui-windows.md#enumber).

**Environment sequence**: A short `ENV_<map>.SMD` music sequence, loaded by `loadsmd`, that loops one long note of the map's environment bank. 103 of the 115 SMD files are environment sequences. See [Audio: music sequences](architecture/audio.md#8-music-sequences).

**EOB callbacks**: The Stage methods `EOB_Always`, `EOB` and `EOB_Escape` that the field calls after a battle against a field enemy, so the script hears about the result. See [Events and cutscenes: calling Stage methods from C](scripting/events.md#calling-stage-methods-from-c).

**eRibbon and eTagFont**: A shaded ribbon widget and a widget for text made of sprite letters. See [User interface and windows: sprites, ribbons, lines and tag fonts](libraries/ui-windows.md#sprites-ribbons-lines-and-tag-fonts).

**Essence**: A read-only template from an A.G.W.S. Battle database, such as `RgWeaponEssence` or `RgShotEssence`. It carries a create function, and the engine builds live objects from it. See [A.G.W.S. Battle engine: object style in C](game/minigames/agws-battle-engine.md#object-style-in-c).

**Ether owner**: The character an ether belongs to, decided only by the ether's id (`MenuEtherWhoCheck`). Only the owner can evolve or transfer an ether. See [Character growth: ether data](game/character-growth.md#ether-data).

**Ether tree**: The display of how ethers evolve, drawn by `ether_tree.c` from the file `etree.bin`, a forest of nine trees with three child ethers per ether. See [Character growth: ether tree](game/character-growth.md#ether-tree) and [Game data tables: the Ether tree](formats/game-tables.md#ether-tree-etreebin).

**EUC-JP**: The Japanese character encoding of the game text. ASCII text is converted to it by a table in the main executable. See [Text, fonts and U.M.N. data: character encoding](formats/text-and-fonts.md#character-encoding).

**Event camera (`CameraWork`)**: One of the eight records `tcamera[8]` that drive the studio cameras for scripts. Each has four channels (position, look-at, roll and field of view), and each channel has a mode in the range `0x10` to `0x15`, such as a spline or CNS. The Java `xeno.Camera` object is the record itself. See [Scenes, stages and time charts: event cameras](scripting/scenes-and-timecharts.md#event-cameras).

**Event flags**: See **Flag area**.

**Event frame**: The order of work in one CF event frame: `TWSYS_update`, `JTHREAD_cntl`, `PLAY_ctrl`, `TCAMERA_update`, `ACT_update` and `MAP_updateUnit`. See [Scenes, stages and time charts: the event frame](scripting/scenes-and-timecharts.md#the-event-frame).

**Event ID**: The id in `GameLoopState.current_event_id` that names the current place or cutscene. Field places (cf) are plain numbers, and cutscenes (ev) have bit 28 (`0x10000000`) set. See [Events and cutscenes: event IDs and the jump natives](scripting/events.md#event-ids-and-the-jump-natives) and **CF**.

**Event level**: A number from 0 to 2 that limits the CPU robots and weapons of A.G.W.S. Battle and unlocks hard mode. It is 2 when story flag `0x12D` is set, 1 when flag `0x65` is set and 0 otherwise. See [A.G.W.S. Battle: the event level](game/minigames/agws-battle.md#the-event-level).

**Event locator**: A locator pair of group 7 in the undulation data of a map that marks an event trigger area. See **Locator** and [Maps and field data: event locators](formats/maps-and-field-data.md#event-locators).

**Event skip**: Skipping a cutscene. START opens the event pause (`PauseSkipCheck`), and TRIANGLE there skips the scene. See [Events and cutscenes: the end of a cutscene](scripting/events.md#the-end-of-a-cutscene).

**Event Slot**: The battle-wide bonus shown in a ring of the HUD, called the event timer in the code (`eventTimer`, values 0 to 3). It is set at random when the HUD is created and changes after every turn. See [Battle system: Event Slot](game/battle.md#event-slot).

**Event symbol**: See **Symbol**.

**Event text file**: A U.M.N. dialogue file `data\umn\eventNN.txt` made of tagged speech blocks with the tags `<speech>`, `<char>`, `<msg>`, `<mot>` and `<memo>`. See [U.M.N.: event text files](game/umn.md#6-event-text-files) and [Text, fonts and U.M.N. data: event texts](formats/text-and-fonts.md#event-texts).

**Event timer**: A delayed method call queued by `Runtime.setEventTimer(name, frames)`. The task counts down once per frame, pauses while the game is paused and then calls the named Stage method, so a script can run a method later without blocking its own thread. See [Events and cutscenes: event timer](scripting/events.md#event-timer).

**Event VM**: See **Fixed script threads**.

**Evolve**: The Ether command that teaches the owner a child ether from the ether tree, paid with the full E.Pts cost of the parent ether. See [Character growth: commands](game/character-growth.md#commands).

**EVS**: The Environmental Simulator, a U.M.N. service that replays areas the party has already visited. Up to seven simulations unlock with story progress, and one can start only while the player stands on an EVS symbol. See [U.M.N.: EVS, the Environmental Simulator](game/umn.md#9-evs-the-environmental-simulator) and [Maps and units: symbols](game/map-and-units.md#symbols-save-shop-evs-and-return).

**eWindow and eCursol**: A plain window e-widget and an animated cursor e-widget. See [User interface and windows: eWindow and eCursol](libraries/ui-windows.md#ewindow-and-ecursol).

**EXM (extra motion)**: The simulation of hair, coats and skirts, with gravity, wind, sway, ground contact and collision. See [Animation: extra motion (EXM)](libraries/animation.md#extra-motion-exm-hair-cloth-and-wind).

## F

**Face actor**: A separate actor, with the id of its owner plus `0x10000`, that holds a character's face model. `JC_face` binds it to head joint 48. See [Actors: relations: face, arms and accessories](libraries/actors.md#relations-face-arms-and-accessories).

**Face and Back programs**: VU1 entry points that draw only the triangles of a strip that face the camera (`Face`) or only the others (`Back`). A two-sided block runs both, back faces first. See [nml: front and back faces](libraries/nml.md#front-and-back-faces).

**FCV2**: The curve format of motions and time charts, with fractional key times and five key types. See [Animation: FCV2 curves](libraries/animation.md#fcv2-curves) and [Skeletons and motion: curve sets](formats/skeletons-and-motion.md#curve-sets-fcv2).

**fdlibm**: Sun's math library, as redistributed in newlib, linked in both its `double` and its `float` versions. The game mostly calls the `float` versions, which need no soft-float code. See [Runtime libraries: the math library (fdlibm)](architecture/runtime-libraries.md#the-math-library-fdlibm).

**File types table**: The list of every file extension on the disc with its reader and its formats page. See [Files and resources: file types](architecture/files-and-resources.md#file-types).

**Final packet callback**: One of the slots that the render step calls at the end of building a frame's packet. Slot 3 is `taskMapChange` and slot 4 is `DefocusMain`. See [Rendering pipeline: the render step](architecture/rendering.md#the-render-step).

**Fixed script threads**: The script threads of kind 0, such as `defaultVM`, `stageVM` and the two event VMs `evtVM[2]`, which `JTHREAD_cntl` never touches and C code calls directly. `stageVM` runs Stage talk methods. The event VMs run `entered` and `CallMethod` calls, and `UseVMFlag` marks which of them are busy. See [Java VM: fixed system threads](scripting/java-vm.md#fixed-system-threads) and [Events and cutscenes: script threads](scripting/events.md#script-threads).

**Flag area**: The story and event state, a packed bit array of 64 KiB inside the save data at `SaveData+0x74`. A flag is addressed by its bit offset and its width with the `xglFlags*` functions, so saving the game saves the flags too. Scripts read and write them as event flags with `Runtime.getFlags` and `Runtime.setFlags`. See [xgl: flags](libraries/xgl.md#flags), [Save data: the flags area](architecture/save-data.md#the-flags-area) and [Events and cutscenes: event flags and registers](scripting/events.md#event-flags-and-registers).

**FLAG_FRAME_60**: A debug setting with three values: 0 for 30 frames per second, 1 for 60 frames per second and 2 for a 30-frame debug mode. Only the enemy page of the in-game debug menu sets it, with START, so it cannot be reached in the retail game. With value 1, `xglRenderSyncMove` waits one vertical sync per frame instead of two. See [Debug tools: the in-game debug menu](game/debug-tools.md#5-the-in-game-debug-menu-gamedebugmenu) and [Field enemies: debug features](game/field-enemies.md#debug-features).

**Flip buffer**: A spare full-screen GS buffer at page `0x150`, used for frame captures and screen filters. See [Rendering pipeline: video memory](architecture/rendering.md#video-memory) and [nml: filters](libraries/nml.md#filters).

**Floor attribute**: The 64-bit value of each collision triangle. It encodes territories, camera zones, ladders, effects and similar rules. See [Maps and units: floor attributes](game/map-and-units.md#floor-attributes).

**Floor light (`GameIdLightSet`)**: A lighting preset chosen by the floor attribute under an actor (an ID light, type `0x6000`). It is blended onto the player and the enemies every frame. See [The field game: floor lights](game/field.md#floor-lights) and [Actors: lights](libraries/actors.md#lights).

**Freeze state**: An enemy command state that hands a field enemy over to the script: release, scripted, stop or slow. See [Field enemies: enemy commands](game/field-enemies.md#enemy-commands).

## G

**Gain record**: The reward record of one enemy in `batdat.bin`: experience, money, T, E and S points and three drop slots. See [Game data tables: gain records](formats/game-tables.md#gain-records).

**Game flags**: The bit word at `GameLoopState+0x10` (`GAME_FLAG_*`), for example `PLAYER_LOCK` (`0x8000`) and `ENTERED_SCRIPT` (`0x100000`). See [The field game: game flags](game/field.md#game-flags).

**Game mode**: The value `GameLoopState.kind`, which selects what the field loop does each frame: CF main, CF event, pause or one of the debug modes. `GAME_MODE_BATTLE` is set during a battle, but the loop never sees it, because the whole battle runs inside one call of `GameModeCfMain`. See [The field game: game modes](game/field.md#game-modes) and [Overview: the field game loop](README.md#the-field-game-loop).

**`Game()`**: The play session function in `src/main/game.c`. It sets up the field systems and the player actor, jumps to the current event ID and then runs one loop pass per frame: the current game mode, the frame's tasks and `GameDrawSync`. At the end of the session it stops sound and movies, tears down models and effects and restores `WorkEnd`. See [Overview: the field game loop](README.md#the-field-game-loop) and [The field game](game/field.md).

**`Game_Data_Push` and `Game_Data_Pop`**: The pair that saves and restores the field state around a battle started by contact and around the overlay minigames. The push copies the actor array, frees the field sound, saves the game state and pushes the effect state. The pop restores them and reloads the field resources. See [The field game: saving and restoring the field state](game/field.md#saving-and-restoring-the-field-state), [Battle system: how a battle starts](game/battle.md#how-a-battle-starts) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**`GameDebugMenu`**: An unreachable six-page debug menu inside the game, game mode 8. See [Debug tools: the in-game debug menu](game/debug-tools.md#5-the-in-game-debug-menu-gamedebugmenu).

**`GameDrawSync`**: The per-frame draw and sync routine of the field. It updates the cameras, prepares the map, the actors, the shadows and the units, steps the visual effects and the movie, then waits for the next frame. See [The field game: GameDrawSync](game/field.md#gamedrawsync-drawing-a-field-frame) and [Overview: the field game loop](README.md#the-field-game-loop).

**`gameEntrySelection` and `logoEntrySelection`**: Linker symbols whose values are title selection values: `0x01245C58` for `Game()` with OV01 and `0x02A007F8` for the logo screen with OV02. See [Boot and title screen: the control loop and the title return value](game/title-and-boot.md#3-the-control-loop-and-the-title-return-value).

**`GameLoopState`**: The `0x2A030`-byte global record that holds the state of the whole play session, such as the game mode (`kind`), the game flags, the current event ID and the encounter record. It is not declared once. Each source file declares its own view that names only the bytes it uses, so one offset can carry different names in different files. See [The field game: GameLoopState](game/field.md#gameloopstate) and [Overview: the field game loop](README.md#the-field-game-loop).

**`GameResourceWorkAlloc`**: The function with which a screen that runs on top of the field, such as the main menu or the U.M.N., borrows several MiB of the resource area. If the borrowed memory overwrites the scene resources, `GameResourceWorkReload` reloads the scene when the screen closes. See [Memory management: borrowing the resource area](architecture/memory.md#borrowing-the-resource-area).

**`GameUserSave`**: The view of the save data at `SaveData+0x50` that holds the game's own variables. `GamePushSaveDataUser` copies them in and `GamePopSaveDataUser` copies them back, field by field in the same order. See [Save data: the game user block](architecture/save-data.md#the-game-user-block).

**Generation table (`gene`)**: Six used bytes of a Xeno Card `CpuDeckRecord`: the minimum number of field cards for five attribute groups plus a total threshold, tested by `CheckGeneration`. See [Xeno Card: the computer opponent](game/minigames/xeno-card.md#9-the-computer-opponent).

**Global track table**: The 10 emitter tracks G0 to G9 of an effect line, such as the emitter position, the target position, the light position and the image animation. See [Effect data: global tracks](formats/effects.md#global-tracks).

**Gnosis database**: The U.M.N. database of the Gnosis the party has met and analysed, kept as bit sets for the units `0x22` to `0x3E`. See [U.M.N.: Gnosis](game/umn.md#gnosis).

**Gnosis filter**: A full-screen nml filter (`nmlModelSetFilterGunosys`) that draws an earlier picture as a wavering grid over the pixels the model marks, which probably produces the see-through look of the Gnosis. Actors select it through the draw hook `ACT_filterGuno`. See [nml: filters](libraries/nml.md#filters) and [Actors: draw hooks and filters](libraries/actors.md#draw-hooks-and-filters).

**`groupStatus`**: The per-group bits of TWSYS that turn off the updates (bit 1) or the drawing (bit 2) of a group of components. See [User interface and windows: slots, kinds and groups](libraries/ui-windows.md#slots-kinds-and-groups).

**Growth counter**: The hidden per-parameter counter that `calcLvUp` moves at every level toward a fixed level-99 target. The parameter grows with it. See [Character growth: points and experience](game/character-growth.md#points-and-experience).

**GS**: The Graphics Synthesizer, the graphics chip of the PlayStation 2, with 4 MiB of local video memory. The EE sends it the drawing commands of each frame in a packet. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [Rendering pipeline: video memory](architecture/rendering.md#video-memory).

**GS environments (`DrawEnv`, `DispEnv` and `ClearEnv`)**: The three small GS register sets for drawing, display and clearing that are rebuilt or resent every frame. See [Rendering pipeline: GS environments](architecture/rendering.md#gs-environments).

**`GsKick`**: The entry point of a VU1 microcode family that sends a GS tag list through VU1 (PATH1). Every family has `GsKick` at address 0 and `GsKickWide` at `0x28`, which the filters use. Its `MSCAL` stops at once, and the `MSCNT` that follows runs the `XGKICK`. See [nml: packet building](libraries/nml.md#packet-building) and [nml: how a program runs](libraries/nml.md#how-a-program-runs).

**GTW**: See **Action stock**.

**Gwork (`CasinoWork`)**: The union that holds the shared state of the Network Casino games. See [Network Casino: the work union](game/minigames/network-casino.md#the-work-union).

## H

**Hair tag**: A type-5 record of a JNT skeleton. Its kind-`0x8A` tag sets how many channels the motion pack drives, and the hair channels after them are simulated. See [Skeletons and motion: the hair tag record](formats/skeletons-and-motion.md#the-hair-tag-record).

**Hard mode**: The hardest CPU profile of A.G.W.S. Battle. The player holds L1 and R1 while choosing 1P Game, which works only when the event level is 2 or more. See [A.G.W.S. Battle: hard mode](game/minigames/agws-battle.md#hard-mode).

**Hash escape**: The sequence `#xx` in ASCII game text, which selects slot xx of the proportional block of the font. See [Text, fonts and U.M.N. data: the hash escape](formats/text-and-fonts.md#the-hash-escape).

**HDD install**: Copying the game's archives to the PlayStation 2 hard disk. `xglHddInstall` creates and formats a partition and copies `xenosaga.00` and the following archives into it. The title screen runs it through `TitleHddInstall`, with the progress callback `HddInstallCB` and the background pictures from `hddi.bin`. See [xgl: install](libraries/xgl.md#install) and [Boot and title screen: HDD installation](game/title-and-boot.md#8-hdd-installation).

**`hdd.res`**: The template of the hard disk partition header: a `SYSTEM.CNF`, a text `icon.sys` with the signature `PS2X` and a 3D icon. See [Movies, images and icons: hdd.res](formats/movies-images-and-icons.md#7-hddres-the-hard-disk-icon).

**`hdd_check` and `mc_check`**: The checks of the hard disk and the memory cards that run before the logo movies. See [Boot and title screen: device checks before the logo](game/title-and-boot.md#4-device-checks-before-the-logo).

**`hddi.bin`**: A `0x40`-byte offset table followed by the five JPEG backgrounds of the HDD install screen. See [Movies, images and icons: the install pictures](formats/movies-images-and-icons.md#44-the-install-pictures-in-hddibin).

**Header list**: The files `header.lst` and `dbheader.lst`, the record lists of the U.M.N. mails and database entries. See [Text, fonts and U.M.N. data: header lists](formats/text-and-fonts.md#header-lists).

**Heading convention**: How directions are measured. The world is Y-up, and a heading (yaw) in radians is measured on the ground plane from the +Z axis toward the +X axis, as `atan2(dx, dz)`. See [Math helpers: coordinate system](libraries/math.md#coordinate-system).

**Heap mark**: A saved allocation point of the xheap, set with `JNI_pushFrame`. `JNI_popFrame` drops every class, object and thread allocated above it. See [Java VM: the xheap object heap](scripting/java-vm.md#the-xheap-object-heap).

**Hermite curve**: A cubic curve from a start point to an end point with a given tangent at each end. `MMathCalcHermite` evaluates it in VU0, and `MMathCalcHermitePrm` computes Catmull-Rom tangents from the neighbouring points. See [Math helpers: Hermite curves](libraries/math.md#hermite-curves).

**Hidden UML text**: U.M.N. page text between `@(` and `@)` or `@<` and `@>`, drawn with random glyphs until the Code Disarm Key Plug-ins are owned. See [U.M.N.: keywords and the UML text format](game/umn.md#keywords-and-the-uml-text-format).

**Hissatu**: The special attack of an A.G.W.S. in A.G.W.S. Battle, from the Japanese word for a finishing move. It replaces the back-weapon command when the two hand weapons match the robot's combination (`RgHissatuCheck`). See [A.G.W.S. Battle: special attacks](game/minigames/agws-battle.md#special-attacks).

**History tree**: The file `histree.bin`, which links each U.M.N. mail to up to three child mails for the thread view. See [U.M.N.: the history tree](game/umn.md#the-history-tree) and [Text, fonts and U.M.N. data: history tree](formats/text-and-fonts.md#history-tree).

**Hit frame**: The frame of an M effect at which its post callback calls `sefHitEffect()`, which tells the effect scheduler that the hit has happened. See [Battle effects: effect lifecycle](game/battle-effects.md#effect-lifecycle).

**Homing graph**: Up to 64 linked positions that an enemy follows back home after a chase (`Homing_Search`). See [Field enemies: homing graphs](game/field-enemies.md#homing-graphs).

## I

**Icon animation speed**: The speed float of a 3D icon. The memory card save sets it to a random value from 0.7 to 1.3, and the source calls it a checksum. See [Movies, images and icons: how a save folder uses it](formats/movies-images-and-icons.md#64-how-a-save-folder-uses-it).

**`icon.sys`**: The 964-byte description of a save folder, with the signature `PS2D`, the title, the background colours, the lights and the names of the icon files. See [Movies, images and icons: icon.sys](formats/movies-images-and-icons.md#62-iconsys).

**ID light**: See **Floor light**.

**Image animation record**: A 4-word record of global track G9 that gives an image, a colour table, a time and a jump. See [Effect data: image animation records](formats/effects.md#image-animation-records).

**Image mapper**: The sef table of 40 type lists that index the loaded textures, effect scripts and models and the GS regions. See [sef: the image mapper](libraries/sef.md#the-image-mapper).

**Image reference**: A 16-bit chunk id plus bits that select a list of the image mapper. Effect scripts name their images this way. See [Effect data: image references](formats/effects.md#image-references).

**Inherit channel**: The one channel of a curve set with the flag `0x8000`. Sampling pauses there, and the skeleton is clip-tested at that point. See [Skeletons and motion: channel records](formats/skeletons-and-motion.md#channel-records).

**Instruction pair**: An 8-byte VU instruction word, made of an upper instruction for the vector math units and a lower one for loads, stores, branches and the divider, which run together. VU1 program memory holds 2048 pairs. See [nml: VU1 microcode](libraries/nml.md#vu1-microcode).

**Inventory boxes**: The item stores of the save data: `itmBox`, `wpnBox`, `bltBox`, `accBox` and `evtBox`, each with 255 16-bit entries and at most 99 of one item, and the 64-bit money box `moneyBox`. The code calls them data boxes (`dataBoxInc` and the other `dataBox*` functions). See [Main menu: inventory boxes and item pickups](game/main-menu.md#inventory-boxes-and-item-pickups) and [Save data: data boxes](architecture/save-data.md#data-boxes).

**Investment**: A U.M.N. mail whose reply costs money. The reply can be sent only when the player has the amount, and sending it takes the money. See [U.M.N.: investments and shop discounts](game/umn.md#investments-and-shop-discounts).

**IOP**: The I/O Processor, the second and smaller MIPS processor of the PlayStation 2. It owns the disc drive, the memory cards, the controllers and the sound chip, and it runs the game's sound drivers SSD and RSSD. The EE sends it requests through SIF. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [IOP sound driver: two modules](architecture/iop-sound-driver.md#1-two-modules).

**`IOPRP24D.IMG`**: Sony's replacement IOP module image. `InitializeSystem` reboots the IOP with it before it loads the other IOP modules. See [Runtime libraries: IOP modules on the disc](architecture/runtime-libraries.md#iop-modules-on-the-disc) and [Overview: start-up](README.md#4-start-up).

**IPU frame flags byte**: The first byte of every frame of an IPU movie. It sets `IPU_CTRL` (IDP, AS, IVF, QST and MP1) and the DTD bit of the IDEC command. See [Movies, images and icons: the frame flags byte](formats/movies-images-and-icons.md#33-the-frame-flags-byte).

**IPU movie**: A short clip in the `ipum` format, a stream of compressed frames that the IPU decodes directly. The `xglMovie*` player shows such clips as moving textures, for example on monitor screens in maps, in battle and in the boot logo. See [xgl: IPU movies](libraries/xgl.md#ipu-movies-xglmovie) and [Movies, images and icons: IPU movies](formats/movies-images-and-icons.md#3-ipu-movies).

**Item box table**: The files `itembox.dat` and `evtitem.dat`, with the contents of the item boxes and the names of event items. See [Maps and field data: item box tables](formats/maps-and-field-data.md#item-box-tables).

## J

**Java virtual machine (Java VM)**: The small interpreter in the main executable that runs the story and map scripts. It has no garbage collector, no `long` or `double` types and no exceptions. Scripts reach the engine only through native methods. See [Java VM: the interpreter](scripting/java-vm.md#the-interpreter) and [Overview: game logic in Java](README.md#8-game-logic-in-java).

**JC (joint chain)**: The consumers that attach a child skeleton to a joint of another actor: `JC_face`, `JC_setParent` and `JC_copyChain`. JC probably stands for "joint chain". See [Animation: attaching skeletons (JC)](libraries/animation.md#attaching-skeletons-jc).

**`JntWork`**: The joint evaluation work area in the scratchpad at `0x70000000`. See [Animation: the joint work area](libraries/animation.md#the-joint-work-area).

**Joint matrix arena (`actMatrix`)**: A `0xC0000`-byte buffer from which `ACT_allocMatrix` hands out the joint matrices of actors. See [Actors: joint matrix memory](libraries/actors.md#joint-matrix-memory).

**Joint producer (`JntProducer`)**: The per-actor list of up to eight joint evaluation functions, called consumers. Consumer 0 evaluates the whole skeleton, and the others run when the evaluation reaches their joint. See [Animation: the joint producer and its consumers](libraries/animation.md#the-joint-producer-and-its-consumers).

**Joint reference**: A VU1 address (`0x2E0` plus 4k) stored in a skinned vertex of a LEX model. It selects the joint matrix that moves the vertex. See [Models and textures: skin references and weights](formats/models-and-textures.md#skin-references-and-weights).

**JS interpreter**: A small text command language, unrelated to Java, that reads the debug light files `lgtNN.rc`. See [Java VM: debug helpers](scripting/java-vm.md#debug-helpers).

**`JThread`**: See **Script thread**.

**`JTHREAD_cntl`**: The function that advances the script threads once per field frame. It walks the thread list, calls the `reset` hook of every runnable thread that is not of kind 0 and finally calls a pending `jthreadResetFunc`. See [Java VM: running once per frame](scripting/java-vm.md#running-once-per-frame).

**`jthreadResetFunc`**: A one-shot function pointer that `JTHREAD_cntl` calls once after its walk over the script threads. The jump natives use it to put off the scene change until the walk has finished. See [Events and cutscenes: script threads](scripting/events.md#script-threads) and [Java VM: running once per frame](scripting/java-vm.md#running-once-per-frame).

**Jump native**: One of the script natives that change the current scene: `Runtime.jumpCF`, `Runtime.jumpEvent` and `Runtime.evsExit`. They do not jump at once. They defer the shared `jump_sub` through `jthreadResetFunc`. See [Events and cutscenes: event IDs and the jump natives](scripting/events.md#event-ids-and-the-jump-natives).

## K

**Key header**: The word pair of one channel component of a curve set. It says whether the component is zero, a constant or a run of spline keys. See [Skeletons and motion: key headers](formats/skeletons-and-motion.md#key-headers).

**Key3 and Key5**: The keyframes of an effect line, 3 or 5 words long. A key list ends with the word `0x400`. See [Effect data: key lists](formats/effects.md#key-lists).

**Keyword database**: The U.M.N. database of hypertext pages in the UML format, unlocked by game flags. See [U.M.N.: keywords and the UML text format](game/umn.md#keywords-and-the-uml-text-format).

**Keyword list**: The file `db_fileno.txt`, which groups the U.M.N. database entries under the index letters. See [Text, fonts and U.M.N. data: keyword list](formats/text-and-fonts.md#keyword-list).

**KickEvent**: See **Locator**.

**KOS-MOS specials**: Abilities for KOS-MOS taught by four U.M.N. mails. They are stored in `UmnKosmosSpecialBox` during the visit and applied after OV01 has been loaded again. See [U.M.N.: KOS-MOS specials](game/umn.md#kos-mos-specials).

**Koware**: The breakable parts of the map model (`KowareKind_t[127]`). See [Maps and units: breakable objects (koware)](game/map-and-units.md#breakable-objects-koware).

## L

**Label texture**: The 120 × 72 rectangle of `title_obj.xtx` that holds the title menu entry labels in 18-pixel rows. See [Title and boot: the frame loop](game/title-and-boot.md#the-frame-loop).

**Ladder (Hashigo)**: The ladder code of the field. Ladders are marked in the collision mesh, not placed as units, and the ladder state lives in the player's enemy work record at `+0x3830`. See [Maps and units: ladders](game/map-and-units.md#ladders).

**Layout store (`s_inLayout`)**: The `0x320`-byte record of per-model settings that the nml setters fill before `nmlModelEntry`. At entry it is copied into the packet, so each registered model keeps its own settings, and then it is reset. See [nml: the layout store](libraries/nml.md#the-layout-store).

**Level-99 target**: The value of the parameter table that a character's growth counter reaches at level 99. `calcLvUp` moves each stat towards it one level at a time. See [Game data tables: experience and level-99 targets](formats/game-tables.md#experience-and-level-99-targets).

**LEX**: The nml model file format. A LEX file is one model in main memory, with the magic `lex`, a header (`NmlModel`) and its blocks and parts. See [nml: LEX models](libraries/nml.md#lex-models) and [Models and textures: LEX models](formats/models-and-textures.md#lex-models).

**LFO**: A low-frequency oscillator of the sound driver. Channels and voices carry up to four LFO states: slot 0 for vibrato, slot 1 for tremolo and slot 2 for the pan "shake". See [IOP sound driver: LFO modulation](architecture/iop-sound-driver.md#13-lfo-modulation).

**libgcc**: The GCC runtime library, with the helpers GCC calls for operations the processor cannot do in one instruction, such as 64-bit multiplication and division, and the soft-float routines. See [Runtime libraries: the GCC runtime (libgcc)](architecture/runtime-libraries.md#the-gcc-runtime-libgcc).

**Light set (`StudioLight`)**: One ambient light and three parallel lights, `0xF0` bytes. Studios and actors each have one, and `xglLightCalcMatrix` turns it into the matrices for vertex lighting. See [xgl: lights](libraries/xgl.md#lights).

**Lighting variant**: The part of a VU1 entry point name that says how a lit vertex is shaded: plain diffuse light, `Spec` (a highlight added to the color), `MultiSpec` (the highlight drawn as a second strip) or `ReflSpec` (the highlight plus a reflection map strip). See [nml: lighting](libraries/nml.md#lighting).

**Line (`SefLineData`)**: An effect emitter with keyframed tracks that spawns particles. All effects share 128 lines. See [sef: lines](libraries/sef.md#lines).

**Line header**: The 6-word header of an effect line: the global track table, the local track table, gravity, collision, height and one unused word. See [Effect data: line headers](formats/effects.md#line-headers).

**Linear congruential generator**: The EE random generator, with a 64-bit seed that starts at `0x12345678` and the step `seed = seed * 1103515245 + 12345`. A second generator is the VU0 random register. See [Math helpers: random numbers](libraries/math.md#random-numbers).

**Link container**: The archive layout of the A.G.W.S. Battle files: a header, 32-byte names, offsets and entries aligned to `0x800` bytes. The NBXX, NBGL, NLNK and PTCL files share it. See [Minigame data files: the link container](formats/minigame-data.md#the-link-container).

**Local track table**: The 9 particle entries L0 to L8 of an effect line. See [Effect data: local tracks](formats/effects.md#local-tracks).

**Locator**: A trigger area stored in the map's ground data (block `0x207`, at most 50 pairs), not in the script. `Check_Locater` tests the player and the field enemies against the locators, and a hit calls the map script's `KickEvent(kind, id)`, which decides what happens. See [Events and cutscenes: event locators](scripting/events.md#event-locators) and [Java VM: map locators and KickEvent](scripting/java-vm.md#map-locators-and-kickevent).

**Locator group**: A numbered list of 16-byte positions with a yaw in an undulation file. The groups hold the layout, the entrances, the characters, the effects and the events of a map. See [Maps and field data: locator groups](formats/maps-and-field-data.md#locator-groups).

**LOOK**: The look-at controllers that turn the spine (joint 36), the head (joint 48) and the eyes toward a point of interest chosen by `Actor.look_mode`. See [Animation: look modes](libraries/animation.md#look-modes).

**Loop target (`link_id`)**: The frame at which a looping effect scheduler restarts when all its slots have ended, taken from the category of the linked descriptor. See [Effects: effect slots](libraries/sef.md#effect-slots).

**Loop word**: The sample record field at `+0x04` of a wave bank. It is never sent to the SPU2, because the loops come from the flags of the ADPCM packets. See [Sound files: sample records](formats/sound.md#sample-records).

**Looping B-spline path**: A closed path of control points along which some field enemies patrol. `GetBSplineLoop` evaluates it as a uniform cubic B-spline that passes near the points but not through them. See [Math helpers: looping B-spline paths](libraries/math.md#looping-b-spline-paths).

**Looping stream**: A `s195xxx.vds` stream encoded with the loop flag `0x02` on every packet and no end packet, so it repeats without end. See [Sound files: looping streams](formats/sound.md#looping-streams).

## M

**M effect**: A hand-coded OV01 battle effect, one object (`MEfObj`) from a pool of 96. A missile command of an effect script starts it with `MEfCreate`, and its callbacks move it, draw it and signal its hit. See [Battle effects: M effects and sef](game/battle-effects.md#m-effects-and-sef) and [sef: coded battle effects](libraries/sef.md#coded-battle-effects).

**M subsystems**: The layer of OV01 files with the `m_` prefix: camera (`MCam`), movies (`MMv`, `MMv2`), GS packets (`MGs`) and effects (`MEf`). Field event code in the main executable also calls them. See [Battle system: what OV01 contains](game/battle.md#what-ov01-contains).

**Macro mode**: The VU0 mode in which the EE executes VU0 instructions one by one in its own instruction stream, using VU0 as coprocessor 2. See [Math helpers: macro mode](libraries/math.md#macro-mode).

**MagicCarpetCome trailer**: A tool string and extra data appended to the LEX models built from September 2001 on. See [Models and textures: the MagicCarpetCome trailer](formats/models-and-textures.md#the-magiccarpetcome-trailer).

**Mail attachment**: An entry of `umn_attach_tbl` attached to a U.M.N. mail: a Plug-in, an item, a weapon or money. See [U.M.N.: attachments](game/umn.md#attachments).

**Mail header**: The `0x90`-byte record of a U.M.N. mail, built from `header.lst`, with the title, the subject, the reply ids, the scenario limit, the attachment, the discounts and the cost. See [U.M.N.: mail data](game/umn.md#mail-data) and [Text, fonts and U.M.N. data: header lists](formats/text-and-fonts.md#header-lists).

**Main executable (`SLUS_204.69`)**: The program that stays in memory the whole time. It holds the engine, the field game, the main menu, the Driller Game and the Java VM, and it is loaded at `0x00200000`. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [Overview: memory map](README.md#3-memory-map).

**`MainMenu`**: The function that runs the whole main menu with its own frame loop. See [Main menu: one menu session, step by step](game/main-menu.md#one-menu-session-step-by-step).

**`MainMenuWorkEnd`**: The bump pointer into the `0x80000`-byte work area of the main menu, which follows the menu model pool inside the memory the menu borrows from the field. The U.M.N. has its own pointer of the same kind, `UmnWorkEnd`. See [Main menu: menu memory](game/main-menu.md#menu-memory) and [Memory management: menu and U.M.N. work areas](architecture/memory.md#menu-and-umn-work-areas).

**Manzai dialogue**: A comic dialogue between the procurator and Shion, started by some U.M.N. mails. See [U.M.N.: manzai dialogues](game/umn.md#manzai-dialogues).

**Map block**: The memory block that holds the `MapResource`, the LEX model, the XTX texture set, the undulation data and the think data of the current map. See [Maps and field data: the map block in memory](formats/maps-and-field-data.md#the-map-block-in-memory).

**Map vertex layout**: The packed vertex form of a LEX model that `materialFlags` bit `0x10` selects. Bit `0x80` selects the float layout. See [Models and textures: vertex layouts](formats/models-and-textures.md#vertex-layouts).

**Mapper index**: The type list of the image mapper that holds the data of an effect. `sefSearchMapperIndex` finds it. See [Effect data: the mapper index](formats/effects.md#the-mapper-index).

**MapUnit**: One of the 64 map unit records (`0x300` bytes each) for the objects of a map, with a kind-specific work area at `+0x1A0`. See [Maps and units: the MapUnit array](game/map-and-units.md#the-mapunit-array).

**Material batch**: A batch in the VIF stream of a LEX block that carries a small GIF packet in A+D mode instead of vertices. The VU1 program kicks its `CLAMP_1` and `TEX0_1` to the GS for the strips that follow, taking the second `TEX0_1` copy when the textures sit in the flip buffer. See [Models and textures: material batches](formats/models-and-textures.md#material-batches) and [nml: how a program runs](libraries/nml.md#how-a-program-runs).

**Material tag**: The VU1 quadword at `0x3F0` that holds a block's specular exponent, highlight scale, the flag that renormalizes normals and the shape target kind. See [nml: VU1 memory](libraries/nml.md#vu1-memory).

**Matrix stack**: A current matrix and a stack of saved matrices, kept inside VU0 as in classic OpenGL. The current matrix is in `vf28` to `vf31`, and the stack holds 32 matrices in VU0 data memory and wraps around when it overflows. See [Math helpers: the matrix stack](libraries/math.md#the-matrix-stack).

**Matrix table**: The four arrays of 64-byte matrices of a LEX model: part matrices, inverse part matrices, skin slots and per-joint matrices. See [Models and textures: the matrix table](formats/models-and-textures.md#the-matrix-table).

**MBUF**: A pool of eight message buffers, each with a `0x800`-byte workspace, that hold converted window text. See [User interface and windows: MBUF and the message queue](libraries/ui-windows.md#mbuf-and-the-message-queue).

**MDL handle (`MdlHandle`)**: A small wrapper of an nml model kept in `Actor.model`, with 128 part visibility bits. See [Animation: MDL model handles and part visibility](libraries/animation.md#mdl-model-handles-and-part-visibility).

**Memory card request queue**: The 8-entry ring of memory card requests. `xglMcRequest` copies the arguments of an operation into the next entry and returns at once. See [xgl: the request queue](libraries/xgl.md#the-request-queue).

**Menu level**: One screen of the main menu, stored in `MenuWork.menu_state[1]` and switched by `ChangeTopLevel`. See [Main menu: menu levels and MenuWork](game/main-menu.md#menu-levels-and-menuwork).

**Menu widget**: A building block of a main menu screen: a function such as `TopInfoMain` plus a work block. The screen calls it once per frame, and its state byte is 0 while it builds its windows and 2 while it runs them. See [Main menu: widgets](game/main-menu.md#widgets).

**`MenuDrillCall`**: The menu flag that makes `GameModeCfMain` store a return point and jump to event `0xA0` (160), the map of the Driller Game. See [Driller Game: entering and leaving the Driller Game](game/minigames/driller-game.md#1-entering-and-leaving-the-driller-game).

**`MenuGameDataPush` and `MenuGameDataPop`**: The pair that saves and restores the field around the main menu and the shops. See [Main menu: restoring the field](game/main-menu.md#restoring-the-field).

**`MenuTask` and `MenuTask_XMX`**: The two task schedulers of the main menu, with `0x20` and `0x10` slots, for animated pieces such as the formation model behind the top menu. See [Main menu: task lists](game/main-menu.md#task-lists).

**`MenuWork`**: The `0x80`-byte global state of the main menu. See [Main menu: menu levels and MenuWork](game/main-menu.md#menu-levels-and-menuwork).

**Mesh header**: A 20-byte record of an undulation file that points to one triangle mesh with its vertices, normals and attribute indices. See [Maps and field data: mesh headers](formats/maps-and-field-data.md#mesh-headers).

**Metrics table**: The 256 pairs of left and right bytes at `0x78040` of `font0.tex` and `font1.tex`. The drawn width of a proportional glyph is right minus left plus 3. See [Text, fonts and U.M.N. data: the metrics table](formats/text-and-fonts.md#the-metrics-table).

**Micro mode**: The VU0 mode in which VU0 runs a program from its own instruction memory while the EE continues. The EE starts a routine with `vcallms` and reads the result with an interlocked transfer. See [Math helpers: micro mode](libraries/math.md#micro-mode).

**Microcode family**: One of the seven VU1 programs that draw models. nml picks the family from the render status of the model and from the block. The programs are kept as retail data and are not decompiled, but their original labels survive in the ELF symbol table. See [nml: VU1 microcode](libraries/nml.md#vu1-microcode).

**MIF2**: The message interface in `msg_init.c`. It converts script text with `/[tag()]` codes into the binary text that the windows show. See [Events and cutscenes: message text and the MIF2 interface](scripting/events.md#message-text-and-the-mif2-interface) and [Text, fonts and U.M.N. data: MIF2 message tags](formats/text-and-fonts.md#mif2-message-tags).

**Missile block**: A 12-word header plus 6-word spawn entries in an effect script table. It describes the missiles an effect fires. See [Effect data: missile blocks](formats/effects.md#missile-blocks).

**Missile command**: A block of an effect script that calls `MEfCreate` to start an M effect. See [Battle effects: how an effect is spawned](game/battle-effects.md#how-an-effect-is-spawned).

**MMv and MMv2**: The two OV01 movie players. MMv uses the IPU movie player (`xglMovie`), and MMv2 uses the MPEG-2 player (`xglMpeg2`) for the cinematic techniques in battle. See [Battle effects: movie players](game/battle-effects.md#movie-players).

**Model id**: The 16-bit id that names a model. Bits 0 to 7 are the 1-based index in its group, bits 8 to 11 the group and bits 12 to 14 the category (CID, RID, WID, EID, MID, AID or MOBJ), and `0x10000` selects the face model. Scene scripts write it as an upper-case text key such as `CID_SHION`. An actor keeps the model id it was created with in `inUseId`. See [Files and resources: model keys and model ids](architecture/files-and-resources.md#model-keys-and-model-ids) and [Actors: the actor id](libraries/actors.md#the-actor-id).

**Model key**: A text such as `CID_ALLEN` in a scene resource script that `search_key` turns into a model id. A key with a typo loads the dummy model. See [Scripts and scene archives: model keys and typos](formats/scripts-and-archives.md#model-keys-and-typos).

**Monitor screen**: The map unit `0x6025`, which shows pictures or an IPU movie on a screen inside the map. See [Maps and units: monitor screens](game/map-and-units.md#monitor-screens).

**Monster set**: The group of monsters that a battle loads. `Check_Encount` picks it at random from the up to eight `monster_set_numbers` of the field enemy and stores it in the encounter record. See [Field enemies: the encounter record](game/field-enemies.md#the-encounter-record), [Battle system: encounter data](game/battle.md#encounter-data) and [Game data tables: the monster set table](formats/game-tables.md#monster-set-table).

**Motion blend**: The mix of the previous and the current pose over `blendDuration` (8 frames) when a motion changes. See [Animation: blending between motions](libraries/animation.md#blending-between-motions).

**Motion clock (ANM)**: The per-actor motion timer: the time in seconds, the speed (1/30 second per game frame by default) and the loop, finish, queue and blend flags. See [Animation: the motion clock (ANM)](libraries/animation.md#the-motion-clock-anm).

**Motion id**: The id of a motion. Bits 0 to 7 select the entry in a motion pack, and bits 8 to 10 the bank, which is the actor's pack slot. Bank 0 holds the character's own pack and bank 1 the event motions. See [Animation: motion ids and banks](libraries/animation.md#motion-ids-and-banks).

**Motion pack**: A relocatable `.fpk` file of FCV2 curve sets, one per motion, with the magic `FPK`. An actor holds up to eight packs. The pack of each event script is installed in bank 1 of every actor with `ACT_initExMotion`. See [Animation: motion packs](libraries/animation.md#motion-packs), [Files and resources: FPK and FCV motion packs](architecture/files-and-resources.md#fpk-and-fcv-motion-packs), [Events and cutscenes: script files and script databases](scripting/events.md#script-files-and-script-databases) and [Skeletons and motion: motion packs](formats/skeletons-and-motion.md#motion-packs-fpk).

**Motion queue**: The motion stored in `nextDataId` when `ACT_setMotion2` asks for a new motion while a blend is still running. It starts when the running blend has finished. See [Animation: ACT_setMotion2 and the motion queue](libraries/animation.md#act_setmotion2-and-the-motion-queue).

**Mount**: A weapon position of an A.G.W.S.: L. Arm, R. Arm, L. Alt or R. Alt. An A.G.W.S. carries up to three weapons, and their total `equipWeight` may not exceed 3 (2 for AG-05). The status panel shows the sum of the weapons' `wagl` values as WAGL. See [Character growth: weapons and mounts](game/character-growth.md#weapons-and-mounts).

**Move kind**: The movement pattern of a field enemy (`pause_kind` and `move_kind`): wander, route, stand, escape or route-chase, together with how it reacts when it sees the player. See [Field enemies: move kinds](game/field-enemies.md#move-kinds).

**MPEG-2 player**: The `xglMpeg2*` player for the full-motion video cutscenes, MPEG-2 program streams with interleaved video and audio, decoded through Sony's `sceMpeg` library. See [xgl: MPEG-2 movies](libraries/xgl.md#mpeg-2-movies-xglmpeg2).

**MsgQueue**: A reader over converted message text that hands out the characters one by one and reports the tags it meets. See [User interface and windows: MBUF and the message queue](libraries/ui-windows.md#mbuf-and-the-message-queue).

**MTD**: Motion trigger data, the second part of a battle motion file. For each motion it lists the frames where a hit lands or a weapon fades in or out. See [Skeletons and motion: motion trigger data](formats/skeletons-and-motion.md#motion-trigger-data-mtd).

**mtnpack**: The folder `data\mtnpack\` with the motion packs (`.fpk`) and the resource volumes (`.arc`) of the event scripts. It holds 107 motion packs and 77 resource volumes, so not every event script has both. `loadScriptCD2` loads them into the resource table. See [Files and resources: loading an event script](architecture/files-and-resources.md#loading-an-event-script) and [Skeletons and motion: script resource volumes](formats/skeletons-and-motion.md#script-resource-volumes).

## N

**Name mangling**: The rule that builds a native's C name: `Java_`, the class name with `/` replaced by `_`, then `_` and the method name, then `__` and the parameter part of the descriptor. The return type is left out. See [Native methods: name mangling](scripting/natives.md#name-mangling).

**Native method**: A Java method declared with the `native` keyword and implemented as a C function named after it, such as `Java_xeno_util_Runtime_minigameExec__I`. The main executable has 401 natives, all listed in the `default_native` table in `src/main/runtime.c`. A native reads its arguments from the Java call and calls ordinary engine code. See [Native methods: calling convention](scripting/natives.md#calling-convention) and [Overview: game logic in Java](README.md#8-game-logic-in-java).

**Native-backed object**: An engine record that serves directly as a Java object, because its first word holds the class's `classPointer`. The records behind `Window`, `Menu`, `Camera` and `PlayControl` work this way and live in engine pools, not in the VM heap. See [Native methods: native-backed objects](scripting/natives.md#native-backed-objects).

**Natural cubic spline**: The spline that `SPL_cardinalInit` builds for cubic interpolation, even though the source calls it cardinal. The curve passes through every sample and has zero curvature at both ends. Evaluation first eases the frame into a key with one of four easing modes: ease in, ease out, constant speed, or ease in and out. See [Math helpers: building the cubic](libraries/math.md#building-the-cubic) and [Math helpers: evaluation](libraries/math.md#evaluation).

**NBGL (`.rbg`)**: The A.G.W.S. Battle background archive, a link container of LEX models and XTX texture sets. See [Minigame data files: NBGL background archives](formats/minigame-data.md#nbgl-background-archives).

**NBXX (Bxx)**: The A.G.W.S. Battle picture archive. Entry 0 is a picture table, the `xtxinfo` (`RgBxxHeader` plus `0x60`-byte `RgBxxPic` records), and entry 1 is one XTX texture set. See [Minigame data files: NBXX picture archives](formats/minigame-data.md#nbxx-picture-archives).

**Network Casino**: The casino minigame in OV11, minigame number 14, with the Slot Machines (including the Nine Reel game), Poker and the coin exchange. It starts with the Casino Passport or from a script with `Runtime.minigameExec`. See [Network Casino](game/minigames/network-casino.md) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**New Reel**: One of the five alternate reel sets of the slot machine. A Curry, Grape or Peach line switches the machine to a random New Reel and starts a bonus time. See [Network Casino: the machine](game/minigames/network-casino.md#the-machine).

**newlib**: The C library linked into the executable, which provides the string, conversion, formatted I/O and memory allocation functions. Like the other runtime libraries it is kept as the original assembly. See [Runtime libraries: the C library (newlib)](architecture/runtime-libraries.md#the-c-library-newlib).

**Nine Reel**: A bonus game on a 3 by 3 grid of O and X marks, started by a Super 7 and paying up to 512 times. See [Network Casino: Nine Reel](game/minigames/network-casino.md#6-nine-reel).

**NLNK (`.npr`)**: A version-2 link container of whole Bxx files. `RgFileSys` prepares it so that its entries take the place of the disc files of the same names. See [Minigame data files: NLNK prepared archives](formats/minigame-data.md#nlnk-prepared-archives).

**nml**: The model library of the engine (`nml*` functions). The control thread registers the models of a frame with `nmlModelEntry`, and the render thread's `nmlModelFlush` turns them into VU1 commands in the packet. See [nml](libraries/nml.md) and [Rendering pipeline: the render step](architecture/rendering.md#the-render-step).

**Nyuru**: The push of an actor along the edge of a map unit it collides with (`HitCheckMapUnitWithNyuru`). The word is a Japanese sound word for sliding smoothly. See [Maps and units: collision with map units](game/map-and-units.md#collision-with-map-units).

## O

**Observer task**: An xgl task that calls a Java method on a system thread once per frame until the thread reports `DONE`. Talks, `entered` calls and calls by name run through observers. See [Java VM: observer tasks](scripting/java-vm.md#observer-tasks).

**`OpenCloseMain`**: The controller of the open and close animation of a window frame. See [User interface and windows: open and close animation](libraries/ui-windows.md#open-and-close-animation).

**Order key**: The third argument of `xglFontPrint`, which is not a colour. Bits 24 to 27 select one of the 16 buckets of the print queue, and the low 24 bits give the GS depth, so a larger value draws on top. See [User interface and windows: order keys and buckets](libraries/ui-windows.md#order-keys-and-buckets) and [xgl: the print queue](libraries/xgl.md#the-print-queue).

**OV01**: The overlay that stays loaded during play. It holds the battle system and battle effects, the accessors for the game data tables that the menus read, the M subsystems that field scripts use for cameras and movies, and the think data. Every other overlay is swapped in for a while, and OV01 is loaded again afterwards. See [Battle system: what OV01 contains](game/battle.md#what-ov01-contains) and [Overview: overlays](README.md#2-overlays).

**OV02**: The overlay with the boot logo, the title screen, the HDD install and the U.M.N. screens. A copy of it is embedded in the main executable, so it is already in memory at boot. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and **Embedded OV02**.

**OV10**: The overlay of the Xeno Card minigame. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [Xeno Card](game/minigames/xeno-card.md).

**OV11**: The overlay of the Network Casino minigame. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [Network Casino](game/minigames/network-casino.md).

**OV12**: The overlay of the A.G.W.S. Battle minigame. Its entry function is `RobotGameMain`. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [A.G.W.S. Battle](game/minigames/agws-battle.md).

**Overlay**: A block of code and data loaded from disc on demand into the overlay slot at `0x00A00000`. All five overlays are linked to run at that address, so only one of them is in memory at a time. `xglCdLoadOverlay(n)` loads `OVnn.OVL` and does nothing when overlay `n` is already loaded. See [Overview: overlays](README.md#2-overlays).

## P

**Pack class**: The u16 at offset `+0x12` of an effect chunk that marks the type of the pack: 0 for `default.esp`, 1 for character and enemy packs, 2 for weapon packs and 4 for `cf_def.esp`. No code reads it. See [Effect data: the pack class field](formats/effects.md#the-pack-class-field).

**Packet**: The DMA chain of drawing commands and data that the game builds for each frame, an `XglPacket` record with a 2 MiB buffer. The two buffers at `0x00C00000` and `0x00E00000` alternate, so one is filled while the other is sent. The chain grows upward from the start of the buffer, and data that must live for the frame is allocated downward from the top. See [Rendering pipeline: packets](architecture/rendering.md#packets), [xgl: packets](libraries/xgl.md#packets) and [Memory management: packet buffers as frame memory](architecture/memory.md#packet-buffers-as-frame-memory).

**Pad record (`XglPadRecord`)**: The `0x68`-byte state of one controller port. See [xgl: the pad record](libraries/xgl.md#the-pad-record).

**Paragraph delimiter (`%`)**: The character that ends a paragraph in the `.info` text databases of A.G.W.S. Battle. See [Minigame data files: .info text databases](formats/minigame-data.md#info-text-databases).

**Part**: A bone of an nml model. Each block belongs to one part, and parts can be hidden by id or, in models with named parts, by name. See [nml: parts and visibility](libraries/nml.md#parts-and-visibility).

**Particle**: One sprite (type 4) or model (type 6) spawned by a line, from a pool of 1024 records of 640 bytes. See [sef: particles](libraries/sef.md#particles).

**Party data**: The `0x188`-byte block at `SaveData+0x10078` with the party membership, the battle formation, the technique levels and the skill levels. The party functions work on this memory directly. See [Character growth: party data](game/character-growth.md#party-data) and [Save data: the party state](architecture/save-data.md#the-party-state).

**Passport**: An event item that starts a minigame: the Battle, Card, Casino and Drill passports are event items 12 to 15. A passport works only while the player stands on an EVS symbol. The item screen then ends the menu with the minigame number in `mini_game_no`. See [Main menu: event items, passports and the segment map](game/main-menu.md#event-items-passports-and-the-segment-map) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**PDB volume**: A small file system inside one buffer, with the magic `"FL00"` (`0x30304C46`). It holds groups, one per directory, each with its file entries, and is relocated on first use. Script archives (`.evt`) and resource volumes (`.arc`) are PDB volumes, searched with `PDB_findFile`. See [Files and resources: PDB volumes](architecture/files-and-resources.md#pdb-volumes), [Java VM: where the classes come from](scripting/java-vm.md#where-the-classes-come-from) and [Scripts and scene archives: PDB volumes](formats/scripts-and-archives.md#pdb-volumes).

**Peer**: The engine object behind a Java object, such as an actor behind a `xeno.Chr` or an effect scheduler behind a `xeno.Effect`. The Java object holds a pointer to it in its `peer` field, and the engine object usually points back. See [Native methods: peers](scripting/natives.md#peers) and [Actors: the Chr peer](libraries/actors.md#the-chr-peer).

**Picture list (`.r2d`)**: An A.G.W.S. Battle file with a `0x40`-byte header and `0x100`-byte records that place named Bxx pictures on the selection screens. See [Minigame data files: .r2d picture lists](formats/minigame-data.md#r2d-picture-lists).

**Play time**: The `XglClock` at `SaveData+0x68` that counts the time played. The game does not count frames for it but adds up readings of the real-time clock. See [Character growth: play time](game/character-growth.md#play-time) and [Save data: dates and play time](architecture/save-data.md#dates-and-play-time).

**PlayControl**: The global time chart player of events. It has a 30 frames per second clock, can drive a camera from a camera motion pack and calls time chart observers. Scripts control it through 12 natives. See [Scenes, stages and time charts: PlayControl and the time chart player](scripting/scenes-and-timecharts.md#playcontrol-and-the-time-chart-player).

**Player block**: The `0x12A000`-byte block at `0x01000000`, slot 0 of the resource table, with the party leader's model, texture, joints, face and motion pack at fixed addresses. Other screens may overwrite it freely, because `GameCfPlayerLoadResource` checks the buffers when they end and reads the files again only when they no longer match. See [Memory management: the resource table](architecture/memory.md#the-resource-table).

**Player record (`plChaData`)**: The `0xA8`-byte record of what a character has earned and learned: experience, the three point pools and the bitmaps of learned ethers and skills. See [Character growth: player records](game/character-growth.md#player-records).

**Plug-in flags**: The 16-bit set of U.M.N. Plug-in bits. Bit 1 enables the Database, bit 3 the EVS, and bits 4 and 5 decode the hidden parts of UML text. See [U.M.N.: Plug-ins](game/umn.md#10-plug-ins).

**Plug-in texts (`umntxt.bin`)**: The six `0x80`-byte descriptions of the U.M.N. Plug-ins. See [Text, fonts and U.M.N. data: plug-in texts](formats/text-and-fonts.md#plug-in-texts).

**Point code**: A code that names a place on the caster, the target or a weapon to which an effect is attached. A null is an accessory joint of the actor used as such a place. See [sef: positions and attachment](libraries/sef.md#positions-and-attachment).

**Poker**: The five-card draw game of the Network Casino, followed by a double-or-nothing game of higher or lower that can raise a win up to 16 times. See [Network Casino: Poker](game/minigames/network-casino.md#7-poker).

**Prefix word**: The u32 value 1 that precedes each curve set in a format-2 motion pack. The pack's data offset of 4 skips it. See [Skeletons and motion: the prefix words](formats/skeletons-and-motion.md#format-2-the-prefix-words).

**Print queue**: The buffer of sorted text requests of the font system, divided into 16 buckets (`fontPrintQueueBuffer`). The order key of each request selects its bucket and depth. See [xgl: the print queue](libraries/xgl.md#the-print-queue).

**`processClass` states**: The link states a Java class passes through before it can be used: 4 when its members are resolved, 6 when its constants are resolved and 11 when it is initialized. `loadClass` always asks for state 11. See [Java VM: linking and initialization](scripting/java-vm.md#linking-and-initialization).

**Procurator**: The 3D guide model of the U.M.N., with id `0x1D`. In English it's name is Bunnie, and the event texts calls it `UKUN`. See [U.M.N.: the procurator and the manzai windows](game/umn.md#5-the-procurator-and-the-manzai-windows).

**Program (SWD)**: An instrument of an SWD wave bank, with an ADSR envelope, a voice range, four LFO slots, a 16-byte name and its key splits. See [Sound files: programs](formats/sound.md#programs).

**Program split**: A key range of an SWD program, mapped to one sample. See [Sound files: program splits](formats/sound.md#program-splits).

**Proportional block**: The cells 3776 to 4031 of the kanji fonts, which hold the 256 Latin glyphs `#00` to `#FF` that the hash escape selects. See [Text, fonts and U.M.N. data: the proportional block](formats/text-and-fonts.md#the-proportional-block).

**ProReal**: The projected-texture shadow pass of nml, which renders a texture-map shadow onto the scene. See [nml: projected shadows and texture maps](libraries/nml.md#projected-shadows-and-texture-maps).

**PS2 3D icon (`.ico`)**: The console's model format for save icons: shapes, vertex records, a weight-key animation and an optional texture. See [Movies, images and icons: the 3D icon format](formats/movies-images-and-icons.md#5-the-3d-icon-format).

**PSS movie**: An MPEG-2 program stream in 16 KiB packs with Sony ADPCM audio in private stream 1. The cutscenes and the battle movies use it. See [Movies, images and icons: PSS movies](formats/movies-images-and-icons.md#2-pss-movies).

**PTCL (`.rpl`)**: The A.G.W.S. Battle particle archive: 30 entries of 8 slots of `0x100` bytes, of which 4 are read as `RgParticleEffectEssence`. See [Minigame data files: PTCL particle archive](formats/minigame-data.md#ptcl-particle-archive).

## Q

**Quadword copy**: A 16-byte copy with the EE instructions `lq` and `sq`, which move a `Vector4` in two instructions. The original code wrote these copies as inline assembly, kept as macros in `include/quadword.h`, and both addresses must be 16-byte aligned. See [Math helpers: quadword copies](libraries/math.md#quadword-copies).

## R

**Radar**: The small map display in the corner of the field screen, with a sonar-like ping and markers for the actors nearby. See [The field game: the radar](game/field.md#the-radar).

**Range record**: A type-2 attribute of a curve set (`FcvPlaybackRange`). It gives the frame range and speed of playback, or a reset range, for one attribute id. See [Skeletons and motion: range records and attribute slots](formats/skeletons-and-motion.md#range-records-and-attribute-slots).

**Rarity letter**: The byte of `card.dat`. It holds C, U, R or P and selects the rarity mark of the card. See [Minigame data files: card.dat](formats/minigame-data.md#carddat).

**Reaction effect**: An effect created automatically on the first hit, through `sefCreateReactionEffect`. See [sef: effect numbers](libraries/sef.md#effect-numbers).

**Read control block (`LW`)**: The static `CdReadControl` struct that holds all state of the file library, including the queue of 128-byte read requests. See [xgl: the read control block](libraries/xgl.md#the-read-control-block).

**Record list**: A numbered list of meshes in an undulation file that marks zones: NPC and enemy layers, camera zones, footsteps, aim and lights. See [Maps and field data: record lists](formats/maps-and-field-data.md#record-lists).

**REGIST, REGIST2 and REGISTCF**: The shared sound data that stays loaded across scenes. `REGIST` fills effect bank 0 with the system and menu sounds at boot. `REGIST2` is a bare wave bank sent at boot and never replaced. `REGISTCF` fills bank 1 with common field sounds such as traps. See [Audio: sound banks](architecture/audio.md#5-sound-banks).

**Render packet**: See **Packet**.

**Render thread**: System thread 0, with the entry point `xglRenderEntry`. On each turn it finishes the frame's packet and sends it to the GS, sends the queued sound commands and reads the controllers. See [Overview: the four system threads](README.md#5-the-four-system-threads) and [Rendering pipeline: the render step](architecture/rendering.md#the-render-step).

**Resident VU0 microprogram**: The game's VU0 program `src/main/vu0/Vu0MicroCode.dvp` (1792 bytes), uploaded once at start-up and never replaced. It keeps the current matrix and the matrix stack and provides routines such as RTPS and the light calculation. See [Math helpers: the resident VU0 microprogram](libraries/math.md#the-resident-vu0-microprogram).

**Resource table (`GameResource`)**: The table of 128 slots that manages the field's models, maps, motions, textures and effects in upper memory. The slots are consecutive extents of one range, and new data always goes into the free slot at the end. Memory is freed only with `GameResourceReset(slot)`, which frees that slot and every slot after it. See [Memory management: the resource table](architecture/memory.md#the-resource-table).

**Resource type (`RES_TYPE_*`)**: The kind of a resource that `RES_loadFile` loads, such as a model block with its model, texture and joints, or a plain file. See [Files and resources: resource types](architecture/files-and-resources.md#resource-types).

**Resource volume (`.arc`)**: A PDB volume `data\mtnpack\SCExxxxx.arc` beside an event script, packed with ARX. It holds camera motion packs, face motion packs and some leftover `.java` and `.tch` files, and `Toolkit.loadResource` fetches its members. 77 of the 181 event scripts have one. See [Scripts and scene archives: resource volumes](formats/scripts-and-archives.md#resource-volumes-arc) and [Skeletons and motion: script resource volumes](formats/skeletons-and-motion.md#script-resource-volumes).

**Return point**: The event id and player position saved before a jump into the EVS or the Driller Game. `Runtime.evsSetRetPoint` stores it, and `Runtime.evsExit` jumps back to it. See [Driller Game: entering and leaving the Driller Game](game/minigames/driller-game.md#1-entering-and-leaving-the-driller-game) and [U.M.N.: EVS, the Environmental Simulator](game/umn.md#9-evs-the-environmental-simulator).

**Return-to-title chord**: Holding SELECT, L1 and R1 and pressing START, which sets `GAME_FLAG_QUIT` and returns to the title screen. It works in the retail game. See [Debug tools: return to the title screen](game/debug-tools.md#return-to-the-title-screen).

**rg and xrg**: The two layers of the A.G.W.S. Battle engine. The rg layer ("robot game") holds the game logic and the engine services, and the xrg layer wraps the main executable's libraries for it. See [A.G.W.S. Battle engine: layers](game/minigames/agws-battle-engine.md#layers).

**`RgChar` and `RgCharMgr`**: The base of the short-lived A.G.W.S. Battle objects, such as background objects, shots, dropped weapons and particle effects, and the manager that holds up to 256 of them and deletes them later rather than at once. See [A.G.W.S. Battle engine: characters and the character manager](game/minigames/agws-battle-engine.md#characters-and-the-character-manager).

**`RgDebugFlags`**: The debug flags of OV12. `flags[1]` and `flags[3]` are on by default, and `modeEnabled` is 0. See [Debug tools: OV12 debug flags](game/debug-tools.md#ov12-debug-flags).

**`RgDraw`**: The draw queue and screen set-up of A.G.W.S. Battle, with 128 draw requests in seven priorities. See [A.G.W.S. Battle engine: drawing](game/minigames/agws-battle-engine.md#drawing).

**`RgGeom`**: The geometry base of A.G.W.S. Battle physics and collision. There are six geometry kinds and a 6 by 6 table of collision handlers. See [A.G.W.S. Battle engine: geometry and collision](game/minigames/agws-battle-engine.md#geometry-and-collision).

**`RgHeap`**: The best-fit allocator of A.G.W.S. Battle, with a 1 MiB normal heap for objects at `0x01000000` and a 15 MiB data heap for file contents and large buffers at `0x01100000`. Every block carries a `0x40`-byte header. See [A.G.W.S. Battle engine: memory, the RgHeap allocator](game/minigames/agws-battle-engine.md#memory-the-rgheap-allocator) and [Memory management: the OV12 heaps](architecture/memory.md#the-ov12-heaps).

**Round**: One battle of an A.G.W.S. Battle match, which is the best of three rounds. A round lasts at most 180 seconds, and at time over the robot that lost less life wins. See [A.G.W.S. Battle: rounds and results](game/minigames/agws-battle.md#rounds-and-results).

**RSRC arena**: A block of memory managed by `src/main/rsrc.c`, with a heap at the bottom and a table of named items at the top. Each item is an anonymous allocation or a file, found by a key. See [Memory management: RSRC arenas](architecture/memory.md#rsrc-arenas).

**RSSD**: The IOP module `RSSD.IRX` (internal name PS2_SSDRemote), the bridge between the EE and SSD. It runs the SIF RPC server on port `0x900` that receives the EE's command packets and calls SSD. Its client thread passes SSD callbacks back to the EE on port `0x901`, and it keeps the stream rings. See [IOP sound driver: two modules](architecture/iop-sound-driver.md#1-two-modules).

**RSSD stream ring**: A ring buffer in IOP memory that holds the 4 KiB stream chunks the EE sends until SSD plays them at the playback rate. See [IOP sound driver: RSSD stream rings](architecture/iop-sound-driver.md#16-rssd-stream-rings).

**`RssdRpcPacket`**: The message between the EE and RSSD: a 32-byte header with the command, followed by up to 64 KiB of data. The reply is written back into the same packet. See [IOP sound driver: the RPC protocol](architecture/iop-sound-driver.md#3-the-rpc-protocol).

**RTPS**: Rotate, translate, perspective: the VU0 routine that turns a 3D point into a GS screen position. See [Math helpers: projection to the screen](libraries/math.md#projection-to-the-screen).

**`Runtime.execBattle`**: The script native that starts a battle. It sets the battle map and the monster set in OV01, runs `battleProc` until the battle ends, then rebuilds the actor scene and restores the saved state. Battles started by touching an enemy take another path, through `GameModeCfMain`. See [Battle system: how a battle starts](game/battle.md#how-a-battle-starts) and [Overview: game logic in Java](README.md#8-game-logic-in-java).

## S

**S.Pts**: Skill points, one of the three point pools of a character, at most 9999. They pay for extracting Skills. See [Character growth: points and experience](game/character-growth.md#points-and-experience).

**Sample record**: The 32-byte entry of an SWD wave bank that gives the body offset, the loop word, volume, pan, base pitch, ADSR and the name of one sample. See [Sound files: sample records](formats/sound.md#sample-records).

**Save checksum**: The check value of the save data, a 64-bit sum over all `0x163B0` bytes in which each byte is weighted by its position and a constant `0x7CA` is added per byte. `FileCheckSumCheck` compares it when the save screen lists the slots. See [Save data: the checksum](architecture/save-data.md#the-checksum).

**Save screen (`MenuFileMain`)**: The save and load screen in `src/main/file.c`, which runs in its own loop with mode 0 for saving and mode 1 for loading. While it runs, `MenuCfTaikiPush` sets aside the final render callbacks of the screen below it. See [Save data: the save and load screen](architecture/save-data.md#the-save-and-load-screen).

**`SaveData`**: The single `0x163B0`-byte save image. It is an array of bytes, not a C structure, and each system that owns a part of it adds a fixed offset and casts the result to its own type. See [Save data: the save image](architecture/save-data.md#the-save-image).

**Scanline interpolation**: The softening of the flicker that one-line details cause on an interlaced display. The global `ScanLineInterpolate` selects mode 0, 1 or 2, and the default mode 1 lets the display hardware blend the lines at no cost. See [Rendering pipeline: scanline interpolation](architecture/rendering.md#scanline-interpolation).

**SCEDummy**: The placeholder Scene class of 72 scene volumes. After a pre-rendered movie it sets an event flag and moves on. See [Scripts and scene archives: scene volumes](formats/scripts-and-archives.md#scene-volumes).

**Scenario**: The source's name for the playback of an attack after it has been computed (`scenarioBatExec`). See [Battle system: attack playback](game/battle.md#attack-playback).

**Scenario number**: The story position, taken as the highest set story flag in fixed ranges (`MenuScenarioNoGet`). See [Main menu: story position](game/main-menu.md#story-position).

**Scene archive**: A file with the extension `.a` that holds all files of a field scene in the order its resource script reads them, for example `data\scene\cf0010.a`. `GameResourceLoad` reads it in one piece to the top of memory, so the scene loads without many seeks on the disc. See [Files and resources: scene archives](architecture/files-and-resources.md#scene-archives) and [Scripts and scene archives: scene archives](formats/scripts-and-archives.md#scene-archives-a).

**Scene archive header overflow**: The end-offset table of a scene archive with more than 63 entries runs past `archeader` into `GameResourceWorkReloadTable`. See [Scripts and archives: header overflow](formats/scripts-and-archives.md#header-overflow).

**Scene resource script**: The text file `data\scene\cfNNNN.txt` or `evNNNN.txt` of a place or cutscene. Between `#scn` and `#end` it lists load commands and a `.txa` archive, and its `script` command loads the script archive of the place. See [Events and cutscenes: scene resource scripts](scripting/events.md#scene-resource-scripts), [Files and resources: scene resource scripts](architecture/files-and-resources.md#scene-resource-scripts) and [Scripts and scene archives: scene resource scripts](formats/scripts-and-archives.md#scene-resource-scripts-txt).

**Scene script**: A Java class that extends `xeno/Scene`. It is a cutscene, and its `play()` method runs in the CF event mode. See [Scenes, stages and time charts: scene and stage classes](scripting/scenes-and-timecharts.md#scene-and-stage-classes).

**Scene window**: A frameless, outlined and centred subtitle window, used for `xeno.Scene.msg`. See [User interface and windows: drawing a text window](libraries/ui-windows.md#drawing-a-text-window).

**`sceneObserver2`**: The task that ends a cutscene, when `play()` returns or the player skips the scene. See [Events and cutscenes: the end of a cutscene](scripting/events.md#the-end-of-a-cutscene).

**Scheduler record**: The part of an effect script table that holds four timeline offsets and the descriptors of the effect slots. See [Effect data: scheduler records](formats/effects.md#scheduler-records).

**Scratchpad**: The 16 KiB of fast memory of the EE at `0x70000000`. The game has no allocator for it. Each routine that needs fast temporary space uses fixed offsets of its own, such as the joint work area `JntWork`. See [Memory management: the scratchpad](architecture/memory.md#the-scratchpad).

**Screen mask**: The `FX_ScreenMask` task for screen fades. Fades that scripts request with `Effect.call` go through `FX_call`. See [sef: screen mask and fades](libraries/sef.md#screen-mask-and-fades).

**Script archive**: A PDB volume (magic `FL00`) with the extension `.evt` that holds compiled Java classes. `system.evt` holds the VM library, `base.evt` the shared game classes, and each place and cutscene has its own `STnnnn.evt` with its Stage or Scene class. The VM reads the classes directly from the loaded image. See [Java VM: where the classes come from](scripting/java-vm.md#where-the-classes-come-from), [Events and cutscenes: script files and script databases](scripting/events.md#script-files-and-script-databases) and [Scripts and scene archives: Java class files](formats/scripts-and-archives.md#java-class-files).

**Script database (`scriptDB`)**: The two `ScriptDbEntry` slots in `src/main/script.c` that hold the script archives of scenes. A new script always loads into the inactive slot, and starting it flips `currentScriptDB`, so a scene can load its successor while it still plays. See [Events and cutscenes: script files and script databases](scripting/events.md#script-files-and-script-databases).

**Script thread**: A record (`JThread`) that holds one Java call stack and the state of any wait. Script threads are not operating system threads. The VM runs them one after another on the control thread, once per frame. See [Java VM: script threads](scripting/java-vm.md#script-threads) and [Overview: game logic in Java](README.md#8-game-logic-in-java).

**`SCRIPT_debugMenu` and EvtTools**: A script debug menu that nothing calls, with the entries ScriptExec, BgChange, VirtualM, Tool, Light, MemoryInfo and Wind, and its cursor tool EvtTools (game mode 7). See [Debug tools: the script debug menu and the event tools](game/debug-tools.md#6-the-script-debug-menu-and-the-event-tools).

**SE queue**: The per-frame list of sound effect commands that the EE collects and sends to the IOP in one batch, the SE packet. See [Audio: sound effects and the SE queue](architecture/audio.md#9-sound-effects-and-the-se-queue).

**SED**: The sound effect file, with the magic `seds`. It holds the effect sequences of an effect bank, and an effect id is the bank id shifted left by 16 plus the index. It forms the bank together with an SWD wave bank of the same name. See [Audio: sound file formats](architecture/audio.md#6-sound-file-formats) and [Sound files: SED effect files](formats/sound.md#sed-effect-files).

**sef**: The visual effect library of the engine (`sef*` functions). `GameDrawSync` only steps the effects, and the render thread draws them in three passes. See [sef](libraries/sef.md).

**`seffect.esp`**: The name table of the effects: 3072 entries of 16 bytes, one per effect number. Despite its extension it holds no images. See [Effect data: the name table](formats/effects.md#the-name-table-seffectesp).

**Segment map**: Event item 11, a map of 18 segments that shows which locations have been found and which rare items have been taken. It is probably the in-menu view of the Segment Addresses. See [Main menu: event items, passports and the segment map](game/main-menu.md#event-items-passports-and-the-segment-map).

**Sequence channel**: One of the eight EE sound slots for music. Each holds an SMD sequence together with its own wave bank. See [Audio: music sequences](architecture/audio.md#8-music-sequences).

**Settlement (`seisan`)**: The battle result screen, shown by OV01 when a battle is won. See [Character growth: battle results](game/character-growth.md#battle-results).

**Shadow kinds**: The shadow types of an actor (`shadow_kind`): 1 circle, 2 round, 3 drop circle, 4 stencil shadow volume, and 5 to 10 texture-map shadows drawn by nml. See [Actors: shadows and footsteps](libraries/actors.md#shadows-and-footsteps).

**Shape binding**: A type-3 pool entry of a JNT skeleton that makes the z translation of a joint drive a shape (morph) target of the model, probably for the mouth and the eyelids. See [Skeletons and motion: rotation limits and shape bindings](formats/skeletons-and-motion.md#rotation-limits-and-shape-bindings).

**Shape blend**: The morph of a LEX block. The VU1 program mixes the pairs of morph targets by the shape weight and writes the result over the positions and normals, or over the colors when the block's `shapeFlags` byte is 2. See [nml: how a program runs](libraries/nml.md#how-a-program-runs) and [Models and textures: shape bytes](formats/models-and-textures.md#shape-bytes).

**Shape bytes**: The bytes of each LEX block that give its math mode and parameters. `nmlModelLexBlockDoit` reads them. See [Models and textures: shape bytes](formats/models-and-textures.md#shape-bytes).

**Shop discount level**: One of five levels in the save data that lower the prices of one price class to `1.0 - 0.1 × level` of the normal price, never below half. Reading certain U.M.N. mails raises a level. See [U.M.N.: investments and shop discounts](game/umn.md#investments-and-shop-discounts).

**`shopdata.bin`**: The shop file, with `0x170`-byte shop records: nine columns of 20 item ids and four A.G.W.S. units for sale. See [Character growth: opening a shop](game/character-growth.md#opening-a-shop) and [Game data tables: shop lists](formats/game-tables.md#shop-lists-shopdatabin).

**Shot thread (`RgShotThread`)**: A small state machine for each weapon slot of an A.G.W.S. Battle robot, with the states IDLE, SETUP, SHOOT, STOP and DISABLED. It plays the shooting motion and fires at a set time. See [A.G.W.S. Battle engine: weapons and shot threads](game/minigames/agws-battle-engine.md#weapons-and-shot-threads).

**SIF**: The link between the EE and the IOP. The EE sends requests to IOP modules through SIF remote procedure calls (SIF RPC), for example the sound commands that RSSD receives. See [Overview: programs on the disc](README.md#1-programs-on-the-disc) and [IOP sound driver: the RPC protocol](architecture/iop-sound-driver.md#3-the-rpc-protocol).

**Singleton registry (`RgSingletonManager`)**: The registry of 15 slots for the shared services of A.G.W.S. Battle. `RgSingletonIDGet` returns a service, and the services are destroyed in reverse order of registration. See [A.G.W.S. Battle engine: singletons](game/minigames/agws-battle-engine.md#singletons).

**Skeleton**: The joint file of a model (`.jnt`): a 16-byte header, an attribute block and 64-byte joint records of types 0 to 6. See [Animation: the skeleton file](libraries/animation.md#the-skeleton-file) and [Skeletons and motion: skeleton files](formats/skeletons-and-motion.md#skeleton-files-jnt).

**Skill extraction**: Learning the skill of an accessory with S.Pts, which also raises the character's skill level. See [Character growth: extracting a skill](game/character-growth.md#extracting-a-skill).

**Small movie (`mvsNNNN.ipu`)**: An IPU clip in `data\movie\small\` that a monitor unit shows. The resource command `loadmovie` preloads it. See [Movies, images and icons: where the IPU movies are](formats/movies-images-and-icons.md#31-where-they-are-and-who-plays-them).

**`smAlloc`**: The best-fit allocator of the effect pool, the static array `_eftBuffer` of `0xD4800` bytes. It rounds every block up to 2 KiB, a whole number of disc sectors. See [Memory management: smAlloc, the effect pool](architecture/memory.md#smalloc-the-effect-pool).

**SMD**: The sound file format of music and ambience sequences, with the magic `smdm`, played on a sequence channel. See [Audio: sound file formats](architecture/audio.md#6-sound-file-formats) and [Sound files: SMD music files](formats/sound.md#smd-music-files).

**Snapshots**: The developer screen capture, which writes JPEG files to `host0:/home/xeno/work/snap/`. See [Debug tools: snapshots](game/debug-tools.md#snapshots) and [Save data: thumbnails and snapshots](architecture/save-data.md#thumbnails-and-snapshots).

**Soft-float**: The libgcc routines (`dpadd`, `dpmul`, `dptofp` and others) that carry out `double` arithmetic, because the EE's FPU handles only single precision. See [Runtime libraries: soft-float](architecture/runtime-libraries.md#soft-float).

**Sony SDK**: The PlayStation 2 libraries of release 2.4.x linked into the executable, used by only a few game files, almost always inside xgl. See [Runtime libraries: the Sony libraries](architecture/runtime-libraries.md#the-sony-libraries) and [Overview: layers](README.md#9-layers).

**Sound file checksum**: The header word at `+0x04` of the SWD, SED and SMD files. It makes all the 32-bit words of the file add up to zero. See [Sound files: the common header](formats/sound.md#the-common-header).

**Sound ID**: The number that plays a sound effect, `(bank << 16) | index`. Before sending the command, the xgl layer replaces the bank number with the wave handle of that bank. See [Audio: sound effects and the SE queue](architecture/audio.md#9-sound-effects-and-the-se-queue).

**`SoundWork`**: The `0x8A4`-byte global that holds the EE side of sound. It keeps only the 16-bit handles that the IOP driver returns for loaded data, never pointers into IOP memory. See [Audio: SoundWork](architecture/audio.md#4-soundwork).

**Spawn list**: The up to five entries of a monster set, each with a unit id, a position, an attack script and a damage script. See [Game data tables: spawn lists](formats/game-tables.md#spawn-lists).

**Special table**: The keys of a screen filter in an effect script table. See [Effect data: special tables](formats/effects.md#special-tables).

**Special timeline**: A key track of an effect that drives the screen filter layers while the effect plays. See [sef: special timeline (screen filters)](libraries/sef.md#special-timeline-screen-filters).

**Sprite id**: The 16-bit id of a UI sprite. The high byte selects the UV table and the low byte the entry. See [User interface and windows: sprite ids and UV tables](libraries/ui-windows.md#sprite-ids-and-uv-tables).

**SPU ADPCM packet**: The 16-byte unit of the SPU2 sound format: 28 samples of 4 bits, a filter and shift byte and the end, repeat and loop-start flags. See [Sound files: SPU ADPCM](formats/sound.md#spu-adpcm).

**SPU2**: The sound chip of the PlayStation 2, with 48 voices, 2 MB of sound RAM and reverb on each of its cores. See [IOP sound driver: memory](architecture/iop-sound-driver.md#5-memory).

**SSD**: The IOP sound engine `SSD.IRX` (internal name PS2_SuzukiSoundDriver). It runs the sequencer, the voices, the sound RAM, the streams and the reverb from a 2 ms hardware timer and from interrupts, and has no thread of its own. See [IOP sound driver: two modules](architecture/iop-sound-driver.md#1-two-modules).

**SShd and SSbd**: The two little-endian headers at the start of the audio stream of a PSS movie. They give the format, the rate, the channels, the interleave and the body size. See [Movies, images and icons: the audio stream](formats/movies-images-and-icons.md#25-the-audio-stream).

**Stage script**: A Java class that extends `xeno/Stage`. It is the field script of a place and answers talks, touches, `entered` calls and `CallMethod` calls. See [Events and cutscenes: overview](scripting/events.md#overview) and [Scenes, stages and time charts: scene and stage classes](scripting/scenes-and-timecharts.md#scene-and-stage-classes).

**Stealth filter**: An nml filter (`nmlModelSetFilterStealth`) that draws strips around ten pairs of joints, textured with a copy of the scene. It appears to make a cloaked figure that bends the scene behind it. Actors select it through the draw hook `ACT_filterStealth`. See [nml: filters](libraries/nml.md#filters) and [Actors: draw hooks and filters](libraries/actors.md#draw-hooks-and-filters).

**STool**: A shared helper class compiled into 16 scene volumes. See [Scripts and scene archives: attributes and debug data](formats/scripts-and-archives.md#attributes-and-debug-data).

**Stream id**: The number passed to `Sound.streamPlay`. A stereo id is 1,000,000 plus six digits and a mono id is the six digits alone, which give the file name. See [Sound files: ids and file names](formats/sound.md#ids-and-file-names).

**Stream ring buffer**: A ring buffer for music, voices or movies read from disc. The CD thread refills it on each turn while the sound or movie code takes data out. Only one stream can be open at a time. See [xgl: streaming](libraries/xgl.md#streaming).

**Stub class**: A 24-byte class file with an empty constant pool. It is left for an interface of constants whose values the compiler copied into the classes that use them. See [Scripts and scene archives: empty stub classes](formats/scripts-and-archives.md#empty-stub-classes).

**Studio**: A set of cameras and lights in xgl. There are four studios. The studio thread updates their cameras each turn, except on the field, where `GameDrawSync` updates them itself. See [xgl: studios](libraries/xgl.md#studios).

**Studio thread**: System thread 2, with the entry point `xglStudioEntry`. On each turn it updates the cameras of every studio, unless the field game has taken over the camera update. See [Overview: the four system threads](README.md#5-the-four-system-threads) and [xgl: studios](libraries/xgl.md#studios).

**Sub-stream tag**: The 4 bytes that start each private-stream-1 payload of a PSS movie. `FF A1 00 00` marks ADPCM channel 0, which is how the MPEG library routes the audio. See [Movies, images and icons: the audio stream](formats/movies-images-and-icons.md#25-the-audio-stream).

**Sub-window**: One of the extra views 1 to 3 that nml can draw as smaller rectangles on screen, each with the active camera of the studio of the same number. View 0 is the main view. See [nml: sub-windows](libraries/nml.md#sub-windows).

**Subcon**: The developers' name for a module of helper functions next to a main module, probably short for "sub-control". It is not a game feature. See [A.G.W.S. Battle: the subcon modules](game/minigames/agws-battle.md#the-subcon-modules).

**Subweapon Attack**: A battle attack with the second weapon, chosen with Circle alone, for 6 AP. See [Battle system: player input and Tech Attacks](game/battle.md#player-input-and-tech-attacks).

**Suspension by unwinding**: How a script waits without blocking the game. A native marks its thread as waiting and returns, every interpreter level returns at once, and on a later frame the interpreter replays its way back down (`resume_frames`) to the point after the native. See [Java VM: waiting and resuming](scripting/java-vm.md#waiting-and-resuming).

**SWD**: The sound file format of wave banks, with the magic `swdm`. It holds the ADPCM samples, their sample records and the programs. See [Audio: sound file formats](architecture/audio.md#6-sound-file-formats) and [Sound files: SWD wave banks](formats/sound.md#swd-wave-banks).

**Symbol**: An interactive field point that is a map unit: a save symbol (type 6), a shop symbol (type 7), an EVS or event symbol (type 8) or a return symbol (type 9). `CheckGameSymbol` sets a game flag while the player stands on one. `checkTalkPoint` then acts on CIRCLE: it opens the save screen, opens the shop, or calls `evsExit` on a return symbol. See [Maps and units: symbols](game/map-and-units.md#symbols-save-shop-evs-and-return) and [Events and cutscenes: event symbols](scripting/events.md#event-symbols).

**Symbol encounter**: A battle started by contact with a visible field enemy. No random encounter code was found. See [Field enemies: starting a battle](game/field-enemies.md#starting-a-battle).

**System threads**: The four EE threads that run the game in a fixed round-robin: render, CD, studio and control. Each thread does one step of its work and then calls `xglSleep`, so one full turn of the four threads makes one frame. Because only one thread runs at a time and each gives up control on purpose, they share data without locks. See [Overview: the four system threads](README.md#5-the-four-system-threads) and [xgl: threads](libraries/xgl.md#threads).

## T

**T.Pts**: Tech points, one of the three point pools of a character, at most 9999. They raise the basic parameters (Use T.Pts) and pay for the Tech Attack upgrades. See [Character growth: points and experience](game/character-growth.md#points-and-experience).

**Talk task**: The task that runs a conversation started on the field. `createTalkTask` and `talktoObserver` run the Stage's talk method on `stageVM`. See [Events and cutscenes: talking to characters](scripting/events.md#talking-to-characters).

**Task**: A callback that runs once per frame. Most screens of the game are built from tasks, such as a menu cursor or a pop-up window. Each task lives in a 128-byte slot of a task scheduler. See [xgl: tasks](libraries/xgl.md#tasks).

**Task scheduler (`XglTaskScheduler`)**: One block of memory with a small header and a fixed pool of 128-byte task slots. Nothing is allocated after it is created, and `xglTaskExecute` runs its tasks once per frame. See [xgl: data structures](libraries/xgl.md#data-structures).

**TComponent**: One `0x5B0`-byte TWSYS slot, which holds a text window, a menu or a slider. See [User interface and windows: slots, kinds and groups](libraries/ui-windows.md#slots-kinds-and-groups).

**Tech Attack**: A battle attack entered as a sequence of Square and Triangle presses ending with Circle (`keyCmdTblCha`). It costs 6 AP, or 4 for a sequence of two presses. See [Battle system: player input and Tech Attacks](game/battle.md#player-input-and-tech-attacks).

**TECH, SPEED and WAIT**: The three upgrades that Use T.Pts offers for a Tech Attack, each paid with T.Pts. See [Character growth: improving a technique](game/character-growth.md#improving-a-technique).

**Technique record**: The `0x18`-byte record of any action in `batdat.bin`: a normal attack, a Tech Attack, an enemy attack, an Ether or a weapon attack. See [Game data tables: technique records](formats/game-tables.md#technique-records).

**Tentacle**: A reach attack of field enemies whose radius grows over frames 5 to 25. It starts an encounter when it reaches the player. See [Field enemies: seeing and hearing the player](game/field-enemies.md#seeing-and-hearing-the-player).

**Territory**: The floor area that a field enemy may not leave, given by `EneTerritoryID[wall_attribute]`. See [Field enemies: territories and movement checks](game/field-enemies.md#territories-and-movement-checks).

**Text tag**: A `/[...]` code in script text, such as `/[waitkey(n)]`, `/[wait(n)]`, `/[clear()]`, `/[close()]` and `/[label()]`, or a tag for colour, font, ruby or gaiji characters. MIF2 converts the tags. See [Events and cutscenes: text tags](scripting/events.md#text-tags).

**Texture area**: The 512 KiB of video memory from block `0x3800` up, shared by models, the font, shadows and windows. Textures are not resident. Each user uploads its texture inside the packet just before it draws. See [Rendering pipeline: textures](architecture/rendering.md#textures).

**TEXUTL list**: The text list of texture pieces that `xtxdec_sub` parses. See [Models and textures: the TEXUTL list](formats/models-and-textures.md#the-texutl-list).

**Think map table (`thinkMapTbl`)**: The table that maps a map id to the number of its battle think file. See [Maps and field data: map think data](formats/maps-and-field-data.md#map-think-data).

**Think operand**: A 16-bit operand of the think VM. It is a literal, a register (bit 15) or an indirect access to the file (bits 15 and 14). See [Game data tables: instruction encoding](formats/game-tables.md#instruction-encoding).

**Think VM**: The OV01 bytecode machine that runs the enemy AI and battle camera scripts, with 99 opcodes and 16 processes. See [Battle system: enemy AI and the think VM](game/battle.md#enemy-ai-and-the-think-vm) and [Game data tables: enemy AI programs](formats/game-tables.md#enemy-ai-programs-think-files).

**Thread kind**: The kind of a script thread: 0 SYSTEM, 1 CHR, 2 STAGE, 3 SCENE, 4 DEFAULT or 5 UNIT. See [Java VM: script threads](scripting/java-vm.md#script-threads).

**Thumbnail**: The small JPEG picture of the screen, 128 by 112 pixels, stored in each save. It is taken at the moment the save screen opens. See [Save data: thumbnails and snapshots](architecture/save-data.md#thumbnails-and-snapshots) and [Movies, images and icons: save thumbnails](formats/movies-images-and-icons.md#8-save-thumbnails).

**Tick**: The time unit of the sound sequences, 1/96 of a beat. A whole note lasts 384 ticks. See [Sound files: tempo and timing](formats/sound.md#tempo-and-timing).

**Time chart (TCH)**: A block of named records of FCV2 curves whose keys mark moments in an event. See [Scenes, stages and time charts: time chart data](scripting/scenes-and-timecharts.md#time-chart-data).

**Time chart observer**: A script method that PlayControl calls at the keys of a time chart curve, with a `TCHParams` argument. See [Scenes, stages and time charts: time chart data](scripting/scenes-and-timecharts.md#time-chart-data).

**Title selection value**: The 32-bit value that `Title()` returns: the overlay number in bits 24 to 31 and the address of the next screen's function in bits 0 to 23. A value of 0 keeps the player on the title screen. See [Boot and title screen: the control loop and the title return value](game/title-and-boot.md#3-the-control-loop-and-the-title-return-value) and [Overview: from the title screen to the game](README.md#6-from-the-title-screen-to-the-game).

**`title_dummy_table`**: An unreferenced table of 21 words in OV02 that lists developer test programs as title selection values, probably a launcher of the developers. See [Debug tools: the developer launcher](game/debug-tools.md#3-the-developer-launcher-title_dummy_table).

**`TitleGameInit`**: The function that prepares the save data and the game loop state of a new session. See [Boot and title screen: preparing a new session](game/title-and-boot.md#7-preparing-a-new-session-titlegameinit).

**TMENU**: The TWSYS script choice menu, with an optional question and up to 16 items. See [User interface and windows: script menus (TMENU)](libraries/ui-windows.md#script-menus-tmenu).

**TMPGEnc**: The PC MPEG-2 encoder whose name is in the user data of 103 cutscene movies. See [Movies, images and icons: the video stream](formats/movies-images-and-icons.md#24-the-video-stream).

**`Toolkit.getPeer`**: The native that creates the engine peer of a `Chr`, `Unit`, `Effect` or `Stage` object together with its script thread. See [Native methods: peers created by Toolkit.getPeer](scripting/natives.md#peers-created-by-toolkitgetpeer).

**Touch**: A talk started by walking into a character that has `SCRIPT_FLAG_TOUCH` set. See [Events and cutscenes: touching characters](scripting/events.md#touching-characters).

**Toumei**: Transparency in nml, from the Japanese word for "transparent". The `TOUMEI` status draws a whole model as transparent, and `nmlModelSetToumeiParts` sets the alpha of one part. See [nml: the layout store](libraries/nml.md#the-layout-store).

**Transfer**: The Ether command that gives a copy of an ether to another party member for half its E.Pts cost. A transferred ether cannot be evolved or transferred again. See [Character growth: commands](game/character-growth.md#commands).

**Trap**: A map object (`0x7001` to `0x7004`) that a shot sets off (signal 1) or that a bump only breaks (signal 2). A trap that goes off hurts the enemies around it. See [Maps and units: traps](game/map-and-units.md#traps) and [Field enemies: trap damage](game/field-enemies.md#trap-damage).

**TSLIDER**: The TWSYS number entry slider. See [User interface and windows: sliders (TSLIDER)](libraries/ui-windows.md#sliders-tslider).

**TWIN**: A TWSYS text window. It types out its text, waits for Circle, scrolls and closes. See [User interface and windows: text windows (TWIN)](libraries/ui-windows.md#text-windows-twin).

**TWSYS**: The window toolkit of the scripts, with four component slots that `TWSYS_update` updates and `TWSYS_draw` draws. See [User interface and windows: components (TWSYS)](libraries/ui-windows.md#components-twsys).

**`tya` helpers**: Helper functions of one programmer used in several parts of the game, such as gauges, the display offset screen and the moving background of the main menu. See [User interface and windows: tya helpers](libraries/ui-windows.md#tya-helpers).

## U

**U-kun Ball**: A container that the Driller Game's drill breaks, a map unit with a serial number from `0x7014` to `0x7027` that holds items 501 to 545. See [Driller Game: containers and their items](game/minigames/driller-game.md#6-containers-and-their-items).

**U.M.N.**: The Unus Mundus Network, the in-game network terminal in OV02, with the services Mail, Database, EVS and Plug-in. The main menu opens it, and scripts can open its mail screen alone with `Runtime.mailExec`. See [U.M.N.: how the game enters and leaves the U.M.N.](game/umn.md#1-how-the-game-enters-and-leaves-the-umn) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**U.M.N. arena**: The `0x480000` bytes of the menu model memory that the U.M.N. borrows for itself. See [U.M.N.: UmnInit and the arena](game/umn.md#umninit-and-the-arena).

**UKUN**: See **Procurator**.

**UML file**: A U.M.N. hypertext page, `NNNN.uml`, unlocked by game flags. Its `@(` and `@<` codes stay hidden until Plug-in bits 4 and 5 are set. See [U.M.N.: keywords and the UML text format](game/umn.md#keywords-and-the-uml-text-format) and [Text, fonts and U.M.N. data: UML pages](formats/text-and-fonts.md#uml-pages).

**`UmnMailSaveData`**: The U.M.N. mail block of the save data at `SaveData+0x1025C`, with the mailbox, the shop discount levels, the Plug-in bits and the Database bits. The flags of each mail start at `SaveData+0x10304`. See [U.M.N.: mail data](game/umn.md#mail-data).

**`UmnObjectTask`**: The task record shared by all U.M.N. windows and animations, with the states INIT, RUN and END, run on the `umn_task` and `task_xmx` schedulers. See [U.M.N.: object tasks](game/umn.md#object-tasks).

**`UmnSimulationNo`**: The value that the U.M.N. sets when the player starts an EVS simulation. After the menu closes, the field saves the return point and jumps to the scene in its low halfword and the entry in its high halfword. See [Main menu: U.M.N. Simulation](game/main-menu.md#umn-simulation).

**`UmnWork`**: The 128-byte global of the U.M.N., with the common screen state and a union for the state of each screen. See [U.M.N.: UmnWork and the screen states](game/umn.md#3-umnwork-and-the-screen-states).

**Undulate**: The triangle collision mesh of a map, read from the `.id` file, and the code that queries it (`Undu*` in `src/main/Undulate.c`). It answers ground height and wall queries for actors, units and enemies. See [Maps and units: the collision mesh (Undulate)](game/map-and-units.md#the-collision-mesh-undulate), [Animation: Undulate, ground height and walls](libraries/animation.md#undulate-ground-height-and-walls) and [Maps and field data: collision and layout data](formats/maps-and-field-data.md#collision-and-layout-data-id).

**Undulation data (`.id`)**: The collision and layout file of a map. It holds the walk and camera floors, the locator groups and the attribute zones. See [Maps and field data: collision and layout data](formats/maps-and-field-data.md#collision-and-layout-data-id).

**Unit argument area**: The script-visible work area of a map unit at `+0x1A0` of its record, read and written with `Unit.getArgs` and `Unit.setArgs`. See [Driller Game: the drill as a map unit](game/minigames/driller-game.md#2-the-drill-as-a-map-unit).

**Unit command**: A 20-byte battle command record run through `unitCmdTbl`. Every action of a battle unit, from its movement to its reactions, is a command. See [Battle system: unit commands](game/battle.md#unit-commands).

**Unit record (`orgData`)**: One of the 33 records of `0x180` bytes, one per unit id, with the numbers and equipment of the characters, the BIC soldiers and the A.G.W.S. See [Character growth: unit records](game/character-growth.md#unit-records).

**Unit sequence**: One of 64 interpolation records (`unitSequence[64]`) that move map units for scripts. See [Scenes, stages and time charts: moving map units](scripting/scenes-and-timecharts.md#moving-map-units).

**Update hook**: The `Actor.update` callback that `ACT_update` runs every frame. It decides how the actor moves and usually ends by advancing its motion. Scripts choose it with `Chr.start`. See [Actors: update hooks](libraries/actors.md#update-hooks).

**Upload rectangle**: One GS upload of an XTX texture set, in the CT32 format, carrying part of a pre-swizzled 8-bit or 4-bit texture atlas. See [Models and textures: XTX texture sets](formats/models-and-textures.md#xtx-texture-sets).

**Use T.Pts**: The menu command that raises one of the eight basic parameters of a character with T.Pts. See [Character growth: Use T.Pts](game/character-growth.md#use-tpts).

**Uwamono**: The map objects with game rules, such as doors, traps, item boxes and symbols, which use 14 resident models. See [Maps and units: map objects (uwamono)](game/map-and-units.md#map-objects-uwamono).

## V

**VAG stream**: ADPCM audio streamed from `.vdm` and `.vds` files and played through the normal SPU2 voices 44 to 47. It supports up to four mono channels, and a stereo stream uses two of them. See [IOP sound driver: the VAG stream](architecture/iop-sound-driver.md#15-the-vag-stream) and [Sound files: VAG streams](formats/sound.md#vag-streams).

**Value mask**: The 10-bit field of a curve set channel that says which components (translation, rotation, scale and flag) have a key header. See [Skeletons and motion: channel records](formats/skeletons-and-motion.md#channel-records).

**Vertex batch**: One UNPACK set plus one MSCNT run of the VU1 program over at most one VU buffer of vertices. See [Models and textures: vertex batches](formats/models-and-textures.md#vertex-batches).

**`virtualMachine`**: The bytecode interpreter of the Java VM, a `switch` loop that runs one method. See [Java VM: the interpreter](scripting/java-vm.md#the-interpreter).

**Vision fan**: The sector of a circle in front of an enemy in which it notices the player. `Check_InsideFan_Wooo` is the simple test, and `Check_InsideFan` adds a line of sight check against the map. See [Math helpers: vision fans](libraries/math.md#vision-fans).

**VMRegister**: The 32 global integer registers of the scripts. See [Events and cutscenes: event flags and registers](scripting/events.md#event-flags-and-registers).

**VMSlot and VMFrame**: The 4-byte stack slot and the `0x18`-byte frame record of the interpreter. See [Java VM: the interpreter](scripting/java-vm.md#the-interpreter).

**Voice allocation**: How SSD picks a voice for a new note (`SsdGetKeyonVoice`). It takes a free voice first, then a released voice with the quietest envelope, then a voice of a lower-priority channel. See [IOP sound driver: voice allocation](architecture/iop-sound-driver.md#voice-allocation).

**VU0**: The first vector unit of the EE. The game uses it in macro mode, as a coprocessor inside the EE's own instruction stream, and in micro mode, with its resident microprogram, for matrices, lighting and projection. See [Math helpers: macro mode](libraries/math.md#macro-mode), [Math helpers: the resident VU0 microprogram](libraries/math.md#the-resident-vu0-microprogram) and [Overview: programs on the disc](README.md#1-programs-on-the-disc).

**VU1**: The second vector unit of the EE. It runs the seven microcode families of the model library, which turn model data into GS primitives. See [nml: VU1 microcode](libraries/nml.md#vu1-microcode) and [Overview: programs on the disc](README.md#1-programs-on-the-disc).

## W

**W-ACT**: An A.G.W.S. attack with the same weapon on both arms. See [Battle system: A.G.W.S. in battle](game/battle.md#agws-in-battle).

**W-Item**: Using two items in one action for 6 AP. See [Battle system: the battle menu](game/battle.md#the-battle-menu).

**Wait kind**: What a waiting script thread waits for: 1 a number of frames, 2 another thread to finish, 3 a window or menu, 4 and 6 an actor to become idle, 5 a signal and 7 a game flag. See [Java VM: waiting and resuming](scripting/java-vm.md#waiting-and-resuming).

**WaveBankType**: The SED and SMD command `0xAB`, which plays a sample of any loaded wave bank. It takes a sample index, then a bank id stored high byte first. See [Sound files: effect tracks](formats/sound.md#effect-tracks).

**Wide ASCII style**: Text style code 25 (style bit 1), which draws ASCII with the full-width or proportional glyphs instead of the 8 × 8 strip. See [XGL: glyphs and fonts](libraries/xgl.md#glyphs-and-fonts).

**Wind**: The global wind of the extra motion (`EXM_WindState`), set by `Runtime.setWindParam`, by `SetUwaWind` and at the start of a battle. See [Animation: wind](libraries/animation.md#wind).

**Window owner**: The speaking actor of a text window. `TW_setPos` with `FOLLOW_OWNER` places the window near that actor. See [User interface and windows: following the speaker](libraries/ui-windows.md#following-the-speaker).

**Window texture**: The UI art sets `cfwin`, `menu01` and `menu02`, XTX files in `data\endou\` that `WindowTexLoad` loads. See [User interface and windows: window textures](libraries/ui-windows.md#window-textures).

**WindowDX**: A titled window with an open animation and a content callback for each of its states. See [User interface and windows: WindowDX](libraries/ui-windows.md#windowdx).

**WindowSP**: A scrolling list window inside a WindowDX. See [User interface and windows: WindowSP list menus](libraries/ui-windows.md#windowsp-list-menus).

**Work memory**: See **`WorkEnd`**.

**`WorkEnd`**: The bump pointer of the work memory, which starts at `0x00A80000`, right after the overlay slot. A loader places its data at `WorkEnd` and moves the pointer past it. Nothing is freed one block at a time. Instead a screen saves `WorkEnd` when it starts and puts it back when it ends, which releases everything the screen loaded. See [Memory management: the work area and WorkEnd](architecture/memory.md#the-work-area-and-workend) and [Overview: memory map](README.md#3-memory-map).

**Wt**: The weight (`capCost`) of an ether. The Wt of all ethers set in a character's 12 ether slots may total at most 12. See [Character growth: ether data](game/character-growth.md#ether-data).

## X

**Xeno Card**: The trading card minigame in OV10, minigame number 13. Decks hold 40 cards, and a player wins by emptying the opponent's deck. See [Xeno Card](game/minigames/xeno-card.md) and [Overview: main menu, U.M.N. and minigames](README.md#7-main-menu-umn-and-minigames).

**xgl**: The system library of the engine (`xgl*` functions): threads, tasks, file reads, rendering, sound, controllers and math. Every other part of the game is built on it. See [xgl](libraries/xgl.md) and [Overview: layers](README.md#9-layers).

**`xglSleep`**: The call that ends a system thread's turn. It wakes the next thread in the round-robin and puts the caller to sleep. Inside a game loop it simply means "wait for the next frame". See [Overview: the four system threads](README.md#5-the-four-system-threads) and [xgl: threads](libraries/xgl.md#threads).

**xheap**: The 256 KiB object heap of the Java VM in `src/main/xheap.c`. It is a bump allocator with mark frames instead of a garbage collector: `xfree` only marks a block as free, and its space is never reused. See [Java VM: the xheap object heap](scripting/java-vm.md#the-xheap-object-heap) and [Memory management: xheap, the Java heap](architecture/memory.md#xheap-the-java-heap).

**XTX**: The nml texture set format. A set starts with `XTX` and holds several images, each with a size, a VRAM offset and its pixel data. See [nml: XTX texture sets](libraries/nml.md#xtx-texture-sets) and [Models and textures: XTX texture sets](formats/models-and-textures.md#xtx-texture-sets).

**`xtxinfo`**: The picture table, entry 0 of an NBXX picture archive. See [Minigame data files: NBXX picture archives](formats/minigame-data.md#nbxx-picture-archives).
