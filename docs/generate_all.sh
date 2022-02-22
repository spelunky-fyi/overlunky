#!/bin/bash
# This is not for humans to run, only the workflow, as you can see it edits your git config to be a robot etc
python generate_emmylua.py
python generate.py
./generate_slate.sh
git config --local user.email "41898282+github-actions[bot]@users.noreply.github.com"
git config --local user.name "github-actions[bot]"
git add fonts images javascripts stylesheets
git commit -am "update slate[no ci]"
