
#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#include <iostream>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <cstdlib>

#include <cstdlib>
#include <pthread.h>
 

using namespace std;

// Global EID shared by multiple threads 
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug;  
} sgx_errlist_t;

// Error code returned by sgx_create_enclave 
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

// Check error conditions for loading enclave 
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL) fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}



void selectionSort(int* ar, int length)
{
	for (int i = 0; i < length-1; i++)
   	{
      		int min = i;
      		for (int j = i+1; j < length; j++)
            		if (ar[j] < ar[min]) min = j;
      		int temp = ar[i];
     		ar[i] = ar[min];
      		ar[min] = temp;
	}
}



void * provision(void *index)
{
	if(initialize_enclave() < 0)
	{
        	printf("Enter a character before exit ...\n");
        	getchar();
        	//return -1; 
    	}

	sgx_destroy_enclave(global_eid);

	pthread_exit(NULL);
}


// Application entry 
int SGX_CDECL main(int argc, char *argv[])
{

    (void)(argc);
    (void)(argv);


	cout<<"\t\tOptions"<<endl;
	cout<<"Enter 1 for Provisioning cost"<<endl;
	cout<<"Enter 2 for Entering and Exiting cost"<<endl;
	cout<<"Enter 3 for Execution cost"<<endl;


	int option=-1;
	while(1>option || option>3)
	{
	
		printf("Option:");
		scanf("%d",&option);
	
		if(1>option || option>3)
		{
			cout<<"Enter valid option"<<endl;
		}

	}

	if(option==1)
	{

		FILE *fptr;
	   	fptr = fopen("./ProvisioningTest.csv","w");

		pthread_attr_t attr;
		void *status;


		int maxth=10;
		cout<<"Enter maximum number of enclaves:";
		scanf("%d",&maxth);

		// Initialize and set thread joinable
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


		fprintf(fptr,"%s\n","Number of enclaves created simultaneously,seconds,mseconds");
		fflush(fptr);


		for(int nt=1; nt<(maxth+1); nt++)
		{

			int NUM_THREADS=nt;
			pthread_t threads[NUM_THREADS];

			auto start = std::chrono::high_resolution_clock::now();
	
			for( int i = 0; i < NUM_THREADS; i++ ) 
			{
				int rc = pthread_create(&threads[i], NULL, provision, (void *)i);

				if (rc) 
				{
					cout << "Error:unable to create thread," << rc << endl;
					exit(-1);
				}
			}


			for( int i = 0; i < NUM_THREADS; i++ ) 
			{
				int rc = pthread_join(threads[i], &status);
				if (rc) 
				{
					cout << "Error:unable to join," << rc << endl;
					exit(-1);
				}
			}


			auto finish = std::chrono::high_resolution_clock::now();

			int seconds=std::chrono::duration_cast<std::chrono::seconds>(finish-start).count();
			int mseconds=std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
			//int nseconds=std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();

			fprintf(fptr,"%d,%d,%d\n",NUM_THREADS,seconds,mseconds);
			fflush(fptr);

			cout<<NUM_THREADS<<","<<seconds<<","<<mseconds<<endl;
		}

		fclose(fptr);

	}

	else
	{
		int upper_limit=10;
		if(option==2)
			printf("Maximum data in megabytes:");
		if(option==3)
			printf("Maximum data in kb:");
	
		scanf("%d",&upper_limit);
	

		// Initialize the enclave 
		if(initialize_enclave() < 0)
		{
			printf("Enter a character before exit ...\n");
			getchar();
			return -1; 
		}
 	
		int one_gb_size=(1024*1024*1024);

		int *pointer;
        	pointer =(int*) malloc(one_gb_size);
	
		int *pointer2;
        	pointer2 =(int*) malloc(one_gb_size);

		int *p;
		p=pointer;

		for(int i=0; i<one_gb_size/4;i++)
		{
    			int random_integer = rand()%100; 
			*p=random_integer;
			p++;
		}
		        
		int byte=1;
		int kilobyte=1024;
		int megabyte=1024*1024;

		//sgx_emmt tool
		//http://multiengined56.rssing.com/chan-24639076/all_p2.html

	
		FILE *fptr;
		
		if (option==2)
		{
		   	fptr = fopen("./Entering_Exiting_Cost.csv","w");

//			cout<<"size of int array, time for copying into enclave (TE-seconds),TE-ms,TE-ns,time for copying to other untrusted memory region(TO-s),TO-ms,TO-ns,kb,mb"<<endl;

			fprintf(fptr,"%s\n","size of int array, time for copying into enclave (TE-seconds),TE-ms,TE-ns,time for copying to other untrusted memory region(TO-s),TO-ms,TO-ns,kb,mb");
		}
		else
		{
		   	fptr = fopen("./Execution_Cost.csv","w");

//			cout<<"size of int array, time for sorting inside enclave (SE-seconds),SE-ms,SE-ns,time for sorting outside enclave(SO-s),SO-ms,SO-ns,kb,mb"<<endl;

			fprintf(fptr,"%s\n","size of int array, time for sorting inside enclave (SE-seconds),SE-ms,SE-ns,time for sorting outside enclave(SO-s),SO-ms,SO-ns,kb,mb");
		}

		int factor=megabyte;
		int start=100*kilobyte;
		if(option==3)
		{
			start=kilobyte;
			factor=kilobyte;
		}

		for(int size=start; size<(upper_limit+1)*factor; size=size+kilobyte)
		{
	
			int seconds1,seconds2,mseconds1,mseconds2,nseconds1,nseconds2;
		
			if (option==2)
			{
				auto start1 = std::chrono::high_resolution_clock::now();
				ecall_my_dtree(global_eid,pointer,size);
				auto finish1 = std::chrono::high_resolution_clock::now();
		
				auto start2 = std::chrono::high_resolution_clock::now();
				memcpy(pointer2,pointer,size);
				auto finish2 = std::chrono::high_resolution_clock::now();

				seconds1=std::chrono::duration_cast<std::chrono::seconds>(finish1-start1).count();
				mseconds1=std::chrono::duration_cast<std::chrono::milliseconds>(finish1-start1).count();
				nseconds1=std::chrono::duration_cast<std::chrono::nanoseconds>(finish1-start1).count();
				
				seconds2=std::chrono::duration_cast<std::chrono::seconds>(finish2-start2).count();
				mseconds2=std::chrono::duration_cast<std::chrono::milliseconds>(finish2-start2).count();
				nseconds2=std::chrono::duration_cast<std::chrono::nanoseconds>(finish2-start2).count();

			}			
			else
			{
				auto start1 = std::chrono::high_resolution_clock::now();
				selection_sort(global_eid,pointer,size);
				auto finish1 = std::chrono::high_resolution_clock::now();
				
				auto start2 = std::chrono::high_resolution_clock::now();
				selectionSort(pointer,size/4);
				auto finish2 = std::chrono::high_resolution_clock::now();

				seconds1=std::chrono::duration_cast<std::chrono::seconds>(finish1-start1).count();
				mseconds1=std::chrono::duration_cast<std::chrono::milliseconds>(finish1-start1).count();
				//nseconds1=std::chrono::duration_cast<std::chrono::nanoseconds>(finish1-start1).count();
				
				seconds2=std::chrono::duration_cast<std::chrono::seconds>(finish2-start2).count();
				mseconds2=std::chrono::duration_cast<std::chrono::milliseconds>(finish2-start2).count();
				//nseconds2=std::chrono::duration_cast<std::chrono::nanoseconds>(finish2-start2).count();
				
				nseconds1=-1;
				nseconds2=-1;
				
			}
					

			//std::cout <<",";
			
			int kb=size/1024;
			int mb=(kb/1024);

//			cout<<seconds1<<","<<mseconds1<<","<<nseconds1<<","<<seconds2<<","<<mseconds2<<","<<nseconds2<<","<<kb<<","<<mb <<endl;

			fprintf(fptr,"%d,%d,%d,%d,%d,%d,%d,%d,%d\n",size/4,seconds1,mseconds1,nseconds1,seconds2,mseconds2,nseconds2,kb,mb);
			fflush(fptr);
		}


		fclose(fptr);

		// Destroy the enclave 
    		sgx_destroy_enclave(global_eid);
	}
    
    return 0;
}

