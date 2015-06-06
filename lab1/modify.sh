#!/bin/sh

#Pavel Khamutou

# Write a script modify with the following syntax:
 
#   modify [-r] [-l|-u] <dir/file names...>
#   modify [-r] <sed pattern> <dir/file names...>
#   modify [-h]
 
#   modify_examples
 
# which will modify file names. The script is dedicated to lowerizing (-l)
# file names, uppercasing (-u) file names or internally calling sed
# command with the given sed pattern which will operate on file names.
# Changes may be done either with recursion (-r) or without it. Write a
# second script, named modify_examples, which will lead the tester of the
# modify script through the typical, uncommon and even incorrect scenarios
# of usage of the modify script.


SAVEIFS=$IFS

#functions deffinition:
modifyFileName () {
	tmpForIFS=`echo $1 | grep " "`

	if [ "$tmpForIFS" != "" ]; then 	#if filename contains spaces
		echo "IFS is changed!"
		IFS=$'\n'			#change IFS to 'end of line'
	fi

	ORIG="$1"
	#echo "ORIG: $1"

	pathToFile=`dirname $1`
	#echo "pathToFile: $pathToFile"

	baseFileName=`basename "$1"`
	#echo "baseName: $baseFileName"

	if [ "$tmp_flag" = "-l" ]; then 	
		newFileName=`echo $baseFileName | tr '[:upper:]' '[:lower:]'`
	elif [ "$tmp_flag" = "-u" ]; then	
		newFileName=`echo $baseFileName | tr '[:lower:]' '[:upper:]'`
	elif [ "$sed_pattern" != "" ]; then
		newFileName=`echo "$baseFileName" |  sed -s $sed_pattern` 
	fi
	

	if [ "$baseFileName" != "$newFileName" ]; then
       		mv "$ORIG" "$pathToFile/$newFileName"
	       	echo "$ORIG -> $pathToFile/$newFileName"
	fi
	IFS=$SAVEIFS			#change IFS back to 'space'

}

printHelp () {
	cat << EOT 
	modify [-r] [-l|-u] <dir/file names...>
	modify [-r] <sed pattern> <dir/file names...>
	modify [-h]

	-r	recursion
	-l	lowerise
	-u	uppercast
	-h	help
EOT
}

h_flag=0
u_flag=0
r_flag=0
l_flag=0
sed_pattern=""

if [ "$#" -eq "0" ];
 then
	echo "No arguments! Pleace follow the instractions: "
	echo
	printHelp
	exit 0
fi

while [ "$#" -ne "0" ]; do
	case "$1" in
		"-h")
			h_flag=1
			break
			;;	
		"-l")
			l_flag=1
			shift
			;;
		"-u")
			u_flag=1
			shift
			;;
	
		"-r")
			r_flag=1
			shift
			;;
		*)
			#if [ -f "$1" ] || [ -d "$1" ]; then
			#	break
			#fi

			#sed_pattern=$1
			#shift
			#break
			if [ $u_flag -eq 0 ] || [ $l_flag -eq 0 ]; then
				if [ -f "$1" ] || [ -d "$1" ]; then
					break
				fi
			    	sed_pattern="$1"
          	        fi
			shift
			break
			;;
	esac	
done

#echo "Flags: h: $h_flag r: $r_flag l: $l_flag u: $u_flag"
#echo "Sed pattern: $sed_pattern"
#echo "\$1: $1"

if [ $h_flag -eq 1  ]; then
	printHelp
	exit 0		#leave script
fi
 
if [ $l_flag -eq 1 ] && [ $u_flag -eq 1 ]; then
	echo "\tYou cannot use both \"-l\" and \"-u\"!\n"
	printHelp
	exit 1
fi



if [ $l_flag -eq 0 ] && [ $u_flag -eq 0 ]; then
	if [ "$sed_pattern" = "" ]; then
		echo "No arguments."
		printHelp
		exit 1
	fi
fi

if [ "$sed_pattern" != "" ]; then
	tmp_flag=$sed_pattern
elif [ $l_flag -eq 1 ]; then
	tmp_flag="-l"
elif [ $u_flag -eq 1 ]; then
	tmp_flag="-u"
fi
#recutsion is here

if [ $r_flag -eq 1 ]; then
	
	ckDirExist=`ls -al $1 | grep '^d' | grep -v [.-..]`
	if [ "$ckDirExist" != "" ];then
		
	
		dir=`ls -ad $1*/`		
		#echo "dir -----> $dir"
		for i in $dir
		do	
			#echo "-----> i ----> $i"
			$0 -r $tmp_flag $i
		done
	fi
#	echo "RENAME: $1"

	#emptyDir=`ls -a $1 | grep -v '^\.\{1,2\}$'`
	#if [ "$emptyDir" = "" ]; then
	#	$0 $tmp_flag $1
	#else 
		$0 $tmp_flag $1*
	#fi
fi


#main loop

while [ "$#" -ne "0" ]; do
	if [ -e "$1" ]; then 
		#echo $1			
		modifyFileName "$1"	
	#else 
	#	echo "File \"$1\" doesn't exist!" 	
	fi
	shift
done
exit 0






 

