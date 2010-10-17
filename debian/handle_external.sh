#! /bin/sh

if [ "$1" = "copy" ]; then
    for i in externalmodules/*; do
        cp -r $i modules
    done
elif [ "$1" = "remove" ]; then
    for i in externalmodules/*; do
        rm -rf modules/${i##*/}
    done
fi
