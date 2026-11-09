#include <stdio.h>
#include <stdlib.h>

#include "dataset_config.h"
#include "dataset.h"
#include "util.h"
#include "model.h"

#define BATCH_SIZE 64
#define EPOCHS 10

int main() {
  Image_Header timg_head;
  Label_Header tlbl_head;
  stat_dataset(MNIST_TRAIN_IMAGE, &timg_head);
  stat_dataset(MNIST_TRAIN_LABEL, &tlbl_head);

  Data train_images[timg_head.id.count];
  load_idx_format(MNIST_TRAIN_IMAGE, train_images, &timg_head);

  printf("Images Loaded: %d %d %d %d\n", timg_head.id.magic_number, timg_head.id.count, timg_head.row, timg_head.column);

  f_array train_img_f[timg_head.id.count];
  binarizeF(train_images, train_img_f, timg_head.id.count);
  free_Data(train_images, timg_head.id.count);

  float vb_init = init_visible_bias(train_img_f, 20000);
  printf("Log-Odd: %f\n", vb_init);

  rbm r;
  init_rbm(&r, vb_init);

  for (size_t a=0; a<EPOCHS; a++) {
    float total_loss = 0.0f;

    for (size_t i=0; i<938; i++) {
      float batch_loss = contrastive_divergence(&r, train_img_f, i);

      total_loss += batch_loss;
      if (i % 200 == 0) {
        printf("loss: %f\n", batch_loss);
      }
    }

    float avg_loss = total_loss / 938;
    printf("EPOCHS %ld: avg_loss : %f\n", a, avg_loss);
  }

  f_array reconstruct[64];
  generate(&r, reconstruct, 64, 300);

  Data res[64];
  touint(reconstruct, res);
  for (int i=0; i<64; i++) {
    base64(res[i].block);
    printf("\n");
  }

  free(r.weight.values);
  free(r.hidden_bias.values);
  free(r.visible_bias.values);
  free_Data(res, 64);
  for (size_t i=0; i<64; i++) {
    free(reconstruct[i].values);
  }
  for (size_t i=0; i<timg_head.id.count; i++) {
    free(train_img_f[i].values);
  }

  return 0;
}
