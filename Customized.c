#define _CRT_SECURE_NO_DEPRECATE


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
	int TotalInode;
	int FreeInode;
}SUPERBLOCK, *PSUPERBLOCK;


typedef struct inode
{
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;			
	int LinkCount;
	int ReferenceCount;
	int permission;		
	struct inode* next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
	int readOffset;
	int writeOffset;
	int count;
	int mode;			
	PINODE ptrInode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrFileTable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj; 
PINODE Head = NULL;

void man(char* name)
{

	if (name == NULL)
	{
		return;
	}

	if (strcmp(name, "create") == 0)
	{
		printf("Description: Used to create new regular file\n");
		printf("Usage: create File_Name Permission \n");
	}
	else if (strcmp(name, "read") == 0)
	{
		printf("Description: Used to read data from regular file\n");
		printf("Usage: read File_Name No_Of_Bytes_To_Read \n");
	}
	else if (strcmp(name, "write") == 0)
	{
		printf("Description: Used to write into regular file\n");
		printf("Usage: write File_Name Data_To_write size_of_data \n");
	}
	else if (strcmp(name, "ls") == 0)
	{
		printf("Description: List files\n");
		printf("Usage: ls\n");
	}
	else if (strcmp(name, "stat") == 0)
	{
		printf("Description: Used to display Information about file\n");
		printf("Usage: stat File_Name\n");
	}
	else if (strcmp(name, "fstat") == 0)
	{
		printf("Description:  Used to display Information about file\n");
		printf("Usage: fstat File_descriptor \n");
	}
	else if (strcmp(name, "truncate") == 0)
	{
		printf("Description: Used to remove data from file\n");
		printf("Usage: truncate File_Name \n");
	}
	else if (strcmp(name, "open") == 0)
	{
		printf("Description: Used to open existing file\n");
		printf("Usage: open File_name mode \n");
	}
	else if (strcmp(name, "close") == 0)
	{
		printf("Description: Used to close opened file\n");
		printf("Usage: close file_Name \n");
	}
	else if (strcmp(name, "closeAll") == 0)
	{
		printf("Description: Used to close all open file\n");
		printf("Usage: closeAll \n");
	}
	else if (strcmp(name, "lseek") == 0)
	{
		printf("Description: Used to change file offset\n");
		printf("Usage: lseek File_Name ChangeInOffset StartPoint\n");
	}
	else if (strcmp(name, "rm") == 0)
	{
		printf("Description: Used to delete the file\n");
		printf("Usage: rm File_Name\n");
	}
	else
	{
		printf("Error: No Manual entry available.\n");
	}
}

void DisplayHelp()
{
	printf("ls: To List out all files\n");
	printf("clear: To clear console\n");
	printf("open : To open the file\n");
	printf("close : To close the file\n");
	printf("closeall : To close all the open file\n");
	printf("read : To read the content from file\n");
	printf("write : To write contents in file\n");
	printf("exit : To exit from program\n");
	printf("stat : To Display information of file using name\n");
	printf("fstat : To Display information of file using file descriptor\n");
	printf("truncate : To remove all data from file\n");
	printf("rm : To delete the file \n");
}


int GetFDFromName(char *name)
{
	int i = 0;

	while (i < 50)
	{
		if (UFDTArr[i].ptrFileTable != NULL)

			if (strcmp((UFDTArr[i].ptrFileTable->ptrInode->FileName), name) == 0)
				break;
		i++;
	}
	if (i == 50)
		return -1;
	else
		return i;
}

PINODE chkFileExist(char *name) 
{
	PINODE Temp = Head;

	if (name == NULL)
	{
		return NULL;
	}

	while (Temp != NULL)
	{
		if (strcmp(name, Temp->FileName) == 0)
			break;
		Temp = Temp->next;
	}

	return Temp;
}

void CreateDILB()
{
	int i = 1;   
	PINODE newn = NULL;
	PINODE Temp = Head;

	for (; i <= MAXINODE; i++)
	{
		newn = (PINODE)malloc(sizeof(INODE));

		newn->InodeNumber = i;
		newn->FileType = 0;
		newn->FileSize = 0;
		newn->ReferenceCount = 0;
		newn->FileSize = 0;
		newn->FileActualSize = 0;
		newn->LinkCount = 0;
		newn->Buffer = NULL;
		newn->next = NULL;


		if (Temp == NULL)
		{
			Head = newn;
			Temp = Head;
		}
		else
		{
			Temp->next = newn;
			Temp = Temp->next;
		}
	}

	printf("DILB intialization completed successfully.\n");
}


void InitialiseSuperBlock()
{
	int i = 0;      

	while (i < MAXINODE)
	{
		UFDTArr[i].ptrFileTable = NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInode = MAXINODE;
	SUPERBLOCKobj.FreeInode = MAXINODE;

	printf("Superblock intialization completed successfully.\n");
}

int CreateFile(char *name, int permission)
{
	int i = 0;
	PINODE Temp = Head;

	if (name == NULL || permission < 1 || permission > 3)
	{
		return -1;
	}
	if (SUPERBLOCKobj.FreeInode == 0)
	{
		return -2;
	}
	if (chkFileExist(name) != NULL)
	{
		return -3;
	}

	while (Temp != NULL)
	{
		if (Temp->FileType == 0)
		{
			break;
		}
		Temp = Temp->next;
	}

	while (i < MAXINODE)
	{
		if (UFDTArr[i].ptrFileTable == NULL)
		{
			break;
		}
 		i++;
	}

	UFDTArr[i].ptrFileTable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if (UFDTArr[i].ptrFileTable == NULL)
	{
		return -4;
	}

	SUPERBLOCKobj.FreeInode--;
	UFDTArr[i].ptrFileTable->readOffset = 0;
	UFDTArr[i].ptrFileTable->writeOffset = 0;
	UFDTArr[i].ptrFileTable->mode = 0;
	UFDTArr[i].ptrFileTable->count = 1;
	UFDTArr[i].ptrFileTable->ptrInode = Temp;

	strcpy(UFDTArr[i].ptrFileTable->ptrInode->FileName, name);
	UFDTArr[i].ptrFileTable->ptrInode->FileType = REGULAR;
	UFDTArr[i].ptrFileTable->ptrInode->FileSize = MAXFILESIZE;
	UFDTArr[i].ptrFileTable->ptrInode->ReferenceCount = 1;
	UFDTArr[i].ptrFileTable->ptrInode->LinkCount = 1;
	UFDTArr[i].ptrFileTable->ptrInode->permission = permission;
	UFDTArr[i].ptrFileTable->ptrInode->Buffer = (char*)malloc(sizeof(1024));

	return i;
}

void DeallocateDS()
{
	int i = 1;
	PINODE Temp = NULL;

	for (; i <= MAXINODE; i++)
	{
		Temp = Head;
		Head = Head->next;

		if (Temp->Buffer != NULL)
		{
			free(Temp->Buffer);
		}

		free(Temp);
	}

	for (i = 0; i < MAXINODE; i++)
	{
		free(UFDTArr[i].ptrFileTable);
	}

	printf("Terminating from the VFS Shell\n");
}

void ls_file()
{
	PINODE Temp = Head;
	while (Temp != NULL)
	{
		if (Temp->FileType != 0)
		{
			printf("%s\t", Temp->FileName);
		}
		Temp = Temp->next;
	}

}

int fstat_file(int fd)
{
	int i = 0;
	//Logic remaining
	return i;
}

int stat_file(int fd)
{
	int i = 0;
	//Logic remaining
	return i;
}

int write(char* name, char *arr, int size)
{
	int fd = GetFDFromName(name);
	//Logic remaining

}


int main()
{
	char *ptr = NULL;
	char str[80];			//width of terminal is 80
	char command[4][80];	//maximum command length is 4
	int count = 0, ret = 0, fd = 0;
	char arr[1024];

	InitialiseSuperBlock();
	CreateDILB();
	
	while (1)
	{
		fflush(stdin);
		strcpy(str, " ");

		printf("\nCustomized dynamic file system > ");
		fgets(str, 80, stdin);

		count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		if (count == 1)
		{
			if (strcmp(command[0], "ls") == 0)
			{
				ls_file();
				continue;
			}
			else if (strcmp(command[0], "closeall") == 0)
			{

			}
			else if (strcmp(command[0], "clear") == 0)
			{
				system("clear");
				continue;
			}
			else if (strcmp(command[0], "help") == 0)
			{
				DisplayHelp();
				continue;

			}
			else if (strcmp(command[0], "exit") == 0)
			{
				DeallocateDS();
				break;
			}
			else
			{
				printf("Error: Command Not Found!\n");
				continue;
			}
		}
		else if (count == 2)
		{
			if (strcmp(command[0], "man") == 0)
			{
				man(command[1]);
				continue;
			}
			else if (strcmp(command[0], "close") == 0)
			{

			}
			else if (strcmp(command[0], "truncate") == 0)
			{
				
			}
			else if (strcmp(command[0], "stat") == 0)
			{
				ret = stat_file(atoi(command[1]));
				if (ret == -1)
				{
					printf("Error: Incomplete parameters.\n");
				}
				else if (ret == -2)
				{
					printf("Error: Their is no such fie\n");
				}
				continue;
			}
			else if (strcmp(command[0], "fstat") == 0)
			{
				ret = fstat_file(atoi(command[1]));
				if (ret == -1)
				{
					printf("Error: Incomplete parameters.\n");
				}
				else if (ret == -2)
				{
					printf("Error: Their is no such fie\n");
				}
				continue;
			}
			else
			{
				printf("Error: Command Not Found!\n");
				continue;
			}
		}
		else if (count == 3)
		{
			if (strcmp(command[0], "create") == 0)
			{
				ret = CreateFile(command[1], atoi(command[2]));

				if (ret >= 0)
					printf("File is succcessfully created with file descriptor: %d\n", ret);

				if (ret == -1)
					printf("Error : Incorrect parameters\n");

				if (ret == -2)
					printf("Error : Their is no Inodes\n");

				if (ret == -3)
					printf("Error : File already exist\n");

				if (ret == -4)
					printf("Error : Memory allocation failure\n");

				continue;
			}
			else if (strcmp(command[0], "open") == 0)
			{

			}
			else if (strcmp(command[0], "write") == 0)
			{
				printf("Enter the data to write in the file\n");
				gets(arr);

				printf("size is %d", strlen(arr));

				write(command[1], arr, strlen(arr));
			}
			else if (strcmp(command[0], "read") == 0)
			{

			}
			else
			{
				printf("Error: Command Not Found!\n");
				continue;
			}
		}
		else if (count == 4)
		{
			if (strcmp(command[0], "lseek") == 0)
			{

			}
		}
		else
		{
			printf("Error : Command Not Found!\n");
		}

	}

	_getch();
	return 0;
}

