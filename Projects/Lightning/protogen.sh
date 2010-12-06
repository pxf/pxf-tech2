#!/bin/sh

cd Source/ClientLib/
protoc *.proto --cpp_out=.
mv *pb.h ../../Include/ClientLib
cd ../../

