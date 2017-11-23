
#include <windows.h>
#include <mysql.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "..//stem.h"
#include "..//IInvFile.h"

IInvFile InvFile;

char * replaceAll(char * src, char oldChar, char* newChar) {
	int len = 0;
	char * head = src;
	char * nc = newChar;
	while (*src != '\0') {
		src++;
		len++;
	}
	char * p = (char*)malloc(2 *len* sizeof(char*));
	char * res = p;
	src = head;
	while (*src != '\0') {
		if (*(src) == oldChar) {
			newChar = nc;
			while (*newChar != '\0') {
				*res = *newChar;
				res++;
				newChar++;
			}
			src++;
		}else {
			*res = *src;
			res++;
			src++;
		}

	}
	*res = '\0';
	return p;
}

int main()
{
	FILE * fpp = NULL;
	FILE * fpf = NULL;
	hnode * h;
	post * p;
	int count = 0;
	char tmp[10000];
	char sql[200];
	int wordcount;
	char term[200];
	char name[100];
	char * path = (char*)malloc(200 * sizeof(char*));
	int docid;
	int pos;

	// open file post and file
	fpp = fopen("..//post1.txt", "rb");
	if (!fpp) {
		printf("file post open fail!\n");
		system("pause");
		exit(0);
	}
	fpf = fopen("..//file.txt", "rb");
	if (!fpf) {
		printf("file file open fail!\n");
		system("pause");
		exit(0);
	}

	/*
	// the last document that has been saved in post is 23745
	// read file "post" into table post
	// only execute once to build table post
	while (!feof(fpp)) {
		fscanf(fpp, "%s %d %d", term, &docid, &pos);
		
		if (docid > 9500 && docid < 10000 || docid>20447) {

			sprintf(sql, "insert into post values(\"%s\",%d,%d)\n", term, docid, pos);
			if (docid % 5000 == 0 || docid == 64813) {
				printf("%s\n", sql);
			}
			if (mysql_query(&InvFile.conn, sql)) {
				printf("error:%s::[%s,%d,%d]\n", mysql_error(&InvFile.conn), term, docid, pos);
			}
		}
	}
	*/

	// convert table post to posting
	// only execute once to build table posting
	// operate in memory
	// build table posting use file post directly
	// use hash table
	InvFile.MakeHashTable(13023973);
	/*while (!feof(fpp)) {
		fscanf(fpp, "%s %d %d", &(term[0]), &docid, &pos);
		//if (docid > 100) {break;}
		h = InvFile.FindNode(term);
		if (!h) {
			h = InvFile.MakeNode(term);
		}
		h->df++;
		p = InvFile.FindPost(h, docid);
		if (!p) {
			p = InvFile.AddPost(h, docid, 1);
		}else {
			p->freq++;
		}
	}*/
	while (fgets(tmp, 10000, fpp) != NULL) {
		// Get the stem, the document identifier and the location
		sscanf(tmp, "%s %d %d", &(term[0]), &docid, &pos);

		// Add posting into the Integrated Inverted index
		// See lecture notes on Implementation
		h = InvFile.FindNode(term);
		if (!h) {
			h = InvFile.MakeNode(term);
		}
		h->df++;
		p = InvFile.FindPost(h, docid);
		if (!p) {
			p = InvFile.AddPost(h, docid, 1);
		}
		else {
			p->freq++;
		}
		// Keep us informed about the progress
		count++;
		if ((count % 100000) == 0) printf("Added [%d]\r\n", count);
	}

	printf("Saving inverted file ...\r\n");
	InvFile.WriteInvFile("..//InvFile.txt");

	// read file "file" into table doclen
	/*while (!feof(fpf)) {
		fscanf(fpf, "%d %d @ %s %s", &docid, &wordcount, &(name[0]), path);
		path = replaceAll(&(path[0]), '\\', "\\\\");
		//printf("%s", path);
		//if (docid > 100) { break;	}
		sprintf(sql, "insert into doclen(docid,name,wordcount,path) values(%d,\"%s\",%d,\"%s\")\n", docid, &(name[0]), wordcount, path);
		if (docid % 10000 == 0) {
			printf("%s", sql);
		}
		if (mysql_query(&InvFile.conn, sql)) {
			printf("error:%s::[%d,\"%s\",%d,\"%s\"]\n", mysql_error(&InvFile.conn), docid, &(name[0]), wordcount, path);
		}
	}
	*/
    return 0;
}

