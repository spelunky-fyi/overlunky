#!/bin/bash
# This is not for humans to run, only the workflow, as you can see it edits your git config to be a robot etc
set -euo pipefail

python generate_emmylua.py
python generate.py
./generate_slate.sh

git config --local user.email "41898282+github-actions[bot]@users.noreply.github.com"
git config --local user.name "github-actions[bot]"
git add fonts images javascripts stylesheets

if git diff --cached --quiet && git diff --quiet; then
    echo "Docs already up to date, nothing to commit."
else
    git commit -am "update slate[no ci]"
fi
