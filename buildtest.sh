#!/bin/bash
#===============================================================================
#
#          FILE:  build.sh
# 
#         USAGE:  ./build.sh 
# 
#   DESCRIPTION:  
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR:   (), 
#       COMPANY:  
#       VERSION:  1.0
#       CREATED:  Thursday 14 June 2007 10:11:28  IST IST
#      REVISION:  ---
#===============================================================================

BaseDir=/home/ndalal/learcode
LibDir=/home/ndalal/learbuild/lib

g++  -O3 -funroll-loops -fomit-frame-pointer -Wall -W -pipe   -march=pentium-m  -DUSE_SSE  -o test_library $BaseDir/app/test_library.cpp -I$BaseDir -L$LibDir -lcmdline -lcvip -L$LibDir/classifier -lclassifier -llearutil  -L/usr/local/lib -lblitz -lboost_program_options-gcc-1_33_1 -lboost_filesystem-gcc-1_33_1 -lboost_date_time-gcc-1_33_1  -L/usr/lib -lImlib2 -lfreetype -lz -L/usr/X11R6/lib -lX11 -lXext -ldl -lm 
