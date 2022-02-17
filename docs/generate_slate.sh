#!/bin/bash
python generate.py
cp src/index.html.md slate/source/
cd slate
bundle config set deployment 'true'
bundle config path vendor/bundle
bundle install --jobs 4 --retry 3
bundle exec middleman build
cp -r build/* ..
rm -rf build
