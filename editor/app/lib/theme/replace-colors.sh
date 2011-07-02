#!/bin/sh

mkdir new
rm -rf new/*

for i in *gif; do echo $i;
    convert "$i" -fill "#a94f06" -opaque "#044f6f" new/$i;
    convert new/$i -fill "#ed9853" -opaque "#54919e" new/$i;
    convert new/$i -fill "#cd6f23" -opaque "#216886" new/$i;
    done

