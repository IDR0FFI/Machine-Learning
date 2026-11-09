#ifndef __MODEL_CONFIG_H
#define __MODEL_CONFIG_H

#ifndef MNIST_DATASET
#define MNIST_DATASET
  /* Removing this idea
  #define MNIST_DIR "./mnist/"
  */

  /* Would need a check if this are specified */
  /* Try to have global path */
  #define MNIST_TRAIN_IMAGE "./mnist/train-images-idx3-ubyte"
  #define MNIST_TRAIN_LABEL "./mnist/train-labels-idx1-ubyte"
  #define MNIST_TEST_IMAGE  "./mnist/t10k-images-idx3-ubyte"
  #define MNIST_TEST_LABEL  "./mnist/t10k-labels-idx1-ubyte"
#endif // MNIST_DATASET

#endif // __MODEL_CONFIG_H
