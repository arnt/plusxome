// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "post.h"

#include "tag.h"
#include "file.h"
#include "document.h"

#include <boost/algorithm/string.hpp>

static map<string,shared_ptr<Post> > posts;



/*! \class Post post.h

    The Post class models a single .post file.
*/

/*! Constructs a Post for \a path and records its existence so find()
    will find the Post.
*/

Post::Post( const string & path )
{
    posts["/" + path] = shared_ptr<Post>( this );
}


/*! Returns a reference to the root DOM node of this posting. */

const Node & Post::rootNode() const
{
    return *root;
}


/*! Returns a pointer to the Post corresponding to \a path. \a path should
    not include the leading base directory or trailing .post.

    Returns a null pointer if there is no such Post.
*/

shared_ptr<Post> Post::find( const std::string & path )
{
    return posts[path]; // XXX is this correct?
}


/*! Returns the set of all Post objects, not sorted at all.
*/

vector< shared_ptr<Post> > Post::all()
{
    vector< shared_ptr<Post> > result;
    auto p = posts.begin();
    while ( p != posts.end() ) {
	result.push_back( p->second );
	++p;
    }
    return result;
}


/*! Reloads the Post from \a path, which must be the name of a file
    (ie. including the trailing .post).
*/

void Post::reload( const string & path )
{
    string contents( File( path ).contents() );
    unsigned int i = 0;
    while ( i < contents.size() && contents[i] != '\n' )
	i++;
    string html( "<h1>" + contents.substr( 0, i ) + "</h1>" );
    i++;
    while ( i < contents.size() && contents[i] != '\n' ) {
	unsigned int s = i;
	while ( i < contents.size() && contents[i] != '\n' )
	    i++;
	if ( contents.substr( s, 5 ) == "meta:" ) {
	    s += 5;
	    while ( contents[s] == ' ' )
		s++;
	    if ( contents.substr( s, 5 ) == "tags=" ) {
		setTags( contents.substr( s + 5, i - s - 5 ) );
	    } else if ( contents.substr( s, 5 ) == "date=" ) {
		setDate( contents.substr( s + 5, i - s - 5 ) );
	    }
	}
	i++;
    }

    if ( i < contents.size() )
	html += contents.substr( i );
    root = Document( html ).getElementsByTag( "body" ).front();
    root->tagName = "article";
}


/*! Splits \a tagList at commas, removes any neighbouring whitespace,
    and makes sure this post belongs (semantically) to the right Tag
    object.
*/

void Post::setTags( const string & tagList )
{
    tags.clear();
    set<string> v;
    split( v, tagList, is_any_of(","), token_compress_on );
    auto s = v.begin();
    while ( s != v.end() ) {
	string t = *s;
	trim( t );
	Tag::ensure( t );
	tags.insert( t );
	++s;
	auto s2 = s;
	while ( s2 != v.end() ) {
	    string t2 = *s2;
	    trim( t2 );
	    string ps = t + "+" + t2;
	    Tag::ensure( ps );
	    tags.insert( ps );
	    ps = t2 + "+" + t;
	    Tag::ensure( ps, false );
	    tags.insert( ps );
	    ++s2;
	}
    }
}


/*! Records that the publication date of this post is \a date.
*/

void Post::setDate( const string & )
{
    // later
}


/*! Returns true if this Post is tagged \a t, and false if not.
*/

bool Post::tagged( const string & t ) const
{
    return tags.find( t ) != tags.end();
}


/*! Returns the publication date of this Post. */

int Post::date() const
{
    return 0;
}
