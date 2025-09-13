#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "shared.h"
#include "src/random.c"

typedef struct OptimizerValues
{
    bool ENABLE_THREADS;
    int TREND_WINDOW;
    int POP_SIZE;
    int EVOLUTION_GENERATIONS;
    int EVALUATION_TESTS;
    int EVALUATION_STEPS;
    int EVALUATION_EARLY_TERMINATION_STEPS;
    float BASE_EVALUATION_PENALTY;
    float EVALUATION_EARLY_TERMINATION_PENALTY;
    int ELITIST_AMOUNT;
    int BASE_TOURNAMENT_SIZE;
    float CROSSOVER_BIAS;
    float BASE_MUTATION_RATE;
    float MAX_MUTATION_AMOUNT;
    float CROSSOVER_ABORT_MAX;
    float CROSSOVER_ABORT_MIN;
} OptimizerValues;

void write_values_to_file(OptimizerValues op_values)
{
    String_Builder sb = {0};
    sb_appendf(&sb,
               "ENABLE_THREADS %s\n"
               "TREND_WINDOW %d\n"
               "POP_SIZE %d\n"
               "EVOLUTION_GENERATIONS %d\n"
               "EVALUATION_TESTS %d\n"
               "EVALUATION_STEPS %d\n"
               "EVALUATION_EARLY_TERMINATION_STEPS %d\n"
               "BASE_EVALUATION_PENALTY %g\n"
               "EVALUATION_EARLY_TERMINATION_PENALTY %g\n"
               "ELITIST_AMOUNT %d\n"
               "BASE_TOURNAMENT_SIZE %d\n"
               "CROSSOVER_BIAS %g\n"
               "BASE_MUTATION_RATE %g\n"
               "MAX_MUTATION_AMOUNT %g\n"
               "CROSSOVER_ABORT_MAX %g\n"
               "CROSSOVER_ABORT_MIN %g\n",
               op_values.ENABLE_THREADS ? "true" : "false",
               op_values.TREND_WINDOW,
               op_values.POP_SIZE,
               op_values.EVOLUTION_GENERATIONS,
               op_values.EVALUATION_TESTS,
               op_values.EVALUATION_STEPS,
               op_values.EVALUATION_EARLY_TERMINATION_STEPS,
               op_values.BASE_EVALUATION_PENALTY,
               op_values.EVALUATION_EARLY_TERMINATION_PENALTY,
               op_values.ELITIST_AMOUNT,
               op_values.BASE_TOURNAMENT_SIZE,
               op_values.CROSSOVER_BIAS,
               op_values.BASE_MUTATION_RATE,
               op_values.MAX_MUTATION_AMOUNT,
               op_values.CROSSOVER_ABORT_MAX,
               op_values.CROSSOVER_ABORT_MIN);

    write_entire_file("./opt_output.txt", sb.items, sb.count);
}

OptimizerValues random_op_values(RandomState *rng)
{
    const int pop_size = 90;
    const int generations = 20;
    return (OptimizerValues){
        .ENABLE_THREADS = true,
        .EVOLUTION_GENERATIONS = generations,
        .TREND_WINDOW = random_uint64_range(rng, 5, generations / 2),
        .POP_SIZE = pop_size,
        .EVALUATION_TESTS = random_uint64_range(rng, 5, 12),
        .EVALUATION_STEPS = random_uint64_range(rng, 600, 1200),
        .EVALUATION_EARLY_TERMINATION_STEPS = random_uint64_range(rng, 30, 50),
        .BASE_EVALUATION_PENALTY = random_float(rng),
        .EVALUATION_EARLY_TERMINATION_PENALTY = random_float_range(rng, 0, 15),
        .ELITIST_AMOUNT = random_uint64_range(rng, 1, pop_size / 10),
        .BASE_TOURNAMENT_SIZE = random_uint64_range(rng, 1, pop_size),
        .CROSSOVER_BIAS = random_float_range(rng, 0.45, 0.9),
        .BASE_MUTATION_RATE = random_float_range(rng, 0.05, 0.6),
        .MAX_MUTATION_AMOUNT = random_float_range(rng, 0.1, 0.7),
        .CROSSOVER_ABORT_MAX = random_float(rng),
        .CROSSOVER_ABORT_MIN = random_float(rng),
    };
}

OptimizerValues append_optimizer_flags(Cmd *cmd, OptimizerValues op_values)
{
    cmd_append(cmd, "-DOPTIMIZER_RUN");
    cmd_append(cmd, temp_sprintf("-DENABLE_THREADS=%s", op_values.ENABLE_THREADS ? "true" : "false"));
    cmd_append(cmd, temp_sprintf("-DTREND_WINDOW=%d", op_values.TREND_WINDOW));
    cmd_append(cmd, temp_sprintf("-DPOP_SIZE=%d", op_values.POP_SIZE));
    cmd_append(cmd, temp_sprintf("-DEVOLUTION_GENERATIONS=%d", op_values.EVOLUTION_GENERATIONS));
    cmd_append(cmd, temp_sprintf("-DEVALUATION_TESTS=%d", op_values.EVALUATION_TESTS));
    cmd_append(cmd, temp_sprintf("-DEVALUATION_STEPS=%d", op_values.EVALUATION_STEPS));
    cmd_append(cmd, temp_sprintf("-DEVALUATION_EARLY_TERMINATION_STEPS=%d", op_values.EVALUATION_EARLY_TERMINATION_STEPS));
    cmd_append(cmd, temp_sprintf("-DBASE_EVALUATION_PENALTY=%g", op_values.BASE_EVALUATION_PENALTY));
    cmd_append(cmd, temp_sprintf("-DEVALUATION_EARLY_TERMINATION_PENALTY=%g", op_values.EVALUATION_EARLY_TERMINATION_PENALTY));
    cmd_append(cmd, temp_sprintf("-DELITIST_AMOUNT=%d", op_values.ELITIST_AMOUNT));
    cmd_append(cmd, temp_sprintf("-DBASE_TOURNAMENT_SIZE=%d", op_values.BASE_TOURNAMENT_SIZE));
    cmd_append(cmd, temp_sprintf("-DCROSSOVER_BIAS=%g", op_values.CROSSOVER_BIAS));
    cmd_append(cmd, temp_sprintf("-DBASE_MUTATION_RATE=%g", op_values.BASE_MUTATION_RATE));
    cmd_append(cmd, temp_sprintf("-DMAX_MUTATION_AMOUNT=%g", op_values.MAX_MUTATION_AMOUNT));
    cmd_append(cmd, temp_sprintf("-DCROSSOVER_ABORT_MAX=%g", op_values.CROSSOVER_ABORT_MAX));
    cmd_append(cmd, temp_sprintf("-DCROSSOVER_ABORT_MIN=%g", op_values.CROSSOVER_ABORT_MIN));
    return op_values;
}

#endif // OPTIMIZER_H
