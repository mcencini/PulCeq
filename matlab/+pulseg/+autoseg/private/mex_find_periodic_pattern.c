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
 * Usage: [start_idx, L] = mex_find_periodic_pattern(arr, min_length)
 */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    if(nrhs < 1) {
        mexErrMsgIdAndTxt("mex_find_periodic_pattern:invalidNumInputs",
                          "At least one input required.");
    }
    
    /* Retrieve the input array */
    if( !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) ) {
        mexErrMsgIdAndTxt("mex_find_periodic_pattern:inputNotRealDouble",
                          "Input array must be type double.");
    }
    double *arr = mxGetPr(prhs[0]);
    mwSize n = mxGetNumberOfElements(prhs[0]);
    
    /* Get min_length, default to 1 */
    mwSize min_length = 1;
    if(nrhs >= 2) {
        if( !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || mxGetNumberOfElements(prhs[1]) != 1 ) {
            mexErrMsgIdAndTxt("mex_find_periodic_pattern:invalidMinLength",
                              "min_length must be a scalar double.");
        }
        min_length = (mwSize) mxGetScalar(prhs[1]);
        if(min_length < 1) {
            mexErrMsgIdAndTxt("mex_find_periodic_pattern:invalidMinLengthValue",
                              "min_length must be at least 1.");
        }
    }
    
    mwSize found_start = (mwSize)-1;
    mwSize found_L = (mwSize)-1;
    
    /* Loop over possible starting indices */
    for(mwSize start = 0; start < n; start++) {
        mwSize max_possible_period = (n - start) / 2;
        for(mwSize L = min_length; L <= max_possible_period; L++) {
            if( array_equal(&arr[start], &arr[start+L], L) ) {
                found_start = start;
                found_L = L;
                goto end_loop;
            }
        }
    }
end_loop:
    /* Return results: convert found_start to 1-indexed */
    plhs[0] = mxCreateDoubleScalar((found_start == (mwSize)-1) ? -1 : (double)(found_start + 1));
    if(nlhs > 1) {
        plhs[1] = mxCreateDoubleScalar((found_L == (mwSize)-1) ? -1 : (double)found_L);
    }
}
