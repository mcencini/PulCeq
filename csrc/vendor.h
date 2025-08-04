/**
 * @file vendor.h
 * @brief Customizable vendor-specific macros.
 *
 */
#ifndef VENDOR_H
#define VENDOR_H

#include <stdlib.h>

/** 
  * Default ALLOC to malloc if it's not already defined
  *
  * Users are encouraged to replace with vendor-specific implementations if needed:
  * 
  * // pulSeg_vendor.h (vendor-specific header)
  * 
  * #include "my_vendor_library.h"  // This contains the definition of MyVendorAlloc
  * 
  * // Override ALLOC to use MyVendorAlloc in the vendor environment
  * #define ALLOC(size) MyVendorAlloc(size)  // Replaces malloc with MyVendorAlloc
  * 
  * #include "pulSeg.h"  // Now include the vendor-agnostic pulSeg.h with the overridden ALLOC
  * 
  * // Other vendor-specific declarations can go here
*/
#ifndef ALLOC
    #define ALLOC(size) malloc(size)
#endif

#ifndef FREE
  #define FREE(ptr) free(ptr)
#endif

/** Waveforms samples type and scaling
  * 
  * Used to rescale and process waveform after
  * parsing from SeqFile:
  *
  * wave[n] = (WAVETYPE)(2 * (__WAVETYPE__)(seqfile_wave[m] * WAVESCALE / 2)));
*/
#ifndef WAVETYPE
  #define WAVETYPE float
#endif

#ifndef __WAVETYPE__ 
  #define __WAVETYPE__ float
#endif

#ifndef WAVESCALE
  #define WAVESCALE 1.0f
#endif

/* We use Siemens as defaults */
#ifndef ADC_RASTER_US
  #define ADC_RASTER_US 0.1f
#endif

#ifndef GRAD_RASTER_US
  #define GRAD_RASTER_US 10.0f
#endif

#ifndef RF_RASTER_US
  #define RF_RASTER_US 1.0f
#endif

#ifndef BLOCK_RASTER_US
  #define BLOCK_RASTER_US 10.0f
#endif

#endif /* VENDOR_H */