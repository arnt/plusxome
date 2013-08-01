// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "asset.h"

#include "file.h"

static map<string,shared_ptr<Asset> > assets;


/*! \class Asset asset.h
  
    The Asset class provides binary assets; files that can be served
    as-is. Typically used only for graphics and CSS.
*/

/*! Constructs an Asset for \a path, which is the local part of a URL.
*/

Asset::Asset( const string & path )
{
    assets[path] = shared_ptr<Asset>( this );
}


/*! Reloads \a file into this Asset. */

void Asset::reload( const string & file )
{
    c = File( file ).contents();
}


/*! Returns a pointer to the Asset corresponding to \a path.

    Returns what exactly if there is no such Asset?
*/

shared_ptr<Asset> Asset::find( const string & path )
{
    return assets[path];
}
