///
///
///

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#include "tpool.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CCOMPASS_IMPLEMENTATION
#include "ccompass.h"

struct image {
    char path[256];
    double azimuth;
};

int index_from_coord(int x, int y, int w) { return x + y * w; }

void worker(void *arg) {
    struct image *img;
    img = arg;

    const char *img_file = img->path;
    printf("loading %s\n", img_file);

    unsigned char* data;
    int img_w, img_h, img_n;

    // data is now a list of pixel "intensity" values
    // 1 enforces greyscale on the incoming data
    data = stbi_load(img_file, &img_w, &img_h, &img_n, 1);
    
    if(data == NULL) {
        fprintf(stderr, "failed: %s\n", img_file);
        return;
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

    double azimuth;
    double *aolps;
    double *dolps;
    aolps = (double*) malloc(sizeof(double) * w * h);
    dolps = (double*) malloc(sizeof(double) * w * h);
    cc_compute_aolp(stokes_vectors, aolps, w, h);
    cc_compute_dolp(stokes_vectors, dolps, w, h);
    cc_hough_transform(aolps, dolps, w, h, &azimuth);

    free(aolps);
    free(dolps);
    free(stokes_vectors);

    img->azimuth = azimuth;
}

int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "usage: %s [image directory]\n", argv[0]);
        return 1;
    }

    tpool_t *tp;
    tp = tpool_create(10);

    // Find all files in a directory
    DIR *dir;
    dir = opendir(argv[1]);

    struct dirent *ent;
    ent = readdir(dir); 

    int cnt = 0;
    struct image *images[100];
    while(ent != NULL) {
        struct image *im;
        im = malloc(sizeof(struct image));
        strcpy(im->path, argv[1]);
        strcat(im->path, ent->d_name);
        
        images[cnt++] = im;
        tpool_add_work(tp, worker, (void*) im);

        ent = readdir(dir);
    }

    closedir(dir);

    tpool_wait(tp);
    tpool_destroy(tp);

    // dump values to a csv
    FILE *fp;
    fp = fopen("dump.csv", "w");

    for(int i = 0; i < cnt; ++i) {
        if(images[i] == NULL)
            continue;

        fprintf(fp, "%s,%0.5f\n", images[i]->path, images[i]->azimuth);
        free(images[i]);
    }

    fclose(fp);

    return 0;
}



