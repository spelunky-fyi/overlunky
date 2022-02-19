#!/bin/bash
python generate.py
./generate_slate.sh
git add fonts images javascripts stylesheets
git commit -am "update slate[no ci]"
