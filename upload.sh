#!/usr/bin/env bash
s3cmd -v -c ~/.s3cfg-me put --acl-public build/illudere.pbw s3://dmnd-public/pebble/
