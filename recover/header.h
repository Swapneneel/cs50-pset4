#include <stdint.h>

typedef uint8_t BYTE;  // it was uint not unit == > uint stands for unsigned integer

typedef struct {
    BYTE byte_each;
} __attribute__ ((__packed__))
JPEG;