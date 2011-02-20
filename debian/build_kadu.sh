#! /bin/sh

# When in clone of Git repository (see Vcs-Git in debian/control), one may use
# this script for building Kadu packages

set -e

git clean -d -f -x
git reset --hard
git co 0.6.6/upstream-kadu
git br -D 0.6.6/master || true
git co -b 0.6.6/master 0.6.6/upstream-kadu
git merge 0.6.6/upstream-modules 0.6.6/upstream-themes 0.6.6/debian
debuild -us -uc -I.git $@

