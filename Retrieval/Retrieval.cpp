
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
	char fname[50] = "queryTDN";
	InvFile.MakeHashTable(13023973);

	printf("Load Inverted File......\n");
	InvFile.ReadPostFile("..//InvFile.txt");
	//InvFile.LoadInvFile();
	printf("Load Document Length File......\n");
	InvFile.ReadDocFile("..//DocLen.txt");
	//InvFile.LoadDocRec();

	printf("Begin to retrieve...\n");
	InvFile.Retrieval(fname);

	system("pause");
	return 0;
}

