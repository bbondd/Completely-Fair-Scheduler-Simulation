#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

/*
int counter = 0;

void sigint_handler( int signo)
{
printf( "알람 발생 %d!!\n", counter++);
alarm(1);
}*/

typedef struct _process_node {
	pid_t pid;
	int nice_value;
	double virtual_runtime;
	char* execution_argument;

	struct timeval last_executed_time;

	struct _process_node* next_node;
	struct _process_node* previous_node;
}process_node;

typedef struct _process_queue {
	process_node* head;
	process_node* tail;
}process_queue;

process_queue* queue;
process_node* currunt_running_node;

typedef pid_t;
char* process_file_path = "./ku_app";
#define nice_number 5

process_node* new_process_node(pid_t pid, int nice_value, char* execution_argument) {
	process_node* node = (process_node*)calloc(1, sizeof(process_node));
	node->pid = pid;
	node->nice_value = nice_value;
	node->virtual_runtime = 0;
	node->execution_argument = execution_argument;

	return node;
}

process_node* insert_process_node(process_queue* queue, process_node* node) {
	process_node* insert_location;

	for (insert_location = queue->head->next_node;
		insert_location != queue->tail;
		insert_location = insert_location->next_node)
		if (insert_location->virtual_runtime > node->virtual_runtime) break;

	process_node* previous_node = insert_location->previous_node;
	process_node* next_node = insert_location;

	previous_node->next_node = node;
	node->previous_node = previous_node;
	next_node->previous_node = node;
	node->next_node = next_node;

	return node;
}

process_node* take_out_first_node(process_queue* queue) {
	process_node* first_node = queue->head->next_node;
	process_node* previous_node = queue->head;
	process_node* next_node = queue->head->next_node->next_node;

	previous_node->next_node = next_node;
	next_node->previous_node = previous_node;

	first_node->previous_node = NULL;
	first_node->next_node = NULL;

	return first_node;
}

process_queue* new_process_queue() {
	process_queue* queue = (process_queue*)calloc(1, sizeof(process_queue));
	process_node* head = new_process_node(NULL, NULL, NULL);
	process_node* tail = new_process_node(NULL, NULL, NULL);

	head->next_node = tail;
	tail->previous_node = head;
	queue->head = head;
	queue->tail = tail;

	return queue;
}

pid_t new_process(char* execution_argument) {
	pid_t pid = fork();
	if (pid == 0) execl(process_file_path, process_file_path, execution_argument);

	return pid;
}

process_queue* initialize_process_queue(process_queue* queue, int* process_numbers) {
	int current_char = 'A';
	for (int i = 0; i < nice_number; i++)
		for (int j = 0; j < process_numbers[i]; j++) {
			char* execution_argument = (char*)calloc(2, sizeof(char));
			execution_argument[0] = current_char;
			current_char++;
			pid_t pid = new_process(execution_argument);

			process_node* node = new_process_node(pid, i - 2, execution_argument);
			insert_process_node(queue, node);
		}

	return queue;
}

void resume_process(process_node* node) {
	struct timeval current_time;
	gettimeofday(&current_time, NULL);

	node->last_executed_time = current_time;
	kill(node->pid, SIGCONT);
}

void pause_process(process_node* node) {
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	kill(node->pid, SIGSTOP);

	node->virtual_runtime += (
		current_time.tv_sec
		+ (double)(current_time.tv_usec / 1000)
		- currunt_running_node->last_executed_time.tv_sec
		- (double)(currunt_running_node->last_executed_time.tv_usec / 1000)
		) * pow(1.25, currunt_running_node->nice_value);
}

void reschedule() {
	pause_process(currunt_running_node);
	insert_process_node(queue, currunt_running_node);

	currunt_running_node = take_out_first_node(queue);
	resume_process(currunt_running_node);

	alarm(1);
}



int main(int argc, char* argv[])
{
	int process_numbers[nice_number];
	for (int i = 0; i < nice_number; i++) process_numbers[i] = atoi(argv[i + 1]);
	int time_slice_number = atoi(argv[nice_number + 1]);

	queue = new_process_queue();
	initialize_process_queue(queue, process_numbers);

	sleep(1);

	currunt_running_node = take_out_first_node(queue);
	resume_process(currunt_running_node);


	alarm(1);
	signal(SIGALRM, reschedule);

	/*

	pid_t child = fork();
	if (child == 0) execl("./printing_a", NULL);

	sleep(3);
	kill(child, SIGSTOP);
	sleep(3);
	kill(child, SIGCONT);

	signal(SIGALRM, sigint_handler);
	alarm(1);
	while (1);*/

	while (1);
}