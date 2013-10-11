// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "base.h"
#include "path.h"



class HttpServer
{
public:
    enum Operation { Get, Invalid };

    HttpServer( int );
    ~HttpServer() {}

    void operator()() { start(); }

    void start();

    string readRequest();
    void parseRequest( string );

    string body() const;
    Operation operation() const { return o; }
    Path path() const { return p; }

    void respond();
    void send( string );

    string httpResponse( int, const string &, const string &,
			 const string & = "" );

    void close();

    static void clearCache();

private:
    Path p;
    Operation o;
    int f;
};


#endif
