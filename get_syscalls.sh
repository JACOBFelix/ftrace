#!/bin/bash

# cat syscallent.h | grep "=" | sed "s/\[/ /g" | sed "s/\]/ /g" | awk '{print "{ " $1 ", " $7 ", " $4 "}"}' | sed "s/\"}/\"/g" | sed "s/,}/ },/g" | sed "s/,,/,/g"

# cat /usr/include/asm/unistd_64.h | cut -d ' ' -f 2,3 | grep __NR_ | sed "s/ /\", /g" | sed "s/__NR_/{ \"/g" | sed ':a;N;$!ba;s/\n/ },\n/g'

TMP_DIR="tmp"
FILE_SYSCALLS="srcs/nums_syscalls.c"
FILE_TYPES="includes/types.h"
TEMPLATE_SYSCALLS="templates/syscalls_base.h"
TEMPLATE_TYPES="templates/types_base.h"
NAME_TAB="g_syscalls"
NAME_SIZE_TAB="g_size_tab"
ARCH_32BITS="0"

if [ $# -lt 1 ]; then
    echo "$0 /PATH/UNISTD_X.h"
    exit -1
fi

if [ $(basename "$1") == "unistd_32.h" ]; then
    FILE_SYSCALLS="srcs/nums_syscalls32.c"
    FILE_TYPES="includes/types32.h"
    TEMPLATE_SYSCALLS="templates/syscalls32_base.h"
    TEMPLATE_TYPES="templates/types32_base.h"
    NAME_TAB="g_syscalls32"
    NAME_SIZE_TAB="g_size_tab32"
    ARCH_32BITS="1"
fi

cd `dirname $0`

mkdir -p $TMP_DIR
cat "$1" | cut -d ' ' -f 2 | grep __NR_ | sed "s/ /\", /g" | sed "s/__NR_//g" > $TMP_DIR/tmp_syscalls
# cat /usr/include/asm/unistd_64.h | cut -d ' ' -f 2 | grep __NR_ | sed "s/ /\", /g" | sed "s/__NR_//g" > $TMP_DIR/tmp_syscalls

echo "Analysing man pages..."
for line in $(cat $TMP_DIR/tmp_syscalls);
do
    echo -en "\r\033[K$line"
    man 2 $line > $TMP_DIR/$line.man 2>/dev/null
    if [ ! -s $TMP_DIR/$line.man ]; then
	echo -n ""
#       echo "Can't find man"
    else
	BEGIN_SYNOPSIS=$(cat $TMP_DIR/$line.man | grep -n "SYNOPSIS" | cut -d ':' -f1)
	END_SYNOPSIS=$(cat $TMP_DIR/$line.man | grep -n "DESCRIPTION" | cut -d ':' -f1)
	BEGIN_SYNOPSIS=$(echo $BEGIN_SYNOPSIS | cut -d ' ' -f1)
	END_SYNOPSIS=$(echo $END_SYNOPSIS | cut -d ' ' -f1)

	BEGIN_PROTO=$(cat $TMP_DIR/$line.man | sed -n "$BEGIN_SYNOPSIS, $END_SYNOPSIS p" | grep "[a-z+] $line" -n | cut -d ':' -f1)
	BEGIN_PROTO=$(echo $BEGIN_PROTO | cut -d ' ' -f1)
	if [ ! $BEGIN_PROTO ]; then
	    BEGIN_PROTO=$(cat $TMP_DIR/$line.man | sed -n "$BEGIN_SYNOPSIS, $END_SYNOPSIS p" | grep "[a-z+] [*?]$line" -n | cut -d ':' -f1)
	fi

	if [ ! $BEGIN_PROTO ]; then
	    echo -n ""
	    # echo "Can't find proto"
	else
	    END_PROTO=$(cat $TMP_DIR/$line.man | sed -n "$BEGIN_SYNOPSIS, $END_SYNOPSIS p" | sed -n "$BEGIN_PROTO, $END_SYNOPSIS p" | grep ";" -n | cut -d ':' -f1)
	    BEGIN_PROTO=$(echo $BEGIN_PROTO | cut -d ' ' -f1)
	    END_PROTO=$(echo $END_PROTO | cut -d ' ' -f1)

	    END_PROTO=$(($BEGIN_PROTO + $END_PROTO - 1))

	    if [ $BEGIN_PROTO ]; then
	    	PROTOTYPE=$(cat $TMP_DIR/$line.man | sed -n "$BEGIN_SYNOPSIS, $END_SYNOPSIS p" | sed -n "$BEGIN_PROTO, $END_PROTO p" | sed ':a;N;$!ba;s/\n//g' | xargs)

		if [ "$PROTOTYPE" ]; then
		    echo "$PROTOTYPE" > $TMP_DIR/$line.proto
		fi
	    else
		echo -n ""
	    	# echo "PAS TROUVER"
	    fi

	fi

    fi

    rm -f $TMP_DIR/$line.man

done

echo -e "\r\033[Kdone"

cat "$1" | cut -d ' ' -f 2,3 | grep __NR_ | sed "s/ /+/g" | sed "s/__NR_//g" > $TMP_DIR/tmp_sysnums
# cat /usr/include/asm/unistd_64.h | cut -d ' ' -f 2,3 | grep __NR_ | sed "s/ /+/g" | sed "s/__NR_//g" > $TMP_DIR/tmp_sysnums

ALL_TYPES=""

cp $TEMPLATE_SYSCALLS  $FILE_SYSCALLS
cp $TEMPLATE_TYPES $FILE_TYPES
echo "" >> $FILE_TYPES

echo "Generating prototypes..."
for line in $(cat $TMP_DIR/tmp_sysnums);
do
    TYPE_RETURN="0"
    PARAM1="0"
    PARAM2=$PARAM1
    PARAM3=$PARAM1
    PARAM4=$PARAM1
    PARAM5=$PARAM1
    PARAM6=$PARAM1

    SYSCALL=$(echo $line | cut -d '+' -f1)
    NUM_SYSCALL=$(echo $line | cut -d '+' -f2)

    echo -en "\r\033[K$SYSCALL"

    if [ ! -e $TMP_DIR/$SYSCALL.proto ] || [ $SYSCALL == "vm86" ]; then

	printf "  { %d, \"%s\", %d, %d, %s, %s, %s, %s, %s, %s, %s },\n" "$NUM_SYSCALL" "$SYSCALL" "0" "6" "$TYPE_RETURN" "$PARAM1" "$PARAM2" "$PARAM3" "$PARAM4" "$PARAM5" "$PARAM6"  >> $FILE_SYSCALLS

    else
	TYPE_RETURN=$(cat $TMP_DIR/$SYSCALL.proto | awk -F "$SYSCALL[(]" '{print $1}' | xargs | tr '[:lower:]' '[:upper:]')

	if [ "$TYPE_RETURN" = "CHAR *" ]; then
	    TYPE_RETURN="PCHAR"
	elif [ "$TYPE_RETURN" = "VOID *" ]; then
	    TYPE_RETURN="PVOID"
	fi

	TYPE_RETURN=$(echo $TYPE_RETURN | sed "s/ /_/g" | sed "s/,//g")

	if [ "$ARCH_32BITS" -eq "1" ]; then
	    TYPE_RETURN="X32$TYPE_RETURN"
	fi

	if [[ "$ALL_TYPES" == *" $TYPE_RETURN "* ]] ; then
	    echo -n ""
	else
	    ALL_TYPES=" $ALL_TYPES $TYPE_RETURN "
	fi

	PARAMS=$(cat $TMP_DIR/$SYSCALL.proto | awk -F "$SYSCALL" '{ $1=""; print}' | xargs | sed "s/(//g" | sed "s/);*//g" | sed '/\/\*.*\*\// d; /\/\*/,/\*\// d' | sed "s/, /;/g")

	IFS=";" read -a array <<< "$PARAMS"

	for index in "${!array[@]}"
	do
	    TYPE=$(echo "${array[index]}" | sed "s/ \*/ \* /g" | rev | cut -d ' ' -f2- | rev)
	    TYPE=$(echo "$TYPE" | tr '[:lower:]' '[:upper:]' | sed "s/ /_/g" | sed "s/\*/P/g")

	    if [[ "$TYPE" == "..." ]] ; then
	    	TYPE="VARGS"
	    fi

	    if [ "$ARCH_32BITS" -eq "1" ]; then
		TYPE="X32$TYPE"
	    fi

	    case $index in
		0)
		    PARAM1=$TYPE;;
		1)
		    PARAM2=$TYPE;;
		2)
		    PARAM3=$TYPE;;
		3)
		    PARAM4=$TYPE;;
		4)
		    PARAM5=$TYPE;;
		5)
		    PARAM6=$TYPE;;
	    esac

	    if [[ "$ALL_TYPES" == *" $TYPE "* ]] ; then
		echo -n ""
	    else
		ALL_TYPES=" $ALL_TYPES $TYPE "
	    fi

	done

	printf "  { %d, \"%s\", %d, %d, %s, %s,\n    %s, %s, %s,\n    %s, %s },\n" "$NUM_SYSCALL" "$SYSCALL" "1" "${#array[@]}" "$TYPE_RETURN" "$PARAM1" "$PARAM2" "$PARAM3" "$PARAM4" "$PARAM5" "$PARAM6" >> $FILE_SYSCALLS

    fi

done

I=1
if [ "$ARCH_32BITS" -eq "1" ]; then
    I=1001
fi
for typeid in $ALL_TYPES;
do
    echo -e "# define $typeid\t($I)" >> $FILE_TYPES
    I=$((I+1))
done

echo -e "\r\033[Kdone"
if [ "$ARCH_32BITS" -eq "1" ]; then
    echo -e "\n#endif /* !TYPES32_H_ */" >> $FILE_TYPES
else
    echo -e "\n#endif /* !TYPES_H_ */" >> $FILE_TYPES
fi

echo -e "};\n\nconst size_t\t$NAME_SIZE_TAB = sizeof($NAME_TAB) / sizeof($NAME_TAB[0]);\n" >> $FILE_SYSCALLS
# if [ "$ARCH_32BITS" -eq "1" ]; then
#     echo -e "\n#endif /* !SYSCALLS32_H_ */" >> $FILE_SYSCALLS
# else
#     echo -e "\n#endif /* !SYSCALLS_H_ */" >> $FILE_SYSCALLS
# fi
#echo -e "};\n\n#endif /* !SYSCALL_H_ */" >> $FILE_SYSCALLS

rm -fr $TMP_DIR
