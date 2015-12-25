#! /bin/sh

# When in clone of Git repository (see Vcs-Git in debian/control), one may use
# this script for building Kadu packages

set -e

n=$(expr $(head -1 debian/changelog | awk '{printf $2}' | /usr/bin/wc -c) - 1)
version=$(head -1 debian/changelog | awk '{printf $2}' | cut -b2-$n)
upstream_version=$(echo $version | awk -F "-" '{printf $1}')
debian_revision=$(echo $version | awk -F "-" '{printf $2}')
version_override="no"

if [ $# = 1 ]; then
    # In case a branch name cannot contain a version (e.g. 3.0~rc1 -- no "~"
    # allowed as part of branch name, it's possible to override branch name with
    # first argument, but then passing "-S" won't work.
    upstream_version=$1
    version_override="yes"
fi

git clean -d -f -x
git reset --hard
git co ${upstream_version}/upstream-kadu
git br -D ${upstream_version}/master || true
git co -b ${upstream_version}/master ${upstream_version}/upstream-kadu
git merge -m "Merging ${upstream_version}/upstream-themes ${upstream_version}/debian" \
    ${upstream_version}/upstream-themes ${upstream_version}/debian

if [ "$debian_revision" = 1 ]; then
    # Generate orig-externalthemes.tar.bz2, but only if it's first
    # Debian revision for particular upstream version
    tar c externalthemes | bzip2 -9 > ../kadu_$upstream_version.orig-externalthemes.tar.bz2
    tar c externalplugins | bzip2 -9 > ../kadu_$upstream_version.orig-externalplugins.tar.bz2
fi

if [ "$version_override" = "no" ]; then
    debuild -us -uc -I.git $@
else
    echo "Now execute:"
    echo "debuild -us -uc -I.git"
fi

