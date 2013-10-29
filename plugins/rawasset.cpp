// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "rawasset.h"

#include "asset.h"

#include <fstream>
#include <sstream>

static Registration<RawAsset> r( "asset" );


/*! \class RawAsset rawasset.h
  
    The RawAsset class provides access to an Asset object, without any
    modifications or filtering of any sort.
*/



/*! Constructs an empty Asset plugin. */

RawAsset::RawAsset()
{
    try {
	std::ifstream mimetypes("/etc/mime.types");
	std::string line;
	while( std::getline( mimetypes, line ) ) {
	    auto comment = line.find( "#" );
	    if ( comment != string::npos )
		line = line.substr( 0, comment );
	    int end = line.size();

	    int space = 0;
	    while ( space < end && 
		    ( line[space] != ' ' && line[space] != '\t' ) )
		space++;
	    string mimetype = line.substr( 0, space );

	    while ( space < end ) {
		while ( space < end && 
			( line[space] == ' ' || line[space] == '\t' ) )
		    space++;
		int start = space;
		while ( space < end && 
			( line[space] != ' ' && line[space] != '\t' ) )
		    space++;
		string suffix = line.substr( start, space-start );
		if ( suffix.size() && mimetype.size() )
		    types[suffix] = mimetype;
	    }
	}
    } catch ( ... ) {
    }
}


/*! Looks for an asset matching \a path and returns it if suitable. */

Rendering RawAsset::render( const Path & path ) const
{
    std::shared_ptr<Asset> a = Asset::find( path.canonical() );
    if ( !a )
	return Plugin::render( path );

    string ct = "application/octet-stream";
    string suffix = path.component( path.components() - 1 );
    auto p = suffix.rfind( "." );
    if ( p <= suffix.size() ) {
	auto f = types.find( suffix.substr( p + 1 ) );
	if ( f != types.end() ) {
	    ct = f->second;
	}
    }

    return Rendering( a->contents(), ct );
}


options_description * RawAsset::options()
{
    return new options_description();
}
