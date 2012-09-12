# $Id$
## @file
# Build.py = build procedure
#

#
# Copyright (C) 2012 Oracle Corporation
#
# This file is part of VirtualBox Open Source Edition (OSE), as
# available from http://www.virtualbox.org. This file is free software;
# you can redistribute it and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software
# Foundation, in version 2 as it comes in the "COPYING" file of the
# VirtualBox OSE distribution. VirtualBox OSE is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
#

## @file
#  Automate the process of building the various reset vector types
#
#  Copyright (c) 2009, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

import glob
import os
import subprocess
import sys

def RunCommand(commandLine):
    print ' '.join(commandLine)
    return subprocess.call(commandLine)

for filename in glob.glob(os.path.join('Bin', '*.raw')):
    os.remove(filename)

for arch in ('ia32', 'x64'):
    for debugType in (None, 'port80', 'serial'):
        output = os.path.join('Bin', 'ResetVector')
        output += '.' + arch
        if debugType is not None:
            output += '.' + debugType
        output += '.raw'
        commandLine = (
            'nasm',
            '-D', 'ARCH_%s' % arch.upper(),
            '-D', 'DEBUG_%s' % str(debugType).upper(),
            '-DVBOX',
            '-I ../../../VBoxPkg/Include/',
            '-o', output,
            'ResetVectorCode.asm',
            )
        ret = RunCommand(commandLine)
        print '\tASM\t' + output
        if ret != 0: sys.exit(ret)

        commandLine = (
            'python',
            'Tools/FixupForRawSection.py',
            output,
            )
        print '\tFIXUP\t' + output
        ret = RunCommand(commandLine)
        if ret != 0: sys.exit(ret)

