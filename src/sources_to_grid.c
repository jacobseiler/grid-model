#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include <assert.h>

#ifdef __MPI
#include <fftw3-mpi.h>
#include <mpi.h>
#else
#include <fftw3.h>
#endif

#include "utils.h"

#include "confObj.h"
#include "grid.h"
#include "sources.h"
#include "sources_to_grid.h"

/* read in / update sources or nion -------------------------------------------------------------*/
void read_update_nion(confObj_t simParam, sourcelist_t *thisSourcelist, grid_t *thisGrid, int snap, int32_t myRank)
{
	char sources_file[MAXLENGTH], nion_file[MAXLENGTH];
	char snap_string[MAXLENGTH];
  int32_t num_chars;
	
	for(int i=0; i<MAXLENGTH; i++) sources_file[i]='\0';
	for(int i=0; i<MAXLENGTH; i++) nion_file[i]='\0';
	if(snap >= 0)
  {
    if (simParam->inputfiles_simulation == 1)
    { 
      num_chars = snprintf(snap_string, MAXLENGTH - 1, "%03d",snap+simParam->SimulationLowSnap);
    }
    else
    { 
      num_chars = snprintf(snap_string, MAXLENGTH - 1, "%03d",snap);
    }

    XASSERT(num_chars < MAXLENGTH - 1, "There was a string overflow when trying to assign the name of the snapshot string in 'read_update_nion'.\n");
		
		strcat(sources_file, simParam->sources_file);
		strcat(sources_file, "_");
		strcat(sources_file, snap_string);
		
		strcat(nion_file, simParam->nion_file);
		strcat(nion_file, "_");
		strcat(nion_file, snap_string);
	}else{
		strcat(sources_file, simParam->sources_file);
		strcat(nion_file, simParam->nion_file);
	}
	  
	if(file_exist(sources_file) == 1){
		//read source files (allocate sources)
		if(thisSourcelist != NULL){
            printf("\n\n ********\n deallocating source list!\n********\n");
			deallocate_sourcelist(thisSourcelist);
		}
		thisSourcelist = read_sources(sources_file);
		
        initialize_grid(thisGrid->nion, thisGrid->nbins, thisGrid->local_n0, 0.);
		
        //map sources to grid
		map_nion_to_grid(thisGrid->nion, thisGrid, thisSourcelist);
        
        //deallocate sources
        deallocate_sourcelist(thisSourcelist);
	}else if(file_exist(nion_file) == 1){
		read_array(thisGrid->nion, thisGrid, nion_file, simParam->input_nionprecision);
	}else{
		fprintf(stderr, "No source or nion file available, or names are incorrect!\nCurrent source file is %s\nCurrent nion file is %s", sources_file, nion_file);
		exit(EXIT_FAILURE);
	}

  if (myRank == 0)
  {
    printf("Boosting photons by a factor of %.4e\n", simParam->nion_factor);
  }	
 	for(int i=0; i<thisGrid->local_n0*thisGrid->nbins*thisGrid->nbins; i++)
  {
 		if(creal(thisGrid->nion[i])>0.)
    {
      thisGrid->nion[i] = creal(thisGrid->nion[i])*simParam->nion_factor;
 			//printf("nion[%d] = %e\n",i,creal(thisGrid->nion[i]));
 		}
 	}
}

