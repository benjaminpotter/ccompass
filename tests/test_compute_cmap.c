
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

    int w = 2048, h = 2048;
    
    double *values;
    values = (double *) malloc(sizeof(double) * w * h);

    for(int y = 0; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            values[index_from_coord(x, y, w)] = (double) x;
        }
    }

    struct cc_color *data;
    data = (struct cc_color*) malloc(sizeof(struct cc_color) * w * h); 
    cc_compute_cmap(values, w * h, 0, w, data);

    int err;
    err = stbi_write_png("test_compute_cmap.png", w, h, 4, data, 0);

    if(err == 0) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());        
        return 1;
    }

    free(values);
    free(data);
    return 0;
}
