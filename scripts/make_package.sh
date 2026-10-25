#!/bin/sh
# Assemble the Max package folder and zip it.
# usage: scripts/make_package.sh <mac externals dir> <win externals dir> <out dir>
# Each externals dir holds naka.dmxusbpro.mxo / naka.dmxusbpro.mxe64; either may be empty.
set -eu

MAC=$1
WIN=$2
OUT=$3
NAME=naka.dmxusbpro
ROOT=$(cd "$(dirname "$0")/.." && pwd)

VERSION=$(sed -n 's/.*"version": *"\([^"]*\)".*/\1/p' "$ROOT/package-info.json")
SRC_VERSION=$(sed -n 's/^#define NAKA_DMXUSBPRO_VERSION "\(.*\)"/\1/p' \
    "$ROOT/source/projects/$NAME/$NAME.c")
if [ "$VERSION" != "$SRC_VERSION" ]; then
    echo "version mismatch: package-info.json $VERSION, source $SRC_VERSION" >&2
    exit 1
fi

PKG="$OUT/$NAME"
rm -rf "$PKG"
mkdir -p "$PKG/externals" "$PKG/help" "$PKG/docs"
cp "$ROOT/package-info.json" "$ROOT/LICENSE" "$ROOT/README.md" "$PKG/"
cp -R "$ROOT/help/." "$PKG/help/"
cp -R "$ROOT/docs/refpages" "$PKG/docs/"
# cp -R keeps the .mxo bundle's symlinks and executable bit
[ -d "$MAC/$NAME.mxo" ] && cp -R "$MAC/$NAME.mxo" "$PKG/externals/"
[ -f "$WIN/$NAME.mxe64" ] && cp "$WIN/$NAME.mxe64" "$PKG/externals/"
if [ ! -d "$PKG/externals/$NAME.mxo" ] || [ ! -f "$PKG/externals/$NAME.mxe64" ]; then
    echo "warning: package is missing a platform build" >&2
fi

ZIP="$OUT/$NAME-$VERSION.zip"
rm -f "$ZIP"
if command -v ditto >/dev/null 2>&1; then
    ditto -c -k --keepParent "$PKG" "$ZIP"
else
    (cd "$OUT" && zip -qry "$NAME-$VERSION.zip" "$NAME")
fi
echo "$ZIP"
