// Copyright(c) 2023 rehans.

#ifndef __asl_wav_file_reader__
#define __asl_wav_file_reader__

#include <cstring>
#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace asl {
static const uint32_t CHUNK_ID_SIZE              = 4;
static const char RIFF_CHUNK_ID[CHUNK_ID_SIZE]   = {'R', 'I', 'F', 'F'};
static const char WAVE_TYPE[CHUNK_ID_SIZE]       = {'W', 'A', 'V', 'E'};
static const char FORMAT_CHUNK_ID[CHUNK_ID_SIZE] = {'f', 'm', 't', ' '};
static const char DATA_CHUNK_ID[CHUNK_ID_SIZE]   = {'d', 'a', 't', 'a'};

static const uint16_t sizeof_format_chunk = 16; //! by specification 16 bytes

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
/*!	@fn read_wave_stream
    @brief reads an audio file stream of type wav
    @param istream An open in stream
    @param wave Result of the reading
*/
//------------------------------------------------------------------------
void read_wave_stream(std::istream& istream, WaveChunk& wave)
{
    char chunk_ID[CHUNK_ID_SIZE];
    memset(chunk_ID, 0, sizeof(chunk_ID));
    istream.read(chunk_ID, sizeof(chunk_ID));
    if (std::strncmp(chunk_ID, RIFF_CHUNK_ID, sizeof(chunk_ID)) == 0)
    {
        istream.read(reinterpret_cast<char*>(&wave.chunk_size),
                     sizeof(wave.chunk_size));
        istream.read(wave.riff_type, sizeof(wave.riff_type));
        while (istream.good())
        {
            istream.read(chunk_ID, sizeof(chunk_ID));
            if (std::strncmp(chunk_ID, FORMAT_CHUNK_ID, sizeof(chunk_ID)) == 0)
            {
                istream.read(reinterpret_cast<char*>(&wave.format),
                             sizeof(WaveChunk::FormatChunk));
                if (wave.format.chunk_size > sizeof_format_chunk)
                {
                    istream.ignore(wave.format.chunk_size -
                                   sizeof_format_chunk);
                }
            }
            else if (std::strncmp(chunk_ID, DATA_CHUNK_ID, sizeof(chunk_ID)) ==
                     0)
            {
                istream.read(reinterpret_cast<char*>(&wave.data.chunk_size),
                             sizeof(wave.data.chunk_size));
                wave.data.buffer.resize(wave.data.chunk_size);
                istream.read(reinterpret_cast<char*>(&wave.data.buffer[0]),
                             wave.data.chunk_size);
            }
            else
            {
                uint32_t chunk_size = 0;
                istream.read(reinterpret_cast<char*>(&chunk_size),
                             sizeof(chunk_size));
                istream.ignore(chunk_size);
            }
        }
    }
}

//------------------------------------------------------------------------
/*!	@fn read_wave_file
    @brief reads an audio file of type wav
    @param file_name Name of the file to load
    @param wave Result of the reading
*/
//------------------------------------------------------------------------
void read_wave_file(const char* file_name, WaveChunk& wave)
{
    std::ifstream file_stream;
    file_stream.open(file_name, std::ifstream::binary);
    if (!file_stream.is_open())
        return;

    read_wave_stream(file_stream, wave);
    file_stream.close();
}

//------------------------------------------------------------------------
/*!	@fn write_wave_stream
    @brief writes a wave chunk to stream
    @param ostream An open out stream
    @param wave Chunk to be written
*/
//------------------------------------------------------------------------
void write_wave_stream(std::ostream& ostream, const WaveChunk& wave)
{
    //!	RIFF
    ostream.write(RIFF_CHUNK_ID, sizeof(RIFF_CHUNK_ID));
    ostream.write(reinterpret_cast<const char*>(&wave.chunk_size),
                  sizeof(wave.chunk_size));
    ostream.write(WAVE_TYPE, sizeof(WAVE_TYPE));

    //! fmt
    ostream.write(FORMAT_CHUNK_ID, sizeof(FORMAT_CHUNK_ID));
    ostream.write(reinterpret_cast<const char*>(&wave.format),
                  sizeof(wave.format));

    //! data
    ostream.write(DATA_CHUNK_ID, sizeof(DATA_CHUNK_ID));
    ostream.write(reinterpret_cast<const char*>(&wave.data.chunk_size),
                  sizeof(wave.data.chunk_size));
    if (wave.data.buffer.size() > 0)
        ostream.write(reinterpret_cast<const char*>(&wave.data.buffer[0]),
                      wave.data.buffer.size());
}

//------------------------------------------------------------------------
/*!	@fn write_wave_file
    @brief writes a wave chunk to file
    @param file_name Name of the file to write
    @param wave Chunk to be written
*/
//------------------------------------------------------------------------
void write_wave_file(const char* file_name, const WaveChunk& wave)
{
    std::ofstream file_stream;
    file_stream.open(file_name, std::ifstream::binary);
    if (!file_stream.is_open())
        return;

    write_wave_stream(file_stream, wave);
    file_stream.close();
}

//------------------------------------------------------------------------
} // namespace asl

#endif // __asl_wav_file_reader__