#!/usr/bin/python3
import os

GENERATED_DIR = "src/generated"

PROTOGENREF = "ref"

Import("env")

env.Execute("$PYTHONEXE -m pip install nanopb")

if not os.path.exists(GENERATED_DIR):
    os.mkdir(GENERATED_DIR)

env.Execute("nanopb_generator -D src/generated --strip-path -I ../proto ../proto/"+PROTOGENREF+".proto")

# sudo apt install protobuf-compiler
# pip3 install nanopb
# nanopb_generator -D src/generated --strip-path -I ../proto ../proto/ref.proto