//
// simulation of Round Robin Algo., Shortest First algo. and Trivial Algo.
//
// Created by Charles H on 2019-10-30.
//
// Wed 30 Oct 2019 14:30:00 EDT
// Wed 30 Oct 2019 19:14:39 EDT
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tq; // max allowed time in a RR queue
int num_p; // number of process in a round
int total_wait_time; // total wait time of a turn

// structure pcs, AKA process, is the main element of this simulation program
typedef struct pcs
{
	int pid;   // the id of a process
	int time;  // the time of a process
	int rtime; // time remaining. AKA how many time a pcs has been left in a working queue
	int wtime; // time waited. AKA how many time, after first time active, a pcs has been waited to finish a task.
} pcs;

// function to read a line from a file. -1 means failed
// param: char* to store result, and FILE* is a opened file
int readline ( char * line, FILE * file )
{
	if ( fgets(line, 1024, file))
		return 0;
	return -1;
}

// read word by word from a char* into *buf
// param: i_line is the index of current word. notice it is a point, which means
//   it is kind of global... and of course, it has to. otherwise the next getWord will lost
// return: -1 if reached end of a file
//        0 if could get continued
int getWord ( const char * line, char * buf, int * i_line )
{
	int i_buf = 0; // i_buf is the index for buf. Notice it starts at 0
	while ( line[*i_line] != ' ' ) // if it is alphabet, then read it from the start to the end
	{
		buf[i_buf] = line[*i_line]; // duplicate it
		( *i_line )++; i_buf++; // increase both the index
		if ( line[*i_line] == 10 || line[*i_line] == 0 ) // 10 is newline and 0 is NULL
		{
			buf[i_buf] = '\0'; // set a NULL
			return -1;
		}
	}
	while ( line[*i_line] == ' ' ) // kill all the spaces
	{
		( *i_line )++;
		if ( line[*i_line] == 10 || line[*i_line] == 0 ) // jump if EOF or NEWLINE
		{
			buf[i_buf] = '\0';
			return -1;
		}
	}
	buf[i_buf] = '\0';
	return 0;
}

// An Insert Sort changed to satisfy structure PCS
void insert_sort ( pcs * p, int n )
{
	int i, j;
	struct pcs k;
	for ( i = 1; i < n; ++i )
	{
		k = p[i];
		j = i - 1;
		while (( j >= 0 ) && ( k.time < p[j].time ))
		{
			p[j + 1] = p[j];
			j--;
		}
		p[j + 1] = k;
	}
}

