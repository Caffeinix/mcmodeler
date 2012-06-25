#!/usr/bin/env python

import os
import os.path
import sys

copyright = """/* Copyright 2012 Brian Ellis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

"""

if not os.path.exists("src"):
  print "Please run this script from the root of the repository."
  sys.exit(1)

for (dirpath, dirnames, filenames) in os.walk('src'):
  for filename in filenames:
    path = os.path.join(dirpath, filename)
    if path.endswith('.h') or path.endswith('.cc'):
      with file(path, 'r') as original:
        content = original.read()
      if content.startswith('/*'):
        continue
      with file(path, 'w') as modified:
        modified.write(copyright + content)
