// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tag.h"

#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

static map<string, Tag*> tags;


/*! \class Tag tag.h

    The Tag class models a single tag, as used to group similar Post
    objects.

    The Tag class knows whether it has resolved its content, and can
    provide a resolved list, sorted by most recent first. That's all.
*/

/*! Constructs an empty Tag with no name. No postings carry this Tag. */

Tag::Tag()
    : e( true ), l( true ), r( false )
{
}


/*! Constructs an empty Tag named \a t, publishing it if it \a regular. */

Tag::Tag( const string & t, bool regular )
    : e( false ), l( false ), r( regular ), n( t )
{
    tags[t] = this;
}


/*! Forgets which Post objects carry this Tag. This is useful for when
    one or more .post files have been edited or deleted.
*/

void Tag::unlearnPosts()
{
    l = false;
    posts.clear();
}


/*! Returns a list containing the Post objects that are tagged with this Tag,
    or an empty list if this Tag has no name.
*/

PostSet Tag::postSet() const
{
    if ( !l ) {
	unique_lock<shared_mutex> lock( m );
	auto all = Post::all();
	auto p = all.begin();
	while ( p != all.end() ) {
	    if ( (*p)->tagged( n ) )
		posts.push_back( *p );
	    ++p;
	}
    }
    return posts;
}


/*! Returns a reference to a Tag named \a s, or to a meaningless empty
    tag if no Tag has that name.
*/

Tag * Tag::find( const string & s )
{
    return tags[s];
}


/*! Returns true if this is a regular Tag, and false if it is in any
    sense internal.
*/

bool Tag::regular() const
{
    return r;
}


/*! Ensures that a Tag named \a n exists. If such a Tag needs to be
    created, then it is created as a regular() tag if \a regular is
    true, and as an internal tag if \a regular is false.
*/

void Tag::ensure( const string & n, bool regular )
{
    if ( tags.find( n ) != tags.end() )
	(void)new Tag( n, regular );
}
