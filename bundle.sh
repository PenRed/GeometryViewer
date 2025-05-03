
QTDIR=$1
QTPLUGINPATH=${QTDIR}/plugins

rm -r lib &> /dev/null
rm -r platforms &> /dev/null
rm -r plugins &> /dev/null

mkdir lib
mkdir platforms
mkdir plugins

# Copy ONLY Qt libraries that are actually used (via ldd), avoiding system libs
ldd GeometryViewer | awk '/=>/ {print $3}' | grep -i qt | while read -r lib; do
    cp "$lib" lib/
done

ldd GeometryViewer | cut -d'>' -f2 | cut -d' ' -f2 | grep "/" | xargs -I % cp % lib/
find ${QTDIR}/lib -name libQt*XcbQ* | xargs -I % cp % lib/
find ${QTDIR}/lib -name "*Wayland*" | grep ".so" | xargs -I % cp % lib/
find ${QTDIR}/lib -name "*OpenGL*" | grep ".so" | xargs -I % cp % lib/

cp ${QTPLUGINPATH}/platforms/*xcb* platforms/
cp ${QTPLUGINPATH}/platforms/*wayland* platforms/
cp -r ${QTPLUGINPATH}/wayland* plugins/
cp -r ${QTPLUGINPATH}/xcb* plugins/
cp -r ${QTPLUGINPATH}/platformthemes plugins/
cp -r ${QTPLUGINPATH}/platforminputcontexts plugins/

tar -cvf GeometryViewer.tar.gz GeometryViewer lib* platforms plugins
