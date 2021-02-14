#!/bin/python3

import os;
import sys;
import subprocess;
from fnmatch import fnmatch;

for root, dirs, files in os.walk('../../'):
    for f in files:
        if len(sys.argv) > 1 and not fnmatch(f, sys.argv[1]):
            continue;

        print(f"Running {f}")
        print(f"{os.path.join(root,f)}")
        r = subprocess.run(["../../../target/src/xwim", os.path.join(root, f)], capture_output=True, encoding='utf-8')
        print(f"{r.stdout}")
        print(f"{r.stderr}", file=sys.stderr)
