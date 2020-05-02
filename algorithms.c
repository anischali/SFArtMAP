#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix))
#include <sys/time.h>
#else
#include <time.h>
#include<WinSock2.h>
#endif

void init_all(algorithms_t *algos)
{
    algos->params = get_config();
    init_som_struct(&algos->som);
    init_sfam_struct(&algos->sfam);
    algos->databases[SOM_DB] = dataset(&algos->params->som_data);
    algos->databases[SFAM_DB] = dataset(&algos->params->sfam_data);
    algos->som.params = &algos->params->som_params;
    algos->sfam.params = &algos->params->sfam_params;
    algos->som.init(&algos->som, algos->databases[SOM_DB]);
    algos->sfam.init(&algos->sfam);
}



void free_algos(algorithms_t *algos)
{
    free_dataset(algos->databases[SOM_DB]);
    free_dataset(algos->databases[SFAM_DB]);
    algos->som.free_map(&(algos->som));
    algos->sfam.free_prototypes(&(algos->sfam));
}

static double get_time_diff(struct timeval b, struct timeval e)
{
    double msb = b.tv_sec * 1000.0 + b.tv_usec / 1000.0,
        mse = e.tv_sec * 1000.0 + e.tv_usec / 1000.0;
    return (double) mse - msb;
}

void time_compar(algorithms_t *algos)
{
    struct timeval b_som, b_sfam, e_som, e_sfam;
    double d_som, d_sfam;
    gettimeofday(&b_som, NULL);
    algos->som.learn(&algos->som, algos->databases[SOM_DB]);
    gettimeofday(&e_som, NULL);
    d_som = get_time_diff(b_som, e_som);
    gettimeofday(&b_sfam, NULL);
    algos->sfam.learn(&algos->sfam, algos->databases[SFAM_DB]);
    gettimeofday(&e_sfam, NULL);
    d_sfam = get_time_diff(b_sfam, e_sfam);
    fprintf(stdout,"SOM Leraning time: %.6f ms\nSFAM Learning time: %.6f ms\n", d_som, d_sfam);   
}