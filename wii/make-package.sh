#! /bin/sh

set -e

NAME=chromono
TMPDIR="$(mktemp -d)"
OUTDIR="$TMPDIR/$NAME"
SRCDIR="$(pwd)"

mkdir -p "$OUTDIR"
cp -a \
    wii/meta.xml \
    wii/icon.png \
    "$OUTDIR"
elf2dol "build/$NAME.elf" "$OUTDIR/boot.dol"

cd $TMPDIR
zip -r "$NAME.zip" "$NAME"
cd -
mv "$TMPDIR/$NAME.zip" .
rm -rf "$TMPDIR"
