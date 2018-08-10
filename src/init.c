#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#ifdef __MPI
#include <fftw3-mpi.h>
#include <mpi.h>
#else
#include <fftw3.h>
#endif

#include "phys_const.h"
#include "confObj.h"
#include "grid.h"
#include "sources.h"
#include "photion_background.h"
#include "sources_to_grid.h"
#include "fraction_q.h"
#include "filtering.h"
#include "self_shielding.h"

#include "density_distribution.h"
#include "recombination.h"
#include "mean_free_path.h"

#include "input_redshifts.h"
#include "input_grid.h"

#include "restart.h"

#include "cifog.h"

#include "init.h"


int32_t init_cifog(char *iniFile, confObj_t *simParam, double **redshift_list, grid_t **grid, 
                   integral_table_t **integralTable, photIonlist_t **photIonBgList, int32_t *num_cycles,
                   int32_t myRank)
{


    double zstart = 0., zend = 0., delta_redshift = 0.;

    //read paramter file
    *simParam = readConfObj(iniFile);
    
    if((*simParam)->calc_ion_history == 1){
        *num_cycles = (*simParam)->num_snapshots;
    }else{
        *num_cycles = 1;
    }

    //verify that helium runs contain helium!
    if((*simParam)->solve_He == 1)
    {
        if((*simParam)->Y <= 0.)
        {
            fprintf(stderr, "If you include helium its mass fraction Y should be larger than zero!\n");
            exit(EXIT_FAILURE);
        }
    }
    
    //read redshift files with outputs
    *redshift_list = NULL;
    if(myRank==0) printf("\n++++\nreading redshift list of files and outputs... ");
    *redshift_list = read_redshift_list((*simParam)->redshift_file, *num_cycles);
    if(*redshift_list != NULL)
    {
      if ((*simParam)->inputfiles_simulation == 1)
      { 
        *num_cycles = (*simParam)->stop_snapshot - (*simParam)->SimulationLowSnap + 1;
      }
      else
      {
        *num_cycles = (*simParam)->stop_snapshot + 1;
      } 
    }

    if(myRank==0) printf("done\n+++\n");

    //read files (allocate grid)
    *grid = initGrid();
    if(myRank==0) printf("\n++++\nreading files to grid... ");
    read_files_to_grid(*grid, (*simParam));
    if(myRank==0) printf("done\n+++\n");
    
    //read photoionization background values 
    if(myRank==0) printf("\n++++\nreading photoionization background rates... ");
    *photIonBgList = read_photIonlist((*simParam)->photHI_bg_file);
    if(myRank==0) printf("done\n+++\n");
    
    if((*simParam)->calc_recomb == 2)
    {
        //read table for recombinations
        if(myRank==0) printf("\n++++\nread table for recombinations... ");
        *integralTable = initIntegralTable((*simParam)->zmin, (*simParam)->zmax, (*simParam)->dz, (*simParam)->fmin, (*simParam)->fmax, (*simParam)->df, (*simParam)->dcellmin, (*simParam)->dcellmax, (*simParam)->ddcell);
        if(myRank==0) printf("done\n+++\n");
    }

    printf("\nThis run computes %d times the ionization field (num_cycles)\n", *num_cycles);
    if((*simParam)->calc_ion_history == 1)
    {
        zstart = (*simParam)->redshift_prev_snap;
        zend = (*simParam)->redshift;
        
        if(*redshift_list == NULL)
        {
            (*simParam)->redshift_prev_snap = zstart;
            delta_redshift = (zstart-zend)/(double)*num_cycles;
            (*simParam)->redshift = zstart - delta_redshift;
            printf("ROQEJROEQJR\n");
            exit(0);
        }
    }

    return EXIT_SUCCESS;  
}
