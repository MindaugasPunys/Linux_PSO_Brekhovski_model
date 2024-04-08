#include "pso.hpp"
#include "pso_utils.hpp"
#include "model.hpp"

/* ================================================================= */
/* DEFINES */
/* ================================================================= */
#define PSO_DIMENSION ARGS_SIZE
#define DECIMAL_PART_64P32 0xFFFFFFFF
#define WHOLE_PART_64P32(x) (((x) << 32) | DECIMAL_PART_64P32)

struct sParticle_t {
    /* Position == ModelArgs */
    ap_fixed_64p32 position[PSO_DIMENSION];
    ap_fixed_64p32 velocity[PSO_DIMENSION];
    ap_fixed_64p32 fitness_current;

    ap_fixed_64p32 position_best[PSO_DIMENSION];
    ap_fixed_64p32 fitness_best;
};

/* ================================================================= */
/* Constants */
/* ================================================================= */
static const ap_fixed_32p16 PI_x_2 = 6.28318530717958647692;

/* ================================================================= */
/* Global variables */
/* ================================================================= */


/* max_mask = INT part + FFFFFFFF float part */
static const sArgConst_t args_const_lut[ARGS_SIZE] = {
/*   max,               max_mask,   inert,  pers,   glb     ARG     */
/*     DECIMAL PART -> |FFFFFFFF|                                   */
    {100,           0x7FFFFFFFFF,   0.1,      1,      1}, /*  alfa0   */
    {10000000,  0x7FFFFFFFFFFFFF,   0.1,      1,      1}, /*  freq0   */
    {2000,         0x7FFFFFFFFFF,   0.1,      1,      1}, /*  c2      */
    {10,             0xFFFFFFFFF,   0.1,      1,      1}, /*  n       */
    {2000,         0x7FFFFFFFFFF,   0.1,      1,      1}, /*  ro2     */
    {1,               0xFFFFFFFF,   0.1,      1,      1}  /*  h       */
};

/* ================================================================= */
/* PRIVATE FUNCTIONS */
/* ================================================================= */

ap_fixed_64p32 pso_fitness(const ap_fixed_64p32 args[ARGS_SIZE],
                           const sModelParams &params,
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
static void pso_swarm_init(sParticle_t swarm[PSO_SWARM_SIZE],
                           const sModelParams &params,
                           const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                           const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    /* Initiate swarm values */
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        for (int j = 0; j < PSO_DIMENSION; j++) {
            swarm[i].position[j] = rand() % args_const_lut[j].max;
            swarm[i].position_best[j] = swarm[i].position[j];
            swarm[i].velocity[j] = 0;
        }

        swarm[i].fitness_current =
            pso_fitness(swarm[i].position, params, meas_signal, ref_signal, freq_axis);
        swarm[i].fitness_best = swarm[i].fitness_current;
        pso_util_print("init", i, swarm[i].position);
    }
}

static void pso_update_velocity(sParticle_t &swarm, sParticle_t &global_best) {
    ap_fixed_64p32 rand_val_1, rand_val_2;
    ap_fixed_64p32 vel_interia, vel_personal, vel_global;
    for (int j = 0; j < PSO_DIMENSION; j++) {
#pragma HLS pipeline
        rand_val_1 = (ap_fixed_64p32)rand() / RAND_MAX;
        rand_val_2 = (ap_fixed_64p32)rand() / RAND_MAX;
        // printf("rand_val[%d]: %f, rand_val[%d]: %f\n", 2 * j,
        // (double)rand_val[2 * j], 2 * j + 1, (double)rand_val[2 * j + 1]);
        vel_interia = args_const_lut[j].inertia * swarm.velocity[j];
        vel_personal = args_const_lut[j].personal_weight * rand_val_1 *
                       (swarm.position_best[j] - swarm.position[j]);
        vel_global = args_const_lut[j].global_weight * rand_val_2 *
                     (global_best.position[j] - swarm.position[j]);
        swarm.velocity[j] = vel_interia + vel_personal + vel_global;
    }
}

static void pso_update_position(sParticle_t &swarm) {
    ap_fixed_64p32 new_position;
    for (int j = 0; j < PSO_DIMENSION; j++) {
#pragma HLS pipeline
        new_position = swarm.position[j] + swarm.velocity[j];
        if (new_position > args_const_lut[j].max) {
            new_position = args_const_lut[j].max;
            swarm.velocity[j] = 0;
        } else if (new_position < 0) {
            new_position = 0;
            swarm.velocity[j] = 0;
        }
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
pso_update_fitness(sParticle_t &swarm, const sModelParams &params,
                   const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                   const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                   const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    swarm.fitness_current =
        pso_fitness(swarm.position, params, meas_signal, ref_signal, freq_axis);
    if (swarm.fitness_current < swarm.fitness_best) {
        swarm.fitness_best = swarm.fitness_current;
        pso_copy_position(swarm.position, swarm.position_best);
    }
}

static void
pso_swarm_update(sParticle_t swarm[PSO_SWARM_SIZE], sParticle_t &global_best,
                 const sModelParams &params,
                 const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE]) {
    /* Update velocity and position */
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        pso_update_velocity(swarm[i], global_best);
        pso_update_position(swarm[i]);
        pso_update_fitness(swarm[i], params, meas_signal, ref_signal, freq_axis);
        // pso_util_print("swarm", i, swarm[i].position);
        // pso_util_print("best", i, swarm[i].position_best);
    }
}

/* Find best particle in the swarm and return its fitness */
static void pso_find_global_best(sParticle_t swarm[PSO_SWARM_SIZE],
                                 sParticle_t &global_best) {
    for (int i = 0; i < PSO_SWARM_SIZE; i++) {
        if (swarm[i].fitness_best < global_best.fitness_best) {
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
                 const sModelParams &params,
                 const ap_fixed_32p16 meas_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_32p16 ref_signal[TRANSFER_FUNC_SIZE],
                 const ap_fixed_64p32 freq_axis[TRANSFER_FUNC_SIZE],
                 const uint16_t itterations) {

    sParticle_t swarm[PSO_SWARM_SIZE];
// #pragma HLS array_partition variable = swarm->position complete
// #pragma HLS array_partition variable = swarm->position_best complete
// #pragma HLS array_partition variable = swarm->velocity complete

    sParticle_t global_best;
// #pragma HLS array_partition variable = global_best.position complete
// #pragma HLS array_partition variable = global_best.position_best complete

    pso_swarm_init(swarm, params, meas_signal, ref_signal, freq_axis);
    global_best = swarm[0];
    pso_find_global_best(swarm, global_best);
    pso_util_print("initial_best", 0, global_best.position);

    for (int iter = 0; iter < PSO_ITERATIONS; iter++) {
        pso_swarm_update(swarm, global_best, params, meas_signal, ref_signal, freq_axis);
        pso_find_global_best(swarm, global_best);
        pso_copy_position(global_best.position, args_estimate);
        pso_util_print("best", iter, global_best.position);
        printf("fitness: %f\n", (double)global_best.fitness_best);
    }
    printf("final fitness: %f\n", (double)global_best.fitness_best);
}
