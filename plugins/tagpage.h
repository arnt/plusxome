// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef TAGPAGE_H
#define TAGPAGE_H

#include "plugin.h"


class TagPage: public Plugin {
public:
    TagPage();

    Document produce( const Path & ) const;

    static Document produce( const PostSet &, Template * );

    void setup();

    static options_description * options();

private:
    static void fillInOtherPosts( std::shared_ptr<Node>,
				  const PostSet &,
				  const map< std::shared_ptr<Post>, Tag * > & );

private:
    Template * t;
};


#endif
