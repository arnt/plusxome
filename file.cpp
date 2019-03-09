// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "file.h"


// fstat
#include <sys/types.h>
#include <sys/stat.h>
// read
#include <sys/uio.h>
#include <unistd.h>
// open
#include <fcntl.h>


/*! \class File file.h

    The File... hm... I don't know. Will find out how this can serve
    all of Template, Post and Asset. The code here may not survive
    long.
*/

/*! Constructs a File corresponding to \a name.

*/

File::File( const string & name )
    : n( name )
{
    int fd = ::open( n.c_str(), O_RDONLY );

    if ( fd < 0 )
        return;

    struct stat st;
    if ( fstat( fd, &st ) >= 0 ) {
	c.resize( st.st_size );
	int a = 0;
	int l = 1;
	while ( l > 0 && a < st.st_size ) {
	    l = ::read( fd, (void*)(a + c.data()), st.st_size - a );
	    if ( l > 0 ) {
		a += l;
		if ( a >= st.st_size )
		    l = -1;
	    }
	}
	c.resize( a );
    }
    ::close( fd );
}


/*! Returns the contents of the file as a string, or an empty string
    in case of read errors.
*/

string File::contents() const
{
    return c;
}
