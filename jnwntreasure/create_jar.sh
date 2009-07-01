#!/bin/sh

cd build/classes
mv jnwntreasure ../../jar
cd ../../jar
jar cmf ../META-INF/MANIFEST.MF jnwntreasure.jar jnwntreasure
rm -rf jnwntreasure
cd ..
