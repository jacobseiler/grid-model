#include <stdlib.h>
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

#include "confObj.h"
#include "grid.h"
#include "sources.h"
#include "sources_to_grid.h"
#include "fraction_q.h"
#include "filtering.h"
#include "self_shielding.h"

int main (int argc, /*const*/ char * argv[]) { 
	int size = 1;
	int myRank = 0;

	char iniFile[1000];
	confObj_t simParam;
	grid_t *grid;
	
	sourcelist_t *sourcelist;
	
	double t1, t2;
	
#ifdef __MPI
	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &size); 
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank); 
	
	t1 = MPI_Wtime();
#else
	t1 = time(NULL);
#endif
	
	//parse command line arguments and be nice to user
	if (argc != 2) {
		printf("cifog: (C)  - Use at own risk...\n");
		printf("USAGE:\n");
		printf("cifog iniFile\n");
		
		exit(EXIT_FAILURE);
	} else {
		strcpy(iniFile, argv[1]);
	}
	
	
	//read paramter file
	simParam = readConfObj(iniFile);
		
	//read files (allocate grid)
	grid = initGrid();
	if(myRank==0) printf("reading files to grid... ");
	read_files_to_grid(grid, simParam);
	if(myRank==0) printf("done\n");
	
	//read source files (allocate sources)
	if(myRank==0) printf("reading sources file... ");
	sourcelist = read_sources(simParam->sources_file);
	if(myRank==0) printf("done\n");

	//map sources to grid
	if(myRank==0) printf("mapping sources to grid... ");
	map_nion_to_grid(grid, sourcelist);
	if(myRank==0) printf("done\n");
	
	//compute fraction Q
	if(myRank==0) printf("computing relation between number of ionizing photons and absorptions... ");
	compute_Q(grid, simParam);
	if(myRank==0) printf("done\n");
	
	//apply filtering
	if(myRank==0) printf("apply tophat filter routine for ionization field... ");
	compute_ionization_field(grid);
	if(myRank==0) printf("done\n");
	
	if(simParam->use_web_model == 1)
	{
		//set photoionization rate to background value
		if(myRank==0) printf("setting photoionization rate to background value... ");
		set_value_to_photoionization_field(grid, simParam);
		if(myRank==0) printf("done\n");
		
		//apply web model
		if(myRank==0) printf("apply web model... ");
		compute_web_ionfraction(grid, simParam);
		if(myRank==0) printf("done\n");
		
		if(simParam->compute_photHIfield ==1)
		{
			//compute photoionization field
			if(myRank==0) printf("compute photoionization field (this may take a while)... ");
			compute_photoionization_field(grid, sourcelist, simParam);
			if(myRank==0) printf("done\n");
			
			//write photoionization rate field to file
			if(myRank==0) printf("writing photoionization field to file... ");
			save_to_file_photHI(grid, simParam->out_photHI_file);
			if(myRank==0) printf("done\n");
		}
	}
	
	//write ionization field to file
	if(myRank==0) printf("writing ionization field to file... ");
	save_to_file_XHII(grid, simParam->out_XHII_file);
	if(myRank==0) printf("done\n");
	
	//deallocate grid
	if(myRank==0) printf("deallocating grid ...");
	deallocate_grid(grid);
	if(myRank==0) printf("done\n");
	
	//deallocate sources
	if(myRank==0) printf("deallocating sources ...");
	deallocate_sourcelist(sourcelist);
	if(myRank==0) printf("done\n");
	
	if(myRank==0) printf("Finished\n");
#ifdef __MPI
	t2 = MPI_Wtime();
	printf("Execution took %f s\n", t2-t1);
	MPI_Finalize();
#else
	t2 = time(NULL);
	printf("Execution took %f s\n", t2-t1);
#endif
	
	return 0;
}