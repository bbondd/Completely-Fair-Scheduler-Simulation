#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void main(int argc, char* argv[]) {
	kill(getpid(), SIGSTOP);

	while (1) {
		printf("%c\n", argv[1][0]);
		sleep(1);
	}
}
