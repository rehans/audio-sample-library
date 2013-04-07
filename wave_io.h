// Copyright René Hansen 2013.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//	http://www.boost.org/LICENSE_1_0.txt)

#ifndef __asl_wav_file_reader__
#define __asl_wav_file_reader__

#include <fstream>
#include <stdint.h>
#include <string>

namespace asl
{
static const char* riff_chunk_ID	= "RIFF";
static const char* riff_type		= "WAVE";
static const char* format_chunk_ID	= "fmt ";
static const char* data_chunk_ID	= "data";

//------------------------------------------------------------------------
/*!	@struct riff_chunk
	@brief Structure of the RIFF WAVE file format
	
	The WAVE format is actually a special version of the RIFF standard.
*/
//------------------------------------------------------------------------
struct wave_chunk
{
	uint32_t chunk_size;	//! riff chunk size
	char riff_type[4];		//! riff type e.g. "WAVE" in this case
	
	struct format_chunk
	{
		uint32_t chunk_size;	//! format chunk size
		uint16_t audio_format;	//! audio format e.g. 0x0001 is PCM
		uint16_t channel_count;	//! number of channels
		uint32_t sample_rate;	//! the sample rate e.g. 44100
		uint32_t byte_rate;		//!
		uint16_t block_align;	//!
		uint16_t bits_per_sample;//! bits per sample e.g. 16
	} fmt;
	
	struct data_chunk
	{
		uint32_t chunk_size;	//! data chunk size
		std::vector<uint8_t> buffer;	//! buffer with e.g. PCM data
	} data;
};

//------------------------------------------------------------------------
/*!	@fn load_wave_file
	@brief Loads an audiofile of type wav
	@param file_name The path including name of the file
	@param riff The resulting RIFF chunk handed in as a reference
*/
//------------------------------------------------------------------------
void load_wave_file (const char* file_name, wave_chunk& riff)
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
			file_stream.read (reinterpret_cast<char*>(&riff.chunk_size), sizeof (riff.chunk_size));
			file_stream.read (riff.riff_type, sizeof (riff.riff_type));
			while (file_stream.good ())
			{
				file_stream.read(chunk_ID, sizeof(chunk_ID));
				if (std::strncmp (chunk_ID, format_chunk_ID, sizeof(chunk_ID)) == 0)
				{
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.chunk_size), sizeof (riff.fmt.chunk_size));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.audio_format), sizeof (riff.fmt.audio_format));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.channel_count), sizeof (riff.fmt.channel_count));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.sample_rate), sizeof (riff.fmt.sample_rate));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.byte_rate), sizeof (riff.fmt.byte_rate));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.block_align), sizeof (riff.fmt.block_align));
					file_stream.read (reinterpret_cast<char*>(&riff.fmt.bits_per_sample), sizeof (riff.fmt.bits_per_sample));
				}
				else if (std::strncmp (chunk_ID, data_chunk_ID, sizeof(chunk_ID)) == 0)
				{
					file_stream.read (reinterpret_cast<char*>(&riff.data.chunk_size), sizeof (riff.data.chunk_size));
					riff.data.buffer.resize (riff.data.chunk_size);
					file_stream.read (reinterpret_cast<char*>(&riff.data.buffer[0]), riff.data.chunk_size);
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
} // namespace asl

#endif // __asl_wav_file_reader__