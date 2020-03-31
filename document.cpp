// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "document.h"

#include "tidy/tidy.h"
#include "tidy/tidyenum.h"
#include "tidy/buffio.h"

#include "node.h"


using namespace std;



/*! \class Document document.h

    The Document class contains an entire DOM tree, with the ability to
    find nodes by ID, add nodes and so on.

    It's not strictly DOM. DOM-like. Mostly it's built by Tidy, and if
    Tidy and the DOM disagree I suppose Tidy might win.
*/

/*! Constructs a DOM tree with a single empty root node.
*/

Document::Document()
    : responseCode( 200 ), lifetime( 86400 )
{

}


static string fromTidyValue( TidyDoc tdoc, TidyNode tidyNode ) {
    TidyBuffer textBuffer;
    (void)::tidyBufInit( &textBuffer );
    (void)::tidyNodeGetValue( tdoc, tidyNode, &textBuffer );
    string r( (const char *)textBuffer.bp, (size_t)textBuffer.size );
    tidyBufFree( &textBuffer );
    return r;
}


static void getTidyAttributes( Node & node, TidyNode tidyNode ) {
    TidyAttr a = ::tidyAttrFirst( tidyNode );
    while ( a ) {
	const char * v = ::tidyAttrValue( a );
	if ( !v )
	    v = "";
	node.attributes[::tidyAttrName( a )] = v;
	a = ::tidyAttrNext( a );
    }
}


// this is a static private helper for the following constructor

static std::shared_ptr<Node> fromTidyNode( TidyDoc tdoc, TidyNode tidyNode ) {
    std::shared_ptr<Node> node( new Node );
    switch ( tidyNodeGetType( tidyNode ) ) {
    case TidyNode_Start:
	node->t = Node::Tag;
	node->tagName = tidyNodeGetName( tidyNode );
	getTidyAttributes( *node, tidyNode );
	break;
    case TidyNode_Text:
	node->t = Node::Text;

	node->text = fromTidyValue( tdoc, tidyNode );
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
	std::shared_ptr<Node> candidate( fromTidyNode( tdoc, child ) );
	if ( candidate->t != Node::Junk )
	    node->children.push_back( candidate );
	child = tidyGetNext( child );
    }
    return node;
}

// this is a static private helper for the following constructor

static std::shared_ptr<Node> fromHtml( const std::string & html ) {
    TidyDoc tdoc = tidyCreate();

    // don't let tidy generate any meta generator tag
    ::tidyOptSetBool( tdoc, TidyMark, no );

    // unicode all the time
    ::tidyOptSetValue( tdoc, TidyCharEncoding, "utf8" );

    // don't let tidy report any errors (at all!)
    TidyBuffer errorBuffer;
    (void)::tidyBufInit( &errorBuffer );
    (void)::tidySetErrorBuffer( tdoc, &errorBuffer );

    (void)::tidyParseString( tdoc, html.c_str() );
    (void)::tidyCleanAndRepair( tdoc );
    tidyBufFree( &errorBuffer );
    auto r = fromTidyNode( tdoc, ::tidyGetRoot( tdoc ) );
    ::tidyRelease( tdoc );
    return r;
}


/*! Constructs a Document based on \a html.

    This runs (more or less) HTML Tidy on \a html, with my preferred
    settings.
*/

Document::Document( const std::string & html )
    : root( fromHtml( html ) ), responseCode( 200 ), lifetime( 86400 )
{
}


/*! Constructs a copy of \a other. The copy is deep in principle, but
    may be optimised shallowly.
*/

Document::Document( const Document & other )
    : responseCode( other.responseCode ), lifetime( other.lifetime )
{
    *this = other;
}


Document & Document::operator=( const Document & other ) {
    if ( other.root )
	root = std::shared_ptr<Node>( new Node( *other.root ) );
    else
	root = std::shared_ptr<Node>( 0 );
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
    list< std::shared_ptr<Node> > outstanding;
    outstanding.push_back( root );
    while ( !outstanding.empty() ) {
	std::shared_ptr<Node> current = outstanding.front();
	outstanding.pop_front();

	auto child = current->children.begin();
	while ( child != current->children.end() ) {
	    outstanding.push_back( *child );
	    ++child;
	}

	if ( current->attributes.find( "id" ) != current->attributes.end() ) {
	    string id = current->attributes["id"];
	    if ( !id.empty() && !ids.count( id ) )
		ids[id] = current;
	}
    }
}


/*! Returns a reference to the Node with ID \a id. If there is no such
    node, a temporary invisible Node is returned.
*/

std::shared_ptr<Node> Document::node( const string & id )
{
    if ( ids.empty() )
	populateIdMap();
    return ids[id];
}


/*! Parses \a s as HTML and updates this Document to reflect \a s. */

void Document::parse( const std::string & s )
{
    root = fromHtml( s );
    ids.clear();
}


/*! Returns a reference to the root Node of the parsed document. */

std::shared_ptr<Node> Document::rootNode()
{
    return root;
}


/*! Records that this document's HTTP response code is \a code. The
    value set at construction time is 200.
*/

void Document::setHttpResponseCode( unsigned int code )
{
    responseCode = code;
}


/*! Returns what was recorded by setHttpResponseCode(), or 200 if
    setHttpResponseCode() has not been called.
*/

unsigned int Document::httpResponseCode() const
{
    return responseCode;
}


/*! Records that this document's HTTP cache lifetime is \a seconds. The
    value set at construction time is 86400 (ony day).
*/

void Document::setCacheLifetime( unsigned int ttl )
{
    lifetime = ttl;
}


/*! Returns what was recorded by setCacheLifetime(), or 86400 if
    setCacheLifetime() has not been called.
*/

unsigned int Document::cacheLifetime() const
{
    return lifetime;
}


static void addElementsByTag( std::shared_ptr<Node> node,
			      const string & name,
			      list<std::shared_ptr<Node> > & r ) {
    if ( name == (*node).tagName )
	r.push_back( node );
    auto c = (*node).children.begin();
    while ( c != (*node).children.end() ) {
	addElementsByTag( *c, name, r );
	++c;
    }
}


/*! Returns a list of the nodes named \a name.
*/

list<std::shared_ptr<Node> > Document::getElementsByTag( const string & name )
{
    list<std::shared_ptr<Node> > r;
    addElementsByTag( root, name, r );
    return r;
}
