#!/bin/sh

if [ ! -f configure ]; then
	autoreconf --force -i
fi

./configure $@
