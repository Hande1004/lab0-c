#ifndef DUDECT_TTEST_H
#define DUDECT_TTEST_H

#include <stdint.h>
#include <stdlib.h>

#define DUDECT_NUMBER_PERCENTILES (100)
#define DUDECT_TESTS (1 + DUDECT_NUMBER_PERCENTILES + 1)

typedef struct {
    double mean[2];
    double m2[2];
    double n[2];
} t_context_t;

typedef struct {
    int64_t *before_ticks;
    int64_t *after_ticks;
    int64_t *exec_times;
    uint8_t *input_data;
    uint8_t *classes;
    t_context_t *ttest_ctxs[DUDECT_TESTS];
    int64_t *percentiles;
} dudect_ctx_t;

void t_push(t_context_t *ctx, double x, uint8_t class);
double t_compute(t_context_t *ctx);
void t_init(t_context_t *ctx);
int cmp(const int64_t *a, const int64_t *b);
int64_t percentile(const int64_t *a_sorted, double which, size_t size);
void prepare_percentiles(dudect_ctx_t *ctx);

#endif
