// Copyright René Hansen 2014.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//	http://www.boost.org/LICENSE_1_0.txt)

#ifndef __asl_wav_file_reader__
#define __asl_wav_file_reader__

#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace asl
{
static const char* riff_chunk_ID	= "RIFF";
static const char* wave_type		= "WAVE";
static const char* format_chunk_ID	= "fmt ";
static const char* data_chunk_ID	= "data";

static const uint16_t sizeof_format_chunk = 16; //! by specification 16 bytes

//------------------------------------------------------------------------
/*!	@struct riff_chunk
	@brief Structure of the RIFF WAVE file format

	WAVE requires two sub chunks "fmt " and "data".
*/
//------------------------------------------------------------------------
struct wave_chunk
{
	uint32_t chunk_size;	//! wave_chunk size
	char riff_type[4];		//! riff type e.g. "WAVE" in this case
	
	struct format_chunk
	{
		uint32_t chunk_size;	//! format_chunk size is 16 bytes by specification for PCM
		uint16_t audio_format;	//! audio format e.g. 0x0001 is PCM
		uint16_t channel_count;	//! number of channels
		uint32_t sample_rate;	//! the sample rate e.g. 44100
		uint32_t byte_rate;		//! TODO
		uint16_t block_align;	//! TODO
		uint16_t bits_per_sample;//! bits per sample e.g. 16
		//! There can be some more in case it is not PCM
	} format;
	
	struct data_chunk
	{
		uint32_t chunk_size;	//! data_chunk size
		std::vector<uint8_t> buffer;//! buffer with e.g. PCM data
	} data;
};

//------------------------------------------------------------------------
/*!	@fn read_wave_file
	@brief reads an audio file of type wav
	@param file_name Name of the file to load
	@param wave Result of the reading
*/
//------------------------------------------------------------------------
void read_wave_file (const char* file_name, wave_chunk& wave)
{
	std::ifstream file_stream;
	file_stream.open(file_name, std::ifstream::binary);
	if (file_stream.is_open())
	{
		char chunk_ID[4];
		memset (chunk_ID, 0, sizeof (chunk_ID));
		file_stream.read(chunk_ID, sizeof(chunk_ID));
		if (std::strncmp (chunk_ID, riff_chunk_ID, sizeof(chunk_ID)) == 0)
		{
			file_stream.read (reinterpret_cast<char*>(&wave.chunk_size), sizeof (wave.chunk_size));
			file_stream.read (wave.riff_type, sizeof (wave.riff_type));
			while (file_stream.good ())
			{
				file_stream.read(chunk_ID, sizeof(chunk_ID));
				if (std::strncmp (chunk_ID, format_chunk_ID, sizeof(chunk_ID)) == 0)
				{
					file_stream.read (reinterpret_cast<char*>(&wave.format), sizeof (wave_chunk::format_chunk));
					if (wave.format.chunk_size > sizeof_format_chunk)
					{
						file_stream.ignore (wave.format.chunk_size - sizeof_format_chunk);
					}
				}
				else if (std::strncmp (chunk_ID, data_chunk_ID, sizeof(chunk_ID)) == 0)
				{
					file_stream.read (reinterpret_cast<char*>(&wave.data.chunk_size), sizeof (wave.data.chunk_size));
					wave.data.buffer.resize (wave.data.chunk_size);
					file_stream.read (reinterpret_cast<char*>(&wave.data.buffer[0]), wave.data.chunk_size);
				}
				else
				{
					uint32_t chunk_size = 0;
					file_stream.read (reinterpret_cast<char*>(&chunk_size), sizeof (chunk_size));
					file_stream.ignore (chunk_size);
				}
			}
		}

		file_stream.close ();
	}
}

//------------------------------------------------------------------------
/*!	@fn write_wave_file
	@brief writes a wave chunk to file
	@param file_name Name of the file to write
	@param wave Chunk to be written
*/
//------------------------------------------------------------------------
void write_wave_file (const char* file_name, const wave_chunk& wave)
{
	std::ofstream file_stream;
	file_stream.open(file_name, std::ifstream::binary);
	if (file_stream.is_open())
	{
		//!	RIFF
		file_stream.write (riff_chunk_ID, sizeof (riff_chunk_ID));
		file_stream.write (reinterpret_cast<const char*>(&wave.chunk_size), sizeof (wave.chunk_size));
		file_stream.write (wave_type, sizeof (wave_type));

		//! fmt
		file_stream.write (format_chunk_ID, sizeof (format_chunk_ID));
		file_stream.write (reinterpret_cast<const char*>(&wave.format), sizeof (wave.format));

		//! data
		file_stream.write (data_chunk_ID, sizeof (data_chunk_ID));
		file_stream.write (reinterpret_cast<const char*>(&wave.data.chunk_size), sizeof (wave.data.chunk_size));
		file_stream.write (reinterpret_cast<const char*>(&wave.data.buffer[0]), wave.data.buffer.size ());

		file_stream.close ();
	}
}

//------------------------------------------------------------------------
} // namespace asl

#endif // __asl_wav_file_reader__