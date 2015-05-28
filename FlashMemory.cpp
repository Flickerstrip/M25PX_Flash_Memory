#include "FlashMemory.h"

//Public Methods
FlashMemory::FlashMemory(byte sck,byte mosi,byte miso,byte cs) : spi(sck,mosi,miso,cs) {
    spi.begin();
}

byte FlashMemory::readStatus() {
    spi.select();
    spi.transfer(READ_STATUS_REGISTER);
    byte status = spi.transfer(0);
    spi.deselect();
    return status;
}

bool FlashMemory::isBusy() {
    return readStatus() & (1 << 0); //LSB is WIP
}

bool FlashMemory::isWritable() {
    return readStatus() & (1 << 1); //bit2 is WEL
}

void FlashMemory::writeByte(uint16_t addr,byte b) {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PAGE_WRITE);
    sendAddress(addr);
    spi.transfer(b);
    spi.deselect();
}

//TODO handle pagebreaks
void FlashMemory::writeBytes(uint16_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PAGE_WRITE);
    sendAddress(addr);
    for (int i=0; i<len; i++) {
        spi.transfer(buf[i]);
    }
    spi.deselect();
}

byte FlashMemory::readByte(uint16_t addr) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_DATA_BYTES);
    sendAddress(addr);
    byte value = spi.transfer(0);
    spi.deselect();

    return value;
}

void FlashMemory::readBytes(uint16_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_DATA_BYTES);
    sendAddress(addr);

    for (int i=0; i<len; i++) {
        buf[i] = spi.transfer(0);
    }
    spi.deselect();
}

//Private Methods
void FlashMemory::enableWrite() {
    spi.select();
    spi.transfer(WRITE_ENABLE);
    spi.deselect();
}

void FlashMemory::disableWrite() {
    spi.select();
    spi.transfer(WRITE_DISABLE);
    spi.deselect();
}

void FlashMemory::sendAddress(uint16_t addr) {
    spi.transfer((addr & 0x0f00) >> 2); //addr 0
    spi.transfer((addr & 0x00f0) >> 1); //addr 1
    spi.transfer((addr & 0x000f)); //addr 2
}

