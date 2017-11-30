#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>

#include "IInvFile.h"

/* Constructor
 * param: void
 * ret: void
 */
IInvFile::IInvFile() {
	hsize = 0;
	htable = NULL;
	MaxDocid = 0;
	Files = NULL;
	rsize = 0;
	rank = NULL;
	EDW[0] = 1.0;
	EDW[1] = 0.55;
	EDW[2] = 0.25;
}

/* Deconstructor
 * param: void
 * ret: void
 */
IInvFile::~IInvFile() {
	ClearHashTable();
	ClearDocRec();
}

// Hash related functions

/* calculate the hush value, and save in hvalue
 * this function is given by Robert
 * param: s-string to hash, h-hash table size
 * ret: hash value(also saved in hvalue)
 */
int IInvFile::Hash(char * s, int h) { 
	char * t = s;
	hvalue = 3;

	if (t == NULL) {
		return 0;
	}

	while (*t != '\0') {
		hvalue = (31 * hvalue + 57 * ((int)(unsigned char)*t));
		if (hvalue < 0) {
			hvalue = -1 * hvalue;
		}
		hvalue = hvalue % h;
		t++;
	}

	return hvalue;
}

/* allocate memory to hash table
 * param: h-hash table size
 * ret: void
 * initialize htable
 */
void IInvFile::MakeHashTable(long int h){
	// in case h is a non-positive integer
	if (h <= 0) {
		htable = NULL;
		printf("Hash table initialization fails!\r\n");
		return;
	}

	hsize = h;
	htable = (hnode **)calloc(hsize, sizeof(hnode*));
	for (int i = 0; i < hsize; i++) {
		htable[i] = NULL;
	}
}

/* create a new hash node with string `s`, and insert this node in the hash table
 * param: s-hash node term value
 * ret: a new hash node
 */
hnode * IInvFile::MakeNode(char * s){
	int hvalue;
	// initialize the node
	hnode * h = (hnode*)malloc(sizeof(hnode));
	h->term = _strdup(s);
	h->df = 0;
	h->posting = NULL;
	h->next = NULL;

	// insert the node in hash table
	hvalue = Hash(s, hsize);
	h->next = htable[hvalue];
	htable[hvalue] = h;

	return h;
}

/* find the term node in hash table
 * param: s-term string
 * ret: hash node with term s
 */
hnode * IInvFile::FindNode(char * s){
	hnode * h;

	// in case hash table has not been initialized
	if (htable == NULL) {
		printf("Hash table does not exist!\r\n");
		return NULL;
	}

	h = htable[Hash(s,hsize)];
	while (h) {
		if (strcmp(h->term, s) == 0) {
			break;
		}
		h = h->next;
	}
	return h;
}

/* helper function to free posting node iteratively
 * param: posting list start pointer
 * ret: void
 */
void IInvFile::FreePosting(post * p) {
	post * pnext;		// save pointer to next post while use p to free current post
	while (p) {
		pnext = p->next;
		free(p);
		p = pnext;
	}
}

/* helper function to free hash node iteratively
 * param: hash node start pointer
 * ret: void
 */
void IInvFile::FreeHnode(hnode * h) {
	hnode * hnext;		// save pointer to next hash node while use h to free current hash node
	
	// for each hash node, release its posting list
	// and then release this node and turn to next node
	while (h) {
		hnext = h->next;
		FreePosting(h->posting);
		free(h->term);				// also need to free char * variable
		free(h);
		h = hnext;
	}
}

/* destroy hash table and release the memory
 * param: void
 * ret: void
 */
void IInvFile::ClearHashTable(){	
	// there is no hash table
	if (htable == NULL) {
		return;
	}

	// for each entry of hash table
	for (int i = 0; i < hsize; i++) {
		FreeHnode(htable[i]);
	}
}

// Inverted file processing

/* add a posting node in the posting list of hash node of term `s`
 * param: s-term string, docid-document ID, freq-term frequency in document docid
 * ret: a post pointer to the new post node
 */
