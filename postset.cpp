// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "postset.h"

#include "post.h"



/*! \class PostSet postset.h

    The PostSet class is a set of postings, ordered and reorderable.

    The name set implies that the postings are not ordered, but the
    class has grown to be more like an array. Perhaps it should be
    renamed.
*/


/*! Constructs an empty PostSet.
*/

PostSet::PostSet()
    : vector< std::shared_ptr<Post> >()
{
    // nothing needed
}


/*! Returns a section of this set, starting with \a start and
    containing at most \a number posts.
*/

PostSet PostSet::section( int start, int number ) const
{
    PostSet r;
    auto p = begin();

    int i = 0;
    while ( i < start ) {
	if ( p == end() )
	    return r;
	++p;
	++i;
    }

    i = 0;
    while ( i < number ) {
	if ( p == end() )
	    return r;
	r.push_back( *p );
	++p;
	++i;
    }

    return r;
}


static bool byDate( const std::shared_ptr<Post> & c1,
		    const std::shared_ptr<Post> & c2 )
{
    const Post * p1 = c1.get();
    const Post * p2 = c2.get();
    return p1->date() > p2->date();
}


/*! Returns a copy of this set, ordered with the most recent post
    first.
*/

PostSet PostSet::mostRecentFirst() const
{
    PostSet r( *this );
    sort( r.begin(), r.end(), byDate );
    return r;
}
