#! /bin/sh
# Generate macros for supplementary versions strings derived from scanning
# file - as specified from the first command parameter - for   RCS Id key word.
# 

if [ -z "$1" ]; then
    revision_string="-unknown"
    change_string=""
elif [ x"$1" = x"-" ]; then
    revision_string=""
    change_string=""
else
#
#retrieve CVS revision number from first RCS Id Keyword in file
#
    revision_string='+'`grep '\$Id: make_extra_version.sh,v 1.1 1999/06/30 17:18:32 he Exp $' < $1 | head -1 | sed 's/^.*,v //
s/ .*$//'`
#
# we should additionally determine whether the file has been changed since
# the last CVS update, which is not done yet
#
    change_string=""
fi

echo '#define E4L_EXTRA_VERSION_REV "'$revision_string'"'
echo '#define E4L_EXTRA_VERSION_CHANGED "'$change_string'"'


