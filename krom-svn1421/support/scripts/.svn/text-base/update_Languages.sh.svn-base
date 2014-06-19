#!/bin/bash
cd ../../src/NetPanzer
xgettext -d NetPanzer -C -k_ --from-code=UTF-8 -o ../../Languages/netPanzer.pot `find -name "*.cpp" -or -name "*.hpp" | sort`
echo "Update Template Done."

cd ../../Languages
for i in ??.po; do msgmerge -U  $i netPanzer.pot; done
echo "Update Done."
read touche
