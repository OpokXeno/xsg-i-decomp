#include "rssd/rssd.h"
#include "rssd/rssdfunc.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_musc.h"
#include "ssd/ssd_sec.h"
#include "ssd/ssd_smpl.h"
#include "ssd/ssd_wave.h"

/* SPU read / wave upload chunk size. */
#define RSSD_SPU_READ_CHUNK_SIZE 0x10000

/* RSSD passes a third argument that SsdNextWaveData in src/audio/ssd does not declare. */
extern int SsdNextWaveData(void *, int, int);

void RssdSpuDmaTransferCallback(int status, void *argument);
void RssdSamplingDmaCallbackProc(int status, void *argument);
void RssdSamplingKeyoffCallbackProc(int status, int channel, int argument);
void RssdSpuReadCallback(int status, void *argument);
void RssdSpuReadNext(void *);

void RssdInit(RssdRpcPacket *request)
{
    int size;
    void *runtime;
    RssdRpcPacket notification;
    sceSdInit(0);
    size = request->arguments[0];
    CpuDisableIntr();
    runtime = AllocSysMemory(SMEM_Low, size, 0);
    RssdWork.spuRead.transferBuffer = AllocSysMemory(SMEM_Low, RSSD_RPC_BUFFER_SIZE, 0);
    CpuEnableIntr();
    SsdInit(runtime, size, 0);
    sceSifBindRpc(&RssdWork.client, RSSD_RPC_EE_CLIENT_ID, 0);
    while (sceSifCheckStatRpc(&RssdWork.client))
        ;
    if (RssdWork.client.server) {
        notification.arguments[0] = (int)runtime;
        notification.arguments[1] = size;
        notification.arguments[2] = 0x6001f4;
        notification.arguments[3] = 0x10250;
        RssdCallFunc(RSSD_EE_INIT_NOTIFY, &notification, 0, 0);
    }
}

void RssdSpuDirectRead(RssdRpcPacket *request)
{
    int total = request->arguments[1];
    unsigned int address = request->arguments[0];
    int size = total;
    if (size > RSSD_SPU_READ_CHUNK_SIZE)
        size = RSSD_SPU_READ_CHUNK_SIZE;
    SsdSpuDmaRead(address, RssdWork.spuRead.transferBuffer, size, RssdSpuReadCallback, 0);
    RssdWork.spuRead.spuAddress = address;
    RssdWork.spuRead.chunkSize = size;
    RssdWork.spuRead.remaining = total - size;
    printf("Rssd spu read  SpuPtr=%06x  TransSize=%x\n", address, total);
}

void RssdSpuReadCallback(int status, void *argument)
{
    RssdRpcPacket request;
    request.arguments[0] = RssdWork.spuRead.spuAddress;
    request.arguments[1] = RssdWork.spuRead.chunkSize;
    request.arguments[2] = RssdWork.spuRead.remaining;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT) {
        RssdCallFunc(RSSD_CMD_SPU_DIRECT_READ, &request, 0, 0);
    } else {
        RssdSetFuncCallback(RssdSpuReadNext, &RssdWork);
        iRssdCallFunc(RSSD_CMD_SPU_DIRECT_READ, &request, RssdWork.spuRead.transferBuffer, RssdWork.spuRead.chunkSize);
    }
}

void RssdSpuReadNext(void *argument)
{
    /* The original walks the transfer words through a pointer to the last one. */
    unsigned int size;
    unsigned int *remainingField = &RssdWork.spuRead.remaining;
    int remaining = *remainingField;
    unsigned int address;
    if (!remaining)
        return;
    address = remainingField[-2] + remainingField[-1];
    RssdWork.spuRead.spuAddress = address;
    if (remaining <= RSSD_SPU_READ_CHUNK_SIZE)
        size = remaining;
    else
        size = RSSD_SPU_READ_CHUNK_SIZE;
    {
        unsigned int *counts = remainingField - 1;
        remainingField[-1] = size;
        counts[1] -= size;
        SsdSpuDmaRead(address, (void *)counts[-2], size, RssdSpuReadCallback, 0);
    }
}

