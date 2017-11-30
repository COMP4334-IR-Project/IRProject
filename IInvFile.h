///////////////////////////////////////////////////////////////////////////////////////
//																					 //
// Author: Eric LIU;													//
// Date: 2017.10																	 //
// Eric LIU (c) 2017																 //
//																					 //
// Integrated Inverted Index Class Implmenetation:									 //
// There are six parts:																 //
// i.mysql related const attribute													 //
// i1.Hashing related attributes and functions										 //
// i1i.File information																 //
// iv.Inverted file processing														 //
// v.Document length file processing												 //
// vi.Retrieval processing															 //
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _IINVFILE_
#define _IINVFILE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stem.h"

// posting structure
typedef struct _post {
	int docid;				// document ID
	int freq;				// term frequency in document
	struct _post * next;	// next post node pointer
} post;

// Hash node for term structure
typedef struct _hnode {
	char * term;			// term string
	post * posting;			// posting pointer
	int df;					// document frequency
	struct _hnode * next;	// next hnode poointer
} hnode;

// Documnet record
typedef struct _DocRec {
	int docid;				// document ID
	char * dname;			// document name
	int maxFreq;			// maximum of term frequency
	double len;				// document length, |D|
} DocRec;

// Retrieval record
typedef struct _RetRec {
	int docid;				// document ID
	float cossim;			// cumulative weight 
} RetRec;

// query term link list
typedef struct _qTerm {
	char * term;			// term in query
	bool exist;				// if the term exists in the hash table		
	struct _qTerm * next;	// point to next term
} qTerm;

// Integrated inverted index class, including all related work
class IInvFile {
public:
	IInvFile();				// Constructor/
	~IInvFile();			// Deconstructor/

	// mysql related const attribute;
//	static const char user[];			// username
//	static const char pswd[];			// password
//	static const char hostname[];		// hostname
//	static const char database[];		// database name
//	static const int port;		// server port        
/*
	MYSQL conn;							// connection with database
	MYSQL_RES * result_1;				// query result
	MYSQL_RES * result_2;				// query result
	MYSQL_ROW sql_row;					// one row of query result
	MYSQL_FIELD *fd;					// one field of query result
	*/
	// Hash related attributes and functions-finished/
	int hsize;						// hash table size
	int hvalue;						// hash value of current term
	hnode ** htable;				// pointer to hash table
	int Hash(char * s, int h);		// calculate the hush value, and save in hvalue, `h` is hash table size
	void MakeHashTable(long int h);		// allocate memory to hash table, `h` is hash table size
	hnode * MakeNode(char * s);		// create a new hash node with string `s`
	hnode * FindNode(char * s);		// find the term node in hash table
	void ClearHashTable();			// destroy hash table and release the memory
	void IInvFile::FreePosting(post * p);	// helper function to free posting node iteratively
	void IInvFile::FreeHnode(hnode * h);	// helper function to free hash node iteratively

	// File information
	int MaxDocid;					// the maximum ID of all documents
	DocRec * Files;					// a list to save all documents' information

	// Inverted file processing/
	// the format in database is: ""
	post * AddPost(hnode * h, int docid, int freq);	// add a posting node in the posting list of hash node of term `s`
	post * FindPost(hnode * h, int docid);			// find a posting with `docid` in the posting list of hash node `t`
	int CountDF(char * s);							// count the document number that contains the term `s`
	double CalcIDF(int df);							// calculate the idf with document frequecy `df`
	void SaveInvFile();								// save the data in mysql, use two table: token and posting
	void WriteInvFile(char * f);					// save the data in txt file f
	void LoadInvFile();								// load the data from mysql to memory htable 
	void ReadPostFile(char * f);					// read posting information from txt file `f`

	// Document length file processing
	void MakeDocRec();						// allocate memory to Files/
	DocRec * CalcDocLen();					// calculate document length of all documents and save in Files
	void ClearDocRec();						// destroy and release memory of Files/
	void SaveDocRec();						// save data in mysql, use table doclen
	void WriteDocRec(char * f);				// save data in doclen file
	void LoadDocRec();						// load data from mysql to memory Files
	void ReadDocFile(char * f);				// read document information from file `f`

	int rsize;								// rank size
	RetRec * rank;							// ranked result records
	stemmer Stemmer;						// stemmer to stem query word
	float EDW[3];							// edit distance weight
	char * GotoNextWord(char * s);			// get next word in query
	qTerm * GetQueryTerm(char * q);			// convert the query string into qTerm link list
	int Edit_Distance(char *a, char *b);	// caluculate edit distance between string a and string b
	RetRec * BM_Search(char * exist, char * unexist);			// use Boolean Model to retrieve query `q`
	RetRec *BM_Search(char * q);
	RetRec * FBM_Search(char * q);			// use Fuzzy Boolean Model to retrieve query `q`
	RetRec * VSM_Search(char * q);			// use VSM to retrieve query `q`
	RetRec * VSM_Search_ED(char *q);		// use VSM with EDIT DISTANCE CONSTRAINT to retrieve query 'q'
	void PrintTop(RetRec * r, int num);		// print top `num` ranked result records
	void IInvFile::PrintTopForBoolean(RetRec * r, int num); // print top 'num' ranked result records for boolean
	void Retrieval();						// retrieve with command in console
	void Retrieval(char * f, int mode, bool normalization);				// retrieve all querys in file `f`
	void SaveResult(FILE * fp, int queryNo, int num);			// save the result in file `f`

};

#endif _IINVFILE_
