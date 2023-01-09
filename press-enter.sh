#!/bin/bash
echo -e "Press [ENTER] to continue\n"
	answer=x
	until [ -z "$answer" ]; do
	echo -e "\033[2A"	
	read answer
	echo -e "\033[1A                                                                              "	
	done
