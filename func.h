#ifndef FUNC_H
#define FUNC_H

typedef struct point2Di_t point2Di_t;

struct point2Di_t
{
    int x;
    int y;
};

double map_dist(const point2Di_t p1, const point2Di_t p2);
double rand_double(double min, double max);
int rand_int(int min, int max);
double data_dist(const double *v1, const double *v2, int vsize);
double abs_double(double v);
double min_double(double a, double b);
double max_double(double a, double b);
int min_int(int a, int b);
int max_int(int a, int b);
double l1_norm(double *vect, int size);
double l2_norm(double *vect, int size);
double *complement_coding(double *vect, int size);
void normalize(double *vect, int size, double (*norm_func)(double *v, int s));
int *make_index(int size);
void shuffle(int *index, int isize);
void print_vect(double *vect, int vsize, const char *label);
void vec_set(double *vec, int vsize, double value);
int vect_max_idx(double *vect, int vsize);
void fuzzy_and(double *v,double *v1, double *v2, int vsize);
#endif 