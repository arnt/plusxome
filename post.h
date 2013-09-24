// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef POST_H
#define POST_H

#include "base.h"
#include "node.h"
#include "path.h"


class Post {
public:
    Post( const string & );

    const Node & rootNode() const;

    static std::shared_ptr<Post> find( const string & );
    static class PostSet all();

    void reload( const string & );

    void setTags( const string & );
    void setDate( const string & );

    bool tagged( const string & ) const;

    ptime date() const;
    Path path() const;

private:
    Path name;
    std::shared_ptr<Node> root;
    set<string> tags;
    ptime posted;
};


#endif