post * IInvFile::AddPost(hnode * h, int docid, int freq){
	post * p;
	p = (post*)malloc(sizeof(post));
	p->docid = docid;
	p->freq = freq;
	
	// link the posting record in the hash node of s
	p->next = h->posting;
	h->posting = p;

	return NULL;
}

/* find a posting with `docid` in the posting list of hash node `h`
 * param: h-hash node with paticular term, docid-expected document ID
 * ret: posting record of paticular term with docid
 */
post * IInvFile::FindPost(hnode * h, int docid) {
	post * p = h->posting;
	while (p) {
		if (p->docid == docid) {
			break;
		}
		p = p->next;
	}
	return p;
}

/* count the document number that contains the term `s`
 * param: s-term string
 * ret: document frequency
 */
int IInvFile::CountDF(char * s){
	hnode * h;
	post * p;
	int count = 0;

	h = FindNode(s);
	p = h->posting;
	
	while (p) {
		count++;
		p = p->next;
	}
	return count;
}

/* calculate the idf with document frequecy `df`
 * param: df-document frequency
 * ret: inverse document frequency
 */
double IInvFile::CalcIDF(int df){
	double idf;
	double N = 1 + (double)MaxDocid;
	idf = (double)log10(N / (double)df);
	return idf;
}

/* save the data in mysql, use two table: token and posting
 * param: void
 * ret: void
 */
void IInvFile::SaveInvFile(){
	
}

/* save the data in file f
*/
void IInvFile::WriteInvFile(char * f) {
	FILE * fp = fopen(f, "wb");		// open the file for writing
	hnode * w;
	post * k;
	if ((hsize > 0) && (htable != NULL)) {	// Is there a hash table?
		for (int i = 0; i < hsize; i++) {	// For each hash table entry do
			w = htable[i];		// Go through each hash node
			while (w != NULL) {
				fprintf(fp, "%s %d:", w->term, w->df);
				k = w->posting;	// Go through each post node
				while (k != NULL) {
					fprintf(fp, "%d %d,", k->docid, k->freq);
					k = k->next;	// next post node
				}
				fprintf(fp, "\r\n");
				w = w->next;		// next hash node
			}
		}
	}
	fclose(fp);				// close the file
}


/* load the data in token and posting from mysql to memory htable 
 * param:
 * ret:
 */
void IInvFile::LoadInvFile(){

}

/* read posting information from file `f`
 * param:
 * ret:
 */
void IInvFile::ReadPostFile(char * f){
	FILE * fp = fopen(f, "rb");
	hnode * w;
	post * k;
	char c;
	bool next;
	int state = 0;
	int cnt;
	char line[1000];
	char stem[1000];
	int df;
	int i;
	int docid;
	int freq;

	if (fp == NULL) {
		printf("Aborted: file not found for <%s>\r\n", f);
		return;
	}
	if ((hsize > 0) && (htable != NULL)) {
		i = 0;
		cnt = 0;
		do {
			next = true;
			if (fread(&c, 1, 1, fp) > 0) { // read a character
				switch (state) {
				case 0:	if (c != ':') line[i++] = c;
						else {
							line[i] = '\0';
							sscanf(line, "%s %d", stem, &df);
							w = FindNode(stem);
							if (w == NULL) {
								w = MakeNode(stem);
								w->df = df;
							}
							i = 0;
							state = 1;
							//printf("Read [%s,%d]\r\n",stem, df);
						}
						break;
				case 1:
					if (c == '\r') i = 0;
					else
						if (c == '\n') {
							cnt = 0;
							i = 0;
							state = 0;
						}
						else if (c == ',') {
							line[i] = '\0';
							cnt++;
							sscanf(line, "%d %d", &docid, &freq);
							k = w->posting; // push the data into the posting field
							w->posting = new post;
							w->posting->docid = docid;
							w->posting->freq = freq;
							w->posting->next = k;
							if (MaxDocid < docid) MaxDocid = docid;
							//printf("[%d] %d %d,\r\n", cnt, docid, freq);
							i = 0;
						}
						else line[i++] = c;
						break;
				}
			}
			else next = false;
		} while (next == true);
	}
	else printf("Aborted: no hash table\r\n");
	fclose(fp);
}

