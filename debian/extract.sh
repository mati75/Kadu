#! /bin/sh

# $1 - $(CURDIR)
# $2 - $(DEB_BUILDDIR)

for dir in "$1/$2/modules" "$1/$2/varia/themes/icons" "$1/$2/varia/themes/emoticons" ; do
	if [ ! -d "$dir" ]; then
		mkdir -p "$dir"
	fi
done

cd "$1/$2"

cd modules
for tarball in $1/external/modules/*; do
	tmp=${tarball%%.tar.*}
	mod_name=${tmp##*/}
	if [ ! -f "$mod_name"/spec ]; then
		if [ "${tarball##*.}" = "bz2" ]; then
			tar fxj "$tarball"
		else
			tar fxz "$tarball"
		fi
	fi
done

cd ../varia/themes/icons
for tarball in $1/external/themes/icons/*; do
	tmp=${tarball%%.tar.*}
	theme_name=${tmp##*/}
	if [ ! -f "$theme_name"/icons.conf ]; then
		if [ "${tarball##*.}" = "bz2" ]; then
			tar fxj "$tarball"
		else
			tar fxz "$tarball"
		fi
	fi
done

rm -f glass16/licence glass22/licence


cd ../emoticons
for tarball in $1/external/themes/emoticons/*; do
        tmp=${tarball%%.tar.*}
        theme_name=${tmp##*/}
        if [ ! -f "$theme_name/1/emots.txt" ]; then
	 	if [ "${tarball##*.}" = "bz2" ]; then
			tar fxj "$tarball"
		else
			tar fxz "$tarball"
		fi
	fi
done

rm -f tango/1/license.txt

