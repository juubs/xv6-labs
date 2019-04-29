#include "types.h"
#include "user.h"
#include "stat.h"
#include "syscall.h"
#include "select.h"


char * shargv[] = { "sh", 0 };

void
pipenet(void)
{
  int toshfds[2], fromshfds[2];
  char buf[100];
  int bytesread = 0;
  int pid;

  fd_set readfds, writefds;

  if (pipe(toshfds) != 0) {
    printf(2, "Pipe failed!");
    exit();
  }

  if (pipe(fromshfds)) {
    printf(2, "Pipe failed!");
    exit();
  }

  if ((pid = fork()) == 0) { // if child
    close(toshfds[1]);   // close write end
    close(fromshfds[0]); // close read end
    close(0);            // close console stdin
    dup(toshfds[0]);     // dup pipe to stdin
    close(1);            // close console stdout
    close(2);            // close console stderr
    dup(fromshfds[1]);   // dup pipe to stdout
    dup(fromshfds[1]);   // dup pipe to stderr
    close(toshfds[0]);   // close duplicated read fd
    close(fromshfds[1]); // close duplicated write fd
    
    exec(shargv[0], shargv); // invoke shell
  } else {
    close(toshfds[0]);  // close read end
    close(fromshfds[1]);// close write end
    printf(1,"\n\n ***** Starting PipeNet ***** \n\n");
    for (;;) {

      FD_ZERO(&readfds);
      FD_ZERO(&writefds);
      FD_SET(0, &readfds); // console read fd
      FD_SET(fromshfds[0], &readfds); // read from shell pipe fd
      FD_SET(toshfds[1], &writefds); // write to shell pipe fd

      if (select(32, &readfds, &writefds) != 0)
        return;

      if (FD_ISSET(0, &readfds)) { // ready to read from console
        //printf(1, "\nReading from console\n");
        bytesread = read(0, buf, sizeof(buf)); // read from console
      }

      if (FD_ISSET(toshfds[1], &writefds)) { // ready to write to pipe
        //printf(1, "\nWriting to pipe\n");
        if (bytesread > 0)
          write(toshfds[1], buf, bytesread); // write console data to pipe
      }

      if (FD_ISSET(fromshfds[0], &readfds)) {
          printf(1, "\nReading from pipe\n");
          bytesread = read(fromshfds[0], buf, sizeof(buf)); // read from pipe
          if (bytesread > 0) {
            printf(1, "\nWriting to console\n");
            if (write(0, buf, bytesread) < 0) { // write data from pipe to console
              printf(2, "Write error!");
              exit();
            }
          } else if (bytesread == 0) { // no data left to read
            break;
          } else { // error
            printf(2, "Pipe read error!");
            exit();
          }
      }
    }
    wait();
  }
}

int
main(void)
{
    pipenet();
    exit();
}
