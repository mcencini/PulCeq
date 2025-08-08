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

/** 
  * Raster units for RF, Grad, ADC and Blocks
  * 
  * We use Siemens as defaults (1us, 10us, 0.1us, 10us)
  *
*/
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

/** 
  * Label support customization
  * 
  * By default all labels are supported, but vendors can disable specific labels
  * by defining macros like LABEL_SUPPORT_SLC as 0
  * 
  * Example for vendor-specific header:
  *
  * // Only support SLC, LIN, and PAR labels
  * #define LABEL_SUPPORT_SEG 0
  * #define LABEL_SUPPORT_REP 0
  * #define LABEL_SUPPORT_AVG 0
  * #define LABEL_SUPPORT_SET 0
  * #define LABEL_SUPPORT_ECO 0
  * #define LABEL_SUPPORT_PHS 0
  * #define LABEL_SUPPORT_ACQ 0
*/
#ifndef LABEL_SUPPORT_SLC
    #define LABEL_SUPPORT_SLC 1
#endif

#ifndef LABEL_SUPPORT_SEG
    #define LABEL_SUPPORT_SEG 1
#endif

#ifndef LABEL_SUPPORT_REP
    #define LABEL_SUPPORT_REP 1
#endif

#ifndef LABEL_SUPPORT_AVG
    #define LABEL_SUPPORT_AVG 1
#endif

#ifndef LABEL_SUPPORT_SET
    #define LABEL_SUPPORT_SET 1
#endif

#ifndef LABEL_SUPPORT_ECO
    #define LABEL_SUPPORT_ECO 1
#endif

#ifndef LABEL_SUPPORT_PHS
    #define LABEL_SUPPORT_PHS 1
#endif

#ifndef LABEL_SUPPORT_LIN
    #define LABEL_SUPPORT_LIN 1
#endif

#ifndef LABEL_SUPPORT_PAR
    #define LABEL_SUPPORT_PAR 1
#endif

#ifndef LABEL_SUPPORT_ACQ
    #define LABEL_SUPPORT_ACQ 1
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

/**
  * Rotation format configuration
  * 
  * ROTATION_FORMAT_QUATERNION: Use quaternions (Siemens format)
  * ROTATION_FORMAT_MATRIX: Use 3x3 rotation matrices (flattened to float[9])
  * 
  * Default is QUATERNION for Siemens compatibility
*/
#ifndef ROTATION_FORMAT
  #define ROTATION_FORMAT ROTATION_FORMAT_QUATERNION
#endif

#ifndef ROTATION_FORMAT_QUATERNION
  #define ROTATION_FORMAT_QUATERNION 0
#endif

#ifndef ROTATION_FORMAT_MATRIX
  #define ROTATION_FORMAT_MATRIX 1
#endif

#endif /* VENDOR_H */