// Copyright(c) 2023 rehans.

#pragma once

#include <stdint.h>
#include <vector>

namespace asl {

//------------------------------------------------------------------------
static const uint32_t CHUNK_ID_SIZE              = 4;
static const char RIFF_CHUNK_ID[CHUNK_ID_SIZE]   = {'R', 'I', 'F', 'F'};
static const char WAVE_TYPE[CHUNK_ID_SIZE]       = {'W', 'A', 'V', 'E'};
static const char FORMAT_CHUNK_ID[CHUNK_ID_SIZE] = {'f', 'm', 't', ' '};
static const char DATA_CHUNK_ID[CHUNK_ID_SIZE]   = {'d', 'a', 't', 'a'};

static const uint16_t SIZEOF_FORMAT_CHUNK = 16; //! by specification 16 bytes

//------------------------------------------------------------------------
/*!	@struct riff_chunk
    @brief Structure of the RIFF WAVE file format

    WAVE requires two sub chunks "fmt " and "data".
*/
//------------------------------------------------------------------------
struct WaveChunk
{
    uint32_t chunk_size;           //! WaveChunk size
    char riff_type[CHUNK_ID_SIZE]; //! riff type e.g. "WAVE" in this case

    struct FormatChunk
    {
        uint32_t chunk_size;      //! FormatChunk size is 16 bytes by
                                  //! specification for PCM
        uint16_t audio_format;    //! audio format e.g. 0x0001 is PCM
        uint16_t channel_count;   //! number of channels
        uint32_t sample_rate;     //! the sample rate e.g. 44100
        uint32_t byte_rate;       //! TODO
        uint16_t block_align;     //! TODO
        uint16_t bits_per_sample; //! bits per sample e.g. 16
        //! There can be some more in case it is not PCM
    } format;

    struct DataChunk
    {
        uint32_t chunk_size;         //! DataChunk size
        std::vector<uint8_t> buffer; //! buffer with e.g. PCM data
    } data;
};

//------------------------------------------------------------------------
} // namespace asl
