// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "asset.h"

#include "file.h"

static map<string,std::shared_ptr<Asset> > assets;


/*! \class Asset asset.h

    The Asset class provides binary assets; files that can be served
    as-is. Typically used only for graphics and CSS.
*/

/*! Constructs an Asset for \a path, which is the local part of a URL.
*/

Asset::Asset( const string & path )
{
    assets[path] = std::shared_ptr<Asset>( this );
}


/*! Reloads \a file into this Asset. */

void Asset::reload( const string & file )
{
    c = File( file ).contents();
}


/*! Returns a pointer to the Asset corresponding to \a path.

    Returns what exactly if there is no such Asset?
*/

std::shared_ptr<Asset> Asset::find( const string & path )
{
    if ( assets.find( path ) != assets.end() )
	return assets[path];
    return std::shared_ptr<Asset>( 0 );
}


/*! Returns the contents of this Asset as a plain string. Maybe it
    would be better to mmap(), but for now this is what I do.
*/

string Asset::contents() const
{
    return c;
}
