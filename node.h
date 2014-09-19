// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef NODE_H
#define NODE_H

#include "base.h"


class Node {
public:
    Node();
    Node( const Node & );
    Node( const char * );
    Node( const std::string & );

    Node & operator=( const Node & other ) {
	t = other.t;
	tagName = other.tagName;
	attributes = other.attributes;
	children.clear();
	auto c = other.children.begin();
	while ( c != other.children.end() ) {
	    children.push_back( std::shared_ptr<Node>( new Node( **c ) ) );
	    ++c;
	}
	text = other.text;
	return *this;
    }

    void append( string & ) const;

    enum Type { Root, Tag, Text, Junk };

    Type t;
    std::string tagName;
    std::map<const std::string, std::string> attributes;

    std::list<std::shared_ptr<Node> > children;

    std::string text;

private:
    void appendChildren( string & ) const;
    void appendAmpersandEscaped( string & ) const;
    void appendAttributes( string & ) const;
};



#endif
