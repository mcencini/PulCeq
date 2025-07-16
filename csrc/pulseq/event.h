/**
 * @file event.h
 * @brief Public API for defining and manipulating Pulseq events.
 *
 * This header declares data structures and functions used for handling
 * RF, gradient, ADC, and trigger events, as well as shape decompression.
 * It forms part of the public API for pulSeg-compliant sequence design.
 */
#ifndef EVENT_H
#define EVENT_H

/****************************************************************/ 
/*            Structs defining normalized shapes                */
/****************************************************************/ 
/** @struct ShapeArbitrary
   * @brief  Arbitrary shape struct
   *
   *  @var ShapeArbitrary::numSamples
   *    Number of uncompressed waveform samples.
   *  @var ShapeArbitrary::numSamples
   *    Number of waveform samples.
   *  @var ShapeArbitrary::samples
   *    Array of waveform samples.
   */
typedef struct {
    int numUncompressedSamples; /**< @brief Number of uncompressed waveform samples */
    int numSamples;             /**< @brief Number of waveform samples */
    float *samples;             /**< @brief Waveform samples */
} ShapeArbitrary; /* mirrors Pulseq CompressedShape */

/**
 * @brief Decompresses a ShapeArbitrary waveform if needed.
 *
 * This function decompresses a run-length compressed waveform (Pulseq shape format).
 * If no decompression is needed (i.e., nSamples == nUncompressedSamples), it simply returns the input.
 * Otherwise, it allocates and returns a new ShapeArbitrary with the fully decompressed waveform.
 *
 * @param[in] encoded Pointer to the compressed ShapeArbitrary.
 * @return Pointer to decompressed ShapeArbitrary. 
 *         May be the same as `encoded` if no decompression was needed.
 *         If decompressed, caller is responsible for freeing both the struct and its `samples` field.
 */
ShapeArbitrary* decompressShape(const ShapeArbitrary* encoded);

/** @struct ShapeTrap
   * @brief  Trapzoid shape struct
   *
   *  @var ShapeTrap::riseTime
   *    Ramp up time of trapezoid (us).
   *  @var ShapeTrap::flatTime
   *    Flat-top time of trapezoid (us).
   *  @var ShapeTrap::fallTime
   *    Ramp down time of trapezoid (us).
   */
typedef struct {
    long riseTime; /**< @brief Ramp up time of trapezoid (us)  */
    long flatTime; /**< @brief Flat-top time of trapezoid (us)  */
    long fallTime; /**< @brief Ramp down time of trapezoid (us) */
} ShapeTrap; /* no Pulseq equivalent */

/****************************************************************/ 
/*                  Pulseq event structs                        */
/****************************************************************/
/** @struct RFEvent
   * @brief  RF event
   *
   *  @var RFEvent::type
   *    Whether RF is NULL (0) or defined (1).
   *  @var RFEvent::amplitude
   *    Peak magnitude of magShape (Hz).
   *  @var RFEvent::magShape
   *    Magnitude waveform shape.
   *  @var RFEvent::phaseShape
   *    Phase waveform shape (for complex-valued RF pulses).
   *  @var RFEvent::timeShape
   *    Timepoints for RF waveform shape (for irregular raster).
   *  @var RFEvent::center
   *    Effective RF center of the pulse shape measured from the start of the shape (us).
   *  @var RFEvent::freqPPM
   *    B0-dependent frequency offset of transmitter (ppm).
   *  @var RFEvent::phasePPM
   *    B0-dependent phase offset of transmitter (rad/MHz).
   *  @var RFEvent::freqOffset
   *    Frequency offset of transmitter (Hz).
   *  @var RFEvent::phaseOffset
   *    Phase offset of transmitter (rad).
   *  @var RFEvent::delay
   *    Delay prior to the pulse (us).
   * @var RFEvent::use 
   *    Single character indicating the intended use of the pulse, e.g. e,r,etc...
   */
