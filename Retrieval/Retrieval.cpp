
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

	printf("Load Inverted File......\n");
	InvFile.LoadInvFile();
	printf("Load Document Length File......\n");
	InvFile.LoadDocRec();

	printf("Begin to retrieve...\n");
	InvFile.Retrieval();
	return 0;
}

