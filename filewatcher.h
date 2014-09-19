// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include "base.h"


class FileWatcher {
public:
    FileWatcher();

    void operator()() { start(); } // what boost::thread wants

    void start();

    void processPaths( const set<string> & );

private:
    void addWatch( const string & );
    void scanOnce();

private:
    int fd;
    map<int, string> watches;
};

#endif
