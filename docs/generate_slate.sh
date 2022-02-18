#!/bin/bash
rm -rf fonts images javascripts stylesheets
cp -r src/* slate/source/
cd slate
bundle config set deployment 'true'
bundle config path vendor/bundle
bundle install --jobs 4 --retry 3
bundle exec middleman build
cp -r build/* ..
rm -rf build
cd ..
#git add fonts images javascripts stylesheets
#git commit -am "update slate[no ci]"
