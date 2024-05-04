///
///
///

#include <stdio.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CCOMPASS_IMPLEMENTATION
#include "ccompass.h"


int index_from_coord(int x, int y, int w) {
    return x + y * w;
}


int main(int argc, char *argv[]) {

    if( argc != 2) {
        fprintf(stderr, "usage: %s [image]\n", argv[0]);
        return 1;
    }

    // 1. Load image.
    // 2. Parse the stokes vectors from the image.
    // 3. Transform stokes.
    // 4. Compute AoLP.
    // 5. Compute DoLP.

    const char *img_file = argv[1];
    char img_name[128];
    char aolp_file[128] = "aolp_";
    char dolp_file[128] = "dolp_";

    printf("loading %s\n", img_file);

    unsigned char* data;
    int img_w, img_h, img_n;

    // data is now a list of pixel "intensity" values
    // 1 enforces greyscale on the incoming data
    data = stbi_load(img_file, &img_w, &img_h, &img_n, 1);
    
    if(data == NULL) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());
        return 1;
    }

    int w = img_w / 2, h = img_h / 2;
    struct cc_stokes *stokes_vectors;
    stokes_vectors = (struct cc_stokes*) malloc(sizeof(struct cc_stokes) * w * h);

    for(int row = 0; row < h; ++row) {
        for(int col = 0; col < w; ++col) {
            struct cc_stokes *stokes = &stokes_vectors[index_from_coord(col, row, w)];
            
            stokes->i = (double)  data[index_from_coord(col + 0, row + 0, img_w)] / 256.0;
            stokes->q = ((double) data[index_from_coord(col + w, row + 0, img_w)] / 128.0) - 1.0;
            stokes->u = ((double) data[index_from_coord(col + 0, row + h, img_w)] / 128.0) - 1.0;
            stokes->v = ((double) data[index_from_coord(col + w, row + h, img_w)] / 128.0) - 1.0;
        }
    }  

    // no longer using image data
    stbi_image_free(data);

    cc_transform_stokes(stokes_vectors, w, h);

    double *aolps;
    aolps = (double*) malloc(sizeof(double) * w * h);
    cc_compute_aolp(stokes_vectors, aolps, w, h);
        
    struct cc_color *pixels = (struct cc_color*) malloc(sizeof(struct cc_color) * w * h); 
    cc_compute_cmap(aolps, w * h, -M_PI / 2.0, M_PI / 2.0, (struct cc_color*) pixels);

    printf("writing AoLP of %s to %s\n", img_file, aolp_file);

    if(!stbi_write_png(aolp_file, w, h, 4, pixels, 0)) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());
        return 1;
    }

    free(aolps);

    double *dolps;
    dolps = (double*) malloc(sizeof(double) * w * h);
    cc_compute_dolp(stokes_vectors, dolps, w, h);

    cc_compute_cmap(dolps, w * h, 0.0, 1.0, (struct cc_color*) pixels);

    printf("writing DoLP of %s to %s\n", img_file, dolp_file);

    if(!stbi_write_png(dolp_file, w, h, 4, pixels, 0)) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());
        return 1;
    }

    free(dolps);

    return 0;
}
