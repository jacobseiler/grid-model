#ifndef INIT_H 
#define INIT_H 
#endif

#include <stdint.h>

int32_t init_cifog(char *iniFile, confObj_t *simParam, double **redshift_list, grid_t **grid, integral_table_t **integralTable, 
                   photIonlist_t **photIonBgList, int32_t *num_cycles, int32_t myRank);

int32_t cleanup_cifog(confObj_t simParam, integral_table_t *integralTable, photIonlist_t *photIonBgList, 
                      grid_t *grid, double *redshift_list, int32_t myRank);
