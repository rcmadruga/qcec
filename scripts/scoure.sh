#!/usr/bin/env bash
git clean -xdf
git submodule foreach git clean -xdf
