#!/bin/sh

cd Source/ClientLib/
protoc *.proto --cpp_out=.
protoc *.proto --python_out=../Tracker/
mv *pb.h ../../Include/ClientLib
cd ../../



