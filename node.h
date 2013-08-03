// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef NODE_H
#define NODE_H

#include "base.h"


class Node {
public:
    Node();

    Node & operator=( const Node & other ) {
	t = other.t;
	tagName = other.tagName;
	attributes = other.attributes;
	children = other.children;
	return *this;
    }

    void append( string & );

    enum Type { Root, Tag, Text, Junk };

    Type t;
    std::string tagName;
    std::map<std::string, std::string> attributes;

    std::list<boost::shared_ptr<Node> > children;

    std::string text;

private:
    void appendChildren( string & );
    void appendAmpersandEscaped( string & );
    void appendAttributes( string & );
};



#endif
