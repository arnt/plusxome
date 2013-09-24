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

    std::shared_ptr<Node> node( const string & );
    std::shared_ptr<Node> rootNode();

    void parse( const std::string & );

    void setHttpResponseCode( int );
    int httpResponseCode() const;

    list< std::shared_ptr<Node> > getElementsByTag( const string & );

private:
    void populateIdMap();

private:
    std::shared_ptr<Node> root;
    map<string,std::shared_ptr<Node> > ids;
    int responseCode;
};

#endif
