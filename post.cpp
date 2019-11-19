// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "post.h"

#include "tag.h"
#include "file.h"
#include "document.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <boost/algorithm/string.hpp>

static map<string,std::shared_ptr<Post> > posts;


/*! \class Post post.h

    The Post class models a single .post file.
*/

Post::Post( const Path & path )
    : name( path )
{
}


/*! Returns a reference to the root DOM node of this posting. */

const Node & Post::rootNode() const
{
    return *root;
}


/*! Returns a Node that's a link to this posting,

*/

const Node & Post::linkHere() const
{
    return *link;
}


/*! Returns a pointer to the Post corresponding to \a path. \a path should
    not include the leading base directory or trailing .post.

    Returns a null pointer if there is no such Post.
*/

std::shared_ptr<Post> Post::find( const Path & requested, bool create )
{
    auto result = posts.find( requested.canonical() );
    if ( result != posts.end() )
	return result->second;
    if ( !create )
	return std::shared_ptr<Post>( 0 );
    auto created = std::shared_ptr<Post>(new Post( requested ) );
    posts.emplace( requested.canonical(), created );
    return created;
}


/*! Returns the set of all Post objects, not sorted at all.
*/

PostSet Post::all()
{
    PostSet r;
    auto p = posts.begin();
    while ( p != posts.end() ) {
	r.push_back( p->second );
	++p;
    }
    return r;
}


/*! Reloads the Post from \a path, which must be the name of a file
    (ie. including the trailing .post).
*/

void Post::reload( const string & path )
{
    string contents( File( path ).contents() );
    if ( contents.empty() ) {
	posts.erase( name.canonical() );
	return;
    }
    unsigned int i = 0;
    while ( i < contents.size() && contents[i] != '\n' )
	i++;
    string html( "<h1>"
		 "<a href=\"" + name.canonical() + "\">" +
		 contents.substr( 0, i ) + "</a></h1>" );
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

    if ( posted == ptime(not_a_date_time) ) {
	struct stat st;
        (void)::stat( path.c_str(), &st );
	posted = from_time_t(st.st_ctime);
    }

    if ( i < contents.size() )
	html += contents.substr( i );
    root = Document( html ).getElementsByTag( "body" ).front();
    root->tagName = "article";
    link = Document( html ).getElementsByTag( "a" ).front();

    root->attributes["data-posting-date"] = postingDate();

    auto ff = html.find( '\f' );
    if ( ff == string::npos ) {
	abbrev = root;
    } else {
	html = html.substr( 0, ff );
	html += " <a class=readmore href=" + name.canonical() + ">[&hellip;More&hellip;]</a>";
	abbrev = Document( html ).getElementsByTag( "body" ).front();
	abbrev->tagName = "article";
	abbrev->attributes["data-posting-date"] = postingDate();
    }
}


/*! Splits \a tagList at commas, removes any neighbouring whitespace,
    and makes sure this post belongs (semantically) to the right Tag
    object.
*/

void Post::setTags( const string & tagList )
{
    explicitTags.clear();
    split( explicitTags, tagList, is_any_of(", "), token_compress_on );
    for( auto t : explicitTags )
	Tag::ensure( t );
}

void Post::addImplicitTags()
{
    // we'll need the explicitly created tags in order to create pairs
    tags = explicitTags;

    // tags for yyyy and yyyy-mm
    auto month = postingDate().substr( 0, 7 );
    auto year = month.substr( 0, 4 );
    Tag::ensure( year, false );
    Tag::ensure( month, false );

    // find and record existing tags mentioned in the path
    int n = name.components();
    set<string> mentioned;
    while ( n-- > 0 ) {
	string tmp = name.component(n);
	split( mentioned, tmp, is_any_of("-"), token_compress_on );
    }
    for ( auto t : mentioned )
	if ( Tag::find( t ) )
	    tags.insert( t );

    // tag pairs
    set<string> pairs;
    for ( auto t1 : tags ) {
	for ( auto t2 : tags ) {
	    if ( t1 != t2 ) {
		string ps = t1 + "+" + t2;
		Tag::ensure( ps, t1 != year && t2 != year && t1 < t2 );
		pairs.insert( ps );
	    }
	}
    }

    // the year, month and pairs aren't be used in pairs
    tags.insert( year );
    tags.insert( month );
    for ( auto p : pairs )
	tags.insert( p );
}


/*! Looks up and returns a list of the Tag objects that ought to
    contain this Post.

    This includes tag pairs.
*/

set<Tag *> Post::findTags() const
{
    set<Tag *> result;
    auto t = tags.begin();
    while ( t != tags.end() ) {
	result.insert( Tag::find( *t ) );
	++t;
    }
    return result;
}


/*! Records that the publication date of this post is \a date.
*/

void Post::setDate( const string & date )
{
    posted = ptime(not_a_date_time);

    istringstream is( date );
    is.imbue( locale( locale::classic(),
		      new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S") ) );
    is >> posted;
    if ( posted != not_a_date_time )
	return;

    is.imbue( locale( locale::classic(),
		      new boost::posix_time::time_input_facet("%Y-%m-%d") ) );
    is >> posted;
}


/*! Returns true if this Post is tagged \a t, and false if not.
*/

bool Post::tagged( const string & t ) const
{
    return tags.find( t ) != tags.end();
}


/*! Returns the publication date of this Post. */

ptime Post::date() const
{
    return posted;
}


/*! Returns the path to this Post. */

Path Post::path() const
{
    return name;
}


/*! Returns the root node of an abbreviated version of this Posting.
    If the .post file contains a formfeed character,
    abbreviatedRootNode() returns only what's before that character,
    otherwise it returns the entire posting.
*/

const Node & Post::abbreviatedRootNode() const
{
    return *abbrev;
}


/*! Returns the date on which this Post was made, in YYYY-mm-dd
    format. For use in data-posting-date.
*/

const string Post::postingDate() const
{
    ostringstream o;
    boost::posix_time::time_facet *facet = new time_facet("%Y-%m-%d");
    o.imbue( locale( o.getloc(), facet ) );
    o << posted;
    return o.str();
}
