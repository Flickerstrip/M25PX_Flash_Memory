#include "M25PXFlashMemory.h"

//Public Methods
M25PXFlashMemory::M25PXFlashMemory(byte sck,byte mosi,byte miso,byte cs) : spi(sck,mosi,miso,cs) {
    spi.begin();
}

byte M25PXFlashMemory::readStatus() {
    spi.select();
    spi.transfer(READ_STATUS_REGISTER);
    byte status = spi.transfer(0);
    spi.deselect();
    return status;
}

bool M25PXFlashMemory::isBusy() {
    return readStatus() & (1 << 0); //LSB is WIP
}

bool M25PXFlashMemory::isWritable() {
    return readStatus() & (1 << 1); //bit2 is WEL
}

void M25PXFlashMemory::programByte(uint32_t addr,byte b) {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PAGE_PROGRAM);
    sendAddress(addr);
    spi.transfer(b);
    spi.deselect();
}

void M25PXFlashMemory::programBytes(uint32_t addr, byte * buf, int len) {
    int i=0;
    while (i < len) {
        int writeSize = 0x100 - (addr & 0xff);
        if (writeSize > len - i) writeSize = len - i;
        //Serial.print("Writing: 0x");
        //Serial.print(addr,HEX);
        //Serial.print("(");
        //Serial.print(writeSize);
        //Serial.println(")");
//        for (int l=0; l<10; l++) {
//            Serial.print("0x");
//            Serial.print(buf[l]);
//            Serial.print(" ");
//        }
//        Serial.println();
        programPage(addr,buf,writeSize);
        addr &= 0xffffff00; //start at the beginning of the page
        addr += 0x00000100; //increment page by one
        buf += writeSize;
        i += writeSize;
    }
}


byte M25PXFlashMemory::readByte(uint32_t addr) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_DATA_BYTES);
    sendAddress(addr);
    byte value = spi.transfer(0);
    spi.deselect();

    return value;
}

void M25PXFlashMemory::readBytes(uint32_t addr, byte * buf, int len) {
    int i=0;
    while (i < len) {
        int readSize = 0x100 - (addr & 0xff);
        if (readSize > len - i) readSize = len - i;
        readPage(addr,buf,readSize);
        addr &= 0xffffff00; //start at the beginning of the page
        addr += 0x00000100; //increment page by one
        buf += readSize;
        i += readSize;
    }
}

void M25PXFlashMemory::readOTP(uint32_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_OTP);
    sendAddress(addr);

    spi.transfer(0); //pass over the dummy byte

    for (int i=0; i<len; i++) {
        buf[i] = spi.transfer(0);
    }
    spi.deselect();
}

void M25PXFlashMemory::programOTP(uint32_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PROGRAM_OTP);
    sendAddress(addr);

    for (int i=0; i<len; i++) {
        spi.transfer(buf[i]);
    }
    spi.deselect();
}

void M25PXFlashMemory::freezeOTP() {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PROGRAM_OTP);

    sendAddress(64); //the write lock

    spi.transfer(0b000); //write 0s to prevent writing

    spi.deselect();
}

void M25PXFlashMemory::eraseSubsector(uint32_t addr) {
    addr = addr & 0xfffff000; //target subsector

    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(SUBSECTOR_ERASE);
    sendAddress(addr);
    spi.deselect();
}

void M25PXFlashMemory::bulkErase() {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(BULK_ERASE);
    spi.deselect();

    while(isBusy()) delay(0); //BULK_ERASE can take awhile, lets just wait until it completes
}

//Private Methods
void M25PXFlashMemory::enableWrite() {
    spi.select();
    spi.transfer(WRITE_ENABLE);
    spi.deselect();
}

void M25PXFlashMemory::disableWrite() {
    spi.select();
    spi.transfer(WRITE_DISABLE);
    spi.deselect();
}

void M25PXFlashMemory::sendAddress(uint32_t addr) {
    spi.transfer((addr & 0xff0000) >> 16); //addr 0
    spi.transfer((addr & 0xff00) >> 8); //addr 1
    spi.transfer((addr & 0xff)); //addr 2
}

void M25PXFlashMemory::programPage(uint32_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards
//    Serial.print("PAGE_WRITE: ");
//    Serial.print(addr,HEX);
//    Serial.print(" ");
//    Serial.print(len);
//    Serial.print(" [");
//    Serial.print(buf[0]);
//    Serial.print(" ");
//    Serial.print(buf[1]);
//    Serial.print(" ");
//    Serial.print(buf[2]);
//    Serial.println("]");

    spi.select();
    spi.transfer(PAGE_PROGRAM);
    sendAddress(addr);
    for (int i=0; i<len; i++) {
        spi.transfer(buf[i]);
    }
    spi.deselect();
}

void M25PXFlashMemory::readPage(uint32_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_DATA_BYTES);
    sendAddress(addr);

    for (int i=0; i<len; i++) {
        buf[i] = spi.transfer(0);
    }
    spi.deselect();
}
