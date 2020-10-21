/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "const_defines.h"
#include "llfunctions.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

volatile int STOP=FALSE;

extern struct termios oldtio;

int main(int argc, char** argv)
{
  int c, res;
  char buf[255];

  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && 
        (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  struct applicationLayer application;

  application.fileDescriptor = open(argv[1], O_RDWR | O_NOCTTY );
  if (application.fileDescriptor < 0) {perror(argv[1]); exit(-1); }

  application.status = RECEIVER;

  if (llopen(&application) < 0) {
    perror("LLOPEN() failed");
    exit(2);
  }
  
  struct linkLayer link;

  strncpy(link.port, argv[1], sizeof(link.port));
  link.sequenceNumber = 0;

  llread(&application, &link);

  if (llclose(&application) < 0) {
    perror("LLCLOSE() failed");
    exit(3);
  }

  sleep(1);

  tcsetattr(application.fileDescriptor,TCSANOW,&oldtio);
  close(application.fileDescriptor);
  return 0;
}

/*int checkBCC1(unsigned char *message, int sizeMessage) {
  unsigned char BCC1;

  BCC1 = A ^ C;

  return 0;
}*/


/*int checkBCC2(unsigned char *message, int sizeMessage) {
  unsigned char BCC2 = message[0];
  for (int i = 1; i < sizeMessage; i++) {
    BCC2 ^= message[i];
  }

  return 0;
}*/