// Document length file processing

char * replaceAll(char * src, char oldChar, char* newChar) {
	int len = 0;
	char * head = src;
	char * nc = newChar;
	while (*src != '\0') {
		src++;
		len++;
	}
	char * p = (char*)malloc(2 * len * sizeof(char*));
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
		}
		else {
			*res = *src;
			res++;
			src++;
		}

	}
	*res = '\0';
	return p;
}

/* allocate memory to Files
 * param: void
 * ret: void
 */
void IInvFile::MakeDocRec() {
	if (Files) {
		ClearDocRec();
	}

	if (MaxDocid > 0) {
		Files = (DocRec *)calloc(MaxDocid+1, sizeof(DocRec));
		for (int i = 0; i <= MaxDocid; i++) {
			Files[i].docid = i;
			Files[i].dname = NULL;
			Files[i].len = 0;
			Files[i].maxFreq = 0;
		}
	}
}

/* calculate document length of all documents and save in Files
 * param: void
 * ret: document length
 */
DocRec * IInvFile::CalcDocLen() {
	if (!htable) {
		printf("No Hash Table!\n\r");
		exit(0);
	}else if (!Files) {
		printf("No Files List!\n\r");
		exit(0);
	}
	
	double idf;
	double idf2;
	double resu;
	hnode * h = NULL;
	post * p = NULL;

	// iterate Hash table to add (tf-idf)^2 of each token to all Files' doclen fields
	for (int i = 0; i < hsize; i++) {
		h = htable[i];
		while (h) {
			idf = CalcIDF(h->df);
			idf2 = idf*idf;
			p = h->posting;
			while (p) {				
				resu = idf2*((double)p->freq);
				resu *= p->freq;
				Files[p->docid].len += resu;	// add tf-idf ^ 2 to doclen of each file
				
				// save the max frequency of document
				if (Files[p->docid].maxFreq < p->freq) {
					Files[p->docid].maxFreq = p->freq;
				}

				p = p->next;
			}
			h = h->next;
		}
	}

	// save square root of each file's sum of td-idf^2
	for (int i = 0; i <= MaxDocid; i++) {
		Files[i].len = (float)sqrt((double)Files[i].len);
	}
	return Files;
}

/* destroy and release memory of Files
 * param: void
 * ret: void
 */
void IInvFile::ClearDocRec(){
	if (Files == NULL) {
		return;
	}
	for (int i = 0; i <= MaxDocid ; i++) {
		free(Files[i].dname);
	}
	free(Files);
}

/* save data in mysql, use table doclen
 * param:
 * ret:
 */
void IInvFile::SaveDocRec() {
}

/* save data in doclen file
*/
void IInvFile::WriteDocRec(char * f) {
	FILE * fp;
	FILE * fpf;
	int docid = 0;
	char name[100];
	char * path = (char*)malloc(200*sizeof(char));
	int wordcount = 0;

	fpf = fopen("..//file.txt", "rb");
	if (!fpf) {
		printf("file file open fail!\n");
		system("pause");
		exit(0);
	}

	while (!feof(fpf)) {
		fscanf(fpf, "%d %d @ %s %s", &docid, &wordcount, &(name[0]), &(path[0]));
		path = replaceAll(&(path[0]), '\\', "\\\\");
		//printf("%s", path);
		//if (docid > 100) { break;	}
		Files[docid].dname = _strdup(name);
	}

	if ((MaxDocid > 0) && (Files != NULL)) {
		fp = fopen(f, "wb");
		fprintf(fp, "%d\r\n", MaxDocid + 1);
		for (int i = 0; i <= MaxDocid; i++) {
			if (Files[i].docid == -1)
				fprintf(fp, "%d %s %e %d\r\n", i, Files[i].dname, Files[i].len, Files[i].maxFreq);
			else
				fprintf(fp, "%d %s %e %d\r\n", Files[i].docid, Files[i].dname, Files[i].len, Files[i].maxFreq);
		}
		fclose(fp);
	}

}

