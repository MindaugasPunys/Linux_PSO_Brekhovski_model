#include "pso.hpp"
#include "pso_utils.hpp"
#include "model.hpp"

/* ================================================================= */
/* DEFINES */
/* ================================================================= */
#define PSO_DIMENSION ARGS_SIZE

struct sParticle_t {
    /* Position == ModelArgs */
    ap_fixed_64p32 position[PSO_DIMENSION];
    ap_fixed_64p32 velocity[PSO_DIMENSION];
    ap_fixed_64p32 fitness_current;

    ap_fixed_64p32 position_best[PSO_DIMENSION];
    ap_fixed_64p32 fitness_best;
};

/* ================================================================= */
/* Global variables */
/* ================================================================= */
static sParticle_t swarm[PSO_SWARM_SIZE];
static sParticle_t global_best;
static sModelParams params;

static const sArgsRange_t args_range[ARGS_SIZE] = {
    {15     , 20    }, /* alfa0 */
    {0.25e6 , 1e6   }, /* freq0 */
    {880    , 1280  }, /* c2    */
    {1      , 2.5   }, /* n     */
    {924    , 1324  }, /* ro2   */
    {0.0005 , 0.0020}  /* h     */
};

static const sParticleCfg_t particle_cfg[ARGS_SIZE] = {
    {0.1, 1, 1}, /* alfa0   */
    {0.1, 1, 1}, /* freq0   */
    {0.1, 1, 1}, /* c2      */
    {0.1, 1, 1}, /* n       */
    {0.1, 1, 1}, /* ro2     */
    {0.1, 1, 1}  /* h       */
};

/* ================================================================= */
/* PRIVATE FUNCTIONS */
/* ================================================================= */

ap_fixed_64p32 pso_fitness(const ap_fixed_64p32 args[ARGS_SIZE],
                           const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    /* Add your code here */
    ap_fixed_64p32 fitness = 0;
    /* TEST */
//     fitness = args[0] + args[1] + args[2] + args[3] + args[4] + args[5];

    /* Call the model */
    sModelArgs model_args = {args[0], args[1], args[2], args[3], args[4], args[5]};
    ap_fixed_32p16 wave_result[TRANSFER_FUNC_SIZE];
    WaveSynthesis(model_args, params, ref_signal, freq_axis, wave_result);

    /* Calculate fitness */
    ap_fixed_64p32 diff;
    ap_fixed_64p32 numerator = 0;
    ap_fixed_64p32 denominator = 0;
    for (int i = 0; i < TRANSFER_FUNC_SIZE; i++) {
        diff = wave_result[i] - meas_signal[i];
        numerator += diff * diff;
        denominator += meas_signal[i] * meas_signal[i];
        // printf(" %-10f += %-10f ^ 2\n", (double)denominator, (double)ref_signal[i]);
    }

    if (denominator == 0) {
        fitness = 999999;
    } else {
        fitness = numerator / denominator;
    }
    return fitness;
}

/* PSO Functions */
static void pso_swarm_init(const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    /* Initiate swarm values */
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        for (int j = 0; j < PSO_DIMENSION; j++) {
#pragma HLS pipeline
            ap_fixed_64p32 rand_val =
                args_range[j].min +
                PRNG_64p32() * (args_range[j].max - args_range[j].min);
            swarm[i].position[j] = rand_val;
            swarm[i].position_best[j] = swarm[i].position[j];
            swarm[i].velocity[j] = 0;
        }

        swarm[i].fitness_current =
            pso_fitness(swarm[i].position, meas_signal, ref_signal, freq_axis);
        swarm[i].fitness_best = swarm[i].fitness_current;
        pso_util_print("init", i, swarm[i].position);
    }
}

static void pso_update_velocity(sParticle_t &swarm) {
    ap_fixed_64p32 rand_val_1, rand_val_2;
    ap_fixed_64p32 vel_interia, vel_personal, vel_global;
    for (int j = 0; j < PSO_DIMENSION; j++) {
#pragma HLS pipeline
        rand_val_1 = PRNG_64p32();
        rand_val_2 = PRNG_64p32();
        vel_interia = particle_cfg[j].inertia * swarm.velocity[j];
        vel_personal = particle_cfg[j].personal_weight * rand_val_1 *
                       (swarm.position_best[j] - swarm.position[j]);
        vel_global = particle_cfg[j].global_weight * rand_val_2 *
                     (global_best.position[j] - swarm.position[j]);
        swarm.velocity[j] = vel_interia + vel_personal + vel_global;
    }
}

static void pso_update_position(sParticle_t &swarm) {
    ap_fixed_64p32 new_position;
    for (int j = 0; j < PSO_DIMENSION; j++) {
#pragma HLS pipeline
        new_position = swarm.position[j] + swarm.velocity[j];
        swarm.position[j] = new_position;
    }
}

static void pso_copy_position(ap_fixed_64p32 *source, ap_fixed_64p32 *dest) {
    for (int i = 0; i < PSO_DIMENSION; i++) {
        // printf(" %f --> %f\n", (double)source[i], (double)dest[i]);
        dest[i] = source[i];
    }
}

static void
pso_update_fitness(sParticle_t &swarm,
                   const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                   const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                   const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    swarm.fitness_current =
        pso_fitness(swarm.position, meas_signal, ref_signal, freq_axis);
    if (swarm.fitness_current < swarm.fitness_best) {
        swarm.fitness_best = swarm.fitness_current;
        pso_copy_position(swarm.position, swarm.position_best);
    }
}

static void
pso_swarm_update(const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    /* Update velocity and position */
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        pso_update_velocity(swarm[i]);
        pso_update_position(swarm[i]);
        pso_update_fitness(swarm[i], meas_signal, ref_signal, freq_axis);
        // pso_util_print("swarm", i, swarm[i].position);
        // pso_util_print("best", i, swarm[i].position_best);
    }
}

/* Find best particle in the swarm and return its fitness */
static void pso_find_global_best(void) {
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        if (swarm[i].fitness_best < global_best.fitness_best) {
            // pso_util_print("global_best", i, swarm[i].position_best);
            global_best.fitness_best = swarm[i].fitness_best;
            pso_copy_position(swarm[i].position, global_best.position);
            pso_copy_position(swarm[i].position_best,
                              global_best.position_best);
        }
    }
}

/* ================================================================= */
/* PUBLIC FUNCTIONS */
/* ================================================================= */
void pso_process(ap_fixed_64p32 args_estimate[ARGS_SIZE],
                 const sModelParams &params_ref,
                 const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {

    /* Copy input arguments */
    params = params_ref;

/* swarm */
#pragma HLS array_partition variable = swarm->position complete
#pragma HLS array_partition variable = swarm->position_best complete
#pragma HLS array_partition variable = swarm->velocity complete
/* global_best */
#pragma HLS array_partition variable = global_best.position complete
#pragma HLS array_partition variable = global_best.position_best complete

    pso_swarm_init(meas_signal, ref_signal, freq_axis);
    global_best = swarm[0];
    pso_find_global_best();
    pso_util_print("initial_best", 0, global_best.position);

    for (int iter = 0; iter < PSO_ITERATIONS; iter++) {
        pso_swarm_update(meas_signal, ref_signal, freq_axis);
        pso_find_global_best();
        pso_copy_position(global_best.position, args_estimate);
        pso_util_print("best", iter, global_best.position);
        printf("fitness: %f\n", (double)global_best.fitness_best);
    }
    printf("final fitness: %f\n", (double)global_best.fitness_best);
}
