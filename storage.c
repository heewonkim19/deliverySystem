#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *context : package context (message string)
*/

typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) 
{
	
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");

}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) 
{

	storage_t storage = { .building = 0, .room = 0, .cnt = 0 };	
	deliverySystem[x][y] = storage;																	

}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) 
{

	char passwd[PASSWD_LEN+1];
	
	printf(" input password for (%i, %i) storage : ", x, y);												
	scanf("%4s", &passwd);																					// scan password
	fflush(stdin);
	
	if(strcmp(deliverySystem[x][y].passwd, passwd) == 0 || strcmp(masterPassword, passwd) == 0) 			// Compare Passwords Matches
		return 0; 
	else 
		return -1;

}


// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) 
{

	int x, y;
	FILE *pf = fopen(filepath, "w");											
	
	fprintf(pf, "%d %d\n", systemSize[0], systemSize[1]);						// systemSize[0] is low, systemSize[1] is coulmn => Print low and coulmn at file
	fprintf(pf, "%s\n", masterPassword);										// Print masterPassword at file

	for(x=0; x<systemSize[0]; x++) 
	{
		for(y=0; y<systemSize[1]; y++) 
		{		
			if (deliverySystem[x][y].cnt > 0) 									// If package exist, Print information of package at file
			{			
				fprintf(pf, "%d %d %d %d %s %s\n", x, y, deliverySystem[x][y].building, deliverySystem[x][y].room, deliverySystem[x][y].passwd, deliverySystem[x][y].context);
			}
		}
	}
	
	if (pf == NULL)																// If failed to backup
		return -1;																// return -1
	
	fclose(pf);

	return 0;
}


//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system)
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) 
{

	int i;
	int x, y;	

	FILE *pf = fopen(filepath, "r"); 
		
	if(pf != NULL) 																			// If file is opened
	{

		fscanf(pf, "%d %d", &systemSize[0], &systemSize[1]);								// Scan systemsize at file	
		fscanf(pf, "%s", &masterPassword);													// Scan systemsize at file

		deliverySystem = (storage_t **) malloc(systemSize[0] *sizeof(storage_t *));			// Allocate memory
		for(i=0; i<systemSize[0]; i++) 														// Allocate 2D array memory
		{
			deliverySystem[i] = (storage_t *) malloc(systemSize[1] * sizeof(storage_t));	
		}
		
		for (x = 0; x < systemSize[0]; x++) 
		{
			for (y = 0; y < systemSize[1]; y++) 
			{
				initStorage(x, y);															// Initialize the storage
			}
		}
		
		// Bring in information at file
		while(feof(pf) == 0)																
		{
			// Initialize and Allocate memory
			storage_t storage = {.building = 0, .room = 0, .cnt = 0, .passwd={}, .context = (char*)malloc((MAX_MSG_SIZE + 1)*sizeof(char))};
			x = -1, y = -1;																	// Reset x and y (x and y can't -1)
			
			fscanf(pf, "%d %d %d %d %s %s", &x, &y, &storage.building, &storage.room, &storage.passwd, storage.context);

			if (x >= 0 && y >= 0) 															// x and y is not -1(>=0)
			{
				storage.cnt = strlen(storage.context);														
				deliverySystem[x][y] = storage;				
				storedCnt++;				
			}
		}

		fclose(pf);

	} 
	else 
		return -1;


	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) 
{
	
	int i;
	
	for(i=0; i<systemSize[0]; i++) 
	{
		free(deliverySystem[i]);			// 2D array release
	}
	
	free(deliverySystem);					// array relase 
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) 
{

	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0; j<systemSize[1]; j++)
	{
		printf(" %i\t\t", j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0; i<systemSize[0]; i++)
	{
		printf("%i|\t",i);
		for (j=0; j<systemSize[1]; j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}


//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) 
{

	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) 
{
		
	if(deliverySystem[x][y].cnt > 0)									// If fail to push 
		return -1;														// return -1

	deliverySystem[x][y].building = nBuilding;							// Input nBuilding to deliverySystem[x][y].building
	deliverySystem[x][y].room = nRoom;									// Input nRoom to deliverySystem[x][y].room
	strcpy(deliverySystem[x][y].passwd, passwd);						// copy passwd to deliverysystem's passwd 					
	deliverySystem[x][y].context = msg;									// Input msg to deliverySystem[x][y].context
	deliverySystem[x][y].cnt = sizeof(deliverySystem[x][y].context);	// Input size of context to cnt 

	storedCnt++;														// If success to push, add +1 to storedCnt
	
	return 0;
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) 
{

	if(inputPasswd(x, y) == 0)    // If the entered password is correct
	{
		printStorageInside(x, y); // Print delivery context
		initStorage(x, y);        // Initialize the storagenitialize Storage
		storedCnt--;			  // storedCnt(The sum of delivery) - 1
	} 
	else 						  // If the entered password isn't correct
		return -1;				  // return -1

	return 0;
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) 
{
	
	int x, y;
	int cnt = 0;																						// Variables that tell if there is or is not a package
	
	for(x=0; x<systemSize[0]; x++)
	{
		for(y=0; y<systemSize[1]; y++) 
		{
			if(deliverySystem[x][y].building == nBuilding && deliverySystem[x][y].room == nRoom) 		// If package exist
			{
				printf(" -----------> Found a package in (%i, %i)\n", x, y);
				cnt++;															 						// If finding package succeed, cnt + 1

			}
		}
	}
	
	return cnt;

}