/* read document information from file `f`
* param:
* ret:
*/
void IInvFile::ReadDocFile(char * f) {
	FILE * fp;
	char line[10000];
	int id;
	int i = 0;
	int maxFreq;
	char str[1000];
	float doclen;

	if ((MaxDocid > 0) && (Files != NULL)) {
		printf("LoadDocRec error: already has document records\r\n");
		return;
	}

	fp = fopen(f, "rb");
	if (fp == NULL) {
		printf("LoadDocRec error: Cannot find file [%s]\r\n", f);
		return;
	}
	if (fgets(line, 10000, fp) != NULL) {
		sscanf(line, "%d", &MaxDocid);
		MaxDocid--;
		MakeDocRec();
		while (fgets(line, 10000, fp) != NULL) {
			sscanf(line, "%d %s %e %d", &id, str, &doclen, &maxFreq);
			Files[i].docid = id;
			Files[i].dname = _strdup(str);
			Files[i].len = doclen;
			Files[i].maxFreq = maxFreq;
			if (i > MaxDocid) printf("LoadDocRec error: MaxDocid incorrect [%d,MaxDocid=%d]\r\n", i, MaxDocid);
			i++;
		}
	}
	fclose(fp);
}

/* load data from mysql to memory Files
 * param:
 * ret:
 */
void IInvFile::LoadDocRec(){
	
}
										
// Retrieval processing

/* get next word in query
 * param:
 * ret:
 */
char * IInvFile::GotoNextWord(char * s){
	char * q = s;

	if ((s == NULL) || (*s == '\0')) {
		return NULL;
	}

	while ((*q == ' ') && (*q == '\t') || (*q<'A' || (*q > 'Z'&&*q < 'a') || *q>'z')) {
		*q = ' ';
		q++;
	}
	while ((*q >= 'A'&&*q <= 'Z') || (*q >= 'a'&&*q <= 'z')) {
		//printf("%c", *q);
		q++;
	}

	while (*q<'A' || (*q > 'Z'&&*q < 'a') || *q>'z') {
		if (*q == '\0') {
			//printf("\n");
			return q;
		}
		*q = '\0';
		q++;
	}
	//printf("\n");
	return q;
}

/* get query number from each line of query file
 * param: str-query line
 * ret: query number
 */
int GetQueryNo(char * q) {
	int num = 0;
	while (*q >= '0' && *q <= '9') {
		num *= 10;
		num += *q - '0';
		q++;
	}
	return num;
}

/* convert the query string into qTerm link list
 * param: q-query string
 * ret: the pointer to qTerm
 */
qTerm * IInvFile::GetQueryTerm(char * q) {
	qTerm * p = NULL;
	qTerm * query = NULL;
	qTerm * newQ;
	char * s = q;
	char * w;
	bool next = true;

	while (*s <'A' || (*s > 'Z'&&*s < 'a') || *s>'z') {
		s++;
	}

	do {
		w = s;
		// get one word stem first
		s = GotoNextWord(s);
		if (*w == ' ') {
			w++;
		}
		if (s == NULL) {
			next = false;
		}
		else {
			// if s is not the end of query, let the space between w and s be '\0'
			if (*s != '\0') {
				*(s - 1) = '\0';
			}
			// stem the word w
			Stemmer.Stem(w);

			// create a new node of qTerm
			newQ = (qTerm*)malloc(sizeof(qTerm));
			newQ->term = _strdup(w);
			newQ->exist = false;
			newQ->next = NULL;

			// link new Q to the link list query
			if (p) {
				// if p is not the header of list
				p->next = newQ;
				p = p->next;
			}else {
				// if p is the header of the list
				p = newQ;
				query = p;
			}
		}
	} while (next);
	return query;
}

