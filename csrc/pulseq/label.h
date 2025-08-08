/**
 * @file label.h
 * @brief Defines structures and functions for handling pulse sequence label limits.
 */
#ifndef LABEL_H
#define LABEL_H

#include "event.h"

/**
 * @brief Limits for a single label type.
 */
typedef struct {
    int min;     /**< Minimum value for this label type */
    int max;     /**< Maximum value for this label type */
} LabelLimit;

/**
 * @brief Limits for all label types.
 */
typedef struct {
    LabelLimit slc;    /**< Slice label limits */
    LabelLimit phs;    /**< Phase label limits */
    LabelLimit rep;    /**< Repetition label limits */
    LabelLimit avg;    /**< Average label limits */
    LabelLimit seg;    /**< Segment label limits */
    LabelLimit set;    /**< Set label limits */
    LabelLimit eco;    /**< Echo label limits */
    LabelLimit par;    /**< Partition label limits */
    LabelLimit lin;    /**< Line label limits */
    LabelLimit acq;    /**< Acquisition label limits */
} LabelLimits;

/**
 * @brief Initialize label limits to default values (min=INT_MAX, max=INT_MIN).
 * 
 * @param[out] limits Pointer to LabelLimits structure to initialize.
 */
void initLabelLimits(LabelLimits* limits);

/**
 * @brief Update label limits based on a label event.
 * 
 * @param[in,out] limits Pointer to LabelLimits structure to update.
 * @param[in] label The label event containing values to consider.
 */
void updateLabelLimits(LabelLimits* limits, const LabelEvent* label);

#endif /* LABEL_H */
