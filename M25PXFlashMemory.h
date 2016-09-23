#ifndef M25PXFlashMemory_h
#define M25PXFlashMemory_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftwareSPI.h"

//Instructions
#define WRITE_ENABLE                    0x06
#define WRITE_DISABLE                   0x04
#define READ_IDENTIFICATION             0x9f
#define READ_IDENTIFICATION2            0x9e
#define READ_STATUS_REGISTER            0x05
#define WRITE_STATUS_REGISTER           0x01
#define WRITE_LOCK_REGISTER             0xe5
#define READ_LOCK_REGISTER              0xe8
#define READ_DATA_BYTES                 0x03
#define READ_DATA_BYTES_FAST            0x0b
#define DUAL_OUTPUT_FAST_READ           0x3b
#define READ_OTP                        0x4b
#define PROGRAM_OTP                     0x42
#define PAGE_PROGRAM                    0x02
#define DUAL_INPUT_FAST_PROGRAM         0xa2
#define SUBSECTOR_ERASE                 0x20
#define SECTOR_ERASE                    0xd8
#define BULK_ERASE                      0xc7
#define DEEP_POWER_DOWN                 0xb9
#define RELEASE_FROM_DEEP_POWER_DOWN    0xab

class M25PXFlashMemory {
public:
    M25PXFlashMemory(byte, byte, byte, byte);
    byte readStatus();
    bool isBusy();
    bool isWritable();
    void programByte(uint32_t addr,byte b);
    void programBytes(uint32_t addr, byte * buf, int len);
    byte readByte(uint32_t addr);
    void readBytes(uint32_t addr, byte * buf, int len);

    void readOTP(uint32_t addr, byte * buf, int len);
    void programOTP(uint32_t addr, byte * buf, int len);
    void freezeOTP();

    void eraseSubsector(uint32_t addr);
    void bulkErase();

    void enableWrite();
    void disableWrite();
    void sendAddress(uint32_t addr);
    void programPage(uint32_t addr, byte * buf, int len);
    void readPage(uint32_t addr, byte * buf, int len);

private:
    SoftwareSPI spi;


};

#endif
