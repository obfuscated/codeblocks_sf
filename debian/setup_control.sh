#!/bin/sh

# Run this in the root of the project. The current working dir should have
# the "debian" folder in it.

# Generate the debian/control file using debian/control.in as template.
# You can generate different control files for different Debian versions.
# The debian version is passed as argument or it is detected to match the
# running system. Pass "detect" as first argument if you want to force an
# explicit detection.

if [ "$#" -ne 1 ] || [ -z "${1##*detect*}" ]; then
    echo "Doing version detection"
    version=`cat /etc/debian_version`
else
    version=$1
fi

echo "Debian version: $version"

if [ -z "${version##*squeeze*}" ] || \
   [ -z "${version##*wheezy*}" ] || \
   [ -z "${version##*jessie*}" ] || \
   [ -z "${version##*buster*}" ] || \
   [ -z "${version##*stretch*}" ];
then
    echo "Distro is matching wxGTK 3.0 + GTK2"
    CB_WXGTK_DEPS=libwxgtk3.0-dev
    CB_GTK_DEPS=libgtk2.0-dev
else
    echo "Distro is matching wxGTK 3.0 + GTK3"
    CB_WXGTK_DEPS=libwxgtk3.0-gtk3-dev
    CB_GTK_DEPS=libgtk-3-dev
fi

echo "Setting dependencies to '$CB_WXGTK_DEPS $CB_GTK_DEPS' "

echo "Creating the debian/control file"
sed -e "s/@CB_WXGTK_DEPS@/$CB_WXGTK_DEPS/g" \
    -e "s/@CB_GTK_DEPS@/$CB_GTK_DEPS/g" \
    debian/control.in > debian/control
