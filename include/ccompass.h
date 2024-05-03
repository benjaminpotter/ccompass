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

struct cc_color CC_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
struct cc_color CC_RED = { 0xFF, 0x00, 0x00, 0xFF };
struct cc_color CC_GREEN = { 0x00, 0xFF, 0x00, 0xFF };
struct cc_color CC_BLUE = { 0x00, 0x00, 0xFF, 0xFF };

/// Compute the angle of linear polarization from Stokes vectors.
/// 
/// @param stokes_vectors array of input Stokes vectors
/// @param aolps          array of output angles
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
void cc_compute_aolp(struct cc_stokes *stokes_vectors, double *aolps, int w, int h);


/// @brief Compute the degree of linear polarization from Stokes vectors.
///
/// @param stokes_vectors array of input Stokes vectors
/// @param dolps          array of output fractions [0, 1]
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
void cc_compute_dolp(struct cc_stokes *stokes_vectors, double *dolps, int w, int h);


/// This transformation is used to change the frame of reference
/// of a polarization image from instrument space to the solar
/// principal plane.
///
/// @brief Apply an in-place beta shift to Stokes vectors.
///
/// @param stokes_vectors array of input Stokes vectors
/// @param w              width of input
/// @param h              height of input
/// @since                1.0
void cc_transform_stokes(struct cc_stokes stokes_vectors[], int w, int h);


/// @brief Extract the solar azimuth using a 1D Hough transform on a 2D matrix of AoLP data.
///
/// @param angles  AoLP matrix
/// @param w       width of matrix
/// @param h       height of matrix
/// @param azimuth extracted solar azimuth
/// @since                1.1
void cc_hough_transform(double *angles, int w, int h, double *azimuth);


/// The input matrix is expected to have size elements. The dimensions of the
/// matrix are unimportant for the colour map computation. Each double is
/// converted to a 32 bit RGBA colour.
///
/// @brief Utility function for converting a matrix of doubles into a colour image.
///
/// @param values input array of doubles
/// @param size   length of input
/// @param min    minimum value of input
/// @param max    maximum value of input
/// @param pixels output list of pixels 
/// @since                1.0
void cc_compute_cmap(double values[], int size, double min, double max, struct cc_color pixels[]);


#ifdef CCOMPASS_IMPLEMENTATION

void cc_compute_aolp(struct cc_stokes *stokes_vectors, double *aolps, int w, int h) {

    for(int i = 0; i < w * h; ++i) {
        aolps[i] = 0.5 * atan2(stokes_vectors[i].u, stokes_vectors[i].q);
    } 
    
}

void cc_compute_dolp(struct cc_stokes *stokes_vectors, double *dolps, int w, int h) {

    for(int i = 0; i < w * h; ++i) {

        if(stokes_vectors[i].i == 0.0) {
            dolps[i] = 0.0;
            continue;
        }

        double squared_magnitude = pow(stokes_vectors[i].q, 2) + pow(stokes_vectors[i].u, 2);
        dolps[i] = sqrt(squared_magnitude) / stokes_vectors[i].i;
    } 

}

void cc_compute_cmap(double values[], int size, double min, double max, struct cc_color pixels[]) {

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
    
}

void cc_transform_stokes(struct cc_stokes stokes_vectors[], int w, int h) {
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

}


double cc_linear_map(double x, double x_min, double x_max, double y_min, double y_max) {
    double x_normalized = (x - x_min) / (x_max - x_min);
    return x_normalized * (y_max - y_min) + y_min;
}

void cc_hough_transform(double *angles, int w, int h, double *azimuth) {

    // set the threshold for including pixels in the feature set.
    double threshold = 0.1 * (M_PI / 180.0);
    double angle_resolution = 0.1 * (M_PI / 180.0);

    // TODO cache the pixel positions in a lookup table rather
    // than computing them each time.

    int accumulator_size = 2 * M_PI / angle_resolution;
    int accumulator[accumulator_size];

    printf("accumulator_size: %d\n", accumulator_size);

    for(int i = 0; i < accumulator_size; ++i)
        accumulator[i] = 0;

    for(int i = 0; i < w * h; ++i) {

        // skip this pixel if it isn't in the threshold.
        if( fabs(angles[i] - M_PI_2) > threshold)
            continue;

        // (x,y) position of the pixel in image space.
        double x,y;
        x = i % w - 1024.0;
        y = -1 * (floor( i / w ) - 1024.0);

        // if(x < -200.0 || x > 200.0 || y < -200.0 || y > 200.0)
        //     continue;
        
        double theta;
        theta = atan2(y, x);

        // "vote" for this theta in accumulator
        // theta is on the range [-M_PI, M_PI].
        int theta_index = cc_linear_map(theta, -M_PI, M_PI, 0, accumulator_size);
        accumulator[theta_index] += 1;
    }

    int azimuth_index = 0;
    for(int i = 0; i < accumulator_size; ++i) {
        if(accumulator[i] > accumulator[azimuth_index])
            azimuth_index = i;
    } 

    // convert index back to angle
    *azimuth = cc_linear_map(azimuth_index, 0, accumulator_size, -M_PI, M_PI);

}


#endif // CCOMPASS_IMPLEMENTATION 


#endif // CCOMPASS_H

