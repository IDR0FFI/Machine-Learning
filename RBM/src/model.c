#include "model.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void binarizeF(const Data d[], f_array fa[], size_t size) {

  for (size_t i=0; i<size; i++) {
    fa[i].size = d[i].size;
    fa[i].values = (float*) malloc(sizeof(float) * d[i].size);
    
    for (size_t j=0; j<d[0].size; j++) {
      fa[i].values[j] = ((float)(d[i].block[j]) / 255) > 0.5;
    }
  }
}
float sigmoid(float f) {
  return 1 / (1+exp(-f));
}
float bernoulli(float f) {
  return f > randf();
}

float init_visible_bias(f_array f[], size_t size) {
  float active_prob = 0;
  int active_count = 0;

  for (size_t i=0; i<size; i++) {
    for (size_t j=0; j<f[0].size; j++) {
      active_count += f[i].values[j];
    }
  }
  
  active_prob = (float)active_count / (f[0].size * size);

  return logf(active_prob / (1 - active_prob));
}
float contrastive_divergence(rbm* r, f_array* f, size_t curr_batch) {
  size_t batch_size = curr_batch == 937 ? 32 : 64;

  f_array h0_prob[batch_size], h0_sample[batch_size], h_prob[batch_size], v_prob[batch_size], v_sample[batch_size];

  f_array *v = f + curr_batch * 64;
  sample_h_given_v(r, v, batch_size, h0_prob, h0_sample);
  sample_v_given_h(r, h0_sample, batch_size, v_prob, v_sample);
  sample_h_given_v(r, v_sample, batch_size, h_prob, h0_sample);

  // multiply f's x batch 0 values with h0's x batch 0 values
  for (size_t v_index=0; v_index<784; v_index++) {
    for (size_t h_index=0; h_index<128; h_index++) {
      float w = 0, vb = 0, hb = 0;
      for (size_t batch=0; batch<batch_size; batch++) {
        w += (v[batch].values[v_index] * h0_prob[batch].values[h_index]) - (v_sample[batch].values[v_index] * h_prob[batch].values[h_index]);
        vb += v[batch].values[v_index] - v_sample[batch].values[v_index];
        hb += h0_prob[batch].values[h_index] - h_prob[batch].values[h_index];
      }
      r->weight.values[v_index * 128 + h_index] += ( 0.01f * w ) / batch_size;
      r->visible_bias.values[v_index] += (0.01f * vb) / batch_size;
      r->hidden_bias.values[h_index] += (0.01f * hb) / batch_size;
    }
  }

  float loss=0.0f;
  for (size_t batch=0; batch<batch_size; batch++) {
    for (size_t v_index=0; v_index<784; v_index++) {
      loss += (v[batch].values[v_index] - v_prob[batch].values[v_index]) * (v[batch].values[v_index] - v_prob[batch].values[v_index]);
    }
  }

  for (size_t batch=0; batch<batch_size; batch++) {
    free(h0_prob[batch].values);
    free(h0_sample[batch].values);
    free(h_prob[batch].values);
    free(v_prob[batch].values);
    free(v_sample[batch].values);
  }

  return loss / (784 * batch_size);
}

void init_rbm(rbm* r, float vb_init) {
  r->visible_nodes = 784;
  r->hidden_nodes = 128;

  r->weight.row = 784;
  r->weight.col = 128;
  r->weight.values = (float*) malloc(sizeof(float) * 784 * 128);
  for (int i=0; i<784; i++) {
    for (int j=0; j<128; j++) {
      r->weight.values[i * 128 + j] = randf() * 0.01;
    }
  }

  r->visible_bias.size = 784;
  r->visible_bias.values = (float*) malloc(sizeof(float) * 784);
  for (int i=0; i<784; i++) {
    r->visible_bias.values[i] = vb_init;
  }

  r->hidden_bias.size = 128;
  r->hidden_bias.values = (float*) calloc(128, sizeof(float));
}
void del_rbm(rbm* r) {
  free(r->weight.values);
  free(r->visible_bias.values);
  free(r->hidden_bias.values);
}
void sample_h_given_v(rbm* r, f_array* v, size_t batch_size, f_array* h_prob, f_array* h_sample) {
  for (size_t batch=0; batch<batch_size; batch++) {
    h_prob[batch].size = 128;
    h_sample[batch].size = 128;
    h_prob[batch].values = (float*) calloc(128, sizeof(float));
    h_sample[batch].values = (float*) calloc(128, sizeof(float));

    for (size_t h_index=0; h_index<128; h_index++) {
      for (size_t v_index=0; v_index<784; v_index++) {
        h_prob[batch].values[h_index] += v[batch].values[v_index] * r->weight.values[v_index * 128 + h_index];
      }
  
      h_prob[batch].values[h_index] = sigmoid(h_prob[batch].values[h_index] + r->hidden_bias.values[h_index]);
      h_sample[batch].values[h_index] = bernoulli(h_prob[batch].values[h_index]);
    }
  }
}
void sample_v_given_h(rbm* r, f_array* h, size_t batch_size, f_array* v_prob, f_array* v_sample) {
  for (size_t batch=0; batch<batch_size; batch++) {
    v_prob[batch].size = 784;
    v_sample[batch].size = 784;
    v_prob[batch].values = (float*) calloc(784, sizeof(float));
    v_sample[batch].values = (float*) calloc(784, sizeof(float));

    for (size_t v_index=0; v_index<784; v_index++) {
      v_prob[batch].values[v_index] = 0;
      for (size_t h_index=0; h_index<128; h_index++) {
        v_prob[batch].values[v_index] += h[batch].values[h_index] * r->weight.values[v_index * 128 + h_index];
      }
  
      v_prob[batch].values[v_index] = sigmoid(v_prob[batch].values[v_index] + r->visible_bias.values[v_index]);
      v_sample[batch].values[v_index] = bernoulli(v_prob[batch].values[v_index]);
    }
  }
}

void generate(rbm* r, f_array* v, size_t samples, size_t step) {
  for (size_t i=0; i<samples; i++) {
    v[i].size = r->visible_nodes;
    v[i].values = (float*) malloc(sizeof(float) * r->visible_nodes);
    for (size_t j=0; j<r->visible_nodes; j++) {
      v[i].values[j] = 0.5f;
    }
  }

  for (size_t i=0; i<step; i++) {
    f_array h_prob[samples], h_sample[samples], v_prob[samples], v_sample[samples];

    sample_h_given_v(r, v, samples, h_prob, h_sample);
    sample_v_given_h(r, h_sample, samples, v_prob, v_sample);

    for (size_t i=0; i<samples; i++) {
      for (size_t j=0; j<r->visible_nodes; j++) {
        v[i].values[j] = v_sample[i].values[j];
      }
    }
  }
  
}
