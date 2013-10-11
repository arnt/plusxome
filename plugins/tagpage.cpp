// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "tagpage.h"

#include "tag.h"

static Registration<TagPage> r( "tagpage" );


static string tn;


/*! \class TagPage homepage.h

    The TagPage plugin produces a page containing all postings with a
    specific tag.
*/

/*! Constructs a TagPage plugin. */

TagPage::TagPage()
{
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
    return produce( posts, t );
}


static bool byCount( Tag * t1, Tag * t2 )
{
    return t1->postSet().size() > t2->postSet().size();
}


/*! This produce() helper produces a rendering based on a set of \a
    posts.
*/

Document TagPage::produce( const PostSet & posts, Template * tp )
{
    Document result( *tp );
    auto p = posts.begin();
    std::shared_ptr<Node> parent = result.node( "postings" );
    set<Tag *> allTags;
    set<std::shared_ptr<Post>> avoid;
    if ( parent ) {
	while ( p != posts.end() ) {
	    std::shared_ptr<Node> r( new Node( (*p)->rootNode() ) );
	    parent->children.push_back( r );
	    avoid.insert( *p );
	    auto tags = (*p)->findTags();
	    allTags.insert( tags.begin(), tags.end() );
	    ++p;
	}
    }

    std::shared_ptr<Node> otherPosts = result.node( "otherposts" );
    if ( otherPosts && !allTags.empty()) {
	// sort the tags involved by number of postings, lowest first
	vector<Tag *> tags( allTags.begin(), allTags.end() );
	sort( tags.begin(), tags.end(), byCount );

	// pick the most recent post in each tag until we have n
	map< std::shared_ptr<Post>, Tag * > sameTag;
	auto t = tags.begin();
	while ( t != tags.end() && sameTag.size() < 5 ) {
	    PostSet inTag = (*t)->postSet().mostRecentFirst();
	    auto p = inTag.begin();
	    while ( p != inTag.end() && avoid.find( *p ) != avoid.end() )
		++p;
	    if ( p != inTag.end() ) {
		sameTag[*p] = *t;
		avoid.insert( *p );
	    }
	    ++t;
	}

	// we'll want a list of five other posts, we start by getting
	// the posts from sameTag and then filling those in with
	// recent postings until we have at least five, and at least
	// two new ones that are not related.
	PostSet other;
	auto mi = sameTag.begin();
	while ( mi != sameTag.end() ) {
	    other.push_back( mi->first );
	    ++mi;
	}
	PostSet recent = Post::all().mostRecentFirst();
	auto ri = recent.begin();
	int unrelated = 0;
	while ( ri != recent.end() && ( other.size() < 5 ||
					unrelated < 2 ) ) {
	    if ( avoid.find( *ri ) == avoid.end() ) {
		other.push_back( *ri );
		unrelated++;
	    }
	    ++ri;
	}
	// and we want to emit them ordered by date
	other = other.mostRecentFirst();

	if ( other.empty() ) {
	    // nothing to link to? best best remove the entire section
	    otherPosts->t = Node::Junk;
	} else {
	    fillInOtherPosts( otherPosts, other, sameTag );
	}
    }
    return result;
}



void TagPage::fillInOtherPosts( std::shared_ptr<Node> otherPosts,
				const PostSet & other,
				const map< std::shared_ptr<Post>, Tag * > & sameTag ) {
    // find the ul inside otherposts, quit early if there isn't any ul there
    std::shared_ptr<Node> ul;
    auto c = otherPosts->children.begin();
    while ( !ul && c != otherPosts->children.end() ) {
	if ( (*c)->tagName == "ul" ) {
	    ul = *c;
	    ul->children.clear();
	}
	++c;
    }
    if ( !ul )
	return;

    // we want to add up to five list items
    auto p = other.begin();
    while ( p != other.end() ) {
	std::shared_ptr<Node> li( new Node );
	li->tagName = "li";
	li->t = Node::Tag;
	ul->children.push_back( li );

	// first: a link to the post, with the title
	std::shared_ptr<Node> a( new Node( (*p)->linkHere() ) );
	li->children.push_back( a );

	// there are at least two postings with this tag: the one
	// linked from and to. if there are at least two more, we
	// append a link to the tag itself.
	if ( sameTag.find( *p ) != sameTag.end() ) {
	    Tag * inTag = sameTag.find( *p )->second;
	    if ( inTag->postSet().size() >= 4 ) {
		li->children.push_back( std::shared_ptr<Node>( new Node( " \xe2\x80\xa2 " ) ) );
		std::shared_ptr<Node> tagA( new Node() );
		ostringstream linkText;
		linkText << inTag->postSet().size()
			 << " posts tagged "
			 << inTag->name();
		tagA->children.push_back( std::shared_ptr<Node>( new Node( linkText.str() ) ) );
		tagA->attributes["href"] = "/" + inTag->name();
		tagA->tagName = "a";
		tagA->t = Node::Tag;
		li->children.push_back( tagA );
	    }
	}
	++p;
    }
}


/*! Reads the template file; nothing more is necessary. */

void TagPage::setup()
{
    t = new Template( tn );
}


options_description * TagPage::options()
{
    options_description * conf
	= new options_description( "Options for "
				   "the tagpage plugin" );

    conf->add_options()
	( "tag-page-template",
	  value<string>( &tn )->default_value( "tag-page.template" ),
	  "tag page template file name" );

    return conf;
}
