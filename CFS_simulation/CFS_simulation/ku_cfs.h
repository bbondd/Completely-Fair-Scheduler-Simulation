#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <math.h>


typedef struct _process_node {
	pid_t process_id;
	int status;
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

char* process_file_path = "./ku_app";
#define nice_number 5
int time_slice_number, time_slice_counter;
int time_slice_size = 1;

process_queue* new_process_queue();
process_node* new_process_node(int nice_value, char* execution_argument);
pid_t new_process(char* execution_argument);

process_node* insert_process_node(process_queue* queue, process_node* node);
process_node* take_out_first_node(process_queue* queue);

void initialize_process_queue(process_queue* queue, int* process_numbers);
void initialize_processes(process_queue* queue);

void resume_process(process_node* node);
void pause_process(process_node* node);
void kill_processes(process_queue* queue);

void set_timer(int sec);
void reschedule();
