/**
 * @file pulseg.h
 * @brief Public API for PulSEG library - Enhanced Pulseq with TR/segment identification.
 */
#ifndef PULSEG_H
#define PULSEG_H

#include "pulseq.h"

/**
 * @struct TRDefinition
 * @brief Definition of a TR (periodic pattern).
 */
typedef struct {
    int trId;       /**< Unique identifier for the TR pattern */
    int nBlocks;    /**< Number of blocks in this TR */
    int* blocks;    /**< Array of block IDs in this TR */
    int nSegments;  /**< Number of segments in this TR */
    int* segments;  /**< Array of segment IDs in this TR */
} TRDefinition;

/**
 * @struct SegmentDefinition
 * @brief Definition of a segment within a TR.
 */
typedef struct {
    int* blockIDs;  /**< Array of block IDs in the segment */
    int nBlocks;    /**< Number of blocks in the segment */
    int navFlag;    /**< Navigation flag (from labelset.nav) */
} SegmentDefinition;

/**
 * @struct pulseg_SeqFile
 * @brief Enhanced sequence file with TR and segment identification.
 */
typedef struct {
    /* Original and unique sequences */
    pulseq_SeqFile* seq;        /**< Original Pulseq sequence */
    pulseq_SeqFile* uniqueSeq;  /**< Deduplicated Pulseq sequence */
    
    /* TR information */
    int* trid;                  /**< TR ID array (non-zero at TR starts) */
    TRDefinition* trDefs;       /**< Array of TR definitions */
    int nTrDefs;                /**< Number of TR definitions */
    
    /* Segment information */
    SegmentDefinition* segments; /**< Array of segment definitions */
    int nSegments;               /**< Number of segments */
    
    /* Block information */
    pulseq_SeqBlock** uniqueBlocks; /**< Array of unique blocks */
    int nUniqueBlocks;              /**< Number of unique blocks */
    
    /* Iterator state */
    int currentBlock;           /**< Current block index (n) */
    int currentTrId;            /**< Current TR ID */
    int currentSegmentId;       /**< Current segment ID */
    int trCursor;               /**< Position within current TR */
    int segmentCursor;          /**< Position within current segment */
    int loopPosition;           /**< Position within seq->blockLibrary */
    
} pulseg_SeqFile;

#endif /* PULSEG_H */
