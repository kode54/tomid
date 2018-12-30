#include <cstdio>

#include <string.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "nlohmann_json/single_include/nlohmann/json.hpp"

#include "midi_processing/midi_processor.h"

#include "nall/utf8.hpp"

using json = nlohmann::json;

#include "8820json.h"

std::string instrument_callback(uint8_t bank_msb, uint8_t bank_lsb, uint8_t instrument)
{
	std::string smsb, slsb, si;

	smsb = std::to_string((int)bank_msb);
	slsb = std::to_string((int)bank_lsb);
	si = std::to_string((int)instrument);

	std::string name;

	try
	{
		name = js[smsb][si];
		std::cout << smsb << ":" << si << " " << name << std::endl;
	}
	catch (...)
	{
		name = "";
	}

	std::string out;

	out += smsb;
	out += ":";
	out += si;
	
	if (name.length())
	{
		out += " ";
		out += name;
	}

	return out;
}

int main(int argc, char ** argv)
{
	nall::utf8_args args(argc, argv);

	if (args.argc() < 2)
	{
		fputs("Usage:\t2mid [-1] [-h x] <input files and/or wildcards>\n\n\t-1 - Promote type 0 files to type 1\n\t-h x\tActivate hackfix x, where x is:\n\t\t0 Remove channel 16\n\t\t1 Remove channels 11 through 16\n\t-lpx - Split tracks on program change events, which would\n\t       help immensely with importing into Logic Pro X.\n\t       Only works with type 1 files, so implies -1 switch.\n", stderr);
		return 1;
	}

	int arg_1 = 0;
	int arg_h = -1;
	int arg_h_value;
	int arg_lpx = 0;

	for (int i = 1; i < args.argc(); i++)
	{
		if ( !strcmp( args.argv()[i], "-1" ) )
		{
			arg_1 = i;
			continue;
		}
		else if ( ( !strcmp( args.argv()[i], "-h" ) || !strcmp( args.argv()[i], "-H" ) ) && i + 1 < args.argc() )
		{
			char * temp;
			arg_h = i;
			arg_h_value = strtoul( args.argv()[i+1], &temp, 10 );
		}
		else if ( !strcmp( args.argv()[i], "-lpx" ) )
		{
			arg_1 = i;
			arg_lpx = i;
			continue;
		}
	}

	for (int i = 1; i < args.argc(); i++)
	{
		if ( arg_1 == i ) continue;
		if ( (unsigned)(i - arg_h) < 2 ) continue;
		if ( arg_lpx == i ) continue;

		const char * in_name = args.argv()[i];
		char * out_name = new char[strlen(in_name) + 5];

		const char * in_extension = strrchr( in_name, '.' );
		if ( in_extension ) ++in_extension;

		strcpy(out_name, in_name);
		strcat(out_name, ".mid");

		FILE * f_in = nall::fopen_utf8(in_name, "rb");
		FILE * f_out = nall::fopen_utf8(out_name, "wb");

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
 
				if ( arg_h ) container.apply_hackfix( arg_h_value );

				if ( arg_lpx ) container.split_by_instrument_changes(instrument_callback);

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
