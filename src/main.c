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

#include "utils.h"

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

#include "cifog.h"

#include "init.h"

// Local Proto-Types //

int32_t parse_params(int32_t argc, char **argv, char *iniFile, int32_t *RestartMode);

// Local Functions //

int32_t parse_params(int32_t argc, char **argv, char *iniFile, int32_t *RestartMode)
{
    int32_t i;

    if (argc < 2 || argc > 4)
    {
        printf("cifog: (C)  - Use at own risk...\n");
        printf("USAGE:\n");
        printf("cifog iniFile\n");
        printf("Optionally: Use -s before iniFile to save a restart file after each output.\n");
        printf("Optionally: Use -r before iniFile to resume from a restart file.\n");

        exit(EXIT_FAILURE);
    }
    else 
    {
      printf("Executing with parameters: %s ", argv[0]);
      // Here we check for any runtime flags and copy the Inifile.
      i = 1;
      while ((i < argc))
      {
        printf("%s ", argv[i]);
        if (argv[i][0] == '-')
        {
          switch (argv[i][1])
          {
            case 's':
              *RestartMode += 1;
              break;

      
            case 'r':
              *RestartMode += 2;
        
              break;
      
            default:
              printf("Invalid flag, Only -s and -r are supported\n");
              exit(EXIT_FAILURE);
          }
        }
        else
        {
          strcpy(iniFile, argv[i]);
        }

        ++i;
      }
      printf("\n");
      if (*RestartMode == 1)
      {
        printf("Saving a restart file after each output.\n");
      }
      else if (*RestartMode == 2)
      {
        printf("Resuming from the restart file specified in %s\n", iniFile);
      } 
      else if (*RestartMode == 3)
      {
        printf("Resuming from the restart file specified in %s and then saving a restart file after each subsequent output.\n", iniFile);
      }
    }

  return EXIT_SUCCESS;

}

int main (int argc, /*const*/ char * argv[]) { 
    int myRank = 0;

    char iniFile[MAXLENGTH];
    confObj_t simParam;
    
    double *redshift_list = NULL;
    
    grid_t *grid = NULL;
    
    sourcelist_t *sourcelist = NULL;
    
    integral_table_t *integralTable = NULL;
    
    photIonlist_t *photIonBgList = NULL;
    
    double t1, t2;
    
    int num_cycles;
    int32_t RestartMode = 0, status;
#ifdef __MPI
    int32_t size;
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank); 
    
    t1 = MPI_Wtime();
    
    fftw_mpi_init();
#else
    t1 = time(NULL);
#endif
    
    //parse command line arguments and be nice to user

    status = parse_params(argc, argv, iniFile, &RestartMode); // Set the input parameters.
    if (status == EXIT_FAILURE)
    {
      exit(EXIT_FAILURE);
    }   
 
    //-------------------------------------------------------------------------------
    // reading input files and prepare grid
    //-------------------------------------------------------------------------------
    status = init_cifog(iniFile, &simParam, &redshift_list, &grid, &integralTable, &photIonBgList, &num_cycles, myRank); 
    if (status !=  EXIT_SUCCESS)
    {
      exit(EXIT_FAILURE);
    } 
    
    // Do some test output.
    if(myRank==0)
    {
        printf("\n++++\nTEST OUTPUT\n");
    //     printf("densSS = %e\n", ss_calc_densSS(simParam, 1.e-13, 1.e4, 6.));
    //     printf("densSS = %e\n", ss_calc_densSS(simParam, 5.1e-11, 1.e4, 14.75));
    //     printf("densSS = %e\n", ss_calc_densSS(simParam, 1.e-12, 1.e4, 9.));
    //     printf("densSS = %e\n", ss_calc_densSS(simParam, 1.e-12, 1.e4, 7.)*simParam->omega_b*simParam->h*simParam->h*rho_g_cm/mp_g*8.*8.*8./(1.-simParam->Y));
        printf(" mean free paths for T=10^4K and photHI_bg = 5.e-13 :\n");
        printf(" z = 6: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 6.));
        printf(" z = 6: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 6.));
        printf("z = 7: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 7.));
        printf("z = 7: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 7.));
        printf("z = 8: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 8.));
        printf("z = 8: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 8.));
        printf("z = 9: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 9.));
        printf("z = 9: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 9.));
        printf("z = 10: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 10.));
        printf("z = 10: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 10.));
        printf("z = 14.75: mfp = %e\n", calc_local_mfp(simParam, 1., 0.5e-12, 1.e4, 14.75));
        printf("z = 14.75: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-12, 1.e4, 14.75));
        printf("z = 14.75: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-13, 1.e4, 14.75));
        printf("z = 14.75: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-14, 1.e4, 14.75));
        printf("z = 14.75: mfp(M2000) = %e\n", dd_calc_mfp(simParam, 0.5e-15, 1.e4, 14.75));
        printf("done\n+++\n");
    }

    // Run the actual code.
    cifog(simParam, redshift_list, grid, sourcelist, integralTable, photIonBgList, num_cycles, myRank, RestartMode);
    
    //--------------------------------------------------------------------------------
    // deallocating grids
    //--------------------------------------------------------------------------------
    status = cleanup_cifog(simParam, integralTable, photIonBgList, grid, redshift_list, myRank);
    if (status !=  EXIT_SUCCESS)
    {
      exit(EXIT_FAILURE);
    } 

#ifdef __MPI
    fftw_mpi_cleanup();
        
    t2 = MPI_Wtime();
    printf("Execution took %f s\n", t2-t1);
    MPI_Finalize();
#else
    fftw_cleanup();
    
    t2 = time(NULL);
    printf("Execution took %f s\n", t2-t1);
#endif
    
    return 0;
}
