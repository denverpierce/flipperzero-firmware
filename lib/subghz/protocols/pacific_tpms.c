#include "pacific_tpms.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocol_Pacific_TPMS"

static const SubGhzBlockConst subghz_protocol_pacific_tpms_const = {
    .te_short = 100,
    .te_long = 100,
    .te_delta = 50,
    .min_count_bit_for_found = 67,
};

struct SubGhzProtocolDecoderPacificTpms {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
};

struct SubGhzProtocolEncoderPacificTpms {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    PacificTpmsDecoderStepReset = 0,
    PacificTpmsDecoderStepFoundPreambula,
    PacificTpmsDecoderStepSaveDuration,
    PacificTpmsDecoderStepCheckDuration,
} PacificTpmsDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_pacific_tpms_decoder = {
    .alloc = subghz_protocol_decoder_pacific_tpms_alloc,
    .free = subghz_protocol_decoder_pacific_tpms_free,

    .feed = subghz_protocol_decoder_pacific_tpms_feed,
    .reset = subghz_protocol_decoder_pacific_tpms_reset,

    .get_hash_data = subghz_protocol_decoder_pacific_tpms_get_hash_data,
    .deserialize = subghz_protocol_decoder_pacific_tpms_deserialize,
    .serialize = subghz_protocol_decoder_pacific_tpms_serialize,
    .get_string = subghz_protocol_decoder_pacific_tpms_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_pacific_tpms_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol subghz_protocol_pacific_tpms = {
    .name = SUBGHZ_PROTOCOL_PACIFIC_TPMS_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable,

    .decoder = &subghz_protocol_pacific_tpms_decoder,
    .encoder = &subghz_protocol_pacific_tpms_encoder,
};

void* subghz_protocol_decoder_pacific_tpms_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderPacificTpms* instance = malloc(sizeof(SubGhzProtocolDecoderPacificTpms));
    instance->base.protocol = &subghz_protocol_pacific_tpms;
    instance->generic.protocol_name = instance->base.protocol->name;

    return instance;
}

void subghz_protocol_decoder_pacific_tpms_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;
    free(instance);
}

void subghz_protocol_decoder_pacific_tpms_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;
    instance->decoder.parser_step = PacificTpmsDecoderStepReset;
}

void subghz_protocol_decoder_pacific_tpms_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;

    switch(instance->decoder.parser_step) {
    case PacificTpmsDecoderStepReset:
        if((level) && (DURATION_DIFF(duration, subghz_protocol_pacific_tpms_const.te_short * 10) <
                       subghz_protocol_pacific_tpms_const.te_delta * 5)) {
            instance->decoder.parser_step = PacificTpmsDecoderStepFoundPreambula;
        }
        break;
    case PacificTpmsDecoderStepFoundPreambula:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_pacific_tpms_const.te_short * 10) <
                        subghz_protocol_pacific_tpms_const.te_delta * 5)) {
            //Found Preambula
            instance->decoder.parser_step = PacificTpmsDecoderStepSaveDuration;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        } else {
            instance->decoder.parser_step = PacificTpmsDecoderStepReset;
        }
        break;
    case PacificTpmsDecoderStepSaveDuration:
        if(level) {
            if(duration >=
               (subghz_protocol_pacific_tpms_const.te_short * 5 + subghz_protocol_pacific_tpms_const.te_delta)) {
                instance->decoder.parser_step = PacificTpmsDecoderStepFoundPreambula;
                if(instance->decoder.decode_count_bit >=
                   subghz_protocol_pacific_tpms_const.min_count_bit_for_found) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                break;
            } else {
                instance->decoder.te_last = duration;
                instance->decoder.parser_step = PacificTpmsDecoderStepCheckDuration;
            }

        } else {
            instance->decoder.parser_step = PacificTpmsDecoderStepReset;
        }
        break;
    case PacificTpmsDecoderStepCheckDuration:
        if(!level) {
            if((DURATION_DIFF(instance->decoder.te_last, subghz_protocol_pacific_tpms_const.te_short) <
                subghz_protocol_pacific_tpms_const.te_delta) &&
               (DURATION_DIFF(duration, subghz_protocol_pacific_tpms_const.te_long) <
                subghz_protocol_pacific_tpms_const.te_delta * 3)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = PacificTpmsDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_pacific_tpms_const.te_short) <
                 subghz_protocol_pacific_tpms_const.te_delta * 3) &&
                (DURATION_DIFF(duration, subghz_protocol_pacific_tpms_const.te_short) <
                 subghz_protocol_pacific_tpms_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = PacificTpmsDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = PacificTpmsDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = PacificTpmsDecoderStepReset;
        }
        break;
    }
}

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 */
static void subghz_protocol_pacific_tpms_check_remote_controller(SubGhzBlockGeneric* instance) {
    uint64_t code_found_reverse =
        subghz_protocol_blocks_reverse_key(instance->data, instance->data_count_bit);
    uint32_t code_fix = code_found_reverse & 0xFFFFFF;

    instance->serial = code_fix & 0xFFFFF;
    instance->btn = (code_fix >> 20) & 0x0F;
}

uint8_t subghz_protocol_decoder_pacific_tpms_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

bool subghz_protocol_decoder_pacific_tpms_serialize(
    void* context,
    FlipperFormat* flipper_format,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, frequency, preset);
}

bool subghz_protocol_decoder_pacific_tpms_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;
    return subghz_block_generic_deserialize(&instance->generic, flipper_format);
}

void subghz_protocol_decoder_pacific_tpms_get_string(void* context, string_t output) {
    furi_assert(context);
    SubGhzProtocolDecoderPacificTpms* instance = context;

    subghz_protocol_pacific_tpms_check_remote_controller(&instance->generic);
    uint64_t code_found_reverse = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);
    uint32_t code_fix = code_found_reverse & 0xFFFFFF;
    uint32_t code_hop = (code_found_reverse >> 24) & 0xFFFFFF;

    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%lX%08lX\r\n"
        "Fix:%06lX \r\n"
        "Hop:%06lX \r\n"
        "Sn:%05lX Btn:%lX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (uint32_t)(instance->generic.data >> 32),
        (uint32_t)instance->generic.data,
        code_fix,
        code_hop,
        instance->generic.serial,
        instance->generic.btn);
}
