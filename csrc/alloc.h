/**
 * @file alloc.h
 * @brief Customizable memory allocation macros.
 *
 */
#ifndef ALLOC_H
#define ALLOC_H

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

#endif /* ALLOC_H */