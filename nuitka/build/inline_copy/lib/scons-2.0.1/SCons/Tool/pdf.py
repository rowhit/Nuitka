#
#     Copyright 2011, Kay Hayen, mailto:kayhayen@gmx.de
#
#     Part of "Nuitka", an optimizing Python compiler that is compatible and
#     integrates with CPython, but also works on its own.
#
#     If you submit Kay Hayen patches to this software in either form, you
#     automatically grant him a copyright assignment to the code, or in the
#     alternative a BSD license to the code, should your jurisdiction prevent
#     this. Obviously it won't affect code that comes to him indirectly or
#     code you don't submit to him.
#
#     This is to reserve my ability to re-license the code at any time, e.g.
#     the PSF. With this version of Nuitka, using it for Closed Source will
#     not be allowed.
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, version 3 of the License.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#     Please leave the whole of this copyright notice intact.
#
"""SCons.Tool.pdf

Common PDF Builder definition for various other Tool modules that use it.
Add an explicit action to run epstopdf to convert .eps files to .pdf

"""

#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "src/engine/SCons/Tool/pdf.py 5134 2010/08/16 23:02:40 bdeegan"

import SCons.Builder
import SCons.Tool

PDFBuilder = None

EpsPdfAction = SCons.Action.Action('$EPSTOPDFCOM', '$EPSTOPDFCOMSTR')

def generate(env):
    try:
        env['BUILDERS']['PDF']
    except KeyError:
        global PDFBuilder
        if PDFBuilder is None:
            PDFBuilder = SCons.Builder.Builder(action = {},
                                               source_scanner = SCons.Tool.PDFLaTeXScanner,
                                               prefix = '$PDFPREFIX',
                                               suffix = '$PDFSUFFIX',
                                               emitter = {},
                                               source_ext_match = None,
                                               single_source=True)
        env['BUILDERS']['PDF'] = PDFBuilder

    env['PDFPREFIX'] = ''
    env['PDFSUFFIX'] = '.pdf'

# put the epstopdf builder in this routine so we can add it after 
# the pdftex builder so that one is the default for no source suffix
def generate2(env):
    bld = env['BUILDERS']['PDF']
    #bld.add_action('.ps',  EpsPdfAction) # this is covered by direct Ghostcript action in gs.py
    bld.add_action('.eps', EpsPdfAction)

    env['EPSTOPDF']      = 'epstopdf'
    env['EPSTOPDFFLAGS'] = SCons.Util.CLVar('')
    env['EPSTOPDFCOM']   = '$EPSTOPDF $EPSTOPDFFLAGS ${SOURCE} --outfile=${TARGET}'

def exists(env):
    # This only puts a skeleton Builder in place, so if someone
    # references this Tool directly, it's always "available."
    return 1

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4: