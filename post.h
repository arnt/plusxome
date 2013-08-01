// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef POST_H
#define POST_H

#include "base.h"
#include "node.h"


class Post {
public:
    Post( const string & );

    const Node & rootNode() const;

    static shared_ptr<Post> find( const string & );

    void reload( const string & );
    
private:
    shared_ptr<Node> root;
};


#endif
