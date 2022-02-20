#!/bin/bash
python generate.py
./generate_slate.sh
git config --local user.email "41898282+github-actions[bot]@users.noreply.github.com"
git config --local user.name "github-actions[bot]"
git add fonts images javascripts stylesheets
git commit -am "update slate[no ci]"
