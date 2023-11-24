// Copyright(c) 2023 rehans.

#pragma once

#include "wave_def.h"
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace asl {

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
                if (wave.format.chunk_size > SIZEOF_FORMAT_CHUNK)
                {
                    istream.ignore(wave.format.chunk_size -
                                   SIZEOF_FORMAT_CHUNK);
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