void query_free(qTerm * q) {
	qTerm * p = q;
	while (q) {
		q = q->next;
		free(p->term);
		free(p);
		p = q;
	}
}

/* caluculate edit distance between string a and string b
 * param: a-one string, b-another string
 * ret: the edit distance between a and b
 */
int IInvFile::Edit_Distance(char *a, char *b){
	int lena = strlen(a);
	int lenb = strlen(b);
	int * d = (int*)calloc(lenb + 1, sizeof(int));
	int old;
	int temp;

	for (int j = 0; j <= lenb; j++) {
		d[j] = j;
	}

	for (int i = 1; i <= lena; i++) {
		old = d[0];
		d[0] = i;
		for (int j = 1; j <= lenb; j++) {
			temp = d[j];
			if (a[i - 1] == b[j - 1]) {
				d[j] = old;
			}
			else {
				//d[j] = min_of_three(d[j] + 1, d[j - 1] + 1, old + 1);
				d[j] = d[j] + 1;
				if (d[j] >= d[j - 1] + 1) {
					d[j] = d[j - 1] + 1;
				}
				if (d[j] >= old + 1) {
					d[j] = old + 1;
				}
			}
			old = temp;
		}
	}
	temp = d[lenb];
	delete []d;
	
	return temp;
}
	
// Comparison function used by qsort(.): see below
int compare(const void * aa, const void * bb) {
	RetRec * a = (RetRec *)aa;
	RetRec * b = (RetRec *)bb;

	if (a->cossim > b->cossim) return -1;
	else if (a->cossim < b->cossim) return 1;
	else return 0;
}

/* use Boolean Model retrieve query `q`
* param:
* ret:

RetRec * IInvFile::BM_Search(char * exist, char * unexist) {
	
}*/

/* use Fuzzy Boolean Model retrieve query `q`
* param:
* ret:
*/
RetRec * IInvFile::FBM_Search(char * q) {

	return NULL;
}

/* use VSM with EDIT DISTANCE CONSTRAINT to retrieve query 'q'
 * param: 
 * ret: 
 */
RetRec * IInvFile::VSM_Search_ED(char *q) {
	int maxED = 2;		// max tolerable edit distance
	int distance;
	qTerm * query;
	qTerm * qnode;
	bool next = true;
	hnode * h;
	post * p;
	float idf;

	// initialize result set
	if (!rank) {
		free(rank);
	}
	rank = (RetRec *)calloc(MaxDocid + 1, sizeof(RetRec));
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].docid = Files[i].docid;
		rank[i].cossim = 0;
	}

	// convert the query string into link list
	query = GetQueryTerm(q);

	if (!query) {
		printf("no query!\n");
		return NULL;
	}

	// for each word in hash table, decide the 'tolerable term' for the query
	// and use all these 'tolerable term' to calculate similarity
	// for each word in hash table
	for (int i = 0; i < hsize; i++) {
		h = htable[i];
		while (h) {

			// for each term, calculate the edit distance between term in hash node and query node
			// use while loop for query link list
			qnode = query;
			while (qnode) {
				distance = Edit_Distance(qnode->term, h->term);
				// if the edit distance between the term in hnode and current term in query is smaller than max tolerable distance
				// then view them as matching
				if (distance <= maxED) {
					qnode->exist = true;

					// add tf of term multiples idf of the token to the power 2 to corresponding document
					// maybe multiply some predefined weight due to different distance
					p = h->posting;
					while (p) {
						idf = CalcIDF(h->df);
						rank[p->docid].cossim += p->freq*idf*idf*EDW[distance];
						p = p->next;
					}
				}					
				qnode = qnode->next;
			}
				
			// get next hnode
			h = h->next;
		}
	}
		
	// print terms that did not match any term in hash table within 'tolerable distance'
	while (query) {
		if (!query->exist) {
			printf("Query term does not exist <%s>\n", query->term);
		}
		query = query->next;
	}

	// for each dot result of document and query, it should be divided by query length multiplying document length
	// beacause the query length is the same, give it const value 1
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].cossim /= Files[i].len;
	}
	
	qsort(rank, MaxDocid + 1, sizeof(RetRec), compare);	// qsort is a C function: sort results

	query_free(query);

	return rank;
}

