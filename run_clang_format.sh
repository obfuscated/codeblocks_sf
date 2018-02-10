#!/bin/bash
 
 listOfFolders=(src/include src/sdk src/src)
 for folder in ${listOfFolders[@]}; do
	echo "$folder"
	find $folder -type f | grep '\.c$\|\.cpp$\|\.h$' | xargs -i clang-format -i {}
 done
 
