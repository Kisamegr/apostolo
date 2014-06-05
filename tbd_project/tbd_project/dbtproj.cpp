#include "dbtproj.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

void ReadFile(FILE *infile, block_t block);
void GenerateFile(FILE *outfile, int nblocks);
int compareRecid(const void * a, const void * b);
int compareNum(const void * a, const void * b);
int compareStr(const void * a, const void * b);
int compareNumStr(const void * a, const void * b);
int min(int a, int b);


int main() {

	int nblocks = 3;	// number of blocks in the file
	record_t record;
	block_t block;
	unsigned int recid = 0;
	FILE *infile, *outfile;

	outfile = fopen("file.bin", "w");

	// generate a file
	GenerateFile(outfile, nblocks);



	//read file
	//infile = fopen("file.bin", "r");
	//ReadFile(infile, block);

	block_t buffer[100];

	MergeSort("file.bin", 0, buffer, 100);


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
//void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios)
void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks)

{
	block_t block;
	segment_t segment;
	int number_of_blocks, number_of_segments = 0;


	// open file and print contents
	FILE *input = fopen(infile, "r");
	while (!feof(input)) { // while end-of-file has not been reached ...

		fread(&block, 1, sizeof(block_t), input); // read the next block
		number_of_blocks = block.blockid + 1; //oxi ektos????????????????????????????/

		// print block contents
		/*for (int i = 0; i<nreserved; ++i) {
		printf("this is block id: %d, record id: %d, num: %d, str: %s\n",
		block.blockid, block.entries[i].recid, block.entries[i].num, block.entries[i].str);
		}*/
	}

	fclose(input);

	block_t *blocks = new block_t[number_of_blocks];
	int total = 0;
	int p = 0;
	FILE *segments_file;


	input = fopen(infile, "r");
	segments_file = fopen(SEGMENTS_FILE, "a");

	while (!feof(input)) { // while end-of-file has not been reached ...

		fread(&blocks[total], 1, sizeof(block_t), input); // read the next block

		buffer[p++] = blocks[total++];

		if (p == nmem_blocks || feof(input))
		{
			record_t *entries = new record_t[p*MAX_RECORDS_PER_BLOCK];
			for (int i = 0; i<p; i++)
			{
				for (int j = 0; j<buffer[i].nreserved; j++)
				{
					entries[i*MAX_RECORDS_PER_BLOCK + j] = buffer[i].entries[j];
				}
			}

			//qsort(entries,p*MAX_RECORDS_PER_BLOCK,sizeof(record_t))

			switch (field)
			{
			case 0:
				qsort(entries, p*MAX_RECORDS_PER_BLOCK, sizeof(record_t), compareRecid); //den dinoume orismata sto compare?
				break;
			case 1:
				qsort(entries, p*MAX_RECORDS_PER_BLOCK, sizeof(record_t), compareNum); //den dinoume orismata sto compare?
				break;
			case 2:
				qsort(entries, p*MAX_RECORDS_PER_BLOCK, sizeof(record_t), compareStr); //den dinoume orismata sto compare?
				break;
			case 3:
				qsort(entries, p*MAX_RECORDS_PER_BLOCK, sizeof(record_t), compareNumStr); //den dinoume orismata sto compare?
				break;

			}

			/*if(number_of_segments == 0)
			{
			segment = malloc(sizeof(segment_t));
			number_of_segments++;
			}
			else
			{
			segment = realloc(segment,sizeof(segment_t) * ++number_of_segments);
			}*/

			for (int i = 0; i<p; i++)
			{
				for (int j = 0; j<buffer[i].nreserved; j++)
				{
					buffer[i].entries[j] = entries[i*p + j];
				}

				segment.blocks[i] = buffer[i];
			}

			segment.nblocks = p;
			fwrite(&segment, 1, sizeof(segment_t), segments_file);


			number_of_segments++;
			p = 0;

		}

		/*switch (field)
		{
		case 0:
		qsort(&blocks[c].entries,blocks[c].nreserved,sizeof(record_t),compareRecid); //den dinoume orismata sto compare?
		break;
		case 1:
		qsort(&blocks[c].entries,blocks[c].nreserved,sizeof(record_t),compareNum); //den dinoume orismata sto compare?
		break;
		case 2:
		qsort(&blocks[c].entries,blocks[c].nreserved,sizeof(record_t),compareStr); //den dinoume orismata sto compare?
		break;
		case 3:
		qsort(&blocks[c].entries,blocks[c].nreserved,sizeof(record_t),compareNumStr); //den dinoume orismata sto compare?
		break;

		}
		*/
		// print block contents
		/*for (int i = 0; i<nreserved; ++i) {
		printf("this is block id: %d, record id: %d, num: %d, str: %s\n",
		block.blockid, block.entries[i].recid, block.entries[i].num, block.entries[i].str);
		}*/
	}

	fclose(input);
	fflush(segments_file);
	fclose(segments_file);



	int current_segment_batch = 0;
	int new_segment_file_counter = 0;
	bool segments_file_ended = false;
	bool second_segment_file = false;
	FILE *new_segments_file = NULL;


	while (new_segment_file_counter > 1 || new_segments_file == NULL)
	{
		// Delete old/used files
		if (!second_segment_file)
			remove(NEW_SEGMENTS_FILE);
		else
			remove(SEGMENTS_FILE);

		// Loop for all the segment batches (if the segments number is greater than the buffer blocks)
		while (!segments_file_ended)
		{

			// Max blocks in buffer, to compare
			int max_blocks_in_buffer = min(number_of_segments, nmem_blocks - 1); // max records???????????///////
			int current_segment_index = 0;
			int new_segment_block_index = 0;
			segment_t new_segment;
			segment_t read_segment;

			// The pointer to the current record in each block
			int *current_record_index = new int[max_blocks_in_buffer];
			int current_block_in_segment_index[MAX_BLOCKS_IN_SEGMENT];
			int current_block = 0;
			int output_record_index = 0;

			// Temp variables
			int min_record_recid = MAX_RECORDS_PER_BLOCK + 1;
			int min_record_num = 1000 + 1; //MAX_RECORD_NUM
			int min_record_block_index = -1;
			char *min_record_string;

			int remaining_segments;


			// Initialize the current record index and add the blocks to the buffer
			if (!second_segment_file)
				segments_file = fopen(SEGMENTS_FILE, "r");
			else
				segments_file = fopen(NEW_SEGMENTS_FILE, "r");

			fseek(segments_file, current_segment_batch*nmem_blocks*sizeof(segment_t), SEEK_SET);
			int i = 0;
			while (i<max_blocks_in_buffer && !feof(segments_file))
			{
				current_record_index[i] = 0;
				//buffer[i] = blocks[current_block++];
				fread(&read_segment, 1, sizeof(segment_t), segments_file);
				buffer[i] = read_segment.blocks[0];
				current_segment_index++;
			}

			if (feof(segments_file))
				segments_file_ended = true;

			remaining_segments = max_blocks_in_buffer;


			// The loop the current segment batch / blocks left in the buffer
			while (remaining_segments >0)
			{
				// The loop for comparing the values
				for (int k = 0; k<max_blocks_in_buffer; k++) // NA GINEI WHILE GIA PERIPTWSEIS
				{
					min_record_block_index = -1;

					if (current_record_index[k] < MAX_RECORDS_PER_BLOCK) //If the block still has records to be compared
					{
						if (min_record_block_index == -1) // Initiate the min variables
						{
							min_record_block_index = k;
							min_record_recid = buffer[k].entries[current_record_index[k]].recid;
							min_record_num = buffer[k].entries[current_record_index[k]].num;
							strcpy(min_record_string, buffer[k].entries[current_record_index[k]].str);
						}

						switch (field)
						{
							// Compare recid
						case 0:
							if (buffer[k].entries[current_record_index[k]].recid < min_record_recid)
							{
								min_record_block_index = k;
								min_record_recid = buffer[k].entries[current_record_index[k]].recid;
							}
							break;
							// Compare number
						case 1:
							if (buffer[k].entries[current_record_index[k]].num < min_record_num)
							{
								min_record_block_index = k;
								min_record_num = buffer[k].entries[current_record_index[k]].num;
							}
							break;
							// Compare string
						case 2:

							if (strcmp(buffer[k].entries[current_record_index[k]].str, min_record_string) < 0)
							{
								min_record_block_index = k;
								strcpy(min_record_string, buffer[k].entries[current_record_index[k]].str);
							}
							break;
							// Compare number & string
						case 3:
							if (buffer[k].entries[current_record_index[k]].num < min_record_num)
							{
								min_record_block_index = k;
								min_record_num = buffer[k].entries[current_record_index[k]].num;
								strcpy(min_record_string, buffer[k].entries[current_record_index[k]].str);
							}
							else if (buffer[k].entries[current_record_index[k]].num == min_record_num && strcmp(buffer[k].entries[current_record_index[k]].str, min_record_string) < 0)
							{
								min_record_block_index = k;
								min_record_num = buffer[k].entries[current_record_index[k]].num;
								strcpy(min_record_string, buffer[k].entries[current_record_index[k]].str);
							}
							break;
						}
					}
				}

				// Pass the min record to the next empty slot of the last block of the buffer
				buffer[max_blocks_in_buffer - 1].entries[output_record_index++] = buffer[min_record_block_index].entries[current_record_index[min_record_block_index]++];

				// If the last output-block is full...
				if (output_record_index == MAX_RECORDS_PER_BLOCK)
				{
					// Write the last block to the new segment
					new_segment.blocks[new_segment_block_index++] = buffer[max_blocks_in_buffer - 1];
					output_record_index = 0;
				}

				// Check if the min value block in the buffer is empty (after the comparison), in order to replace it
				if (current_record_index[min_record_block_index] == MAX_RECORDS_PER_BLOCK)
				{
					//buffer[min_record_block_index] = blocks[current_block++];
					fseek(segments_file, current_segment_batch*nmem_blocks + min_record_block_index * sizeof(segment_t), SEEK_SET);
					fread(&read_segment, 1, sizeof(segment_t), segments_file);

					// If the segment still has more blocks to compare...
					if (read_segment.nblocks > current_block_in_segment_index[min_record_block_index])
					{
						// Replace the finished block with a new one
						buffer[min_record_block_index] = read_segment.blocks[current_block_in_segment_index[min_record_block_index]];
						current_record_index[min_record_block_index] = 0;
					}
					else // Else lower the remaining_segments counter;
						remaining_segments--;


				}
			}

			fclose(segments_file);

			new_segment.segment_id = 0;
			new_segment.nblocks = new_segment_block_index;

			if (!second_segment_file)
				new_segments_file = fopen(NEW_SEGMENTS_FILE, "a");
			else
				new_segments_file = fopen(SEGMENTS_FILE, "a");


			fwrite(&new_segment, 1, sizeof(segment_t), new_segments_file); // ALLAGH GIATI DN GINETE DUNAMIKO

			fflush(new_segments_file);
			fclose(new_segments_file);


			current_segment_batch++;

			delete current_record_index;

		}
		
		second_segment_file = !second_segment_file;

	}

	//read the input file
	//Read each block one at a time
	//When a block is read, put it in the buffer
	//Then quicksort the records in the block
	//Continue reading the next block
	//Concurency would be good

	// SECOND PART

	// Put buffer_size-1 blocks in the buffer, and compare their record's values
	// Put the result in the last buffer slot
	// When the last slot is full, save the new block to the ram and continue the comparing
	// Do this until there are not blocks in the buffer
	// Then put the next batch and do the same
	// When all the old blocks are merged, do the same with the new bigger-merged blocks, until you got the final merged block

	delete blocks;

	FILE *output_file;

	if (!second_segment_file)
		output_file = fopen(SEGMENTS_FILE, "r");
	else
		output_file = fopen(NEW_SEGMENTS_FILE, "r");

	fread(&segment, 1, sizeof(segment_t), output_file);

	for (int i = 0; i<segment.nblocks; i++)
	{
		for (int j = 0; j<MAX_RECORDS_PER_BLOCK; j++)
		{
			cout << "Block #" << i << " Record: " << j << endl;
			cout << "   recid: " << segment.blocks[i].entries[j].recid << endl;
			cout << "   number: " << segment.blocks[i].entries[j].num << endl;
			cout << "   string: " << segment.blocks[i].entries[j].str << endl;


		}
		cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
	}

}

/** ----------------------------------------------------------------------------------------------------------------------
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

/** ----------------------------------------------------------------------------------------------------------------------
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


int compareRecid(const void * a, const void * b)
{
	record_t *r1 = (record_t*)a;
	record_t *r2 = (record_t*)b;
	int r1recid = r1->recid;
	int r2recid = r2->recid;
	return (r1recid - r2recid);
}

//used to compare numbers. Returns their difference
int compareNum(const void * a, const void * b)
{
	record_t *r1 = (record_t*)a;
	record_t *r2 = (record_t*)b;
	int r1num = r1->num;
	int r2num = r2->num;
	return (r1num - r2num);
}

int compareStr(const void * a, const void * b)
{
	record_t *r1 = (record_t*)a;
	record_t *r2 = (record_t*)b;
	char* r1str = r1->str;
	char* r2str = r2->str;
	return strcmp(r1str, r2str);
}

int compareNumStr(const void * a, const void * b)
{
	if (compareNum(a, b) != 0)
		return compareNum(a, b);

	return compareStr(a, b);
}

int min(int a, int b)
{
	if (a <= b)
		return a;
	return b;
}





//---------------------------klassikes sinartiseis --------------------------------//
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
