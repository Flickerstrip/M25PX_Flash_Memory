#ifndef FlashMemory_h
#define FlashMemory_h

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
#define READ_STATUS_REGISTER            0x05
#define WRITE_STATUS_REGISTER           0x01
#define WRITE_LOCK_REGISTER             0xe5
#define READ_LOCK_REGISTER              0xe8
#define READ_DATA_BYTES                 0x03
#define READ_DATA_BYTES_FAST            0x0b
#define PAGE_WRITE                      0x0a
#define PAGE_PROGRAM                    0x02
#define PAGE_ERASE                      0xdb
#define SUBSECTOR_ERASE                 0x20
#define SECTOR_ERASE                    0xd8
#define BULK_ERASE                      0xc7
#define DEEP_POWER_DOWN                 0xb9
#define RELEASE_FROM_DEEP_POWER_DOWN    0xab

class FlashMemory {
public:
    FlashMemory(byte, byte, byte, byte);
    byte readStatus();
    bool isBusy();
    bool isWritable();
    void writeByte(uint16_t addr,byte b);
    void writeBytes(uint16_t addr, byte * buf, int len);
    byte readByte(uint16_t addr);
    void readBytes(uint16_t addr, byte * buf, int len);

    void enableWrite();
    void disableWrite();
    void sendAddress(uint16_t addr);

private:
    SoftwareSPI spi;


};

#endif
