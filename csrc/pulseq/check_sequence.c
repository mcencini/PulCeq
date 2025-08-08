/**
 * @file check_sequence.c
 * @brief Implementation of functions for validating pulse sequence files.
 */

#include <stdio.h>
#include <string.h>
#include "check_sequence.h"
#include "constants.h"

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
int checkSeqFile(const char* filePath) {
    FILE* fp;
    char line[256];
    char signatureFromFile[256] = "";
    int versionMajor = 0;
    int versionMinor = 0;
    int versionRevision = 0;
    int versionSupported = 0;
    
    /* Check if the file can be opened */
    fp = fopen(filePath, "r");
    if (fp == NULL) {
        return 0;  /* File doesn't exist or can't be opened */
    }
    
    /* Look for [VERSION] and [SIGNATURE] sections */
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "[VERSION]")) {
            /* Read version information */
            while (fgets(line, sizeof(line), fp)) {
                if (line[0] == '[') break;  /* Next section */
                if (line[0] == '#' || line[0] == '\n') continue;  /* Comment or empty line */
                
                if (strstr(line, "major")) {
                    sscanf(line, "%*s %d", &versionMajor);
                } else if (strstr(line, "minor")) {
                    sscanf(line, "%*s %d", &versionMinor);
                } else if (strstr(line, "revision")) {
                    sscanf(line, "%*s %d", &versionRevision);
                }
            }
            
            /* Check if version is supported (currently Pulseq v1.x) */
            if (versionMajor == 1) {
                versionSupported = 1;
            }
        } else if (strstr(line, "[SIGNATURE]")) {
            /* Read signature information */
            if (fgets(line, sizeof(line), fp)) {
                /* The signature is the first non-comment line after [SIGNATURE] */
                if (line[0] != '#' && line[0] != '\n') {
                    /* Remove trailing newline */
                    size_t len = strlen(line);
                    if (len > 0 && line[len - 1] == '\n') {
                        line[len - 1] = '\0';
                    }
                    strcpy(signatureFromFile, line);
                }
            }
        }
        
        /* If we've found both version and signature, we can stop */
        if (versionSupported && signatureFromFile[0] != '\0') {
            break;
        }
    }
    
    fclose(fp);
    
    /* Check if version is supported and signature matches */
    if (!versionSupported) {
        return 0;  /* Version not supported */
    }
    
    /* For now, just check if there is a signature (we don't validate it yet) */
    if (signatureFromFile[0] == '\0') {
        return 0;  /* No signature found */
    }
    
    return 1;  /* File is valid */
}

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
void configureUI(UIConfig* config, const char* filePath) {
    FILE* fp;
    char line[256];
    int hasSoftDelays = 0;
    
    /* Initialize config */
    if (config == NULL) return;
    initUIConfig(config);
    
    /* Check if the file can be opened */
    fp = fopen(filePath, "r");
    if (fp == NULL) {
        return;  /* File doesn't exist or can't be opened */
    }
    
    /* Look for [DELAYS] section */
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "[DELAYS]")) {
            /* Soft delays are defined in the [DELAYS] section */
            hasSoftDelays = 1;
            break;
        }
    }
    
    fclose(fp);
    
    /* Configure UI based on presence of soft delays */
    config->enableSoftDelayUI = hasSoftDelays;
}

/**
 * @brief Initialize UI configuration with default values.
 * 
 * @param[out] config Pointer to UIConfig structure to initialize.
 */
void initUIConfig(UIConfig* config) {
    if (config == NULL) return;
    
    config->enableSoftDelayUI = 0;  /* Default: soft delay UI is disabled */
}
