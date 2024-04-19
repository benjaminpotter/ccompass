///
///
///

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <errno.h>

#ifndef CCOMPASS_H
#define CCOMPASS_H

struct cc_stokes {
    double i;
    double q;
    double u;
    double v;
};

/// 32bit color
struct cc_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


/// Compute the angle of linear polarization from Stokes vectors.
/// 
/// @param stokes_vectors array of input Stokes vectors
/// @param aolps          array of output angles
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
int cc_compute_aolp(struct cc_stokes *stokes_vectors, double *aolps, int w, int h);


/// Compute the degree of linear polarization from Stokes vectors.
///
/// @param stokes_vectors array of input Stokes vectors
/// @param dolps          array of output fractions [0, 1]
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
int cc_compute_dolp(struct cc_stokes *stokes_vectors, double *dolps, int w, int h);


/// Apply an in-place beta shift to Stokes vectors.
///
/// This transformation is used to change the frame of reference
/// of a polarization image from instrument space to the solar
/// principal plane.
///
/// @param stokes_vectors array of input Stokes vectors
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
int cc_transform_stokes(struct cc_stokes stokes_vectors[], int w, int h);


/// Utility function for converting a matrix of doubles into a colour image.
///
/// The input matrix is expected to have size elements. The dimensions of the
/// matrix are unimportant for the colour map computation. Each double is
/// converted to a 32 bit RGBA colour.
///
/// @param values input array of doubles
/// @param size   length of input
/// @param min    minimum value of input
/// @param max    maximum value of input
/// @param pixels output list of pixels 
/// @since                1.0
int cc_compute_cmap(double values[], int size, double min, double max, struct cc_color pixels[]);


#ifdef CCOMPASS_IMPLEMENTATION

int cc_compute_aolp(struct cc_stokes *stokes_vectors, double *aolps, int w, int h) {

    for(int i = 0; i < w * h; ++i) {
        aolps[i] = 0.5 * atan2(stokes_vectors[i].u, stokes_vectors[i].q);
    } 
    
    return 0;
}

int cc_compute_dolp(struct cc_stokes *stokes_vectors, double *dolps, int w, int h) {

    for(int i = 0; i < w * h; ++i) {

        if(stokes_vectors[i].i == 0.0) {
            dolps[i] = 0.0;
            continue;
        }

        double squared_magnitude = pow(stokes_vectors[i].q, 2) + pow(stokes_vectors[i].u, 2);
        dolps[i] = sqrt(squared_magnitude) / stokes_vectors[i].i;
    } 

    return 0;
}

int cc_compute_cmap(double values[], int size, double min, double max, struct cc_color pixels[]) {

    for(int i = 0; i < size; ++i) {
        double f = values[i];

        if(f < min)
            f = min;

        if(f > max)
            f = max;

        double df = max - min;
        double fr = 1.0, fg = 1.0, fb = 1.0;

        if(f < (min + 0.25 * df)) {
            fr = 0;
            fg = 4 * (f - min) / df;
        }

        else if(f < (min + 0.5 * df)) {
            fr = 0;
            fb = 1 + 4 * (min + 0.25 * df - f) / df;
        }
        
        else if(f < (min + 0.75 * df)) {
            fr = 4 * (f - min - 0.5 * df) / df;
            fb = 0;
        }

        else {
            fg = 1 + 4 * (min + 0.75 * df - f) / df;
            fb = 0;
        }

        pixels[i].r = fr * 255.0;
        pixels[i].g = fg * 255.0;
        pixels[i].b = fb * 255.0;
        pixels[i].a = 0xFF;
    }
    
    return 0;
}

int cc_transform_stokes(struct cc_stokes stokes_vectors[], int w, int h) {
    for(int i = 0; i < w * h; ++i) {
        // (x,y) position of the pixel in image space
        double x,y;
        x = i % w - 1024.0;
        y = -1 * (floor( i / w ) - 1024.0);

        double beta;
        beta = 2.0 * atan2(y, x);
        
        double dx, dy;
        dx = cos(beta);
        dy = sin(beta);

        double q, u;
        q = stokes_vectors[i].q;
        u = stokes_vectors[i].u;

        stokes_vectors[i].q = q * dx + u * dy;
        stokes_vectors[i].u = q * (-dy) + u * dx;
    }

    return 0;
}


#endif // CCOMPASS_IMPLEMENTATION 


#endif // CCOMPASS_H