// Main function of the program.
//   it will run both initialization, FCFS algorithm, SJF algorithm and RR in one iteration
//
// @param: char* line, a set of processes with information about queue and tq
// @return: 0 if all peace
//
int init_FCFS_SJF_RR ( char * line, FILE * out)
{
	// init
	tq = num_p = total_wait_time = 0;
	int * start = malloc(sizeof(int)); // start is the global index for the input
	*start = 0;                        // ... of course set to 0
	char * buf = malloc(64);       // buf is the buffer read from input, word by word
	getWord(line, buf, start);         //   read the first word and discard it
	getWord(line, buf, start);         // This position is the iteration's number, so
	int turn = atoi(buf);              //   turn, is current iteration's number
	getWord(line, buf, start);
	getWord(line, buf, start);         // This position is tq
	tq = atoi(buf);
	int copy_start = *start;            // Make a copy of the first meaningful element

	//
	//   q 1 tq 5 p1 2 ........
	//            |
	//        The first meaningful element

	struct pcs e; // create a process

	num_p = 0;    // number of processes
	for ( int sign = 0; sign == 0; )  // sign is the return value of getWord, to know if we could continue or not
	{
		buf = malloc(8);   // buff to get the word
		getWord(line, buf, start);  // discard the first thing
		sign = getWord(line, buf, start);  //  here we must check if getWord will continue
		num_p++;
	}

	pcs process[num_p]; // create an array of processes
	*start = copy_start; // start will return back to the first meaningful element
	for ( int sign = 0, i = 0; sign == 0; i++ ) // store process into array
	{
		buf = malloc(8);
		getWord(line, buf, start);
		sign = getWord(line, buf, start); // sign == 0 makes sure it could continue read
		e.pid = i + 1; // i starts at 0 so...
		e.time = atoi(buf);
		e.rtime = e.time;
		e.wtime = 0;
		process[i] = e;
	}

	// FIFO
	//
	// The algorithm for FIFO is straightforward
	//
	// p1  p2  p3  p4
	//     |
	//    p2's wait time = total wait time so far
	//    total wait time += p2's running time
	//
	printf("Ready Queue %d Applying FCFS Scheduling:\n\nOrder of selection by CPU: \n", turn);
	fprintf(out, "Ready Queue %d Applying FCFS Scheduling:\n\nOrder of selection by CPU: \n", turn);
	for ( int i = 1; i <= num_p; i++ )
	{
		printf("p%d ", i);
		fprintf(out, "p%d ", i);
	}
	printf("\n\nIndividual waiting times for each process: \n");
	fprintf(out, "\n\nIndividual waiting times for each process: \n");

	int time_previous = 0;
	total_wait_time = 0;
	for ( int i = 0; i < num_p; i++ )
	{
		process[i].wtime += time_previous;
		total_wait_time += process[i].wtime;
		time_previous += process[i].time;
		printf("p%d = %d\n", process[i].pid, process[i].wtime);
		fprintf(out, "p%d = %d\n", process[i].pid, process[i].wtime);
	}
	printf("\nAverage waiting time = %.1f\n", total_wait_time * 1.0 / num_p);
	fprintf(out, "\nAverage waiting time = %.1f\n", total_wait_time * 1.0 / num_p);

	// SJF
	// the algorithm of SJF is exactly the same, sort first, run next
	//
	// notice here I created another array because the last algo, RR, doesn't need sort, so...
	//
	printf("\nReady Queue %d Applying SJF Scheduling: \n\n", turn);
	fprintf(out, "\nReady Queue %d Applying SJF Scheduling: \n\n", turn);
	// init
	pcs process_SJF[num_p];
	for ( int i = 0; i < num_p; i++ )
	{
		process_SJF[i] = process[i];
		process_SJF[i].wtime = 0;
		process[i].wtime = 0; // this step is for round robin algorithm lol
	}
	// sort
	insert_sort(process_SJF, num_p);
	// print order
	printf("Order of selection by CPU:\n");
	fprintf(out, "Order of selection by CPU:\n");
	for ( int i = 0; i < num_p; i++ )
	{
		printf("p%d ", process_SJF[i].pid);
		fprintf(out, "p%d ", process_SJF[i].pid);
	}
	printf("\n\nIndividual waiting times for each process: \n");
	fprintf(out, "\n\nIndividual waiting times for each process: \n");
	// the rest is exactly same as FIFO
	total_wait_time = 0;
	time_previous = 0;
	for ( int i = 0; i < num_p; i++ )
	{
		process_SJF[i].wtime += time_previous;
		total_wait_time += process_SJF[i].wtime;
		time_previous += process_SJF[i].time;
		printf("p%d = %d\n", process_SJF[i].pid, process_SJF[i].wtime);
		fprintf(out, "p%d = %d\n", process_SJF[i].pid, process_SJF[i].wtime);
	}
	printf("\nAverage waiting time = %.1f\n", total_wait_time * 1.0 / num_p);
	fprintf(out, "\nAverage waiting time = %.1f\n", total_wait_time * 1.0 / num_p);

	// RR
	// finish_order is used to store the order of finished processes
	int finish_order[num_p * 2];
	// init
	printf("\nReady Queue %d Applying RR Scheduling:\n\nOrder of selection by CPU:\n", turn);
	fprintf(out, "\nReady Queue %d Applying RR Scheduling:\n\nOrder of selection by CPU:\n", turn);

	//
	//  this is the coolest algorithm among here!
	//
	//  tq = 4
	//  1  3  2  5  8  9  3
	//  p1 p2 p3 p4 p5 p6 p7
	//
	//  0  0  0  1  4  5  0
	//  p1 p2 p3 p4 p5 p6 p7
	//
	//  I will keep the 0 processes on the array
	//
	//  the finished order = who get zero, who out;
	//  the turnaround time = total running time - first time the process actives
	//     (I store the active time at process.rtime, so you can use:
	//
	//   (when a process is finished)
	//
	//   total_time - process.rtime = turnaround time
	//
	//  some details:
	//  1. when a round is finished, start from 0.
	//  2. how to know when the thing is ended? when all the process' time is now 0

	// param: i: process iterator; count: zero counter; itr: finish_order index
	for ( int i = 0, count = 0, itr = 0, total_time = process[i].time; count < num_p; i++ )
	{
		if ( i == num_p )
		{
			count = 0; // clean up the zero counter and i;
			i = 0;
		}
		if ( process[i].time != 0 )
		{
			if ( process[i].wtime == 0 )
				process[i].wtime = total_time;
			printf("p%d ", process[i].pid);
			fprintf(out, "p%d ", process[i].pid);
			if ( process[i].time <= tq )
			{
				total_time += process[i].time;
				process[i].time = 0;
				process[i].wtime = total_time - process[i].wtime; // the turnaround time
				finish_order[itr] = process[i].pid;
				itr++;
			} else
			{
				process[i].time -= tq;
				total_time += tq;
				process[i].rtime += tq;
			}
		} else
			count++;
	}
	printf("\n\nTurnaround times for each process:");
	fprintf(out, "\n\nTurnaround times for each process:");
	// print the rest thing
	for ( int i = 0; i < num_p; i++ ){
		printf("\np%d = %d", finish_order[i], process[finish_order[i] - 1].wtime);
		fprintf(out, "\np%d = %d", finish_order[i], process[finish_order[i] - 1].wtime);
	}
	return 0;
}

int main ()
{
	// file name and open file
	char const * const fileName = "cpu_scheduling_input_file.txt"; // make sure it is consistent using const pointer and const value
	FILE * file = fopen(fileName, "r");
	FILE * output = fopen ("cpu_scheduling_output_file.txt","w");
	if ( file == NULL) return 1;
	// read line by line and send to each iteration
	char * line = malloc(1024);
	while ( readline(line, file) == 0 )
	{
		init_FCFS_SJF_RR(line, output);
		printf("\n");
		fprintf(output, "\n");
	}
	// close and end
	fclose(file);
	return 0;
}
