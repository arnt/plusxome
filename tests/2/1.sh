#!/bin/sh
rm $(echo posts/*.npost | sed s/npost/post/g) 2>/dev/null
