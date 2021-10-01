import numpy as np
import time
import random


def gen_numpy_set(n=6, dimension=2000):
    dataset = []
    for _ in range(n):
        dataset.append((np.random.rand(dimension, dimension), np.random.rand(dimension, dimension)))

    return dataset


def numpy_load(dataset):
    start = time.time()
    for data in dataset:
        np.matmul(data[0], data[1])

    return time.time() - start


def gen_python_set(n=7000):
    dataset = []
    for _ in range(n * n):
        dataset.append((random.getrandbits(64), random.getrandbits(64)))

    return dataset


def python_load(dataset):
    start = time.time()
    for data in dataset:
        float(str(data[0] / data[1]))

    return time.time() - start
