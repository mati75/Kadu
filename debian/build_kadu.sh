#! /bin/sh

# When in clone of Git repository (see Vcs-Git in debian/control), one may use
# this script for building Kadu packages

set -e

n=$(expr $(head -1 debian/changelog | awk '{printf $2}' | /usr/bin/wc -c) - 1)
version=$(head -1 debian/changelog | awk '{printf $2}' | cut -b2-$n)
upstream_version=$(echo $version | awk -F "-" '{printf $1}')
debian_revision=$(echo $version | awk -F "-" '{printf $2}')

git clean -d -f -x
git reset --hard
git co 1.0/upstream-kadu
git br -D 1.0/master || true
git co -b 1.0/master 1.0/upstream-kadu
git merge -m "Merging 1.0/upstream-themes 1.0/debian" \
    1.0/upstream-themes 1.0/debian

if [ "$debian_revision" = 1 ]; then
    # Generate orig-externalthemes.tar.bz2, but only if it's first
    # Debian revision for particular upstream version
    tar c externalthemes | bzip2 -9 > ../kadu_$upstream_version.orig-externalthemes.tar.bz2
fi
debuild -us -uc -I.git $@

