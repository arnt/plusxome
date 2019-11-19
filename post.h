// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef POST_H
#define POST_H

#include "base.h"
#include "node.h"
#include "path.h"


class Post {
public:
    const Node & rootNode() const;
    const Node & linkHere() const;

    const Node & abbreviatedRootNode() const;

    static std::shared_ptr<Post> find( const Path &, bool );
    static class PostSet all();

    void reload( const string & );

    set<class Tag *> findTags() const;

    void setTags( const string & );
    void setDate( const string & );

    bool tagged( const string & ) const;

    ptime date() const;
    Path path() const;

    bool isPublished() const;
    const string postingDate() const;

    void addImplicitTags();

private:
    Post( const Path & );

    Path name;
    std::shared_ptr<Node> root;
    std::shared_ptr<Node> abbrev;
    std::shared_ptr<Node> link;
    set<string> explicitTags;
    set<string> tags;
    ptime posted;
};


#endif
