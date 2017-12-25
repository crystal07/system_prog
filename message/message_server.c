#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
	long mtype;
	char mtext[1024];
	int receiver;
	int sender;
} MsgType;

void send_message(MsgType msg, int que_id);

int main() {
	key_t key[3];
	int que_id[3];

	for (int i = 0; i < 3; i++) {
		key[i] = 4070 + (i + 1);
		que_id[i] = msgget(key[i], IPC_CREAT | 0666);
		printf("%d th que_id : %d\n", i+1, que_id[i]);
	}

	MsgType received_msg;
	int msg_size = sizeof(received_msg) - sizeof(received_msg.mtype);
	int nbytes;

	while (1) {
		for (int i = 0; i < 3; i++) { //client message
			nbytes = msgrcv(que_id[i], &received_msg, msg_size, 10, IPC_NOWAIT);
			if (nbytes <= 0) {
				//not message in queue
				//printf("no message\n");
			}
			else {
				printf("message received from %d\n", received_msg.sender);
				send_message(received_msg, que_id[i]);
			}
		}
	}

	return 0;
}

void send_message(MsgType msg, int que_id) {
	MsgType sended_msg;
	int msg_size = sizeof(sended_msg) - sizeof(sended_msg.mtype);

	sended_msg.mtype = msg.receiver;
	strcpy(sended_msg.mtext, msg.mtext);
	sended_msg.receiver = msg.receiver;
	sended_msg.sender = msg.sender;
	if (sended_msg.receiver > 0) {
		msgsnd(que_id, &sended_msg, msg_size, IPC_NOWAIT);
		printf("send message to %d\n", sended_msg.receiver);
	}
	else;
}