/* use VSM search with query `q`
 * param:
 * ret:
 */
RetRec * IInvFile::VSM_Search(char * q) {
	char * s = q;
	char * w;
	bool next = true;
	hnode * h;
	post * k;
	float idf;

	// initialize result set
	if (!rank) {
		free(rank);
	}
	rank = (RetRec *)calloc(MaxDocid + 1, sizeof(RetRec));
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].docid = Files[i].docid;
		rank[i].cossim = 0;
	}

	// for each word in query
	do {
		w = s;
		// get one word stem first
		if ((s = GotoNextWord(s)) == NULL) {
			next = false;
		}
		else {
			// if s is not the end of query, let the space between w and s be '\0'
			if (*s != '\0') {
				*(s - 1) = '\0';
			}
			// stem the word w
			Stemmer.Stem(w);
			// find the hnode of this stem in hash table
			h = FindNode(w);
			if (h) {
				// for each posting of the token
				// add tf of term multiples idf of the token to the power 2 to corresponding document
				k = h->posting;
				while (k) {
					idf = CalcIDF(h->df);
					rank[k->docid].cossim += k->freq*idf*idf;
					k = k->next;
				}
			}
			else if (strlen(w) > 0) {
				printf("Query term does not exist <%s>\n", w);
			}
		}
	} while (next);

	// for each dot result of document and query, it should be divided by query length multiplying document length
	// beacause the query length is the same, give it const value 1
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].cossim /= (float)Files[i].len;
	}

	qsort(rank, MaxDocid + 1, sizeof(RetRec), compare);	// qsort is a C function: sort results

	return rank;
}

/* use VSM search with query `q`
* param:
* ret:
*/
RetRec * IInvFile::VSM_Search_MF(char * q) {
	char * s = q;
	char * w;
	bool next = true;
	hnode * h;
	post * k;
	float idf;

	// initialize result set
	if (!rank) {
		free(rank);
	}
	rank = (RetRec *)calloc(MaxDocid + 1, sizeof(RetRec));
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].docid = Files[i].docid;
		rank[i].cossim = 0;
	}

	// for each word in query
	do {
		w = s;
		// get one word stem first
		if ((s = GotoNextWord(s)) == NULL) {
			next = false;
		}
		else {
			// if s is not the end of query, let the space between w and s be '\0'
			if (*s != '\0') {
				*(s - 1) = '\0';
			}
			// stem the word w
			Stemmer.Stem(w);
			// find the hnode of this stem in hash table
			h = FindNode(w);
			if (h) {
				// for each posting of the token
				// add tf of term multiples idf of the token to the power 2 to corresponding document
				k = h->posting;
				while (k) {
					idf = CalcIDF(h->df);
					rank[k->docid].cossim += k->freq*idf*idf;
					rank[k->docid].cossim /= Files[k->docid].maxFreq;
;
					k = k->next;
				}
			}
			else if (strlen(w) > 0) {
				printf("Query term does not exist <%s>\n", w);
			}
		}
	} while (next);

	// for each dot result of document and query, it should be divided by query length multiplying document length
	// beacause the query length is the same, give it const value 1
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].cossim /= (float)Files[i].len;
	}

	qsort(rank, MaxDocid + 1, sizeof(RetRec), compare);	// qsort is a C function: sort results

	return rank;
}

