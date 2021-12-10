#ifndef K_IMUFF_H
#define K_IMUFF_H

#include <cuda.h>
#include <vector_types.h>
#include <vector_functions.h>

typedef unsigned char uchar;

extern "C"
{
void gIMUFFDiff(const uchar* src1data, const uchar* src2data, uchar* delta, int totalElements);
void gIMUFFPatch(const uchar* src1data, const uchar* delta, void* imgfinal, int totalElements);
void gIMUFFMerge(const uchar* imgbase, const uchar* imgA, const uchar* imgB, void* outimage, int totalElements);
void gIMUFFDiffSequence(uchar* d_data1, uchar* d_delta, int img_width, int img_height, int numFrames);
void gIMUFFPatchSequence(uchar* keyed_frames, uchar* result, int img_width, int img_height, int amount);
void gStartCuda(int device);
}

#endif // K_IMUFF_H
