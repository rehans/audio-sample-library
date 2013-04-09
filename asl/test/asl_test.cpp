// Copyright René Hansen 2013.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//	http://www.boost.org/LICENSE_1_0.txt)

#include "wave_io.h"

/*!	@fnc Test silence wav file
*/
bool test_silence ()
{
	asl::wave_chunk::format_chunk expected_fmt_chunk = {
		16, 1, 1, 44100, 88200, 2, 16
	};

	int32_t expected_data_chunk_size = 88200;

	asl::wave_chunk wav;
	asl::load_wave_file("silence_1_sec_mono_16-bit_PCM.wav", wav);

	bool cmp_result = memcmp (&wav.fmt, &expected_fmt_chunk, sizeof (expected_fmt_chunk)) == 0
					&& wav.data.chunk_size == expected_data_chunk_size
					&& wav.data.chunk_size == wav.data.buffer.size ();

	return cmp_result;
}

int main(int argc, char** argv)
{
	bool test_result = test_silence ();

	return test_result == true ? 0 : 1;
}