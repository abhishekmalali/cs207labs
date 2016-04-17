# distutils: extra_compile_args = -fopenmp
# distutils: extra_link_args = -fopenmp
#your code here
cimport cython
import numpy as np
cimport numpy as np
from cython.parallel cimport prange

cdef inline double norm2(double complex z) nogil:
    return z.real*z.real + z.imag*z.imag

cdef int escape(int maxiter, double complex z, double complex c) nogil:
    cdef int n=0
    while n < maxiter and norm2(z) < 4:
        z=z*z+c
        n+=1
    return n

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef run_cython_amalgamated2(int gridsize, box, double complex c, int maxiter):
    #your code here
    cdef unsigned int i = 0
    cdef unsigned int j = 0
    cdef double x,y
    cdef double start_x = box.x1
    cdef double start_y = box.y1
    cdef double stepsize_x = (box.x2 - box.x1)/(gridsize - 1.0)
    cdef double stepsize_y = (box.y2 - box.y1)/(gridsize - 1.0)
    cdef double complex[::1] zs = np.zeros(gridsize*gridsize, dtype=np.complex)
    cdef double complex[::1] output = np.zeros(gridsize*gridsize, dtype=np.complex)
    for i in range(gridsize):
        x = start_x + i*stepsize_x
        for j in range(gridsize):
            y = start_y + j*stepsize_y
            zs[i*gridsize + j] = complex(x,y)
            output[i*gridsize + j] = escape(maxiter, zs[i*gridsize + j], c)
    return (zs, output)