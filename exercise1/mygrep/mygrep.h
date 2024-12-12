#ifndef __MYGREP_H__
#define __MYGREP_H__

#include <stdio.h>
#include <stdbool.h>

//allgemein
int printUsage(char* programName);
void searchFile(FILE *infile, char *keyword, bool ignoreCase, FILE *outfile);





#endif //__MYGREP_H__
