/**
 * @file label.c
 * @brief Implementation of functions for handling pulse sequence label limits.
 */
#include <limits.h>
#include "label.h"

/**
 * @brief Initialize label limits to default values (min=INT_MAX, max=INT_MIN).
 * 
 * @param[out] limits Pointer to LabelLimits structure to initialize.
 */
void initLabelLimits(LabelLimits* limits) {
    if (!limits) return;
    
    /* Initialize all limits with min=INT_MAX, max=INT_MIN */
    limits->slc.min = INT_MAX;
    limits->slc.max = INT_MIN;
    
    limits->phs.min = INT_MAX;
    limits->phs.max = INT_MIN;
    
    limits->rep.min = INT_MAX;
    limits->rep.max = INT_MIN;
    
    limits->avg.min = INT_MAX;
    limits->avg.max = INT_MIN;
    
    limits->seg.min = INT_MAX;
    limits->seg.max = INT_MIN;
    
    limits->set.min = INT_MAX;
    limits->set.max = INT_MIN;
    
    limits->eco.min = INT_MAX;
    limits->eco.max = INT_MIN;
    
    limits->par.min = INT_MAX;
    limits->par.max = INT_MIN;
    
    limits->lin.min = INT_MAX;
    limits->lin.max = INT_MIN;
    
    limits->acq.min = INT_MAX;
    limits->acq.max = INT_MIN;
}

/**
 * @brief Update a specific label limit with a new value.
 * 
 * @param[in,out] limit Pointer to LabelLimit structure to update.
 * @param[in] value The label value to consider.
 */
static void updateLimit(LabelLimit* limit, int value) {
    if (!limit) return;
    
    /* Update min/max if value is valid (not INT_MIN) */
    if (value != INT_MIN) {
        if (value < limit->min) limit->min = value;
        if (value > limit->max) limit->max = value;
    }
}

/**
 * @brief Update label limits based on a label event.
 * 
 * @param[in,out] limits Pointer to LabelLimits structure to update.
 * @param[in] label The label event containing values to consider.
 */
void updateLabelLimits(LabelLimits* limits, const LabelEvent* label) {
    if (!limits || !label) return;
    
    /* Update each limit type with the corresponding value from the label event */
    updateLimit(&limits->slc, label->slc);
    updateLimit(&limits->phs, label->phs);
    updateLimit(&limits->rep, label->rep);
    updateLimit(&limits->avg, label->avg);
    updateLimit(&limits->seg, label->seg);
    updateLimit(&limits->set, label->set);
    updateLimit(&limits->eco, label->eco);
    updateLimit(&limits->par, label->par);
    updateLimit(&limits->lin, label->lin);
    updateLimit(&limits->acq, label->acq);
}
