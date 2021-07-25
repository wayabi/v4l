#!/bin/sh

gen_arg_parse_h \
	ip_address,string,man \
	port,int,12345 \
	exposure,int,-1 \
	brightness,int,0 \
	contrast,int,50 \
	colour,int,0 \
	> src/arg_parse.h

