/*
 *  confObj.h
 *  uvff
 *
 *  Created by 
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CONFOBJ_H
#define CONFOBJ_H

/*--- Includes ----------------------------------------------------------*/
#include "parse_ini.h"
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct confObj_struct *confObj_t;


/*--- Implemention of main structure ------------------------------------*/
struct confObj_struct {
    //General
    int            num_snapshots;
    int32_t        stop_snapshot;
    char           *redshift_file[512];
    double         redshift_prev_snap;
    double         redshift;
    double         evol_time;

    double         lin_scales;
    double         inc_log_scales;
    double         max_scale;
    int            ionize_sphere;
    
    int            default_mean_density;
    int            use_web_model;
    int            calc_ion_history;
    int            photHI_model;
    int            calc_mfp;
    int            const_recomb;
    int            calc_recomb;
    int            solve_He;
    
    int            padded_box;

    //Input
    int            grid_size;
    double         box_size;
    
    int            input_doubleprecision;
    int            input_nionprecision;
    int            inputfiles_comoving;

    int            inputfiles_simulation;
    int            SimulationLowSnap;
    int            SimulationHighSnap;

    char           *igm_density_file[512];
    char           *igm_density_suffix[512];
    int            dens_in_overdensity;
    double         mean_density;

    char           *igm_clump_file[512];
    
    char           *sources_file[512];
    char           *nion_file[512];
    double         nion_factor;   
 
    //Output
    char           *out_XHII_file[512];
    int            write_photHI_file;
    char           *out_photHI_file[512];
    char           *out_restart_file[512];
    
    //Cosmology
    double         h;
    double         omega_b;
    double         omega_m;
    double         omega_l;
    double         sigma8;
    double         Y;
    
    //Photoionization
    char           *photHI_bg_file[512];
    double         photHI_bg;
    double         mfp;
    double         source_slope_index;

    //Recombinations
    double         dnrec_dt;
    char           *recomb_table[512];
    double         zmin, zmax, dz;
    double         fmin, fmax, df;
    double         dcellmin, dcellmax, ddcell;
    
    //Helium
    char           *sources_HeI_file[512];
    char           *nion_HeI_file[512];
    char           *sources_HeII_file[512];
    char           *nion_HeII_file[512];
    
    double         dnrec_HeI_dt;
    double         dnrec_HeII_dt;
    
    char           *out_XHeII_file[512];
    char           *out_XHeIII_file[512];
    
    double         f;
    double         factor;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern confObj_t
readConfObj(char *fileName);

extern confObj_t
confObj_new(parse_ini_t ini);

extern void
confObj_del(confObj_t *config);


#endif
