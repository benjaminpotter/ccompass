
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CCOMPASS_IMPLEMENTATION
#include "ccompass.h"
#include "common.h"


int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "usage: %s [filepath]", argv[0]);
        return 1;
    }    

    const char *filepath = argv[1];

    printf("loading %s...\n", filepath); 

    int img_w, img_h, img_n;
    unsigned char *data = stbi_load(filepath, &img_w, &img_h, &img_n, 1); // n=1 to enforce greyscale

    if(data == NULL) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());        
        return 1;
    }

    printf("successfully loaded image with w=%d h=%d n=%d\n", img_w, img_h, img_n);
    
    // TODO this needs to be informed by the image, or this needs to log an
    // error if the loaded image is not this size.
    int w = 2048, h = 2048;
    
    struct cc_stokes *stokes_vectors;
    stokes_vectors = (struct cc_stokes*) malloc(sizeof(struct cc_stokes) * w * h);
    parse_stokes(data, stokes_vectors, w, h);

    stbi_image_free(data);

    double *dolps;
    dolps = (double*) malloc(sizeof(double) * w * h);
    cc_transform_stokes(stokes_vectors, w, h);
    cc_compute_dolp(stokes_vectors, dolps, w, h);

    // unsigned char *data;
    data = (unsigned char*) malloc(sizeof(unsigned char) * w * h * 4); 
    cc_compute_cmap(dolps, w * h, 0, 1, (struct cc_color*) data);

    int err;
    err = stbi_write_png("test_compute_dolp.png", w, h, 4, data, 0);

    if(err == 0) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());        
        return 1;
    }

    free(stokes_vectors);
    free(dolps);
    free(data);
    return 0;
}

