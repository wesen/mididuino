#!/bin/sh

#ant release
rm *zip *.tar.gz
cd dist
zip -r ../patchmanager-win.zip * -x \*.svn\*
tar zcvf ../patchmanager-linux.tar.gz --exclude \*.exe *
cd ../osx-dist
zip -r ../patchmanager-macosx.zip README.txt PatchManager.app PatchManagerJava5.app -x \*PatchManagerAdmin\* 
cd ..
