#! /bin/sh

if [ "$1" = "linux" ]; then
    sed -i 's/lib\/kadu/lib\/\*\/kadu/' debian/kadu.install
elif [ "$1" = "non-linux" ]; then
    sed -i 's/lib\/\*\/kadu/lib\/kadu/' debian/kadu.install
fi

