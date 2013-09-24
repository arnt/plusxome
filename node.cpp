// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "node.h"


/*! \class Node node.h

    The Node class provides a single node in a Document. It's
    effectively a struct: Everything is public for the moment. I don't
    know whether that will remain open.
*/


/*! Constructs an empty Junk node. */

Node::Node()
    : t( Junk )
{
}


/*! Appends the contents of this Node to \a output.

    This also calls the children, so if this is called on the root
    node of a Document, \a output represents the entire document
    afterwards.
*/

void Node::append( string & output )
{
    switch ( t ) {
    case Root:
	appendChildren( output );
	break;

    case Tag:
	if ( !output.empty() &&
	     output[output.size()-1] != '\n' &&
	     ( tagName == "p" ||
	       tagName == "div" ||
	       tagName == "head" || tagName == "body" ||
	       tagName == "h1" || tagName == "h2" ||
	       tagName == "section" || tagName == "article" ||
	       tagName == "aside" ||
	       tagName == "nav" ||
	       tagName == "article" ||
	       tagName == "header" || tagName == "footer" ) )
	    output.push_back( '\n' );
	output.append( "<" );
	output.append( tagName );
	appendAttributes( output );
	output.append( ">" );
	appendChildren( output );
	if ( tagName != "p" ) {
	    output.append( "</" );
	    output.append( tagName );
	    output.append( ">" );
	    if ( tagName == "div" || tagName == "html" ) {
		output.push_back( '\n' );
	    }
	}
	break;

    case Text:
	appendAmpersandEscaped( output );
	break;

    case Junk:
	break;

    }
}


/*! Appends each child in turn to \a output. Does nothing if there are
    no children (but also does not fail).
*/

void Node::appendChildren( string & output )
{
    auto child = children.begin();
    while ( child != children.end() ) {
	(*child)->append( output );
	++child;
    }
}


/*! Appends the contents of this text node to \a output, escaping
    ampersands and whatever else is needed appropriately. Does nothing
    (but also doesn't fail) for other kinds of nodes.
*/

void Node::appendAmpersandEscaped( string & output )
{
    auto c = text.begin();
    while ( c != text.end() ) {
	switch ( *c ) {
	case '&':
	    output.append( "&amp;" );
	    break;
	case '<':
	    output.append( "&lt;" );
	    break;
	case '>':
	    output.append( "&gt;" );
	    break;
	default:
	    output.push_back( *c );
	    break;
	}
	++c;
    }
}


/*! Outputs each of this Node's attributes to \a output, with a
    preceding space, or does nothing if there aren't any. Never fails.
*/

void Node::appendAttributes( string & output )
{
    auto a = attributes.begin();
    while ( a != attributes.end() ) {
	output.push_back( ' ' );
	output.append( a->first );
	if ( !a->second.empty() ) {
	    output.push_back( '=' );
	    output.push_back( '"' );
	    auto c = a->second.begin();
	    while ( c != a->second.end() ) {
		switch ( *c ) {
		case '"':
		    output.append( "FOO" );
		    break;
		default:
		    output.push_back( *c );
		    break;
		};
		++c;
	    }
	    output.push_back( '"' );
	}
	++a;
    }
}
