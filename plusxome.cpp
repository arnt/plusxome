// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include <sysexits.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

using namespace boost::program_options;

#include <boost/filesystem.hpp>

using namespace boost::filesystem;


string postdir;
string posttemplate;
string assetdir;


void serve() {
    
}


void loop() {
}

int main( int argc, char ** argv ) {
    string config( "/etc/plusxome/config" );

    options_description cli( "Command-line options" );
    cli.add_options()
	( "help,h", "produce help message" )
	( "config,c", value<string>(&config)->default_value( "plusxome.cf" ),
	  "specify configuration file" )
	( "version,v", "show Plusxome's version number" );
    options_description conf( "Configuration file (and command-line) options" );

    conf.add_options()
	( "post-directory",
	  value<string>( &postdir )->default_value( "" ),
	  "specify base directory for postings" )
	( "asset-directory",
	  value<string>( &assetdir )->default_value( "" ),
	  "specify base directory for asserts" )
	( "post-template",
	  value<string>( &posttemplate )->default_value( "" ),
	  "specify post template name" );
    cli.add( conf );

    variables_map vm;
    store( parse_command_line( argc, argv, cli ), vm );
    notify( vm );

    if ( vm.count( "help" ) ) {
	cout << "Plusxome serves thoughtful writings to the world. See" << endl
	     << "http://rant.gulbrandsen.priv.no/plusxome "
	     << "for more information." << endl;
	exit( 0 );
    }

    if ( vm.count( "version" ) ) {
	cout << "Plusxome version 0.0" << endl;
	exit( 0 );
    }
    
    //scanPostings();
    //serve();

    return 0;
}
