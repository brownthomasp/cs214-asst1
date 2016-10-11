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
		ptr[count] = NULL;
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
		ptr = NULL;
		count++;
	}
}

//Randomly mallocs or frees 1 byte 6000 times, such that
//3000 mallocs and 3000 frees are performed in a random order.
void test_C() {
	char* ptr[3000];
	int choice = rand() % 2		 //Define choice == 0 as malloc()
           ,nextIndexToMalloc = 0	 //and choice == 1 as free().
           ,nextIndexToFree = 0;
	srand((unsigned int)time(NULL));

	//When the next index for malloc and free have both reached 3000, then we've performed 3000 of both operations. Terminate the loop.
	while ((nextIndexToMalloc + nextIndexToFree) < 6000) {
		
		//If (malloc) and (haven't malloc'd 3000 things yet
		if (choice == 0 && nextIndexToMalloc < 3000) {
			ptr[nextIndexToMalloc] = malloc(1);
			
			//If null was returned, don't advance to the next index.
			if (ptr[nextIndexToMalloc] != NULL) {
				nextIndexToMalloc++;
			}

		} else {
			
			//If the number of successful frees is less than the number of successful mallocs,
			//we can go ahead and free the next pointer. Otherwise, we'd be attempting to free
			//a pointer that wasn't malloc'd yet.
			if (nextIndexToFree < nextIndexToMalloc) {	
				free(ptr[nextIndexToFree]);
				ptr[nextIndexToFree] = NULL;
				nextIndexToFree++;
			}
		}

		//Make a new choice.
		choice = rand() % 2;
	}
}

//Randomly mallocs and frees N bytes 6000 times, such that 
//3000 mallocs and 3000 frees are performed in a random order.
void test_D() {
	char* ptr[3000];
	int bytesForPointer[3000]; //List of how many bytes were used for a certain malloc + its metadata.
	int bytesAllocated = 0
           ,choice = rand() % 2 //See test_C regarding this variable.
           ,newBytes = rand() % 19997	//May return a ridiculous value.
           ,nextIndexToMalloc = 0
           ,nextIndexToFree = 0;
	srand((unsigned int)time(NULL));

	//Loop will terminate when nextIndex is 3000 for malloc and free, meaning we have performed 3000 mallocs and 3000 frees. 
	while ((nextIndexToMalloc + nextIndexToFree) < 6000) {

		//If (malloc) and (haven't malloc'd 3000 things yet)
		if (choice == 0 && nextIndexToMalloc < 3000) {

			//Check and make sure we have enough free bytes to cover this malloc.
			//Note: this doesn't check to make sure that we have enough *contiguous*
			//bytes, since memory fragmentation is still a real world problem. 
			//Therefore, the malloc may still create an "out of memory" error
			//If a contiguous block is unavailable.
			 if ((bytesAllocated + newBytes + 4) <= 20000) { 

				ptr[nextIndexToMalloc] = malloc(newBytes);
			
				//If malloc was successful, record how many bytes were malloc'd as well as how many bytes are used total.
				if (ptr[nextIndexToMalloc] != NULL) {
					bytesForPointer[nextIndexToMalloc] = newBytes + 4;
					bytesAllocated += (newBytes + 4);
					nextIndexToMalloc++;
				}
			}

		} else {
	
			//If the number of successful frees is less than the number of successful mallocs,
			//we can go ahead and free the next pointer. Otherwise, we'd be attempting to free
			//a pointer that wasn't malloc'd yet. Also update the number of bytes we're currently using.
			if (nextIndexToFree < nextIndexToMalloc) {
				free(ptr[nextIndexToFree]);
				ptr[nextIndexToFree] = NULL;
				bytesAllocated -= (bytesForPointer[nextIndexToFree]);
				nextIndexToFree++;
			}
		}

		//Make a new choice and get a new number of bytes to malloc.
		choice = rand() % 2;
		newBytes = rand() % 19997;
	}
}

