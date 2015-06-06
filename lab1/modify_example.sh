#!/bin/sh

#This sctipt is created to test modify.sh for all possible situations. 

echo "\tCreating files with differet names in tmp/ directory..."

if [ -d "tmp" ] || [ -d "TMP" ]; then
        rm -rf tmp
	rm -rf TMP
        mkdir "tmp" 
else
        mkdir "tmp"
fi

touch tmp/fileOne tmp/FILEWTO tmp/FIlefour tmp/filehous tmp/ss.sss*hey! tmp/soul\ keeper tmp/s12345 

ls -l tmp/ 

echo "\n\tChange tmp/fileOne and tmp/filehous to uppercase..."

./modify.sh -u tmp/fileOne tmp/filehous

ls -l tmp/ 

echo "\n\tChange all names in  tmp/ to lower case..."

./modify.sh -l tmp/*
ls -l tmp/ 

echo "\n\tCreation directy tree in tmp/..."

mkdir tmp/temp1 tmp/temp2 tmp/temp3 tmp/temp1/one tmp/temp1/two

ls --color -l -R tmp/ 
echo "\n\tAdding files to these directories..."
touch tmp/temp1/QWER tmp/temp1/TREWQ tmp/temp1/one/Mk,SA22 tmp/temp1/two/WOMEFILE

ls --color -l -R tmp/ 

echo "\n\t Recurtion uperrcasting of tmp/..."

./modify.sh -r -u tmp/

ls --color -l -R TMP/

echo "\n\t Ustin sed pattern..."
echo "\n\t Changin 'file' to '7731'..."

./modify.sh -r y/FILE/7731/ TMP/

ls --color -l -R TMP/
