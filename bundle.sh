
QTDIR=$1
QTPLUGINPATH=${QTDIR}/plugins

rm -r lib &> /dev/null
rm -r platforms &> /dev/null
rm -r plugins &> /dev/null

mkdir lib
mkdir platforms
mkdir plugins

#ldd GeometryViewer | grep "Qt" | cut -d'>' -f2 | cut -d' ' -f2 | xargs -I % cp % lib/
ldd GeometryViewer | cut -d'>' -f2 | cut -d' ' -f2 | grep "/" | xargs -I % cp % lib/
find ${QTDIR}/lib -name libQt*XcbQ* | xargs -I % cp % lib/
find ${QTDIR}/lib -name "*Wayland*" | grep ".so"
cp ${QTPLUGINPATH}/platforms/*xcb* platforms/
cp ${QTPLUGINPATH}/platforms/*wayland* platforms/
cp -r ${QTPLUGINPATH}/wayland* plugins/
cp -r ${QTPLUGINPATH}/xcb* plugins/
cp -r ${QTPLUGINPATH}/platformthemes plugins/
cp -r ${QTPLUGINPATH}/platforminputcontexts plugins/

tar -cvf GeometryViewer.tar.gz GeometryViewer lib* platforms plugins
