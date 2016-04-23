#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#define BUFFER_SIZE 1024           
static char receive[BUFFER_SIZE];     

int main() 
{
	int option, opening, get;
	int running = 1;
	char stringToSend[BUFFER_SIZE];

	printf("Starting Test Code For Device Driver");

	while (running == 1)
	{
		opening = open("/dev/projectdriver", O_RDWR);             // Open the device with read/write access
		if (opening < 0)
		{
			printf("Failed To Open Device\n");
			return errno;
		}

		printf("Would You Like To:\n");
		printf("1. Write To Device\n");
		printf("2. Read From Device\n");
		printf("3. Close Program\n");
		scanf("%d", &option);

		switch (option)
		{
		case 1:
			printf("Type in a short string to send to the kernel module:\n");
			scanf(" %[^\n]%*c", stringToSend);                // Read in a string (with spaces)
			printf("Writing message to the device [%s].\n", stringToSend);
			get = write(opening, stringToSend, strlen(stringToSend)); // Send the string to the LKM
			if (get < 0) 
			{
				printf("Failed to write the message to the device.");
				return errno;
			}
			break;
		case 2:
			printf("Reading from the device...\n");
			get = read(opening, receive, BUFFER_SIZE);        // Read the response from the LKM
			if (get < 0) 
			{
				printf("Failed to read the message from the device.");
				return errno;
			}
			printf("The received message is: %s\n\n", receive);
			break;
		case 3:
			printf("Program Terminating\n");
			running = 0;
			break;
		default:
			printf("Invalid Entry\n");
			break;
		}
	}
	return 0;
}