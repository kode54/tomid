#include <stdio.h>

#include "midi_processing/midi_processor.h"

#include "nall/utf8.hpp"

int main(void)
{
	nall::utf8_args args;

	if (args.argc() < 2)
	{
		fputs("Usage:\t2mid [-1] <input files and/or wildcards>\n\n\t-1 - Promote type 0 files to type 1\n", stderr);
		return 1;
	}

	int arg_1 = 0;

	for (int i = 1; i < args.argc(); i++)
	{
		if ( !strcmp( args.argv()[i], "-1" ) )
		{
			arg_1 = i;
			break;
		}
	}

	for (int i = 1; i < args.argc(); i++)
	{
		if ( arg_1 == i ) continue;

		const char * in_name = args.argv()[i];
		char * out_name = new char[strlen(in_name) + 5];

		const char * in_extension = strrchr( in_name, '.' );
		if ( in_extension ) ++in_extension;

		strcpy(out_name, in_name);
		strcat(out_name, ".mid");

		FILE * f_in = _wfopen(nall::utf16_t(in_name), L"rb");
		FILE * f_out = _wfopen(nall::utf16_t(out_name), L"wb");

		if ( !f_in )
		{
			if ( f_out ) fclose( f_out );
			fprintf( stderr, "Unable to open input file: %s\n", in_name );
			delete [] out_name;
			return 1;
		}

		if ( !f_out )
		{
			fclose( f_in );
			fprintf( stderr, "Unable to open output file: %s\n", out_name );
			delete [] out_name;
			return 1;
		}

		fseek( f_in, 0, SEEK_END );

		size_t in_length = ftell( f_in );

		fseek( f_in, 0, SEEK_SET );

		try
		{
			std::vector<uint8_t> buffer;

			buffer.resize( in_length );

			if ( fread( &buffer[0], 1, in_length, f_in ) != in_length )
			{
				fclose( f_out );
				fclose( f_in );
				delete [] out_name;
				fprintf( stderr, "Unable to read input file: %s\n", in_name );
				return 1;
			}

			fclose( f_in );

			{
				midi_container container;

				midi_processor::process_file( buffer, in_extension, container );

				if ( arg_1 ) container.promote_to_type1();

				std::vector<uint8_t> out_buffer;

				container.serialize_as_standard_midi_file( out_buffer );

				if ( fwrite( &out_buffer[0], 1, out_buffer.size(), f_out ) != out_buffer.size() )
				{
					fclose( f_out );
					fprintf( stderr, "Error writing to output file: %s\n", out_name );
					delete [] out_name;
					return 1;
				}

				fclose( f_out );
			}

			delete [] out_name;
		}
		catch (std::exception const& e)
		{
			fclose(f_out);
			delete [] out_name;
			fprintf(stderr, "Error while converting file (%s): %s\n", in_name, e.what());
		}
	}

	return 0;
}
