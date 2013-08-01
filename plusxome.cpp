// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include <sysexits.h>

#include "filewatcher.h"
#include "httplistener.h"
#include "plugin.h"

#include <iostream>
#include <fstream>

using namespace std;

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

using namespace boost::program_options;

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

#include "config.h"

string Config::singlePostTemplate;
string Config::categoryTemplate;
string Config::homePageTemplate;

string activePlugins;

string basedir;
string Config::postDirectory;
string Config::assetDirectory;


int main( int argc, char ** argv ) {
    string config( "/etc/plusxome/config" );

    options_description cli( "Command-line options" );
    cli.add_options()
	( "help,h", "produce help message" )
	( "config,c", value<string>(&config)->default_value( "/etc/plusxome/plusxome.cf" ),
	  "configuration file" )
	( "version,v", "show Plusxome's version number" );
    options_description conf( "Configuration file (and command-line) options" );

    conf.add_options()
	( "base-directory",
	  value<string>( &basedir )->default_value( "/usr/local/blog" ),
	  "base directory for postings" )
	( "post-directory",
	  value<string>( &Config::postDirectory )->default_value( "posts" ),
	  "base directory for postings (relative to base-directory)" )
	( "asset-directory",
	  value<string>( &Config::assetDirectory )->default_value( "assets" ),
	  "base directory for assets (relative to base-directory)" )
	( "post-template",
	  value<string>( &Config::singlePostTemplate )->default_value( "post.template" ),
	  "post template file name" )
	( "category-template",
	  value<string>( &Config::categoryTemplate )->default_value( "tag.template" ),
	  "category page template file name" )
	( "home-template",
	  value<string>( &Config::homePageTemplate )->default_value( "home.template" ),
	  "home page template file name" )
	( "plugins",
	  value<string>( &activePlugins )->default_value( "singlepost" ),
	  "active plugins (comma-separated)" );
    cli.add( conf );

    variables_map vm;
    store( parse_command_line( argc, argv, cli ), vm );
    notify( vm );

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

    Plugin::setActivePlugins( activePlugins );

    ::chdir( basedir.c_str() );

    (void)new FileWatcher;
    HttpListener v6( HttpListener::V6, 3080 );
    HttpListener v4( HttpListener::V4, 3080 );
    while ( v4.valid() || v6.valid() ) {
	::sleep( 5 );
    }

    return 0;
}
