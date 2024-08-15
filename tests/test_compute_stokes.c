
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


int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "usage: %s [filepath]\n", argv[0]);
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
    
    int w = img_w / 2, h = img_h / 2;
    
    struct cc_stokes *stokes_vectors;
    stokes_vectors = (struct cc_stokes*) malloc(sizeof(struct cc_stokes) * w * h);
    cc_compute_stokes(data, stokes_vectors, w, h);

    // No longer require raw image data.
    stbi_image_free(data);

    // Test by writing out each component of the stokes vector.
    unsigned char *s0;
    s0 = (unsigned char*) malloc(sizeof(unsigned char) * w * h);

    unsigned char *s1;
    s1 = (unsigned char*) malloc(sizeof(unsigned char) * w * h);

    unsigned char *s2;
    s2 = (unsigned char*) malloc(sizeof(unsigned char) * w * h);

    for(int i = 0; i < w * h; ++i) {
        struct cc_stokes *stokes = &stokes_vectors[i];
        s0[i] = stokes->i * 255;
        s1[i] = (stokes->q + 1.0) * (255/2.0);
        s2[i] = (stokes->u + 1.0) * (255/2.0);
    }

    int err;
    err = stbi_write_png("s0.png", w, h, 1, s0, 0);
    err = stbi_write_png("s1.png", w, h, 1, s1, 0);
    err = stbi_write_png("s2.png", w, h, 1, s2, 0);

    if(err == 0) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());        
        return 1;
    }

    free(stokes_vectors);
    return 0;
}
