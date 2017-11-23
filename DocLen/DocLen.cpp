
#include <windows.h>
#include <mysql.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "..//stem.h"
#include "..//IInvFile.h"

IInvFile InvFile;

int main()
{
 	InvFile.MakeHashTable(13023973);

	printf("Loading Inverted File\r\n");
	InvFile.ReadPostFile("..//InvFile.txt");

	printf("Initializing Document Records (size = %d)\r\n", InvFile.MaxDocid + 1);
	InvFile.MakeDocRec();

	printf("Compute Document Lengths...\r\n");
	InvFile.CalcDocLen();

	printf("Save Document Lengths\r\n");
	InvFile.WriteDocRec("..//DocLen.txt");

	system("pause");

	return 0;
}

