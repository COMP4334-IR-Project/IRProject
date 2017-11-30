
#include <windows.h>
#include <mysql.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "..//stem.h"
#include "..//IInvFile.h"

IInvFile InvFile;

int main(int argc, char *argv[])
{
	char fname[50] = "queryTDN";
	InvFile.MakeHashTable(13023973);
	int mode;
	mode = (int) argv[1];
	
	printf("Load Inverted File......\n");
	InvFile.ReadPostFile("..//InvFile.txt");
	//InvFile.LoadInvFile();
	printf("Load Document Length File......\n");
	InvFile.ReadDocFile("..//DocLen.txt");
	//InvFile.LoadDocRec();

	printf("Begin to retrieve...\n");
	InvFile.Retrieval(fname, mode, true);

	system("pause");
	return 0;
}

