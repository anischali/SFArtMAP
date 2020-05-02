#ifndef NETWORK_H
#define NETWORK_H
#include "dataset.h"
#include "param.h"
#include <string.h>





typedef struct neuron_t neuron_t;
typedef struct som_t som_t;
typedef struct sfam_t sfam_t;
typedef struct cluster_t cluster_t;
typedef struct algorithms_t algorithms_t;




struct cluster_t
{
    int psize;
    double **prototypes;
    int *csizes;
    int *labels;
    int **datas;
};




struct neuron_t
{
    double *weights;
    int label;
};

struct som_t
{
    int height;
    int width;
    int stat;
    double error_percent;
    som_param_t *params;
    neuron_t *map;
    double *vdata;
    void (*init)(som_t *, dataset_t *);
    void (*learn)(som_t *, dataset_t *);
    void (*tag)(som_t *, dataset_t *);
    void (*map_stat)(som_t *, dataset_t *);
    void (*print_map)(som_t *);
    void (*free_map)(som_t *);  
};


struct sfam_t
{
    int prototype_size;
    int prototype_index;
    double *activities;
    neuron_t *prototypes;
    sfam_param_t *params;
    int stat;
    double *vdata;
    double rho;
    cluster_t *clusters;
    void (*init)(sfam_t *);
    void (*learn)(sfam_t *, dataset_t *);
    void (*prototypes_stat)(sfam_t *, dataset_t *);
    void (*make_clusters)(sfam_t *, dataset_t *);
    void (*print_prototypes)(sfam_t *, dataset_t *);
    void (*free_prototypes)(sfam_t *);
};






struct algorithms_t
{
    som_t som;
    sfam_t sfam;
    dataset_t *databases[2];
    param_t *params;
    void (*init)(algorithms_t *);
    void (*free_all)(algorithms_t *);
    void (*time_compar)(algorithms_t *);
};



void init_map(som_t *som, dataset_t *db);
void learn(som_t *som, dataset_t *db);
void tag_map(som_t *som, dataset_t *db);
void print_map(som_t *som);
void map_stat(som_t *som, dataset_t *db);
void free_map(som_t *som);

void init_prototypes(sfam_t *map);
void free_proto(sfam_t *map);
void sfam_learn(sfam_t *map, dataset_t *db);
void sfam_stat(sfam_t *map, dataset_t *db);
void make_clusters(sfam_t *map, dataset_t *db);
void print_clusters(sfam_t *map, dataset_t *db);

void init_sfam_struct(sfam_t *sfam);
void init_som_struct(som_t *som);
void init_all(algorithms_t *algos);
void free_algos(algorithms_t *algos);
void time_compar(algorithms_t *algos);
#endif