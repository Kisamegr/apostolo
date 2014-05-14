#include "dbtproj.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void ReadFile(FILE *infile, block_t block);
void GenerateFile(FILE *outfile, int nblocks);


int main(int argc, char** argv) {

	int nblocks = 3;	// number of blocks in the file
	record_t record;
	block_t block;
	unsigned int recid = 0;
	FILE *infile, *outfile;


	// generate a file 
	outfile = fopen("file.bin", "w");
	//GenerateFile(outfile, nblocks);
	
	for (int b = 0; b<nblocks; ++b) { // for each block

		block.blockid = b;
		for (int r = 0; r<MAX_RECORDS_PER_BLOCK; ++r) { // for each record

			// prepare a record
			record.recid = recid++;
			record.num = rand() % 1000;
			strcpy(record.str, "hello");   // put the same string to all records
			record.valid = true;

			memcpy(&block.entries[r], &record, sizeof(record_t)); // copy record to block
		}

		block.nreserved = MAX_RECORDS_PER_BLOCK;
		block.valid = true;

		fwrite(&block, 1, sizeof(block_t), outfile);	// write the block to the file
	}

	fclose(outfile);

	
	//read file
	infile = fopen("file.bin", "r");
	ReadFile(infile, block);

	system("pause");
	return 0;
}


/**
   infile: the name of the input file
   field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nsorted_segs: number of sorted segments produced (this should be set by you)
   npasses: number of passes required for sorting (this should be set by you)
   nios: number of IOs performed (this should be set by you)

*/
void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios)
{
	// FIRST PART
	/*
	
	// open file and print contents
	FILE *input = fopen(infile, "r");
	while (!feof(input)) { // while end-of-file has not been reached ... 

		fread(&block, 1, sizeof(block_t), input); // read the next block
		nreserved = block.nreserved;

		// print block contents
		for (int i = 0; i<nreserved; ++i) {
			printf("this is block id: %d, record id: %d, num: %d, str: %s\n",
			block.blockid, block.entries[i].recid, block.entries[i].num, block.entries[i].str);
		}
	}

	fclose(input);

	for (int i = 0; i < 21; i++)
	{

	}*/
	//read the input file
	//Read each block one at a time
	//When a block is read, put it in the buffer
	//Then quicksort the records in the block
	//Continue reading the next block
	//Concurency would be good

	//hello! my first commit!


	// SECOND PART

	// Put buffer_size-1 blocks in the buffer, and compare their record's values
	// Put the result in the last buffer slot 
	// When the last slot is full, save the new block to the ram and continue the comparing
	// Do this until there are not blocks in the buffer
	// Then put the next batch and do the same
	// When all the old blocks are merged, do the same with the new bigger-merged blocks, until you got the final merged block
	

}

/* ----------------------------------------------------------------------------------------------------------------------
infile: the name of the input file
field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
buffer: pointer to memory buffer
nmem_blocks: number of blocks in memory
outfile: the name of the output file
nunique: number of unique records in file (this should be set by you)
nios: number of IOs performed (this should be set by you)
----------------------------------------------------------------------------------------------------------------------
*/
void EliminateDuplicates(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios)
{
	// FIRST PART
	
	// Do a mergesort to the file, in order to the the sorted records

	
	// SECOND PART

	// Pass the sorted file and delete the duplicates

}

/* ----------------------------------------------------------------------------------------------------------------------
infile1: the name of the first input file
infile2: the name of the second input file
field: which field will be used for the join: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
buffer: pointer to memory buffer
nmem_blocks: number of blocks in memory
outfile: the name of the output file
nres: number of pairs in output (this should be set by you)
nios: number of IOs performed (this should be set by you)
----------------------------------------------------------------------------------------------------------------------
*/
void MergeJoin(char *infile1, char *infile2, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios)
{
	// Read the 2 files
	// Merge the files into 1, and apply merge sort
	// Pass the new sorted file and find the duplicates
	// Save each record that 

}


void GenerateFile(FILE *outfile, int nblocks)
{
	// generate a file 
	block_t block;
	record_t record;
	unsigned int recid = 0;

	for (int b = 0; b<nblocks; ++b) { // for each block

		block.blockid = b;
		for (int r = 0; r<MAX_RECORDS_PER_BLOCK; ++r) { // for each record

			// prepare a record
			record.recid = recid++;
			record.num = rand() % 1000;
			strcpy(record.str, "hello");   // put the same string to all records
			record.valid = true;

			memcpy(&block.entries[r], &record, sizeof(record_t)); // copy record to block
		}

		block.nreserved = MAX_RECORDS_PER_BLOCK;
		block.valid = true;

		fwrite(&block, 1, sizeof(block_t), outfile);	// write the block to the file
	}

	fclose(outfile);
}

void WriteFile()
{

}

void ReadFile(FILE *infile, block_t block)
{
	// open file and print contents
	int nreserved;
	while (!feof(infile)) { // while end-of-file has not been reached ... 

		fread(&block, 1, sizeof(block_t), infile); // read the next block
		nreserved = block.nreserved;

		// print block contents
		for (int i = 0; i<nreserved; ++i) {
			printf("this is block id: %d, record id: %d, num: %d, str: %s\n",
				block.blockid, block.entries[i].recid, block.entries[i].num, block.entries[i].str);
		}
	}

	fclose(infile);
}