/* use VSM search with query `q`
* param:
* ret:
*/
RetRec * IInvFile::VSM_Search_DOT(char * q) {
	char * s = q;
	char * w;
	bool next = true;
	hnode * h;
	post * k;
	float idf;

	// initialize result set
	if (!rank) {
		free(rank);
	}
	rank = (RetRec *)calloc(MaxDocid + 1, sizeof(RetRec));
	for (int i = 0; i <= MaxDocid; i++) {
		rank[i].docid = Files[i].docid;
		rank[i].cossim = 0;
	}

	// for each word in query
	do {
		w = s;
		// get one word stem first
		if ((s = GotoNextWord(s)) == NULL) {
			next = false;
		}
		else {
			// if s is not the end of query, let the space between w and s be '\0'
			if (*s != '\0') {
				*(s - 1) = '\0';
			}
			// stem the word w
			Stemmer.Stem(w);
			// find the hnode of this stem in hash table
			h = FindNode(w);
			if (h) {
				// for each posting of the token
				// add tf of term multiples idf of the token to the power 2 to corresponding document
				k = h->posting;
				while (k) {
					idf = CalcIDF(h->df);
					rank[k->docid].cossim += k->freq*idf*idf;
					k = k->next;
				}
			}
			else if (strlen(w) > 0) {
				printf("Query term does not exist <%s>\n", w);
			}
		}
	} while (next);

	// dot product does not need to divide document length

	qsort(rank, MaxDocid + 1, sizeof(RetRec), compare);	// qsort is a C function: sort results

	return rank;
}


/* print top `num` ranked result records
 * param:
 * ret:
 */
void IInvFile::PrintTop(RetRec * r, int num){
	int count = 0;
	qsort(r, MaxDocid + 1, sizeof(RetRec), compare);	// qsort is a C function: sort results

	while (count < num) {
		// if no more realted document
		if ((rank[count].docid == 0) && (rank[count].cossim == 0.0)) {
			return;
		}
		//printf("[%d]\t%d\t%s\t%f\r\n", count + 1, rank[count].docid, Files[rank[count].docid].dname, rank[count].cossim);
		count++;
	}
}

/* retrieve with command in console
* param:
* ret:
*/
void IInvFile::Retrieval() {
	int cho;
	int count;
	bool next = true;
	char cmd[10000];
	char * temp_cmd;
	RetRec * temp_rank = (RetRec*)calloc(1000, sizeof(RetRec));

	do {
		printf("Type the query or \"_quit\" to exit\r\n");
		gets_s(cmd);
		temp_cmd = _strdup(cmd);
		if (strcmp(cmd, "_quit") == 0) {
			next = false;
		}else {
			count = 0;
			/*printf("1.VSM\n2.VSM with edit distance\n");
			scanf("%d%*c", &cho);
			switch (cho) {
			case 1: {
				VSM_Search(cmd);
				break;
			}
			case 2: {
				VSM_Search_ED(cmd);
				break;
			}

			}
			PrintTop(rank, 10);*/
			/* compare how many documents in the rank of top 1000 of VSM match those of VSM with ED*/
			VSM_Search(cmd);
			for (int i = 0; i < 1000; i++) {
				temp_rank[i].docid = rank[i].docid;
				temp_rank[i].cossim = rank[i].cossim;
			}
			VSM_Search_ED(temp_cmd);
			for (int i = 0; i < 1000; i++) {
				for (int j = 0; j < 1000; j++) {
					if (temp_rank[i].docid == rank[j].docid) {
						count++;
						printf("VSM   : [%d]\t%d\t%s\t%f\r\n", i + 1, temp_rank[i].docid, Files[temp_rank[i].docid].dname, temp_rank[i].cossim);
						printf("VSM_ED: [%d]\t%d\t%s\t%f\r\n", j + 1, rank[j].docid, Files[rank[j].docid].dname, rank[j].cossim);
					}
				}
			}
			printf("total match number: %d\n", count);

		}
	} while (next == true);
}

/* retrieve all querys in file `f`
 * param:
 * ret:
 */