typedef struct {
    short type;                /**< @brief NULL or ARBITRARY */    
    float amplitude;           /**< @brief Peak magnitude of magShape (Hz) */
    ShapeArbitrary magShape;   /**< @brief Arbitrary waveform, unitary peak amplitude */
    ShapeArbitrary phaseShape; /**< @brief Abitrary waveform */
    ShapeArbitrary timeShape;  /**< @brief Arbitrary waveform */
    float center;              /**< @brief Effective RF center of the pulse shape measured from the start of the shape (us) */
    float freqPPM;             /**< @brief B0-dependent frequency offset of transmitter (ppm) */
    float phasePPM;            /**< @brief B0-dependent phase offset of transmitter (rad/MHz) */
    float freqOffset;          /**< @brief Frequency offset of transmitter (Hz) */
	float phaseOffset;         /**< @brief Phase offset of transmitter (rad) */
    int delay;                 /**< @brief Delay prior to the pulse (us) */
    char use;                  /**< @brief Single character indicating the intended use of the pulse, e.g. e,r,etc... */
} RFEvent; /* mirrors Pulseq RFEvent */

/** @struct PulseqGrad
   * @brief  Gradient event. 
   *
   *  @var GradEvent::type
   *    Whether gradient is NULL (0), TRAP (1) or ARBITRARY (2).
   *  @var GradEvent::amplitude
   *    Peak amplitude of the gradient (Hz/m).
   *  @var GradEvent::delay
   *    Delay prior to the gradient (us).
   *  @var GradEvent::trap
   *    Trapezoid shape (for type == 1).
   *  @var GradEvent::waveShape
   *    Gradient waveform shape (for type == 2).
   *  @var GradEvent::timeShape
   *    Timepoints for Gradient waveform shape (for type == 2).
   *  @var GradEvent::first
   *    Amplitude at the start of the shape (for type == 2).
   *  @var GradEvent::last
   *    Amplitude at the end of the shape(for type == 2).
   */
typedef struct {
    short type;               /**< @brief NULL, TRAP, or ARBITRARY */  
    float amplitude;          /**< @brief Peak amplitude of the gradient (Hz/m) */
    int delay;                /**< @brief Delay prior to the gradient (us) */
    ShapeTrap trap;           /**< @brief Trapezoid, unitary plateau amplitude */
    ShapeArbitrary waveShape; /**< @brief Arbitrary waveform, unitary peak amplitude */
    ShapeArbitrary timeShape; /**< @brief Arbitrary waveform */
    float first;              /**< @brief Amplitude at the start of the shape for arbitrary gradient */
    float last;               /**< @brief Amplitude at the end of the shape for arbitrary gradient */
} GradEvent; /* mirrors Pulseq GradEvent */

/** @struct ADCEvent
   * @brief  ADC event. 
   *
   *  @var ADCEvent::type
   *    Whether ADC is NULL (0) or defined (1).
   *  @var ADCEvent::numSamples
   *    Number of ADC samples.
   *  @var ADCEvent::dwellTime
   *    Dwell time of ADC readout (ns).
   *  @var ADCEvent::delay
   *    Delay before first sample (us).
   *  @var ADCEvent::freqPPM
   *    B0-dependent frequency offset of receiver (ppm).
   *  @var ADCEvent::phasePPM
   *    B0-dependent phase offset of receiver (rad/MHz).
   *  @var ADCEvent::freqOffset
   *    Frequency offset of receiver (Hz).
   *  @var ADCEvent::phaseOffset
   *    Phase offset of receiver (rad).
   *  @var ADCEvent::phaseModulationShape
   *    Phase modulation shape of receiver (rad).
   */
typedef struct {
    short type;                          /**< @brief NULL or ADC */
    int numSamples;                      /**< @brief Number of ADC samples */
    int dwellTime;                       /**< @brief Dwell time of ADC readout (ns) */
    int delay;                           /**< @brief Delay before first sample (us) */
    float freqPPM;                       /**< @brief B0-dependent frequency offset of receiver (ppm) */
    float phasePPM;                      /**< @brief B0-dependent phase offset of receiver (rad/MHz) */
    float freqOffset;                    /**< @brief Frequency offset of receiver (Hz) */
	float phaseOffset;                   /**< @brief Phase offset of receiver (rad) */
    ShapeArbitrary phaseModulationShape; /**< @brief Phase modulation shape of receiver (rad) */
} ADCEvent; /* mirrors Pulseq ADCEvent */

