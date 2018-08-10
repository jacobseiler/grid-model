#ifndef INIT_H 
#define INIT_H 
#endif

#include <stdint.h>

int32_t init_cifog(char *iniFile, confObj_t *simParam, double **redshift_list, grid_t **grid, integral_table_t **integralTable, 
                   photIonlist_t **photIonBgList, int32_t *num_cycles, int32_t myRank);
