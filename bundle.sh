
QT5DIR=$1
QT5PLUGINPATH=${QT5DIR}/plugins

rm -r lib &> /dev/null
rm -r platforms &> /dev/null
rm -r plugins &> /dev/null

mkdir lib
mkdir platforms
mkdir plugins

#ldd GeometryViewer | grep "Qt" | cut -d'>' -f2 | cut -d' ' -f2 | xargs -I % cp % lib/
ldd GeometryViewer | cut -d'>' -f2 | cut -d' ' -f2 | grep "/" | xargs -I % cp % lib/
find ${QT5DIR}/lib -name libQt*XcbQ* | xargs -I % cp % lib/
find ${QT5DIR}/lib -name "*Wayland*" | grep ".so"
cp ${QT5PLUGINPATH}/platforms/*xcb* platforms/
cp ${QT5PLUGINPATH}/platforms/*wayland* platforms/
cp -r ${QT5PLUGINPATH}/wayland* plugins/
cp -r ${QT5PLUGINPATH}/xcb* plugins/
cp -r ${QT5PLUGINPATH}/platformthemes plugins/
cp -r ${QT5PLUGINPATH}/platforminputcontexts plugins/

tar -cvf GeometryViewer.tar.gz GeometryViewer lib* platforms plugins
