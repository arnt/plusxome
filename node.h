// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef NODE_H
#define NODE_H

#include "base.h"


class Node {
public:
    Node();

    enum Type { Root, Tag, Text, Junk };

    Type t;
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::list<boost::shared_ptr<Node> > children;
};



#endif
