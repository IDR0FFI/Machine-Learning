#ifndef __MODEL_H
#define __MODEL_H

#include <stddef.h>
#include "util.h"

typedef struct {
  /* Would not be necessary as both the bias stores this value */
  size_t visible_nodes,
         hidden_nodes;

  f_matrix weight;
  f_array  visible_bias;
  f_array  hidden_bias;
} rbm;

void binarizeF(const Data*, f_array*, size_t);
void create_batch(f_array*, f_matrix*, size_t, size_t);
float sigmoid(float);
float bernoulli(float);

float init_visible_bias(f_array*, size_t);
float contrastive_divergence(rbm*, f_array*, size_t);

void init_rbm(rbm*, float);
void del_rbm(rbm*);
void sample_h_given_v(rbm*, f_array*, size_t, f_array*, f_array*);
void sample_v_given_h(rbm*, f_array*, size_t, f_array*, f_array*);

void generate(rbm*, f_array*, size_t, size_t);

#endif // __MODEL_H
