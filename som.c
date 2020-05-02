#include "network.h"
#include "func.h"
#include <math.h>
#include <string.h>


#define PHASE_I 0.8
#define PHASE_II 0.2



void init_som_struct(som_t *som)
{
    som->init = init_map;
    som->learn = learn;
    som->tag = tag_map;
    som->print_map = print_map;
    som->map_stat = map_stat;
    som->free_map = free_map;
}


double *mean_dataset(dataset_t *db)
{
    double *mean = calloc(db->vsize, sizeof *mean);
    struct data_t *data = db->data;
    for (int ivec = 0; ivec < db->vsize; ++ivec)
    {
        for (int idata = 0; idata < db->db_size; ++idata)
        {
            mean[ivec] += data[idata].vector[ivec];
        }
        mean[ivec] /= db->db_size;
    }
    return mean;
}

void mean_init(som_t *som, dataset_t *db)
{
    double min = rand_double(0.0, 0.1);
    double max = rand_double(0.0, 0.1);
    double *mean = mean_dataset(db);
    int msize = som->height * som->width;
    int vsize = db->vsize;
    for (int ineuron = 0; ineuron < msize; ++ineuron)
    {
        for (int ivec = 0; ivec < vsize; ++ivec)
        {
            som->map[ineuron].weights[ivec] = rand_double(mean[ivec] - min, mean[ivec] + max);
        }
    }
    free(mean);
    mean = NULL;
}

void rand_init(som_t *som)
{
    neuron_t *map = som->map;
    int msize = som->height * som->width;
    int vsize = som->params->vsize;
    for (int ineuron = 0; ineuron < msize; ++ineuron)
    {
        for (int ivec = 0; ivec < vsize; ++ivec)
        {
            map[ineuron].weights[ivec] = rand_double(0.0, 1.0);
        }
    }
}

void init_map(som_t *som, dataset_t *db)
{
    som_param_t *params = som->params;
    int vsize = params->vsize;
    som->height = params->map_height;
    som->width = params->map_width;
    int msize = som->height * som->width;
    neuron_t *map = calloc(msize, sizeof *map);
    for (int ineuron = 0; ineuron < msize; ++ineuron)
    {
        map[ineuron].weights = calloc(vsize, sizeof *map[ineuron].weights);
    }
    som->map = map;
    switch (params->init_type)
    {
    case 1:
        mean_init(som, db);
        break;
    case 2:
        rand_init(som);
        break;

    default:
        break;
    }
}

void rule(double *weights, double *vdata, int vsize, double fact)
{
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        weights[ivec] += fact * (vdata[ivec] - weights[ivec]);
    }
}

void update_weights(som_t *som, int bmu_idx)
{
    point2Di_t bmu = {.x = (bmu_idx % som->width), .y = (bmu_idx / som->width)};
    point2Di_t begin, end, tmp;
    som_param_t *params = som->params;
    neuron_t *map = som->map;
    double ray = params->ray, r2 = params->ray * params->ray * 2.0, gauss = params->alpha;
    int idx = 0;
    begin.x = max_int(0, bmu.x - (int)ray);
    begin.y = max_int(0, bmu.y - (int)ray);
    end.x = min_int(bmu.x + (int)ray, som->width - 1);
    end.y = min_int(bmu.y + (int)ray, som->height - 1);
    for (int iy = begin.y; iy <= end.y; ++iy)
    {
        for (int ix = begin.x; ix <= end.x; ++ix)
        {
            tmp.x = ix;
            tmp.y = iy;
            idx = iy * som->width + ix;
            if (params->learning_type == 0)
            {
                gauss = exp(-map_dist(tmp, bmu) / r2) * params->alpha;
            }
            rule(map[idx].weights, som->vdata, params->vsize, gauss);
        }
    }
}

int compete(neuron_t *map, const double *vdata, int vsize, int map_size)
{
    double min = 1000.0, dist = 0.0;
    int bmu_idx = 0;
    for (int ineuron = 0; ineuron < map_size; ++ineuron)
    {
        dist = data_dist(map[ineuron].weights, vdata, vsize);
        if (dist < min)
        {
            min = dist;
            bmu_idx = ineuron;
        }
    }
    return bmu_idx;
}

