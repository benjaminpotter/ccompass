/// @file main.c
/// @brief A command line tool for processing sky images using the ccompass library.
///
/// @author Ben Potter
/// @date 15 July 2024

// HIGHLY WIP, caution is advised.

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Included compatibility layer for Windows devices.
// https://github.com/tronkko/dirent.git
// #include "dirent.h"
#include <dirent.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CCOMPASS_IMPLEMENTATION
#include "ccompass.h"


// @struct image
// @brief Stores information about a single image.
typedef struct image {
    char path[512];
    char name[256];
    double azimuth;
    
    struct image* next;
} image;


// @struct image_queue
// @brief Stores images in a queue structure.
typedef struct image_queue {
    image* root;
    size_t length;
} image_queue;


void image_create(image* im, const char* path, const char* name) {
    if(im == NULL)
        return;

    strcpy(im->path, path);
    strcpy(im->name, name);
    im->azimuth = 0.0;
    im->next = NULL;
}


int index_from_coord(int x, int y, int w) { return x + y * w; }
void image_process(image* im) {
    if(im == NULL)
        return;

    unsigned char* data;
    int img_w, img_h, img_n;

    data = stbi_load(im->path, &img_w, &img_h, &img_n, 1);
 
    if(data == NULL) {
        fprintf(stderr, "%s failed to process.\n", im->path);
        return;
    }
    
    int w = img_w / 2, h = img_h / 2;
    struct cc_stokes *stokes_vectors;
    stokes_vectors = (struct cc_stokes*) malloc(sizeof(struct cc_stokes) * w * h);

    for(int row = 0; row < h; ++row) {
        for(int col = 0; col < w; ++col) {
            struct cc_stokes *stokes = &stokes_vectors[index_from_coord(col, row, w)];
            
            double i = (double) data[index_from_coord(col + 0   , row + 0   , img_w)];
            double q = (double) data[index_from_coord(col + 2048, row + 0   , img_w)];
            double u = (double) data[index_from_coord(col + 0   , row + 2048, img_w)];
            
            stokes->i = i / 255.0;
            stokes->q = (q * 2.0 / 255.0) - 1.0;
            stokes->u = (u * 2.0 / 255.0) - 1.0;
            stokes->v = 0;
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

    im->azimuth = azimuth;
}


void image_destroy(image* im) {
    if(im == NULL)
        return;
    
}


void queue_create(image_queue* queue) {
    if(queue == NULL)
        return;

    queue->root = NULL;
    queue->length = 0;
}


void image_enqueue(image_queue* queue, image* node) {
    if(queue == NULL || node == NULL)
        return;

    image* tail = queue->root;
     
    // If there is no root node yet, enqueue this node as the root.
    if(tail == NULL) {
        queue->root = node;
        queue->length++;
        return;
    }
    
    // Otherwise, traverse the list to the current tail.
    while(tail->next != NULL) {
        tail = tail->next;
    }

    // At the current tail, enqueue the node as the next attribute.
    tail->next = node;
    queue->length++;
}


image* image_dequeue(image_queue* queue) {
    if(queue == NULL)
        return NULL;

    image* result = queue->root;
 
    // Shift the queue root forward one.
    if(queue->root != NULL) {
        queue->root = queue->root->next;
        queue->length--;
    }
   
    return result;
}


bool image_queue_isempty(image_queue* queue) {
    return queue->length == 0;
}


void collect_images(image_queue* queue, const char* root) {

    DIR *dir;
    dir = opendir(root);

    if(dir == NULL) {
        // If opening this directory fails for any reason, we can just bail.
        fprintf(stderr, "failed to open directory\n");
        exit(1);
    }

    struct dirent* ent;
    while((ent = readdir(dir)) != NULL) {
        
        // If the file is a directory recursively visit its children.
        if(ent->d_type == DT_DIR) {

            // Checking if the first character is a period.
            // Does this work in all cases?
            if(ent->d_name[0] == '.')
                continue;

            fprintf(stdout, "scanning directory %s\n", ent->d_name);

            // Append directory name to the path.
            char path[512];
            strcpy(path, root);
            strcat(path, ent->d_name);
            strcat(path, "/");

            collect_images(queue, path);
        }

        // Otherwise, look for regular files.
        else if(ent->d_type == DT_REG) {
            
            // Generate the full path for this file.
            char path[512];
            strcpy(path, root);
            strcat(path, ent->d_name);

            // Check if the file is an image.
            // This is done using a utility function provided by STB.
            int x,y,n;
            if(!stbi_info(path, &x, &y, &n)) {
                fprintf(stdout, "ignoring %s\n", ent->d_name);
                continue;
            }

            // List entry name.
            fprintf(stdout, "entry %s\n", ent->d_name);
            
            // Add to the image queue. 
            image* node;
            node = (image*) malloc(sizeof(image));
            image_create(node, path, ent->d_name); 
            
            image_enqueue(queue, node);
        }
    }
    
    closedir(dir);
}


int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "usage: %s [image directory]\n", argv[0]);
        return 1;
    }

    // Pop the directory name from the arg stack.
    char root_name[256];
    strcpy(root_name, argv[1]);

    // Append a slash if there is not one.
    int path_length = strlen(root_name);
    if(root_name[path_length-1] != '/')
        strcat(root_name, "/");
    
    // Scan directory. 
    fprintf(stdout, "beginning filesystem scan with root (%s)\n", root_name);

    image_queue queue;
    queue_create(&queue);
    collect_images(&queue, root_name);
 
    fprintf(stdout, "scan complete\n\n");
    fprintf(stdout, "continue with processing %ld files?\n", queue.length);
    getchar();

    // Clears any data currently stored in the csv.
    FILE *fp;
    fp = fopen("dump.csv", "w");
    fprintf(fp, "path,azimuth\n");
    fclose(fp);
    
    // Process image queue.
    while(!image_queue_isempty(&queue)) {
        image* node = image_dequeue(&queue);
        fprintf(stdout, "processing %s\n", node->path);
        
        // Populates the azimuth field of the image node.
        image_process(node);
        
        // Dump values to a csv.
        // TODO Test if its better to hold the file or open each time.
        fp = fopen("dump.csv", "a");
        fprintf(fp, "%s,%0.5f\n", node->path, node->azimuth);
        fclose(fp);

        image_destroy(node);
        free(node);
    }

    return 0;
}



