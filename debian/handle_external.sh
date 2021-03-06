#! /bin/sh

if [ "$1" = "copy" ]; then
    for i in externalplugins/*; do
        cp -r $i plugins
    done

    for i in externalthemes/emoticons/*; do
        cp -r $i varia/themes/emoticons
    done
    for i in externalthemes/sounds/*; do
        cp -r $i varia/themes/sounds
    done
elif [ "$1" = "remove" ]; then
    for i in externalplugins/*; do
        rm -rf plugins/${i##*/}
    done

    for i in externalthemes/emoticons/*; do
        rm -rf varia/themes/emoticons/${i##*/}
    done
    for i in externalthemes/sounds/*; do
        rm -rf varia/themes/sounds/${i##*/}
    done
fi
