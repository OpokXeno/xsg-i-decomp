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

/* Initializes the driver over the runtime memory the EE requested, then notifies the EE of its address and size. */
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

/* Starts reading SPU memory at the requested address into the transfer buffer, one chunk at a time. */
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

/* Reports a finished chunk to the EE. In thread context the report carries no data; otherwise it is sent with the chunk's data after RssdSetFuncCallback registers RssdSpuReadNext. */
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

/* Starts the next chunk of a multi-chunk SPU read, at the address after the previous chunk. */
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

/* Copies a wave data request into the transfer buffer and adds it as a wave bank; the bank's address and size go back in the arguments. */
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

/* Like RssdAddWaveData, but passes the request to SsdNextWaveData. */
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

/* Reports a finished wave transfer to the EE. */
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

/* Starts the sequence in the request and stores its result. */
void RssdPlaySequenceData(RssdRpcPacket *request)
{
    request->value = SsdPlaySequence(request->data, request->arguments[1], request->arguments[2]);
}

/* Adds sequence data and returns the free memory size and block count in the arguments. */
void RssdAddSequenceData(RssdRpcPacket *request)
{
    request->value = SsdAddSequenceData((SsdMusicData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

/* Adds sequence effect data and returns the free memory size and block count in the arguments. */
void RssdAddSeqEffectData(RssdRpcPacket *request)
{
    request->value = SsdAddSeqEffectData((SsdMusicData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

/* Adds effect data and returns the free memory size and block count in the arguments. */
void RssdAddEffectData(RssdRpcPacket *request)
{
    request->value = SsdAddEffectData((SsdEffectData *)request->data);
    request->arguments[0] = SsdGetMemoryFreeSize();
    request->arguments[1] = SsdGetMemoryBlockCount();
}

/* Initializes sampling and registers the DMA and keyoff callbacks that report to the EE. */
void RssdInitSampling(RssdRpcPacket *request)
{
    SsdInitSampling(request->arguments[0], request->arguments[1], request->arguments[2]);
    SsdSetSampleDmaCallback(RssdSamplingDmaCallbackProc);
    SsdSetSampleKeyoffCallback(RssdSamplingKeyoffCallbackProc);
}

/* Transfers the request data into the sampling area of the channel in the arguments. */
void RssdTransferSampling(RssdRpcPacket *request)
{
    SsdTransferSampling(request->arguments[0], request->data, request->arguments[2]);
}

/* Does nothing. */
void RssdTransferSamplingNext(RssdRpcPacket *request)
{
}

/* Reports a finished sampling DMA to the EE. */
void RssdSamplingDmaCallbackProc(int status, void *argument)
{
    RssdRpcPacket request;
    request.arguments[0] = status;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT)
        RssdCallFunc(RSSD_EE_SAMPLING_DMA_DONE, &request, 0, 0);
    else
        iRssdCallFunc(RSSD_EE_SAMPLING_DMA_DONE, &request, 0, 0);
}

/* Reports a sampling channel's keyoff to the EE. */
void RssdSamplingKeyoffCallbackProc(int status, int channel, int argument)
{
    RssdRpcPacket request;
    request.arguments[0] = status;
    if (status == RSSD_CALLBACK_THREAD_CONTEXT)
        RssdCallFunc(RSSD_EE_SAMPLING_KEYOFF, &request, 0, 0);
    else
        iRssdCallFunc(RSSD_EE_SAMPLING_KEYOFF, &request, 0, 0);
}
