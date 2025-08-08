/**
 * @file check_sequence.h
 * @brief Functions for validating pulse sequence files.
 */
#ifndef CHECK_SEQUENCE_H
#define CHECK_SEQUENCE_H

#include "seqfile.h"
#include "ui_config.h"

/**
 * @brief Check if a sequence file is valid (correct version and valid signature).
 * 
 * This function checks if:
 * 1. The file exists and can be opened
 * 2. The version is supported
 * 3. The signature in the file matches the expected signature
 * 
 * @param filePath Path to the sequence file to check.
 * @return int 1 if the file is valid, 0 otherwise.
 */
int checkSeqFile(const char* filePath);

/**
 * @brief Configure UI based on sequence file content.
 * 
 * This function:
 * 1. Checks if the file has soft delays defined
 * 2. Configures the UI accordingly
 * 
 * @param config Pointer to UI configuration to update.
 * @param filePath Path to the sequence file.
 */
void configureUI(UIConfig* config, const char* filePath);

#endif /* CHECK_SEQUENCE_H */
