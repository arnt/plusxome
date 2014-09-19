// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "asset.h"

#include "file.h"

// fstat
#include <sys/types.h>
#include <sys/stat.h>
// read
#include <sys/mman.h>
// open
#include <fcntl.h>


static map<string,std::shared_ptr<Asset> > assets;


/*! \class Asset asset.h

    The Asset class provides binary assets; files that can be served
    as-is. Typically used only for graphics and CSS.
*/

/*! Constructs an Asset for \a path, which is the local part of a URL.
*/

Asset::Asset( const string & path )
    : c( 0 ), s( 0 )
{
    assets[path] = std::shared_ptr<Asset>( this );
}


/*! Reloads \a file into this Asset. */

void Asset::reload( const string & file )
{
    if ( c )
	::munmap( c, s );

    int fd = ::open( file.c_str(), O_RDONLY );

    if ( fd < 0 )
        return;

    struct stat st;
    if ( ::fstat( fd, &st ) < 0 )
        return;

    s = st.st_size;
    c = reinterpret_cast<char*>
	( ::mmap( 0, s, PROT_READ, MAP_PRIVATE, fd, 0 ) );
    ::close( fd );
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
    return string( c, s );
}
