#!/bin/bash

if [ ! -d ".old" ]
then 
	echo -e "\ncreata cartella .old\n"
	mkdir .old
fi


case $1 in

	main)
	echo -e "\nDownload di main.c in corso...\n"
	mv ./main.c ./.old/main-old.c
	curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/main.c
	;;

	nave)
	echo -e "\nDownload di nave.c in corso\n"
	mv ./nave.c ./.old/nave-old.c
	curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/nave.c
	;;

	porto)
	echo -e "\nDownload di porto.c in corso\n"
	mv ./porto.c ./.old/porto-old.c
	curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/porto.c
	;;

	utilities)
	mv ./utilities.c ./.old/utilities-old.c
	mv ./utilities.h ./.old/utilities-old.h
	echo -e "\nDownload di utilites.c e utilities.h in corso\n"
	curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/utilites.c
	curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/utilites.h
	;;

	*)
	echo -e "\nFile non esistente o scritto incorrettamente\n"
	;;

esac