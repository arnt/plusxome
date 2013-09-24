// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tagpage.h"

#include "tag.h"


static string tn;


/*! \class TagPage homepage.h

    The TagPage plugin produces a page containing all postings with a
    specific tag.
*/

/*! Constructs a TagPage plugin and adds its configurability to \a cf. */

TagPage::TagPage( options_description & cf )
{
    options_description * conf
	= new options_description( "Options for "
				   "the tagpage plugin" );

    conf->add_options()
	( "tag-page-template",
	  value<string>( &tn )->default_value( "tag-page.template" ),
	  "tag page template file name" );

    cf.add( *conf );
}


Document TagPage::produce( const Path & path ) const
{
    if ( path.components() != 1 )
	return Plugin::produce( path );

    Tag * tag = Tag::find( path.component( 0 ) );
    if ( !tag )
	return Plugin::produce( path );

    auto posts = tag->postSet().mostRecentFirst();
    if ( posts.empty() )
	return Plugin::produce( path );

    Document result( *t );
    auto p = posts.begin();
    std::shared_ptr<Node> parent = result.node( "postings" );
    if ( parent ) {
	while ( p != posts.end() ) {
	    std::shared_ptr<Node> r( new Node( (*p)->rootNode() ) );
	    parent->children.push_back( r );
	    ++p;
	}
    }
    return result;
}


/*! Reads the template file; nothing more is necessary. */

void TagPage::setup()
{
    t = new Template( tn );
}
