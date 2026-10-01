#ifndef OV01_M_EF_CREATE_BP_00_H
#define OV01_M_EF_CREATE_BP_00_H

typedef struct BP00Visibility {
    unsigned short flag[7];
} BP00Visibility;

typedef struct BP00Trail {
    Vector4 point[7];
} BP00Trail;

typedef struct BP00Work {
    unsigned char unmodeled_00[8];
    unsigned int actorId;
    unsigned int actorPart;
    Vector4 creationOffset;
    unsigned char unmodeled_20[4];
    unsigned int coordinateActorId;
    unsigned char unmodeled_28[0x48];
    int frame;
    int counter;
    unsigned char unmodeled_78[8];
    Vector4 position;
    Vector4 actorCoordinates;
    BP00Visibility visibility[5];
    unsigned char unmodeled_E6[10];
    BP00Trail trail[5];
    Vector4 vectorOrigin[5];
    unsigned char packet[0x14];
} BP00Work;

typedef void BP00Callback(void *self, void *work);

typedef struct BP00Object {
    unsigned char unmodeled_00[4];
    BP00Callback *processCallback;
    unsigned char unmodeled_08[4];
    BP00Callback *drawCallback;
    BP00Callback *postCallback;
    unsigned char unmodeled_14[0x0C];
    BP00Work work;
} BP00Object;

#endif
