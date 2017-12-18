#include "./src/TinyMT/tinymt32.clh"

 __kernel void RotateTiles(__global tinymt32wp_t * d_status, __global int* tiling, const int N)
{
    // Attempts to rotate all tilings of a given color. Color determined by which tiling array is given as input.
    int i = get_global_id(0);
    int j = get_global_id(1);
    
    tinymt32wp_t tiny;
  
	tinymt32_status_read(&tiny, d_status);
  	float rd = tinymt32_single01(&tiny); 
 	tinymt32_status_write(d_status, &tiny);  
    
    // need to implement random number. Right now does deterministic flipping.
    if ( rd < .5 && i < N && j < (N/2) ) {
        if (tiling[i*(N/2)+j] == 3) {
            tiling[i*(N/2)+j] = 12;
        } else if (tiling[i*(N/2)+j]==12) {
            tiling[i*(N/2)+j] = 3;
        }
    }    
}

__kernel void UpdateTiles(__global int* tiling, __global int* reftiling, const int N, const int t)
{
    // Updates tilings, given the state of the adjacent tilings (reftiling)
    int i = get_global_id(0);
    int j = get_global_id(1);
    
  
    if (i < N-1 && j < (N/2)-1 && i>0 && j>0 ) {
        tiling[i*(N/2)+j] = (reftiling[(i-1)*(N/2)+j]&2)/2 + 2*(reftiling[(i+1)*(N/2)+j]&1) + (reftiling[i*(N/2)+j-(i+t+1)%2]&8)/2 + 2*(reftiling[i*(N/2)+j+(i+t)%2]&4);
    }
}

__kernel void InitTinyMT(__global tinymt32wp_t * d_status, uint seed)
{
    tinymt32wp_t tiny;
    const size_t id = get_global_id(0);
    tinymt32_status_read(&tiny, d_status);
    tinymt32_init(&tiny, seed+id);
    tinymt32_status_write(d_status, &tiny);
}
