// Copyright Arnt Gulbrandsen, arnt@gulbrandsen.priv.no.

#include "node.h"



/*! \class Node node.h
  
    The Node class provides a single node in a Document. It's
    effectively a struct: Everything is public for the moment. I don't
    know whether that will remain open.
*/


/*! Constructs an empty Junk node. */

Node::Node()
    : t( Junk )
{
}
