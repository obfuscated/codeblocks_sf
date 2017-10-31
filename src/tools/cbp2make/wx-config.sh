#!/bin/bash
opts="--local --config variable --add -set default -name wx"
cbp2make $opts -field lflags -value "`wx-config --libs`"
cbp2make $opts -field cflags -value "`wx-config --cflags`"
