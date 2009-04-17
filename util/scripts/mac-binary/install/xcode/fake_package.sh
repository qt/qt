#!/bin/sh

#for now
VERSION_MAJOR=4
VERSION_MINOR=0
BINDIR="$QTDIR"

translate_cp()
{
    DO_TRANSLATE="$PWD/do_translate.sh"
    if [ -d "$1" ]; then
	(cd "$1" && find . -type f -exec "$DO_TRANSLATE" {} "$2" \;)
    else
	"$DO_TRANSLATE" "$1" "$2"
    fi
}

make_link()
{
    base=`basename "$1"`
    rm -rf "$2/$base"
    mkdir -p "$2"
    ln -sf "$1" "$2"
}

#copy the templates
rm -rf "$PWD/.faked_integration"
mkdir -p "$PWD/.faked_integration"
translate_cp integration/templates "$PWD/.faked_integration"
(cd "$PWD/.faked_integration/CustomDataViews/QtDataFormatters.bundle/Contents/MacOS" 
chmod 755 ./build_bundle.sh
QTDIR="$BINDIR" ./build_bundle.sh)
#links
templ_dir="/Library/Application Support/Apple/Developer Tools/"
make_link "$PWD/.faked_integration/CustomDataViews/QtDataFormatters.bundle" "$templ_dir/CustomDataViews"
make_link "$PWD/.faked_integration/Project Templates/Application/Qt Application" "$templ_dir/Project Templates/Application"
make_link "$PWD/.faked_integration/File Templates/Qt" "$templ_dir/File Templates"
make_link "$PWD/.faked_integration/Scripts/999-Qt" "$templ_dir/Scripts"
for a in `find "$PWD/.faked_integration/Specifications/" -name *.pb*spec`; do
    make_link "$a" "$templ_dir/Specifications"
done

#copy the scripts
SCRPT_DIR="/Library/Frameworks/QtCore.framework/Versions/${VERSION_MAJOR}.${VERSION_MINOR}/Resources/xcode"
mkdir -p "$SCRPT_DIR"
for script in integration/scripts/*; do
    make_link "$PWD/$script" "$SCRPT_DIR"
done
ln -sf "Versions/${VERSION_MAJOR}.${VERSION_MINOR}/Resources" "/Library/Frameworks/QtCore.framework/Resources"
