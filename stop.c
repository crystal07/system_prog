#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <unistd.h>

static void sig_fn(int signo) {
	printf("Ctrl-C is pressed. Try Again\n");
}

int main(void) {
	unsigned int unslept;

	struct sigaction act, oact;
	act.sa_handler = sig_fn;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	while(1) {
		if (sigaction(SIGINT, &act, &oact) != 0) {
			fprintf(stderr, "sinal error");
		}
		pause();
	}


	return 0;
}
