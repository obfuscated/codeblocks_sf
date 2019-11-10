#include <avr/io.h>

NVM_FUSES_t __fuse FUSEMEM = {
   .FUSEBYTE0 = FUSE0_DEFAULT,
   .FUSEBYTE1 = FUSE1_DEFAULT,
   .FUSEBYTE2 = FUSE2_DEFAULT,
   .FUSEBYTE4 = FUSE4_DEFAULT,
   .FUSEBYTE5 = FUSE5_DEFAULT
   // check datasheet for details about fuses
};
