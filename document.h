// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "node.h"

#include "base.h"


class Document {
public:
    Document();
    Document( const std::string & );
    Document( const Document & other );

    Document & operator=( const Document & other );

    Node & node( const string & );
    boost::shared_ptr<Node> rootNode();

    void parse( const std::string & );

private:
    void populateIdMap();

private:
    boost::shared_ptr<Node> root;
    std::map<std::string,boost::shared_ptr<Node> > ids;
};

#endif
