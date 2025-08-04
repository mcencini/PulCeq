/**
 * @file segmenter.c
 * @brief Segment extraction and deduplication for Pulseq TRs.
 */

#include <string.h>

#include "../../pulseq/alloc.h"

#include "segmenter.h"

/* Helper function to compare segments for equality */
static int segment_equal(const SegmentDefinition* a, const SegmentDefinition* b)
{
    int i;
    if (a->n_blocks != b->n_blocks) return 0;
    if (a->nav_flag != b->nav_flag) return 0;
    for (i = 0; i < a->n_blocks; i++) {
        if (a->blockIDs[i] != b->blockIDs[i]) return 0;
    }
    return 1;
}

/**
 * @brief Extracts and deduplicates segments from TR definitions.
 */
SegmentResults* extractSegments(const pulseq_SeqFile* seq, const SequencePatterns* patterns)
{
    int i, j, k, tr, seg_start, seg_end, seg_idx, blockID, nav_flag, last_grad, adc_found, merge_count;
    int total_blocks, max_segments, n_unique;
    SegmentDefinition* segments;
    int* blockToSegment;
    int* unique_map;
    SegmentDefinition* uniqueSegments;
    SegmentResults* results;
    TRDefinition* tr_def;
    pulseq_SeqBlock* block;

    total_blocks = patterns->n_blocks;
    max_segments = total_blocks;
    segments = (SegmentDefinition*)ALLOC(sizeof(SegmentDefinition) * max_segments);
    blockToSegment = (int*)ALLOC(sizeof(int) * total_blocks);
    seg_idx = 0;

    for (tr = 0; tr < patterns->n_tr_definitions; tr++) {
        tr_def = &patterns->tr_definitions[tr];
        seg_start = 0;
        while (seg_start < tr_def->n_blocks) {
            seg_end = seg_start;
            nav_flag = -1;
            adc_found = 0;
            while (seg_end < tr_def->n_blocks) {
                blockID = tr_def->blocks[seg_end];
                block = pulseq_getBlock(seq, blockID, 1);
                if (block->rf.type == 1 && seg_end != seg_start) {
                    break;
                }
                if (block->adc.type == 1) {
                    adc_found = 1;
                    last_grad = (block->gx.last != 0) || (block->gy.last != 0) || (block->gz.last != 0);
                    while (last_grad && seg_end < tr_def->n_blocks) {
                        seg_end++;
                        blockID = tr_def->blocks[seg_end];
                        block = pulseq_getBlock(seq, blockID, 1);
                        last_grad = (block->gx.last != 0) || (block->gy.last != 0) || (block->gz.last != 0);
                    }
                    if (nav_flag == -1) nav_flag = block->labelset.nav;
                    else if (nav_flag != block->labelset.nav) {
                        nav_flag = block->labelset.nav;
                    }
                    break;
                }
                seg_end++;
            }
            segments[seg_idx].n_blocks = seg_end - seg_start + 1;
            segments[seg_idx].blockIDs = (int*)ALLOC(sizeof(int) * segments[seg_idx].n_blocks);
            for (k = 0; k < segments[seg_idx].n_blocks; k++) {
                segments[seg_idx].blockIDs[k] = tr_def->blocks[seg_start + k];
            }
            segments[seg_idx].nav_flag = nav_flag;
            for (k = 0; k < segments[seg_idx].n_blocks; k++) {
                blockToSegment[tr_def->blocks[seg_start + k]] = seg_idx;
            }
            seg_idx++;
            seg_start = seg_end + 1;
        }
    }

    /* Merge adjacent nav segments */
    merge_count = 0;
    i = 1;
    while (i < seg_idx) {
        if (segments[i - 1].nav_flag == 1 && segments[i].nav_flag == 1) {
            int new_n_blocks = segments[i - 1].n_blocks + segments[i].n_blocks;
            int* new_blockIDs = (int*)ALLOC(sizeof(int) * new_n_blocks);
            memcpy(new_blockIDs, segments[i - 1].blockIDs, sizeof(int) * segments[i - 1].n_blocks);
            memcpy(new_blockIDs + segments[i - 1].n_blocks, segments[i].blockIDs, sizeof(int) * segments[i].n_blocks);
            FREE(segments[i - 1].blockIDs);
            FREE(segments[i].blockIDs);
            segments[i - 1].blockIDs = new_blockIDs;
            segments[i - 1].n_blocks = new_n_blocks;
            for (j = i; j < seg_idx - 1; j++) {
                segments[j] = segments[j + 1];
            }
            seg_idx--;
            merge_count++;
        } else {
            i++;
        }
    }

    /* Remove duplicate segments */
    unique_map = (int*)ALLOC(sizeof(int) * seg_idx);
    n_unique = 0;
    for (i = 0; i < seg_idx; i++) {
        int found = 0;
        for (j = 0; j < n_unique; j++) {
            if (segment_equal(&segments[i], &segments[unique_map[j]])) {
                found = 1;
                break;
            }
        }
        if (!found) {
            unique_map[n_unique++] = i;
        }
    }

    uniqueSegments = (SegmentDefinition*)ALLOC(sizeof(SegmentDefinition) * n_unique);
    for (i = 0; i < n_unique; i++) {
        uniqueSegments[i].n_blocks = segments[unique_map[i]].n_blocks;
        uniqueSegments[i].blockIDs = (int*)ALLOC(sizeof(int) * uniqueSegments[i].n_blocks);
        for (j = 0; j < uniqueSegments[i].n_blocks; j++) {
            uniqueSegments[i].blockIDs[j] = segments[unique_map[i]].blockIDs[j];
        }
        uniqueSegments[i].nav_flag = segments[unique_map[i]].nav_flag;
    }

    for (i = 0; i < total_blocks; i++) {
        for (j = 0; j < n_unique; j++) {
            if (segment_equal(&segments[blockToSegment[i]], &uniqueSegments[j])) {
                blockToSegment[i] = j;
                break;
            }
        }
    }

    for (i = 0; i < seg_idx; i++) {
        if (segments[i].blockIDs) FREE(segments[i].blockIDs);
    }
    FREE(segments);
    FREE(unique_map);

    results = (SegmentResults*)ALLOC(sizeof(SegmentResults));
    results->uniqueSegments = uniqueSegments;
    results->n_uniqueSegments = n_unique;
    results->blockToSegment = blockToSegment;
    return results;
}

/**
 * @brief Frees memory allocated for SegmentResults.
 */
void freeSegmentResults(SegmentResults* results)
{
    int i;
    if (!results) return;
    if (results->uniqueSegments) {
        for (i = 0; i < results->n_uniqueSegments; i++) {
            if (results->uniqueSegments[i].blockIDs) {
                FREE(results->uniqueSegments[i].blockIDs);
            }
        }
        FREE(results->uniqueSegments);
    }
    if (results->blockToSegment) {
        FREE(results->blockToSegment);
    }
    FREE(results);
}