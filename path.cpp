// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "path.h"

#include "config.h"


/*! \class Path path.h

    The Path class models the local parts of URLs, as far as Plusxome
    needs. This is just a series of strings separated by slashes.
*/

/*! Constructs an empty Path; the same as Path("/").
*/

Path::Path()
    : trailingSlash( true )
{

}


/*! Constructs a Path for \a s, which must already have been
    URL-deescaped.
*/

Path::Path( const string & s )
    : trailingSlash( false )
{
    unsigned int i = 0;
    while ( i < s.size() && s[i] != '?' ) {
	while ( s[i] == '/' )
	    ++i;
	unsigned int cs = i;
	while ( i < s.size() && s[i] != '?' && s[i] != '/' )
	    ++i;
	if ( i > cs )
	    parsed.push_back( s.substr( cs, i-cs ) );
	else if ( i >= s.size() || s[i] == '?' )
	    trailingSlash = true;
    }
}


/*! Constructs a copy of \a p. */

Path::Path( const Path & p )
    : parsed( p.parsed ), trailingSlash( p.trailingSlash )
{
}


/*! Returns path component number \a n, or a null string if there is
    no such component (typically because n is not smaller than
    components()).

    The return value generally does not contain '/'. Perhaps it can, I
    haven't looked up how URL escaping interacts with path
    separators. But it generally does not.
*/

string Path::component( int n ) const
{
    if ( n >= (int)parsed.size() )
	return "";
    return parsed[n];
}


/*! Returns the canonical representation of this Path, which is a
    UTF-8 string with minimal URL escaping.
*/

string Path::canonical() const
{
    string r;
    auto i = parsed.begin();
    while ( i < parsed.end() ) {
	r += "/";
	r += *i; // escape something maybe?
	++i;
    }
    if ( trailingSlash )
	r += "/";
    return r;
}


/*! Returns the number of path components, which is 0 for /, i for
    /foo, 1 for /foo/ and 2 of /arnt/razor-blades.
*/

int Path::components() const
{
    return parsed.size();

}


/*! Returns the absolute form of this Path, likely starting with http://
*/

std::string Path::absolute() const
{
    string r = Config::homePageUrl;
    if ( r[r.size()] == '/' )
	r += canonical().substr( 2 );
    else
	r += canonical();
    return r;
}
