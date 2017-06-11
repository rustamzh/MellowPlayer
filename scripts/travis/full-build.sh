#!/usr/bin/env bash
# Compile in release mode, run all tests and create a package and upload it to a github release.
echo "*************************** Performing a FULL build"

tagName='Continuous';
if [[ -n "$TRAVIS_TAG" ]]; then
    tagname = "$TRAVIS_TAG";
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  # build and run tests
  ./scripts/build/linux/build.sh;
  ./scripts/build/linux/runtests.sh;
  # create package
  ./scripts/packaging/make_appimage.sh /opt/qt59;
fi

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  ./scripts/build/osx/build.sh --release;
  ./scripts/build/osx/runtests.sh --release;
  # todo: create dmg image...
fi

python ./scripts/travis/upload.py ${tagName} dist/*
