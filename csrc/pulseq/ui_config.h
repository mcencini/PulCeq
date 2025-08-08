/**
 * @file ui_config.h
 * @brief Defines structures for vendor UI configuration.
 */
#ifndef UI_CONFIG_H
#define UI_CONFIG_H

/**
 * @brief Simplified UI configuration for enabling/disabling features.
 */
typedef struct {
    int enableSoftDelayUI;    /**< Flag to enable soft delay UI elements */
} UIConfig;

/**
 * @brief Initialize UI configuration with default values.
 * 
 * @param[out] config Pointer to UIConfig structure to initialize.
 */
void initUIConfig(UIConfig* config);

#endif /* UI_CONFIG_H */
