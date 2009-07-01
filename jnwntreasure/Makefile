init :
	rm -rf jar
	mkdir jar
	rm -rf docs
	mkdir docs
	rm -rf build
	mkdir -p build/classes

compile : init
	javac -classpath src src/jnwntreasure/JNWNTreasure.java -d build/classes

jar : compile 
	./create_jar.sh

javadoc : compile
	javadoc `find src -name "*.java"` -d docs

run : jar
	java -jar jar/jnwntreasure.jar
	