void RssdAddWaveData(RssdRpcPacket *request)
{
    unsigned char *data;
    int bank_id;
    SsdWaveBank *bank;
    SsdMemoryCopy(RssdWork.spuRead.transferBuffer, request, request->dataSize + sizeof(RssdRpcPacket));
    data = RssdWork.spuRead.transferBuffer->data;
    SsdSetWaveTransCallback(RssdSpuDmaTransferCallback, 0);
    bank_id = SsdAddWaveData(data, request->arguments[1], request->arguments[2]);
    bank = 0;
    if (bank_id >= 0)
        bank = SsdGetWSPtrBank(bank_id);
    request->value = bank_id;
    request->arguments[0] = (int)bank;
    if (bank) {
        request->arguments[1] = bank->spu_address;
        request->arguments[2] = bank->body_size;
    }
}

void RssdNextWaveData(RssdRpcPacket *request)
{
    unsigned char *data;
    int bank_id;
    SsdWaveBank *bank;
    SsdMemoryCopy(RssdWork.spuRead.transferBuffer, request, request->dataSize + sizeof(RssdRpcPacket));
    data = RssdWork.spuRead.transferBuffer->data;
    SsdSetWaveTransCallback(RssdSpuDmaTransferCallback, 0);
    bank_id = SsdNextWaveData(data, request->arguments[1], request->arguments[2]);
    bank = 0;
    if (bank_id >= 0)
        bank = SsdGetWSPtrBank(bank_id);
    request->value = bank_id;
    request->arguments[0] = (int)bank;
    if (bank) {
        request->arguments[1] = bank->spu_address;
        request->arguments[2] = bank->body_size;
    }
}

void RssdSpuDmaTransferCallback(int status, void *argument)
{
    RssdRpcPacket request;
    request.arguments[0] = status;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT) {
        RssdCallFunc(RSSD_EE_WAVE_TRANSFER_DONE, &request, 0, 0);
    } else {
        iRssdCallFunc(RSSD_EE_WAVE_TRANSFER_DONE, &request, 0, 0);
    }
}

void RssdPlaySequenceData(RssdRpcPacket *request)
{
    request->value = SsdPlaySequence(request->data, request->arguments[1], request->arguments[2]);
}

void RssdAddSequenceData(RssdRpcPacket *request)
{
    request->value = SsdAddSequenceData((SsdMusicData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

void RssdAddSeqEffectData(RssdRpcPacket *request)
{
    request->value = SsdAddSeqEffectData((SsdMusicData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

void RssdAddEffectData(RssdRpcPacket *request)
{
    request->value = SsdAddEffectData((SsdEffectData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

void RssdInitSampling(RssdRpcPacket *request)
{
    SsdInitSampling(request->arguments[0], request->arguments[1], request->arguments[2]);
    SsdSetSampleDmaCallback(RssdSamplingDmaCallbackProc);
    SsdSetSampleKeyoffCallback(RssdSamplingKeyoffCallbackProc);
}

void RssdTransferSampling(RssdRpcPacket *request)
{
    SsdTransferSampling(request->arguments[0], request->data, request->arguments[2]);
}

void RssdTransferSamplingNext(RssdRpcPacket *request)
{
}

void RssdSamplingDmaCallbackProc(int status, void *argument)
{
    RssdRpcPacket request;
    request.arguments[0] = status;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT)
        RssdCallFunc(RSSD_EE_SAMPLING_DMA_DONE, &request, 0, 0);
    else
        iRssdCallFunc(RSSD_EE_SAMPLING_DMA_DONE, &request, 0, 0);
}

void RssdSamplingKeyoffCallbackProc(int status, int channel, int argument)
{
    RssdRpcPacket request;
    request.arguments[0] = status;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT)
        RssdCallFunc(RSSD_EE_SAMPLING_KEYOFF, &request, 0, 0);
    else
        iRssdCallFunc(RSSD_EE_SAMPLING_KEYOFF, &request, 0, 0);
}
