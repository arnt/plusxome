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
    static vector< shared_ptr<Post> > all();

    void reload( const string & );

    void setTags( const string & );
    void setDate( const string & );

    bool tagged( const string & ) const;

    int date() const;

private:
    shared_ptr<Node> root;
    set<string> tags;
};


#endif
