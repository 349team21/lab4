/** @file ub_test.c
 * 
 * @brief The UB Test for basic schedulability
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif

void bubble_sort(task_t tasks[], size_t array_size);
double rhs_calc(unsigned int k);
 
/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */
int assign_schedule(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	double summation = 0;
	double current = 0;
	task_t* curTask;
	int i;

	bubble_sort(*tasks, num_tasks);

	for(i = 0; i < (int) num_tasks; i++){
		curTask = tasks[i];
		summation += ((double) curTask->C) / ((double) curTask->T);
		current = ((double) curTask->B) / ((double) curTask->T);
		printf("%i: summation:%f current:%f rhs_calc:%f\n", i, summation, current, rhs_calc(i+1));

		if( (summation + current) > rhs_calc(i+1) )
			return 0;
	}

	return 1;

}

void bubble_sort(task_t tasks[], size_t array_size)
{
	unsigned i, j;
	task_t temp;
 
	for (i = (array_size-1); i>0; i--){
		for (j = 1; j <= i; j++){
			if (tasks[j-1].T > tasks[j].T){
				temp = tasks[j-1];
				tasks[j-1] = tasks[j];
				tasks[j] = temp;
			}
		}
	}
}

static double lookup_table[] =
{

1.0, 0.828427125, 0.77976315, 0.75682846, 0.743491775,
0.73477229, 0.728626596, 0.724061861, 0.72053765, 0.717734625,
0.715451984, 0.713557132, 0.711958994, 0.710592941, 0.709411842,
0.708380519, 0.707472181, 0.706666069, 0.705945844, 0.705298477,
0.704713443, 0.704182154, 0.703697529, 0.703253679, 0.702845666,
0.702469318, 0.702121087, 0.701797935, 0.701497247, 0.70121676,
0.700954504, 0.700708757, 0.700478008, 0.700260926, 0.70005633,
0.699863173, 0.699680523, 0.699507544, 0.699343489, 0.699187684,
0.699039522, 0.698898454, 0.698763983, 0.698635657, 0.698513063,
0.698395826, 0.698283602, 0.698176077, 0.698072961, 0.69797399,
0.697878916, 0.697787516, 0.69769958, 0.697614915, 0.697533342,
0.697454694, 0.697378816, 0.697305566, 0.697234808, 0.697166418,
0.697100279, 0.697036281, 0.696974323, 0.696914307
};

double rhs_calc(unsigned int k) {

        if (k == 0) return 1.0;
        if (k > 64) return lookup_table[63];
        else {
                return lookup_table[k];
        }

}
	


