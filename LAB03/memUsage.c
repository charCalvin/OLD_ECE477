/*
 *	calvin skinner
 *	Feb 20, 2019
 *	477 Lab 3
 *	memory usage in percent
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int memPercent();

int main(int argc, char *argv){
	printf("Memory used(%): %d\n", memPercent());
	
	return 0;
}

int memPercent(){
	FILE	*fp;
	int	memTotal = 0;
	int	memFree = 0;
	int	memUsed = 0;
	int	percent = 0;


	// open file and retreive data
	fp = fopen("/proc/meminfo", "r");

	fscanf(fp, "%*s %d %*s\n%*s %d", &memTotal, &memFree);

	fclose(fp);	// close file

	// caculate memory usage:
	memUsed = memTotal - memFree;	// calculate memUsed
	percent = ((double)memUsed / memTotal) * 100;	// Calculate percent of memory used
	
	return percent;
}
