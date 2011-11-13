//
//     Copyright 2011, Kay Hayen, mailto:kayhayen@gmx.de
//
//     Part of "Nuitka", an optimizing Python compiler that is compatible and
//     integrates with CPython, but also works on its own.
//
//     If you submit Kay Hayen patches to this software in either form, you
//     automatically grant him a copyright assignment to the code, or in the
//     alternative a BSD license to the code, should your jurisdiction prevent
//     this. Obviously it won't affect code that comes to him indirectly or
//     code you don't submit to him.
//
//     This is to reserve my ability to re-license the code at any time, e.g.
//     the PSF. With this version of Nuitka, using it for Closed Source will
//     not be allowed.
//
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, version 3 of the License.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//     Please leave the whole of this copyright notice intact.
//
#ifndef __NUITKA_FRAMEGUARDS_H__
#define __NUITKA_FRAMEGUARDS_H__

inline static void assertCodeObject( PyCodeObject *code_object )
{
    assertObject( (PyObject *)code_object );
}

inline static void assertFrameObject( PyFrameObject *frame_object )
{
    assertObject( (PyObject *)frame_object );

    assertCodeObject( frame_object->f_code );
}

NUITKA_MAY_BE_UNUSED static PyFrameObject *INCREASE_REFCOUNT( PyFrameObject *frame_object )
{
    assertFrameObject( frame_object );

    Py_INCREF( frame_object );
    return frame_object;
}
inline static void popFrameStack( void )
{
    PyThreadState *tstate = PyThreadState_GET();

    PyFrameObject *old = tstate->frame;

#ifdef _DEBUG_REFRAME
    printf( "Taking off frame %s %s\n", PyString_AsString( PyObject_Str( (PyObject *)old ) ), PyString_AsString( PyObject_Str( (PyObject *)old->f_code ) ) );
#endif

    tstate->frame = old->f_back;

#ifdef _DEBUG_REFRAME
    printf( "Now at top frame %s %s\n", PyString_AsString( PyObject_Str( (PyObject *)tstate->frame ) ), PyString_AsString( PyObject_Str( (PyObject *)tstate->frame->f_code ) ) );
#endif
}

inline static void pushFrameStack( PyFrameObject *frame_object )
{
    PyThreadState *tstate = PyThreadState_GET();

    // Look at current frame.
    PyFrameObject *old = tstate->frame;

#ifdef _DEBUG_REFRAME
    printf( "Upstacking to frame %s %s\n", PyString_AsString( PyObject_Str( (PyObject *)old ) ), PyString_AsString( PyObject_Str( (PyObject *)old->f_code ) ) );
#endif

    // No recursion allowed of course, assert against it.
    assert( old != frame_object );

    // Push the new frame as the currently active one.
    tstate->frame = frame_object;

    // We don't allow touching cached frame objects where this is not true.
    assert( frame_object->f_back == NULL );

    if ( old != NULL )
    {
        assertFrameObject( old );
        frame_object->f_back = INCREASE_REFCOUNT( old );
    }

#ifdef _DEBUG_REFRAME
    printf( "Now at top frame %s %s\n", PyString_AsString( PyObject_Str( (PyObject *)tstate->frame ) ), PyString_AsString( PyObject_Str( (PyObject *)tstate->frame->f_code ) ) );
#endif
}

#ifdef _DEBUG_REFRAME
static inline void dumpFrameStack( void )
{
    PyFrameObject *current = PyThreadState_GET()->frame;
    int total = 0;

    while( current )
    {
        total++;
        current = current->f_back;
    }

    current = PyThreadState_GET()->frame;

    puts( ">--------->" );

    while( current )
    {
        printf( "Frame stack %d: %s %s\n", total--, PyString_AsString( PyObject_Str( (PyObject *)current ) ), PyString_AsString( PyObject_Str( (PyObject *)current->f_code ) ) );

        current = current->f_back;
    }

    puts( ">---------<" );
}
#endif

class FrameGuard
{
public:
    FrameGuard( PyFrameObject *frame_object )
    {
        assertFrameObject( frame_object );

        // Remember it.
        this->frame_object = frame_object;

        // Push the new frame as the currently active one.
        pushFrameStack( frame_object );

        // Keep the frame object alive for this C++ objects live time.
        Py_INCREF( frame_object );

#ifdef _DEBUG_REFRAME
        dumpFrameStack();
#endif
    }

    ~FrameGuard()
    {
        // Our frame should be on top.
        assert( PyThreadState_GET()->frame == this->frame_object );

        // Put the previous frame on top instead.
        popFrameStack();

        assert( PyThreadState_GET()->frame != this->frame_object );

        // Should still be good.
        assertFrameObject( this->frame_object );

        // Release the back reference immediately.
        Py_XDECREF( this->frame_object->f_back );
        this->frame_object->f_back = NULL;

        // Now release our frame object reference.
        Py_DECREF( this->frame_object );
    }

    PyFrameObject *getFrame() const
    {
        return INCREASE_REFCOUNT( this->frame_object );
    }

    PyFrameObject *getFrame0() const
    {
        assertFrameObject( this->frame_object );

        return this->frame_object;
    }

private:
    PyFrameObject *frame_object;
};

#endif