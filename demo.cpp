
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

// uint8_t pat[16] = { 0x20, 0x64, 0xa8, 0xec, 0xff, 0xef, 0xde, 0xcd, 0xbc, 0x9a, 0x78, 0x56, 0x44, 0x33, 0x22, 0x11,  };
// uint8_t pat[16] = { 0x20, 0x64, 0xa8, 0xec, 0xfe, 0xff, 0xef, 0xee, 0xdd, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x22, 0x11,  }	;
// uint8_t pat[16] = { 0x31, 0x96, 0xdb, 0xee, 0xee, 0xff, 0xff, 0xde, 0xed, 0xff, 0xff, 0xee, 0xee, 0xbd, 0x69, 0x13,  };
// uint8_t pat[16] = { 0x20, 0x64, 0xa8, 0xdc, 0xfe, 0xef, 0xed, 0xff, 0xee, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x1,  };
// uint8_t pat[16] = { 0x10, 0x32, 0x54, 0x76, 0xa8, 0xdc, 0xee, 0x7f, 0xf7, 0xee, 0xcd, 0x8a, 0x67, 0x45, 0x23, 0x1,  };
uint8_t pat[16] = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  };


// "emulate" 1/60 second of sound
static void emulate_frame()
{
	static int delay;
	if ( --delay <= 0 )
	{
		delay = 48;

		apu.write_register( 0xff1A, 0x00 ); // reset
		// set pattern
		for (uint8_t s = 0; s < 16; s++) {
			// printf("Writing %x at %x\n", pat[s], 0xff30 + s);
			apu.write_register(0xff30 + s, pat[s] & 0xff);
		}
		apu.write_register( 0xff1A, 0x80 ); // enable
		apu.write_register( 0xff1B, 128 ); // sound length

		apu.write_register( 0xff1C, 0b00100000 ); // set 100% volume
		int freq = 1200;
		apu.write_register( 0xff1D, freq & 0xff ); // lower freq bits
		apu.write_register( 0xff1E, (freq >> 8) | 0xC0); // trigger, high freq bits

		// Start a new random tone
		// int chan = rand() & 0x11;
		// apu.write_register( 0xff25, 0xff ); // terminal selection, all channels to LR
		// apu.write_register( 0xff11, 0x80 ); // wave duty + length, normal (50%), none
		// int freq = (rand() & 0x3ff) + 0x300;
		// int freq = 1000;
		// apu.write_register( 0xff13, freq & 0xff ); // lower freq bits
		// apu.write_register( 0xff12, 0xf1 ); // 0b11110001, full volume, decrease, one sweep only
		// apu.write_register( 0xff14, (freq >> 8) | 0x80 ); // 0x80 == trigger, high freq bits
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