void print_vec(int *p, int size)
{
    for (int ivec = 0; ivec < size; ++ivec)
        printf("%d ", p[ivec]);
    printf("\n");
}

void phase(som_t *som, dataset_t *db, double phase, double alpha)
{
    som_param_t *params = som->params;
    neuron_t *map = som->map;
    struct data_t *data = db->data;
    int map_size = som->height * som->width, niter = (int)params->niter * phase,
        db_size = db->db_size, update = (int)(niter / params->ray), vsize = params->vsize;
    int *index = make_index(db_size);
    for (int iter = 1; iter <= niter; ++iter)
    {
        shuffle(index, db_size);
        params->ray -= (iter % update == 0 && params->ray > 1.0) ? 1.0 : 0.0;
        params->alpha = alpha * (1.0 - ((double)iter / (double)niter));
        for (int idata = 0; idata < db_size; ++idata)
        {
            som->vdata = data[index[idata]].vector;
            int bmu = compete(map, som->vdata, vsize, map_size);
            update_weights(som, bmu);
        }
    }
    free(index);
}

void learn(som_t *som, dataset_t *db)
{
    double alpha = som->params->alpha;
    phase(som, db, PHASE_I, alpha);
    som->params->ray = 1.0;
    alpha /= 10.0;
    phase(som, db, PHASE_II, alpha);
}



void tag_map(som_t *som, dataset_t *db)
{
    int map_size = som->height * som->width, db_size = db->db_size, vsize = db->vsize, data_idx = 0;
    neuron_t *map = som->map;
    double dist = 0.0, min = 1000.0;
    struct data_t *data = db->data;
    for (int imap = 0; imap < map_size; ++imap)
    {
        min = 1000.0;
        for (int idata = 0; idata < db_size; ++idata)
        {
            dist = data_dist(data[idata].vector, map[imap].weights, vsize);
            if (dist < min)
            {
                min = dist;
                data_idx = idata;
            }
        }
        map[imap].label = data[data_idx].ilabel;
    }
}

void map_stat(som_t *som, dataset_t *db)
{
    neuron_t *map = som->map;
    struct data_t *data = db->data;
    int map_size = som->height * som->width,
        db_size = db->db_size, vsize = som->params->vsize;
    int *index = make_index(db_size);
    shuffle(index, db_size);
    int result = 0;
    map = som->map;
    for (int idata = 0; idata < db_size; ++idata)
    {
        som->vdata = data[index[idata]].vector;
        int bmu = compete(map, som->vdata, vsize, map_size);
        if (data[index[idata]].ilabel == map[bmu].label)
            ++result;
    }
    som->stat = result;
    som->error_percent = 100.0 - (100.0 * result / (double)db_size);
    free(index);
}

void print_map(som_t *som)
{
    const char *color[] = {GREEN_PRINT, RED_PRINT, BLUE_PRINT};
    int height = som->height, width = som->width;
    neuron_t *map = som->map;
    for (int ih = 0; ih < height; ++ih)
    {
        for (int iw = 0; iw < width; ++iw)
        {
            fprintf(stdout," %s%d", color[map[ih * width + iw].label], map[ih * width + iw].label);
        }
        fprintf(stdout,"%s\n", DEFAULT_PRINT);
    }
    fprintf(stdout,"\n\nError percentage: %.2f\n", som->error_percent);
    fprintf(stdout, "Number of epoches are: %d\n",som->params->niter);
}


void free_map(som_t *som)
{
    int map_size = som->height * som->width;
    neuron_t *map = som->map;
    for (int imap = 0; imap < map_size; ++imap)
    {
        free(map[imap].weights);
    }
    free(map);
    map = NULL;
    som->vdata = NULL;
    som->free_map = NULL;
    som->init = NULL;
    som->learn = NULL;
    som->print_map = NULL;
    som->map_stat = NULL;
    som->tag = NULL;
    
}