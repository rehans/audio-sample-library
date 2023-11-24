// Copyright Ren� Hansen 2013.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//	http://www.boost.org/LICENSE_1_0.txt)

#include "asl/wave_io.h"
#include <algorithm>

/*!	@fn test_read_wave_silence
 */
bool test_read_wave_silence()
{
    int32_t expected_riff_chunk_size               = 88236;
    int32_t expected_data_chunk_size               = 88200;
    asl::WaveChunk::FormatChunk expected_fmt_chunk = {16,    1, 1, 44100,
                                                      88200, 2, 16};

    asl::WaveChunk wav;
    asl::read_wave_file("silence_1_sec_mono_16-bit_PCM.wav", wav);

    bool cmp_result = wav.chunk_size == expected_riff_chunk_size &&
                      memcmp(&wav.format, &expected_fmt_chunk,
                             sizeof(expected_fmt_chunk)) == 0 &&
                      wav.data.chunk_size == expected_data_chunk_size &&
                      wav.data.chunk_size == wav.data.buffer.size();

    return cmp_result;
}

/*!	@fn test_write_wave_silence
 */
bool test_write_wave_silence()
{
    asl::WaveChunk wav;
    asl::read_wave_file("silence_1_sec_mono_16-bit_PCM.wav", wav);
    asl::write_wave_file("silence_1_sec_mono_16-bit_PCM_test.wav", wav);

    asl::WaveChunk wav2;
    memset(&wav2, 0, sizeof(wav2));
    asl::read_wave_file("silence_1_sec_mono_16-bit_PCM_test.wav", wav2);

    bool cmp_result =
        wav.chunk_size == wav2.chunk_size &&
        memcmp(&wav.format, &wav2.format, sizeof(wav2.format)) == 0 &&
        wav.data.chunk_size == wav2.data.chunk_size &&
        wav.data.chunk_size == wav2.data.buffer.size();

    return cmp_result;
}

/*!	@fn main
 */
int main(int argc, char** argv)
{
    bool test_result = true;
    test_result      = test_result && test_read_wave_silence();
    test_result      = test_result && test_write_wave_silence();

    return test_result == true ? 0 : 1;
}