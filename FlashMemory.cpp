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

void FlashMemory::writeBytes(uint16_t addr, byte * buf, int len) {
    int i=0;
    while (i < len) {
        int writeSize = 0x100 - (addr & 0xff);
        if (writeSize > len - i) writeSize = len - i;
        Serial.print("Writing: 0x");
        Serial.print(addr,HEX);
        Serial.print("(");
        Serial.print(writeSize);
        Serial.println(")");
//        for (int l=0; l<10; l++) {
//            Serial.print("0x");
//            Serial.print(buf[l]);
//            Serial.print(" ");
//        }
//        Serial.println();
        writePage(addr,buf,writeSize);
        addr &= 0xffffff00; //start at the beginning of the page
        addr += 0x00000100; //increment page by one
        buf += writeSize;
        i += writeSize;
    }
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

void FlashMemory::erasePage(uint16_t addr) {
    addr = addr & 0xffffff00; //target page

    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards

    spi.select();
    spi.transfer(PAGE_ERASE);
    sendAddress(addr);
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
    spi.transfer((addr & 0xff0000) >> 4); //addr 0
    spi.transfer((addr & 0xff00) >> 2); //addr 1
    spi.transfer((addr & 0xff)); //addr 2
}

void FlashMemory::writePage(uint16_t addr, byte * buf, int len) {
    byte cbuf[16];
    Serial.print("cbuf: ");
    readPage(0x300,cbuf,16);
    for (int i=0; i<16; i++) {
      if (i % 16 == 0 && i != 0) Serial.println();
      Serial.print("0x");
      byte c = cbuf[i];
      if (c < 0x10) Serial.print("0");
      Serial.print(c,HEX);
      Serial.print(" ");
    }
    Serial.println();
    

    while(isBusy()) delay(0);

    enableWrite(); //write is disabled automatically afterwards
    Serial.print("PAGE_WRITE: ");
    Serial.print(addr,HEX);
    Serial.print(" ");
    Serial.print(len);
    Serial.print(" [");
    Serial.print(buf[0]);
    Serial.print(" ");
    Serial.print(buf[1]);
    Serial.print(" ");
    Serial.print(buf[2]);
    Serial.println("]");

    spi.select();
    spi.transfer(PAGE_WRITE);
    sendAddress(addr);
    for (int i=0; i<len; i++) {
        spi.transfer(buf[i]);
    }
    spi.deselect();
}

void FlashMemory::readPage(uint16_t addr, byte * buf, int len) {
    while(isBusy()) delay(0);

    spi.select();
    spi.transfer(READ_DATA_BYTES);
    sendAddress(addr);

    for (int i=0; i<len; i++) {
        buf[i] = spi.transfer(0);
    }
    spi.deselect();
}
