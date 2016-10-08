#include <stdlib.h>
#include "mymalloc.h"
#include <time.h>
#include <sys/time.h>

//Tests are named as specified in the assignment description.

//Mallocs 1 byte 3000 times, and then frees the pointers one 
//by one by one.
void test_A() {
	char* ptr[3000];
	int count = 0;

	//Malloc each pointer.
	while (count < 3000) {
		ptr[count] = malloc(1);
		count++;
	}

	count = 0;

	//Free each pointer.
	while (count < 3000) {
		free(ptr[count]);
		count++;
	}
}

//Mallocs 1 byte and frees it immediately, then repeats 
//the process 2999 more times.
void test_B() {
	char *ptr = NULL;
	int count = 0;

	//Malloc and free each pointer together.
	while (count < 3000) {
		ptr = malloc(1);
		free(ptr);
		count++;
	}
}

//Randomly mallocs or frees 1 byte 6000 times, such that
//3000 mallocs and 3000 frees are performed in a random order.
void test_C() {
	char* ptr[3000];
	srand((unsigned int)time(NULL));
	int count = 0
           ,choice = rand() % 2 //Define choice == 0 as malloc()
           ,numMallocs = 0	 //and choice == 1 as free().
           ,numFrees = 0;

	//Count to 6000 (3000 mallocs + 3000 frees)
	while (count < 6000) {
		if (choice == 0) {
			ptr[numMallocs] = malloc(1);

			if (ptr[numMallocs] != NULL) {
				numMallocs++;
			}

		} else {
			if (numFrees < numMallocs) {
				free(ptr[numFrees]);
				numFrees++;
			}
		}

		//The ternary statement below forces a free operation
		//by setting choice to 1 if there have been 3000 mallocs.
		count++;
		(numMallocs >=  3000) ? (choice = 1) : (choice = rand() % 2);
	}
}

//Randomly mallocs and frees N bytes 6000 times, such that 
//3000 mallocs and 3000 frees are performed in a random order.
void test_D() {
	char *ptr[3000];
	srand((unsigned int)time(NULL));
	int count = 0
           ,choice = rand() % 2 //See test_C regarding this variable.
           ,bytes = rand()	//May return a ridiculous value.
           ,numMallocs = 0
           ,numFrees = 0
           ,bytesAllocated = 0;

	while (count < 6000) {
	
		//Metadata is stored in a 4 byte block; hence checking if we
		//have the requested space as well as the space for metadata.
		if (choice == 0 && (bytesAllocated + bytes + 4 <= 20000)) {
			ptr[numMallocs] = malloc(bytes);
			if (ptr[numMallocs] != NULL) {
				numMallocs++;
				bytesAllocated += bytes + 4;
			}
		} else {
			if (numFrees < numMallocs) {
				free(ptr[numFrees]);
				numFrees++;
			}
		}

		//See test_C regarding the ternary operator.
		//Get a new random number of bytes to malloc
		//on the next loop iteration.
		count++;
		(numMallocs >= 3000) ? (choice = 1) : (choice = rand() % 2);
		bytes = rand();
	}
}

//Test E attempts to see how speedily the malloc implementation handles error cases.
//It does several things: first, it mallocs the maximum number of one-byte blocks 
//for a 20,000 byte memory pool with 4 bytes metadata, which is 4000 blocks.
//Then, it attempts to malloc 4000 more one-byte blocks (all of which should fail).
//Then, it frees the the first 4000 blocks, followed by attempting to free those same blocks
//again as well as 4000 more blocks which were never allocated.
//Finally, it will attempt to free a non-pointer variable 3000 times.
void test_E() {
	char *ptr[8000];
	int count = 0;

	//Only the first 4000 mallocs will succeed. The next 4000 should fail.
	while (count < 8000) {
		ptr[count]  = malloc(1);
		count++;
	}

	count = 0;

	//Frees the properly malloc'd pointers.
	while (count < 4000) {
		free(ptr[count]);
		count++;
	}

	count = 0;

	//Attempts to free the properly malloc'd pointers again as well 
	//as the pointers which were not successfully malloc'd.
	while (count < 8000) {
		free(ptr[count]);
		count++;
	}

	count = 0;
	
	//Attempt to free the address of the count variable 3000 times, 
	//which should fail each time.
	while (count < 3000) {
		free(&count);
		count++;
	}
}

//Test F requests the maximum single block of memory that can be stored in 
//a 20,000 byte memory pool with 4 bytes metdata, which is 19,996 bytes,
//and then frees it immediately, 3000 times over. This will force the 
//malloc routine to search the entirety of myblock for the correct 
//amount of space.
void test_F() {
	char *ptr = NULL;
	int count = 0;

	while (count < 3000) {
		ptr = malloc(19996);
		free(ptr);
		count++;
	}
}

float calculateMeanOfArray(float arr[], int size) {
	int i = 0;
	float total = 0;	

	while (i < size) {
		total += arr[i];
		i++;
	}

	return (total/(float)size);
}

int main() {
	int count = 0;
	float testATimes[100]
           ,testBTimes[100]
           ,testCTimes[100]
           ,testDTimes[100]
           ,testETimes[100]
           ,testFTimes[100];	
	struct timeval start
                       ,end;

	//Do test A 100 times.
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_A();
		gettimeofday(&end, NULL);
		testATimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000);
		count++;
	} 

	printf("The average elapsed time for test A is %f.\n", calculateMeanOfArray(testATimes, 100));

	count = 0;

	//Do test B 100 times.
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_B();
		gettimeofday(&end, NULL);
		testBTimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_sec - start.tv_usec)/1000000);
		count++;
	}
	
	printf("The average elapsed time for test B is %f.\n", calculateMeanOfArray(testBTimes, 100));

	count = 0;
	
	//Do test C 100 times.	
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_C();
		gettimeofday(&end, NULL);
		testCTimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_sec - start.tv_usec)/1000000);
		count++;
	}
	
	printf("The average elapsed time for test C is %f.\n", calculateMeanOfArray(testCTimes, 100));

	count = 0;
	
	//Do test D 100 times.
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_D();
		gettimeofday(&end, NULL);
		testDTimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_sec - start.tv_usec)/1000000);
		count++;
	}
	
	printf("The average elapsed time for test D is %f.\n", calculateMeanOfArray(testDTimes, 100));

	count = 0;
	
	//Do test E 100 times.	
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_E();
		gettimeofday(&end, NULL);
		testETimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000);
		count++;
	}
	
	printf("The average elapsed time for test E is %f.\n", calculateMeanOfArray(testETimes, 100));

	count = 0;
	
	//Do test F 100 times.
	while (count < 100) {
		//printf("count = %d\n", count);
		gettimeofday(&start, NULL);
		test_F();
		gettimeofday(&end, NULL);
		testFTimes[count] = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000);
		count++;
	}
	
	printf("The average elapsed time for test F is %f.\n", calculateMeanOfArray(testFTimes, 100));

	return 0;
}
