#!/bin/bash
set -x
echo "If .md files do not show in display format (ie show only raw .md":
echo "pip3 install markdown"
cd ../
python3 MyProgrammingSetup/DocumentServer/md_server.py
