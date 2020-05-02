#ifndef PARAMS_H
#define PARAMS_H

#define SIZE_FILENAME 1024


typedef struct data_property_t data_property_t;
typedef struct som_param_t som_param_t; 
typedef struct sfam_param_t sfam_param_t; 
typedef struct param_t param_t;


struct sfam_param_t
{
    double alpha;
    double beta;
    double epsilon;
    double rho_baseline;
    double norm;
    int vsize;
    int prototype_size;
    int nepoch;
};

struct som_param_t
{
    double alpha;
    double ray;
    int niter;
    int vsize;
    int init_type;
    int learning_type;
    int map_height;
    int map_width;
};


struct data_property_t
{
    char filename[SIZE_FILENAME];
    int vsize;
    int db_size;
    int norm_type;
};

struct param_t 
{
    data_property_t som_data;
    data_property_t sfam_data;
    som_param_t som_params;
    sfam_param_t sfam_params;
};


param_t *get_config();
#endif // !1