//Test E attempts to see how speedily the malloc implementation handles error ases.
//It does several things: first, it mallocs the maximum number of one-byte blocks
//for a 20,000 byte memory pool with 4 bytes metadata per block, which is 4000 blocks.
//Then, it attempts to malloc 4000 more one-byte blocks (all of which should fail).
//Then, it frees the first 4000 blocks, followed by attempting to free those same blocks
//again, as well as 4000 more blocks which were never allocated in the first place.
//Finally, it will attempt to free an address it never allocated 3000 times.
void test_E() {
	char* ptr[8000];
	int count = 0;
	
	//Should produce 4000 successful mallocs and then 4000 failed mallocs.
	while (count < 8000) {
		ptr[count] = malloc(1);
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

//Test F requests the maximum number of  1 bytes blocks of memory that 
//can be stored in a 20,000 byte memory pool with 4 bytes metdata, which is 
//4000, and then attempts to free every other  pointer. This should cause  
//a great deal of fragmentation. Then, every other of the remaining allocated
//blocks is freed to force merges of the concecutive free blocks.  Then, it 
//will allocate several larger blocks and smaller blocks to see how well it 
//handles filling in the fragmented memory.   
void test_F() {
	char* ptr[4000];
	int count = 0;

	while (count < 4000) {
		ptr[count] = malloc(1);
		count++;
	}

	count = 1;

	while (count < 4000) {
		free(ptr[count]);
		count += 2;
	}

	count = 2;

	while (count < 4000) {
		free(ptr[count]);
		count += 4;
	}
	
	count = 1;
	
	while (count < 4000) {
		ptr[count] = malloc(5);
		count += 4;
	}
	
	count = 2;
	
	while (count < 400) {
		ptr[count] = malloc(1);
		count += 4;
	}
	
	for (count = 0; count < 4000; count++) {
		if (ptr[count]) {
			free(ptr(count));
		}
	}
	
}

float calculateMeanOfArray(int arr[], int size) {
	int i = 0;
	int total = 0;	

	while (i < size) {
		total += arr[i];
		i++;
	}

	return (1.0*total/size);
}

int main() {
	int count = 0;
	int testATimes[100]
             ,testBTimes[100]
             ,testCTimes[100]
             ,testDTimes[100]
             ,testETimes[100]
             ,testFTimes[100];	
	struct timeval start
                       ,end;

	//Do test A 100 times.
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_A();
		gettimeofday(&end, NULL);
		testATimes[count] = end.tv_sec - start.tv_sec;
		count++;
	} 


	count = 0;

	//Do test B 100 times.
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_B();
		gettimeofday(&end, NULL);
		testBTimes[count] = end.tv_sec - start.tv_sec;
		count++;
	}
	

	count = 0;
	
	//Do test C 100 times.	
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_C();
		gettimeofday(&end, NULL);
		testCTimes[count] = end.tv_sec - start.tv_sec;
		count++;
	}
	

	count = 0;

	//Do test D 100 times.
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_D();
		gettimeofday(&end, NULL);
		testDTimes[count] = end.tv_sec - start.tv_sec;
		count++;
	}
	

	count = 0;


	//Do test E 100 times.	
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_E();
		gettimeofday(&end, NULL);
		testETimes[count] = end.tv_sec - start.tv_sec;
		count++;
	}

	count = 0;
	
	//Do test F 100 times.
	while (count < 100) {
		gettimeofday(&start, NULL);
		test_F();
		gettimeofday(&end, NULL);
		testFTimes[count] = end.tv_sec - start.tv_sec;
		count++;
	}

	printf("The average elapsed time for test A is %f.\n", calculateMeanOfArray(testATimes, 100));
	printf("The average elapsed time for test B is %f.\n", calculateMeanOfArray(testBTimes, 100));
	printf("The average elapsed time for test C is %f.\n", calculateMeanOfArray(testCTimes, 100));
	printf("The average elapsed time for test D is %f.\n", calculateMeanOfArray(testDTimes, 100));
	printf("The average elapsed time for test E is %f.\n", calculateMeanOfArray(testETimes, 100));
	printf("The average elapsed time for test F is %f.\n", calculateMeanOfArray(testFTimes, 100));
	return 0;
}
