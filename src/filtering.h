#ifndef FILTERING_H
#define FILTERING_H
#endif

void construct_tophat_filter(fftw_complex *filter, int nbins, ptrdiff_t local_0_start, ptrdiff_t local_n0, float smooth_scale);
void convolve_fft(grid_t *thisGrid, fftw_complex *filter, fftw_complex *nion_smooth);
void determine_ion_fractions(fftw_complex *nion_smooth, int nbins, ptrdiff_t local_n0, int smallest_scale);
void choose_ion_fraction(fftw_complex *nion_smooth, fftw_complex *XHII_tmp, grid_t *thisGrid);
void map_bubbles_to_nrec(fftw_complex *XHII_tmp, grid_t *thisGrid);
void combine_bubble_and_web_model(fftw_complex *XHII_tmp, grid_t *thisGrid);
void copy_grid_array(fftw_complex *XHII_tmp, grid_t *thisGrid);
void compute_ionization_field(confObj_t simParam, grid_t *thisGrid);
