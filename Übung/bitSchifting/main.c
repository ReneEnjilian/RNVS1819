#include <stdio.h>
#include <memory.h>
#include <stdint.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

int is_bit_set_at(uint8_t byte, int pos) {
    return (byte >> pos) & 0x01; // hexadec.
}

int clear_bit_at(uint8_t byte, int pos) {
    byte &= ~(1UL << pos);
    return byte;
}

int main() {
    char bytes[6];
    unsigned char byte = '\0';
    uint8_t byte2 = 0xff;

    memset(bytes, 0, 6);

    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(1U));

    //message->header[4] = (uint8_t) ((message->valLength >> 8) & 0xff);
    //message->header[5] = (uint8_t) (message->valLength & 0xff);

    printf("--------\n");
    byte |= 1UL << 6;
    byte |= 1UL << 3;

    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte2));

    printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY((0xff)>>8), BYTE_TO_BINARY(0xf));

    printf("--> %d\n", byte & 1U);
    printf("--------\n");

    byte &= ~(1 << 6);

    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(1U));

    printf("--> %d\n", byte & 1U);
    printf("--------\n");

    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte >> 3));
    printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(1U));

    printf("--> %d\n", (byte >> 3) & 1U);
    printf("--------\n");

    /*
    for (int i = 0; i < 8; i++) {
        printf("Bit %d: %d\n", i, (byte >> i) & 1U);
    }
     */

    printf("%d\n", 0x01);
    printf("%li\n", 1UL);

    return 0;
}