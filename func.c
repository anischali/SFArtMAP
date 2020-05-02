#include "func.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

int min_int(int a, int b)
{
    return (a < b) ? a : b;
}

double max_double(double a, double b)
{
    return (a > b) ? a : b;
}

double min_double(double a, double b)
{
    return (a < b) ? a : b;
}

double abs_double(double v)
{
    return (v < 0.0) ? v * -1.0 : v;
}

double data_dist(const double *v1, const double *v2, int vsize)
{
    double sum = 0.0;
    double v = 0.0;
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        v = v2[ivec] - v1[ivec];
        sum += (v * v);
    }
    return sqrt(sum);
}

double map_dist(const point2Di_t p1, const point2Di_t p2)
{
    point2Di_t diff = {.x = p2.x - p1.x, .y = p2.y - p1.y};
    diff.x *= diff.x;
    diff.y *= diff.y;
    return sqrt((double)(diff.x + diff.y));
}

int rand_int(int min, int max)
{
    return (int)(rand() % (max - min) + min);
}

double rand_double(double min, double max)
{
    return (double)((rand() / (double)(RAND_MAX + 1.0)) * (max - min) + min);
}

double l1_norm(double *vect, int size)
{
    double sum = 0.0;
    for (int ivec = 0; ivec < size; ++ivec)
    {
        sum += abs_double(vect[ivec]);
    }
    return sum;
}

double l2_norm(double *vect, int size)
{
    double sum = 0.0;
    for (int ivec = 0; ivec < size; ++ivec)
    {
        sum += (vect[ivec] * vect[ivec]);
    }
    return sqrt(sum);
}

void normalize(double *vect, int size, double (*norm_func)(double *v, int s))
{
    double norm = norm_func(vect, size);
    for (int ivect = 0; ivect < size; ++ivect)
    {
        vect[ivect] /= norm;
    }
}

double *complement_coding(double *vect, int size)
{
    double *cc_vect = (double *)calloc(size * 2, sizeof *cc_vect);
    for (int ivec = 0; ivec < size; ++ivec)
    {
        cc_vect[ivec] = vect[ivec];
        cc_vect[ivec + size] = 1.0 - vect[ivec];
    }
    free(vect);
    return cc_vect;
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int *make_index(int size)
{
    int *index = (int *) malloc(size * sizeof *index);
    for (int idx = 0; idx < size; ++idx)
    {
        index[idx] = idx;
    }
    return index;
}


void shuffle(int *index, int isize)
{
    int r = 0;
    for (int idx = 0; idx < isize; ++idx)
    {
        do
        {
            r = rand_int(idx, isize);
        } while (r == idx && idx != isize - 1);
        swap(&index[idx], &index[r]);
    }
}


void print_vect(double *vect, int vsize, const char *label)
{
    printf("%s: ", label);
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        printf("%.2f ", vect[ivec]);
    }
    printf("\n");
}


void vec_set(double *vec, int vsize, double value)
{
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        vec[ivec] = value;
    }
}


int vect_max_idx(double *vect, int vsize)
{
    int idx = 0;
    double max = vect[0];
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        if (vect[ivec] > max)
        {
            max = vect[ivec];
            idx = ivec;
        }
    }
    return idx;
}



void fuzzy_and(double *v, double *v1, double *v2, int vsize)
{
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        v[ivec] = min_double(v1[ivec], v2[ivec]);
    }
}