// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include <sysexits.h>

#include "filewatcher.h"
#include "httplistener.h"
#include "plugin.h"

#include <iostream>
#include <fstream>

#include <boost/program_options/parsers.hpp>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

#include "config.h"

string Config::categoryTemplate;
string Config::homePageTemplate;

string activePlugins;

string basedir;
string Config::postDirectory;
string Config::assetDirectory;

int port;


int main( int argc, char ** argv ) {
    string config( "/etc/plusxome/config" );

    options_description cli( "Command-line options" );
    cli.add_options()
	( "help,h", "produce help message" )
	( "config,c", value<string>(&config)->default_value( "/etc/plusxome/config" ),
	  "configuration file" )
	( "version,v", "show Plusxome's version number" );
    options_description conf( "Configuration file (and command-line) options" );

    conf.add_options()
	( "port,p", value<int>( &port )->default_value( 80 ),
	  "TCP port to serve HTTP on" )
	( "base-directory",
	  value<string>( &basedir )->default_value( "/usr/local/blog" ),
	  "base directory for postings" )
	( "post-directory",
	  value<string>( &Config::postDirectory )->default_value( "posts" ),
	  "base directory for postings (relative to base-directory)" )
	( "asset-directory",
	  value<string>( &Config::assetDirectory )->default_value( "assets" ),
	  "base directory for assets (relative to base-directory)" )
	( "plugins",
	  value<string>( &activePlugins )->default_value( "all" ),
	  "active plugins (comma-separated, or the word all)" );
    conf.add( Plugin::pluginOptions() );
    cli.add( conf );

    variables_map vm;
    store( parse_command_line( argc, argv, cli ), vm );

    ifstream cfs( config.c_str() );
    store( parse_config_file( cfs, conf, true ), vm );
    notify( vm );
    ::chdir( basedir.c_str() );
    Plugin::setActivePlugins( activePlugins );

    if ( vm.count( "help" ) ) {
	cout << "Plusxome serves thoughtful writings to the world." << endl
	     << endl
	     << cli << endl
	     << "See http://rant.gulbrandsen.priv.no/plusxome "
	     << "for more information." << endl;
	exit( 0 );
    }

    if ( vm.count( "version" ) ) {
	cout << "Plusxome version 0.0" << endl;
	exit( 0 );
    }


    (void)new FileWatcher;
    Plugin::setupPlugins();
    HttpListener v6( HttpListener::V6, port );
    HttpListener v4( HttpListener::V4, port );
    while ( v4.valid() || v6.valid() ) {
	::sleep( 5 );
    }

    return 0;
}
