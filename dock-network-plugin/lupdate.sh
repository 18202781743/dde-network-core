#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f dock-network-plugin.ts
lupdate ../ ../../src ../../common-plugin -ts -no-ui-lines -locations none -no-obsolete dock-network-plugin.ts
cd ../

tx push -s -b m23