void IInvFile::Retrieval_VSM(char * f){
	FILE * fp;
	FILE * fpr;
	char query[10000];
	char fname[100];
	int queryNo = 0;
	sprintf(fname,"%s_VSM",f);

	// open retrieval file
	fp = fopen(f,"rb");
	if (!fp) {
		printf("File %s open fails!\n", f);
		system("pause");
		exit(0);
	}

	fpr = fopen(fname, "wb");
	if (!fp) {
		printf("File %s open fails!\n", fname);
		system("pause");
		exit(0);
	}

	while (fgets(query, 10000, fp) != NULL) {
		queryNo = GetQueryNo(query);
		VSM_Search(query);
		PrintTop(rank, 10);
		SaveResult(fpr,queryNo,1000);
	}

	fclose(fp);
	fclose(fpr);
}

/* retrieve all querys in file `f`
* param:
* ret:
*/
void IInvFile::Retrieval_VSM_MF(char * f) {
	FILE * fp;
	FILE * fpr;
	char query[10000];
	char fname[100];
	int queryNo = 0;
	sprintf(fname, "%s_VSM_MF", f);

	// open retrieval file
	fp = fopen(f, "rb");
	if (!fp) {
		printf("File %s open fails!\n", f);
		system("pause");
		exit(0);
	}

	fpr = fopen(fname, "wb");
	if (!fp) {
		printf("File %s open fails!\n", fname);
		system("pause");
		exit(0);
	}

	while (fgets(query, 10000, fp) != NULL) {
		queryNo = GetQueryNo(query);
		VSM_Search_MF(query);
		PrintTop(rank, 10);
		SaveResult(fpr, queryNo, 1000);
	}

	fclose(fp);
	fclose(fpr);
}

/* retrieve all querys in file `f`
* param:
* ret:
*/
void IInvFile::Retrieval_VSM_DOT(char * f) {
	FILE * fp;
	FILE * fpr;
	char query[10000];
	char fname[100];
	int queryNo = 0;
	sprintf(fname, "%s_VSM_DOT", f);

	// open retrieval file
	fp = fopen(f, "rb");
	if (!fp) {
		printf("File %s open fails!\n", f);
		system("pause");
		exit(0);
	}

	fpr = fopen(fname, "wb");
	if (!fp) {
		printf("File %s open fails!\n", fname);
		system("pause");
		exit(0);
	}

	while (fgets(query, 10000, fp) != NULL) {
		queryNo = GetQueryNo(query);
		VSM_Search_DOT(query);
		PrintTop(rank, 10);
		SaveResult(fpr, queryNo, 1000);
	}

	fclose(fp);
	fclose(fpr);
}

/* retrieve all querys in file `f`
* param:
* ret:
*/
void IInvFile::Retrieval_VSM_ED(char * f) {
	FILE * fp;
	FILE * fpr;
	char query[10000];
	char fname[100];
	int queryNo = 0;
	sprintf(fname, "%s_VSM_ED", f);

	// open retrieval file
	fp = fopen(f, "rb");
	if (!fp) {
		printf("File %s open fails!\n", f);
		system("pause");
		exit(0);
	}

	fpr = fopen(fname, "wb");
	if (!fp) {
		printf("File %s open fails!\n", fname);
		system("pause");
		exit(0);
	}

	while (fgets(query, 10000, fp) != NULL) {
		queryNo = GetQueryNo(query);
		VSM_Search_ED(query);
		PrintTop(rank, 10);
		SaveResult(fpr, queryNo, 1000);
	}

	fclose(fp);
	fclose(fpr);
}

/* save the result in file `f`
 * param:
 * ret:
 */
void IInvFile::SaveResult(FILE * fp, int queryNo, int num){
	for (int i = 0; i < num; i++) {
		fprintf(fp, "%d Q0 %s %d %.1f HKPU-1\n", queryNo, Files[rank[i].docid].dname, i, rank[i].cossim);
	}
}