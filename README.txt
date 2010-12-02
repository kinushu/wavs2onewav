wavs2onewav is a wav file converter that merge from multiple wav files to one wavfile.

example:	wavs2onewave "test1.wav"(44kHz 2ch) "test2.wav"(44kHz 2ch) -o "output.wav"
			output.wav is 44kHz 4ch.
notice:		wavs2onewav can convert only same sampling rate and bits wav files.

Usage:	 	wavs2onewav [-o output file] [input file] [input file] ...
			-o	output (.wav) file name
