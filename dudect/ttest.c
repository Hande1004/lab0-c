/**
 * Online Welch's t-test.
 *
 * Tests whether two populations have same mean.
 * This is basically Student's t-test for unequal
 * variances and unequal sample sizes.
 *
 * See https://en.wikipedia.org/wiki/Welch%27s_t-test
 */

#include "ttest.h"
#include <assert.h>
#include <constant.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

void t_push(t_context_t *ctx, double x, uint8_t class)
{
    assert(class == 0 || class == 1);
    ctx->n[class]++;

    /* Welford method for computing online variance
     * in a numerically stable way.
     */
    double delta = x - ctx->mean[class];
    ctx->mean[class] = ctx->mean[class] + delta / ctx->n[class];
    ctx->m2[class] = ctx->m2[class] + delta * (x - ctx->mean[class]);
}

double t_compute(t_context_t *ctx)
{
    double var[2] = {0.0, 0.0};
    var[0] = ctx->m2[0] / (ctx->n[0] - 1);
    var[1] = ctx->m2[1] / (ctx->n[1] - 1);
    double num = (ctx->mean[0] - ctx->mean[1]);
    double den = sqrt(var[0] / ctx->n[0] + var[1] / ctx->n[1]);
    double t_value = num / den;
    return t_value;
}

void t_init(t_context_t *ctx)
{
    for (int class = 0; class < 2; class ++) {
        ctx->mean[class] = 0.0;
        ctx->m2[class] = 0.0;
        ctx->n[class] = 0.0;
    }
    return;
}
int cmp(const int64_t *a, const int64_t *b)
{
    if (*a == *b)
        return 0;
    return (*a > *b) ? 1 : -1;
}

int64_t percentile(const int64_t *a_sorted, double which, size_t size)
{
    size_t array_position = (size_t) ((double) size * (double) which);
    assert(array_position < size);
    return a_sorted[array_position];
}

/*
 set different thresholds for cropping measurements.
 the exponential tendency is meant to approximately match
 the measurements distribution, but there's not more science
 than that.
*/
void prepare_percentiles(dudect_ctx_t *ctx)
{
    qsort(ctx->exec_times, N_MEASURES, sizeof(int64_t),
          (int (*)(const void *, const void *)) cmp);
    for (size_t i = 0; i < DUDECT_NUMBER_PERCENTILES; i++) {
        ctx->percentiles[i] = percentile(
            ctx->exec_times,
            1 - (pow(0.5, 10 * (double) (i + 1) / DUDECT_NUMBER_PERCENTILES)),
            N_MEASURES);
    }
}