/** @struct TriggerEvent
   * @brief  Trigger event. 
   *
   *  @var TriggerEvent::type
   *    Whether trigger is OFF (0) or ON (1).
   *  @var TriggerEvent::duration
   *    Duration of trigger event (us).
   *  @var TriggerEvent::delay
   *    Delay prior to the trigger event (us).
   *  @var TriggerEvent::triggerType
   *    Type of trigger (system dependent). 0: undefined / unused.
   *  @var TriggerEvent::triggerChannel
   *    Channel of trigger (system dependent). 0: undefined / unused.
   */
typedef struct {
    short type;         /**< @brief OFF or ON */
    long duration;      /**< @brief Duration of trigger event (us) */
    long delay;         /**< @brief Delay prior to the trigger event (us) */
    int triggerType;    /**< @brief Type of trigger (system dependent). 0: undefined / unused */
    int triggerChannel; /**< @brief Channel of trigger (system dependent). 0: undefined / unused */
} TriggerEvent; /* mirrors Pulseq TriggerEvent */

/** @struct RotationEvent
   * @brief  Rotation event. 
   *
   *  @var RotationEvent::type
   *    Whether rotation is NULL (0) or DEFINED (1).
   *  @var RotationEvent::rotQuaternion
   *    Gradient rotation quaternion.
   */
typedef struct {
    short type;        /**< @brief NULL or DEFINED */
    double rotQuaternion; /**< @brief Gradient rotation quaternion */
} RotationEvent; /* mirrors Pulseq RotationEvent */

#define SOFT_DELAY_HINT_LENGTH 32
/** @struct SoftDelayEvent
   * @brief  Soft Delay event. 
   *
   *  @var SoftDelayEvent::type
   *    Whether delay is NULL (0) or DEFINED (1).
   *  @var SoftDelayEvent::numID
   *    Numeric index of the soft delay to help the intepreter (together with the hint string) to identify the delay and allocate it to the UI element.
   *  @var SoftDelayEvent::offset
   *    Offset (positive or negative) added to the delay after the division by the factor (us).
   *  @var SoftDelayEvent::factor
   *    Factor by which the value on the user interface needs to be divided for calculating the final delay applied to the sequence.
   *  @var SoftDelayEvent::hint
   *    Text hint corresponding to this soft delay, e.g. TE.
   */
typedef struct {
    short type;                        /**< @brief NULL or DEFINED */
    int numID;                         /**< @brief Numeric index of the soft delay to help the intepreter (together with the hint string) to identify the delay and allocate it to the UI element */
    int offset;                        /**< @brief Offset (positive or negative) added to the delay after the division by the factor (us) */
    int factor;                        /**< @brief Factor by which the value on the user interface needs to be divided for calculating the final delay applied to the sequence */
    char hint[SOFT_DELAY_HINT_LENGTH]; /**< @brief Text hint corresponding to this soft delay, e.g. TE */
} SoftDelayEvent; /* mirrors Pulseq SoftDelayEvent */

/** @struct RfShimmingEvent
   * @brief  RF Shimming event. 
   *
   *  @var RfShimmingEvent::type
   *    Whether rf shimming is NULL (0) or DEFINED (1).
   *  @var RfShimmingEvent::ID
   *    Unique ID of the RF shimming object.
   *  @var RfShimmingEvent::nChan
   *    Number of RF channels.
   *  @var RfShimmingEvent::amplitudes
   *    Amplitude scaling factor for each channel.
   *  @var RfShimmingEvent::phases
   *    Additional phase for each channel.
   */
typedef struct {
    short type;        /**< @brief NULL or DEFINED */
    int ID;            /**< @brief Unique ID of the RF shimming object */
    int nChan;         /**< @brief Number of RF channels */
    float* amplitudes; /**< @brief Amplitude scaling factor for each channel */
    float* phases;     /**< @brief Additional phase for each channel */
} RfShimmingEvent; /* mirrors Pulseq RfShimmingEvent */

#endif /* EVENT_H */