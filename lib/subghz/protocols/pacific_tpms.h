#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_PACIFIC_TPMS_NAME "Pacific TPMS"

typedef struct SubGhzProtocolDecoderPacificTpms SubGhzProtocolDecoderPacificTpms;
typedef struct SubGhzProtocolEncoderPacificTpms SubGhzProtocolEncoderPacificTpms;

extern const SubGhzProtocolDecoder subghz_protocol_pacific_tpms_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_pacific_tpms_encoder;
extern const SubGhzProtocol subghz_protocol_pacific_tpms;

/**
 * Allocate SubGhzProtocolDecoderPacificTpms.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderPacificTpms* pointer to a SubGhzProtocolDecoderPacificTpms instance
 */
void* subghz_protocol_decoder_pacific_tpms_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderPacificTpms.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 */
void subghz_protocol_decoder_pacific_tpms_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderPacificTpms.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 */
void subghz_protocol_decoder_pacific_tpms_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_pacific_tpms_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_pacific_tpms_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderPacificTpms.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param frequency The frequency at which the signal was received, Hz
 * @param preset The modulation on which the signal was received, FuriHalSubGhzPreset
 * @return true On success
 */
bool subghz_protocol_decoder_pacific_tpms_serialize(
    void* context,
    FlipperFormat* flipper_format,
    uint32_t frequency,
    FuriHalSubGhzPreset preset);

/**
 * Deserialize data SubGhzProtocolDecoderPacificTpms.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_pacific_tpms_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderPacificTpms instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_pacific_tpms_get_string(void* context, string_t output);
