
// Use Basic_Gb_Apu to play random tones. Writes output to sound file "out.wav".
// g++ Wave_Writer.cpp demo.cpp libgbsndemu.a -I /usr/include/SDL2 -I . -o demo && ./demo && aplay out.wav

#include "Basic_Gb_Apu.h"
#include "Wave_Writer.h"

#include <stdlib.h>

static Basic_Gb_Apu apu;

#define REG_SOUNDONOFF 0xff26
#define REGVAL_SOUNDONOFF_ALL 0x80
#define REG_SOUNDCHANNELTERMINAL 0xff25
#define REGVAL_SOUNDCHANNELTERMINAL_ALL 0xff

// "emulate" 1/60 second of sound
static void emulate_frame()
{
	static int delay;
	if ( --delay <= 0 )
	{
		delay = 24;

		// Start a new random tone
		// int chan = rand() & 0x11;
		apu.write_register( 0xff26, 0x80 ); // sound on/off, all on
		apu.write_register( 0xff25, 0xff ); // terminal selection, all channels to LR
		apu.write_register( 0xff11, 0x80 ); // wave duty + length, normal (50%), none
		// int freq = (rand() & 0x3ff) + 0x300;
		int freq = 1750;
		apu.write_register( 0xff13, freq & 0xff ); // lower freq bits
		apu.write_register( 0xff12, 0xf1 ); // 0b11110001, full volume, decrease, one sweep only
		apu.write_register( 0xff14, (freq >> 8) | 0x80 ); // 0x80 == trigger, high freq bits
	}

	// Generate 1/60 second of sound into APU's sample buffer
	apu.end_frame();
}

int main( int argc, char** argv )
{
	long const sample_rate = 44100;

	// Set sample rate and check for out of memory error
	apu.set_sample_rate( sample_rate );

	// Generate a few seconds of sound into wave file
	Wave_Writer wave( sample_rate );
	wave.stereo( true );
	for ( int n = 60 * 4; n--; )
	{
		// Simulate emulation of 1/60 second frame
		emulate_frame();

		// Samples from the frame can now be read out of the apu, or
		// allowed to accumulate and read out later. Use samples_avail()
		// to find out how many samples are currently in the buffer.

		size_t const buf_size = 2048;
		std::vector<Wave_Writer::sample_t> buf(buf_size);

		// Play whatever samples are available
		long count = apu.read_samples(buf);
		wave.write(buf.data(), count);
	}

	return 0;
}
