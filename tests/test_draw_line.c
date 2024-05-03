
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

    struct cc_color *data;
    data = (struct cc_color*) malloc(sizeof(struct cc_color) * w * h); 
    for(int i = 0; i < w * h; ++i)
        data[i] = CC_BLACK;
    
    cc_draw_line(-0.500 * M_PI, data, w, h);
    cc_draw_line(-0.375 * M_PI, data, w, h);
    cc_draw_line(-0.250 * M_PI, data, w, h);
    cc_draw_line(-0.125 * M_PI, data, w, h);
    cc_draw_line( 0.000 * M_PI, data, w, h);
    cc_draw_line( 0.125 * M_PI, data, w, h);
    cc_draw_line( 0.250 * M_PI, data, w, h);
    cc_draw_line( 0.375 * M_PI, data, w, h);

    int err;
    err = stbi_write_png("test_draw_line.png", w, h, 4, data, 0);

    if(err == 0) {
        fprintf(stderr, "failed: %s\n", stbi_failure_reason());        
        return 1;
    }

    free(data);
    return 0;
}
