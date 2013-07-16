// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "path.h"



/*! Constructs a Path for \a s. */

Path::Path( const string & s )
{
    int i = 0;
    while ( i < s.size() && s[i] != '?' ) {
	while ( s[i] == '/' )
	    ++i;
	int cs = i;
	while ( i < s.size() && s[i] != '?' && s[i] != '/' )
	    ++i;
	if ( i > cs )
	    parsed.push_back( unescaped( s.substr( cs, i-cs ) ) );
    }
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
    if ( n >= parsed.size() )
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
    }
    return r;
}