void read_update_nion_HeI(confObj_t simParam, sourcelist_t *thisSourcelist, grid_t *thisGrid, int snap)
{
	char sources_file[MAXLENGTH], nion_file[MAXLENGTH];
	char snap_string[8];
	
  printf("SNAP %d\n", snap); 
  exit(0);
	for(int i=0; i<MAXLENGTH; i++) sources_file[i]='\0';
	for(int i=0; i<MAXLENGTH; i++) nion_file[i]='\0';
	if(snap >= 0){
		sprintf(snap_string,"%03d",snap); 
		
		strcat(sources_file, simParam->sources_HeI_file);
		strcat(sources_file, "_");
		strcat(sources_file, snap_string);
		
		strcat(nion_file, simParam->nion_HeI_file);
		strcat(nion_file, "_");
		strcat(nion_file, snap_string);
	}else{
		strcat(sources_file, simParam->sources_HeI_file);
		strcat(nion_file, simParam->nion_HeI_file);
	}
 
	if(file_exist(sources_file) == 1){
		//read source files (allocate sources)
		if(thisSourcelist != NULL){
			deallocate_sourcelist(thisSourcelist);
		}
		thisSourcelist = read_sources(sources_file);
		
        initialize_grid(thisGrid->nion_HeI, thisGrid->nbins, thisGrid->local_n0, 0.);

		//map sources to grid
		map_nion_to_grid(thisGrid->nion_HeI, thisGrid, thisSourcelist);
        
        //deallocate sources
        deallocate_sourcelist(thisSourcelist);
	}else if(file_exist(nion_file) == 1){
		read_array(thisGrid->nion_HeI, thisGrid, nion_file, simParam->input_doubleprecision);
	}else{
		fprintf(stderr, "No source or nion file available or names are incorrect!\nCurrent source file is %s\nCurrent nion file is %s\n", sources_file, nion_file);
		exit(EXIT_FAILURE);
	}
	for(int i=0; i<thisGrid->nbins*thisGrid->nbins*thisGrid->nbins; i++){
// 		if(creal(thisGrid->nion_HeI[i])>0.){
// 			thisGrid->nion[i] = creal(thisGrid->nion[i])*5.e0 + 0.*I;
// 			printf("nion[%d] = %e\n",i,creal(thisGrid->nion[i]));
// 		}
	}
}

void read_update_nion_HeII(confObj_t simParam, sourcelist_t *thisSourcelist, grid_t *thisGrid, int snap)
{
	char sources_file[MAXLENGTH], nion_file[MAXLENGTH];
	char snap_string[8];
	
	for(int i=0; i<MAXLENGTH; i++) sources_file[i]='\0';
	for(int i=0; i<MAXLENGTH; i++) nion_file[i]='\0';
	if(snap >= 0){
		sprintf(snap_string,"%03d",snap); 
		
		strcat(sources_file, simParam->sources_HeII_file);
		strcat(sources_file, "_");
		strcat(sources_file, snap_string);
		
		strcat(nion_file, simParam->nion_HeII_file);
		strcat(nion_file, "_");
		strcat(nion_file, snap_string);
	}else{
		strcat(sources_file, simParam->sources_HeII_file);
		strcat(nion_file, simParam->nion_HeII_file);
	}
  
	if(file_exist(sources_file) == 1){
		//read source files (allocate sources)
		if(thisSourcelist != NULL){
			deallocate_sourcelist(thisSourcelist);
		}
		thisSourcelist = read_sources(sources_file);
		
        initialize_grid(thisGrid->nion_HeII, thisGrid->nbins, thisGrid->local_n0, 0.);

		//map sources to grid
		map_nion_to_grid(thisGrid->nion_HeII, thisGrid, thisSourcelist);
        
        //deallocate sources
        deallocate_sourcelist(thisSourcelist);
	}else if(file_exist(nion_file) == 1){
		read_array(thisGrid->nion_HeII, thisGrid, nion_file, simParam->input_doubleprecision);
	}else{
		fprintf(stderr, "No source or nion file available, or names are incorrect!\nCurrent source file is %s\nCurrent nion file is %s\n", sources_file, nion_file);
		exit(EXIT_FAILURE);
	}
}
/* map number of ionizing photons to grid --------------------------------------------------------*/
void map_nion_to_grid(fftw_complex *thisNionArray, grid_t *thisGrid, sourcelist_t *thisSourcelist)
{
	int num_sources;
	int comx, comy, comz;
	int nbins;
	int local_0_start;
	int local_n0;
		
	num_sources = thisSourcelist->numSources;
    	
	nbins = thisGrid->nbins;
	local_0_start = thisGrid->local_0_start;
	local_n0 = thisGrid->local_n0;
	
	for(int i=0; i<num_sources; i++)
	{
		assert(i<num_sources && "map_nion_to_grid: index should be lower than number of sources!");
		source_t source = thisSourcelist->source[i];
		
		comx = (int)(source.pos[0]*nbins);
		comy = (int)(source.pos[1]*nbins);
		comz = (int)(source.pos[2]*nbins);
				
		if(comx==nbins) comx = comx-1;
		if(comy==nbins) comy = comy-1;
		if(comz==nbins) comz = comz-1;
		
		if(comz>=local_0_start && comz<local_0_start+local_n0)
		{
			thisNionArray[(comz-local_0_start)*nbins*nbins + comy*nbins + comx] += source.Nion*source.fesc+0.*I;
// 			thisGrid->igm_clump[(comz-local_0_start)*nbins*nbins + comy*nbins + comx] = 1.+0.*I;
		}
	}
}
