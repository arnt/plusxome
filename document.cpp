// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "document.h"

#include "tidy/tidy.h"
#include "tidy/tidyenum.h"
#include "tidy/buffio.h"

#include "node.h"


using namespace std;
using boost::shared_ptr;



/*! \class Document document.h

    The Document class contains an entire DOM tree, with the ability to
    find nodes by ID, add nodes and so on.

    It's not strictly DOM. DOM-like. Mostly it's built by Tidy, and if
    Tidy and the DOM disagree I suppose Tidy might win.
*/

/*! Constructs a DOM tree with a single empty root node.
*/

Document::Document()
{

}


// this is a static private helper for the following constructor

static shared_ptr<Node> fromTidyNode( TidyNode tidyNode ) {
    shared_ptr<Node> node( new Node );
    switch ( tidyNodeGetType( tidyNode ) ) {
    case TidyNode_Start:
	node->t = Node::Tag;
	node->tagName = tidyNodeGetName( tidyNode );
	// todo: also attributes
	break;
    case TidyNode_Text:
	node->t = Node::Text;
	// xxx
	break;
    case TidyNode_Root:
	node->t = Node::Root;
	// who cares, really? but we need the children.
	break;
    default:
	node->t = Node::Junk;
	// we ignore comments, doctype and whatever.
	break;
    }
    TidyNode child = tidyGetChild( tidyNode );
    while ( child ) {
	boost::shared_ptr<Node> candidate( fromTidyNode( child ) );
	if ( candidate->t != Node::Junk )
	    node->children.push_back( candidate );
	child = tidyGetNext( child );
    }
    return node;
}

// this is a static private helper for the following constructor

static shared_ptr<Node> fromHtml( const std::string & html ) {
    TidyDoc tdoc = tidyCreate();

    TidyBuffer errorBuffer;
    (void)::tidyBufInit( &errorBuffer );
    (void)::tidySetErrorBuffer( tdoc, &errorBuffer );

    (void)::tidyParseString( tdoc, html.c_str() );
    (void)::tidyCleanAndRepair( tdoc );
    return fromTidyNode( ::tidyGetRoot( tdoc ) );
}


/*! Constructs a Document based on \a html.

    This runs (more or less) HTML Tidy on \a html, with my preferred
    settings.
*/

Document::Document( const std::string & html )
    : root( fromHtml( html ) )
{
}


/*! Constructs a copy of \a other. The copy is deep in principle, but
    may be optimised shallowly.
*/

Document::Document( const Document & other )
{
    *this = other;
}


Document & Document::operator=( const Document & other ) {
    root = other.root;
    ids.clear();
    return *this;
}


/*! Populates the ID-to-Node map based on the nodes' ID attribute.

    If two nodes have the same ID, this function allows the first to
    win and silently ignores subsequent IDs.
*/

void Document::populateIdMap()
{
    ids.clear();
    list< shared_ptr<Node> > outstanding;
    outstanding.push_back( root );
    while ( !outstanding.empty() ) {
	shared_ptr<Node> current = outstanding.front();
	outstanding.pop_front();

	auto child = current->children.begin();
	while ( child != current->children.end() ) {
	    outstanding.push_back( *child );
	    ++child;
	}

	string id = current->attributes["id"];
	if ( !id.empty() && !ids.count( id ) )
	    ids[id] = current;
    }
}


/*! Returns a reference to the Node with ID \a id. If there is no such
    node, a temporary invisible Node is returned.
*/

Node & Document::node( const string & id )
{
    return *(ids[id]);
}


/*! Parses \a s as HTML and updates this Document to reflect \a s. */

void Document::parse( const std::string & s )
{
    root = fromHtml( s );
    ids.clear();
}


/*! Returns a reference to the root Node of the parsed document. */

boost::shared_ptr<Node> Document::rootNode()
{
    return root;
}
