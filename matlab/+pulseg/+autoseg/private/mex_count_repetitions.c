#include "mex.h"
#include <stdbool.h>

/* Compare two arrays element-wise. Returns true if equal. */
bool array_equal(const double* a, const double* b, mwSize len) {
    for (mwSize i = 0; i < len; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/* The MEX gateway function.
 * Usage: count = mex_count_repetitions(arr, start, L)
 * 'start' is expected to be 1-indexed.
 */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    if(nrhs < 3) {
        mexErrMsgIdAndTxt("mex_count_repetitions:invalidNumInputs",
                          "Three inputs required: arr, start, L.");
    }
    
    /* Retrieve the input array */
    if( !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) ) {
        mexErrMsgIdAndTxt("mex_count_repetitions:inputNotRealDouble",
                          "Input array must be type double.");
    }
    double *arr = mxGetPr(prhs[0]);
    mwSize n = mxGetNumberOfElements(prhs[0]);
    
    /* Get the starting index (1-indexed) and convert to 0-indexed */
    if( !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || mxGetNumberOfElements(prhs[1]) != 1 ) {
        mexErrMsgIdAndTxt("mex_count_repetitions:invalidStart",
                          "Start must be a scalar double.");
    }
    mwSize start = (mwSize) mxGetScalar(prhs[1]);
    if(start < 1 || start > n) {
        mexErrMsgIdAndTxt("mex_count_repetitions:invalidStartValue",
                          "Start is out of bounds.");
    }
    start = start - 1;  /* convert to 0-indexing */
    
    /* Get period length L */
    if( !mxIsDouble(prhs[2]) || mxIsComplex(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 1 ) {
        mexErrMsgIdAndTxt("mex_count_repetitions:invalidL",
                          "L must be a scalar double.");
    }
    mwSize L = (mwSize) mxGetScalar(prhs[2]);
    if(L < 1 || start + L > n) {
        mexErrMsgIdAndTxt("mex_count_repetitions:invalidLValue",
                          "Invalid period length L.");
    }
    
    mwSize count = 0;
    while(start + (count + 1) * L <= n) {
        if( array_equal(&arr[start + count * L], &arr[start], L) ) {
            count++;
        } else {
            break;
        }
    }
    
    plhs[0] = mxCreateDoubleScalar((double)count);
}
