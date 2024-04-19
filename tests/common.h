
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ccompass.h"

int index_from_coord(int x, int y, int w) {
    return x + y * w;
}



///
///
///
int parse_stokes(unsigned char *pixels, struct cc_stokes *stokes_vectors, int w, int h) {
    
    // The test image is a 4096x4096 Stokes vector encoding. Each quadrant of 
    // the image forms a different component of the 4D Stokes vector. For 
    // example, the first Stokes vector would be composed of pixel positions
    // {(0, 0), (2048, 0), (0, 2048), (2048, 2048)}. There are 2048x2048
    // vectors in total (ex. 4096 / 2).

    int img_w = 2 * w;
    // int img_h = 2 * h;

    for(int row = 0; row < h; ++row) {
        for(int col = 0; col < w; ++col) {
            struct cc_stokes *stokes = &stokes_vectors[index_from_coord(col, row, w)];
            
            stokes->i = (double)  pixels[index_from_coord(col + 0   , row + 0   , img_w)] / 256.0;
            stokes->q = ((double) pixels[index_from_coord(col + 2048, row + 0   , img_w)] / 128.0) - 1.0;
            stokes->u = ((double) pixels[index_from_coord(col + 0   , row + 2048, img_w)] / 128.0) - 1.0;
            stokes->v = ((double) pixels[index_from_coord(col + 2048, row + 2048, img_w)] / 128.0) - 1.0;
        }
    }  

    return 0; 
}


