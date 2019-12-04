// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 1 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 1; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 1 * sizeof(PyObject *) );
            memcpy( python_pars + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 1 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 1 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 1; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 1 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 1 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 1 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 1, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 1 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 2 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 2; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 2 * sizeof(PyObject *) );
            memcpy( python_pars + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 2 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 2 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 2; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 2 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 2 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 2 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 2, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 2 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 3 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 3; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 3 * sizeof(PyObject *) );
            memcpy( python_pars + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 3 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 3 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 3; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 3 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 3 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 3 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 3, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 3 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 4; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 4 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 4; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 4 * sizeof(PyObject *) );
            memcpy( python_pars + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 4 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 4 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 4; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 4 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 4 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 4 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 4, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 4 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 5; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 5 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 5; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 5 * sizeof(PyObject *) );
            memcpy( python_pars + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 5 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 5 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 5; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 5 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 5 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 5 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 5, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 5 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );
            return CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS1( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            }
            else if ( descr_get != NULL )
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1( method, args );
                Py_DECREF( method );
                return result;
            }
            else
            {
                return CALL_FUNCTION_WITH_ARGS1( called_object, args );
            }

        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
MOD_INIT_DECL( __main__ );
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    { (char *)"__main__", MOD_INIT_NAME( __main__ ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"_bsddb", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_cn", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_hk", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_iso2022", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_jp", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_kr", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_tw", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_csv", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses_panel", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hotshot", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_json", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lsprof", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multibytecodec", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"dbm", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"mmap", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"parser", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"readline", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"resource", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"termios", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"BaseHTTPServer", NULL, 1003, 21674, NUITKA_BYTECODE_FLAG },
    { (char *)"Bastion", NULL, 22677, 6622, NUITKA_BYTECODE_FLAG },
    { (char *)"CGIHTTPServer", NULL, 29299, 10984, NUITKA_BYTECODE_FLAG },
    { (char *)"Canvas", NULL, 40283, 15396, NUITKA_BYTECODE_FLAG },
    { (char *)"ConfigParser", NULL, 55679, 25087, NUITKA_BYTECODE_FLAG },
    { (char *)"Cookie", NULL, 80766, 22574, NUITKA_BYTECODE_FLAG },
    { (char *)"Dialog", NULL, 103340, 1902, NUITKA_BYTECODE_FLAG },
    { (char *)"DocXMLRPCServer", NULL, 105242, 10149, NUITKA_BYTECODE_FLAG },
    { (char *)"FileDialog", NULL, 115391, 9666, NUITKA_BYTECODE_FLAG },
    { (char *)"FixTk", NULL, 125057, 2078, NUITKA_BYTECODE_FLAG },
    { (char *)"HTMLParser", NULL, 127135, 13655, NUITKA_BYTECODE_FLAG },
    { (char *)"MimeWriter", NULL, 140790, 7338, NUITKA_BYTECODE_FLAG },
    { (char *)"Queue", NULL, 148128, 9360, NUITKA_BYTECODE_FLAG },
    { (char *)"ScrolledText", NULL, 157488, 2646, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleDialog", NULL, 160134, 4309, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleHTTPServer", NULL, 164443, 7980, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleXMLRPCServer", NULL, 172423, 22783, NUITKA_BYTECODE_FLAG },
    { (char *)"SocketServer", NULL, 195206, 23948, NUITKA_BYTECODE_FLAG },
    { (char *)"StringIO", NULL, 219154, 11434, NUITKA_BYTECODE_FLAG },
    { (char *)"Tix", NULL, 230588, 95416, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkconstants", NULL, 326004, 2236, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkdnd", NULL, 328240, 12765, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkinter", NULL, 341005, 199207, NUITKA_BYTECODE_FLAG },
    { (char *)"UserDict", NULL, 540212, 9613, NUITKA_BYTECODE_FLAG },
    { (char *)"UserList", NULL, 549825, 6501, NUITKA_BYTECODE_FLAG },
    { (char *)"UserString", NULL, 556326, 14720, NUITKA_BYTECODE_FLAG },
    { (char *)"_LWPCookieJar", NULL, 571046, 5414, NUITKA_BYTECODE_FLAG },
    { (char *)"_MozillaCookieJar", NULL, 576460, 4445, NUITKA_BYTECODE_FLAG },
    { (char *)"__future__", NULL, 580905, 4203, NUITKA_BYTECODE_FLAG },
    { (char *)"_abcoll", NULL, 585108, 25466, NUITKA_BYTECODE_FLAG },
    { (char *)"_osx_support", NULL, 610574, 11712, NUITKA_BYTECODE_FLAG },
    { (char *)"_pyio", NULL, 622286, 64343, NUITKA_BYTECODE_FLAG },
    { (char *)"_strptime", NULL, 686629, 15124, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata", NULL, 701753, 271, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata_nd", NULL, 702024, 20828, NUITKA_BYTECODE_FLAG },
    { (char *)"_threading_local", NULL, 722852, 6347, NUITKA_BYTECODE_FLAG },
    { (char *)"_weakrefset", NULL, 729199, 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"abc", NULL, 738773, 6113, NUITKA_BYTECODE_FLAG },
    { (char *)"aifc", NULL, 744886, 30305, NUITKA_BYTECODE_FLAG },
    { (char *)"anydbm", NULL, 775191, 2786, NUITKA_BYTECODE_FLAG },
    { (char *)"argparse", NULL, 777977, 64049, NUITKA_BYTECODE_FLAG },
    { (char *)"ast", NULL, 842026, 12882, NUITKA_BYTECODE_FLAG },
    { (char *)"asynchat", NULL, 854908, 8742, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncore", NULL, 863650, 18767, NUITKA_BYTECODE_FLAG },
    { (char *)"atexit", NULL, 882417, 2183, NUITKA_BYTECODE_FLAG },
    { (char *)"audiodev", NULL, 884600, 8407, NUITKA_BYTECODE_FLAG },
    { (char *)"bdb", NULL, 893007, 18971, NUITKA_BYTECODE_FLAG },
    { (char *)"binhex", NULL, 911978, 15350, NUITKA_BYTECODE_FLAG },
    { (char *)"bisect", NULL, 927328, 3053, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb", NULL, 930381, 12368, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"bsddb.db", NULL, 942749, 582, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbobj", NULL, 943331, 18601, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbrecio", NULL, 961932, 5269, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbshelve", NULL, 967201, 12910, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbtables", NULL, 980111, 24402, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbutils", NULL, 1004513, 1615, NUITKA_BYTECODE_FLAG },
    { (char *)"cProfile", NULL, 1006128, 6263, NUITKA_BYTECODE_FLAG },
    { (char *)"calendar", NULL, 1012391, 27747, NUITKA_BYTECODE_FLAG },
    { (char *)"cgi", NULL, 1040138, 32417, NUITKA_BYTECODE_FLAG },
    { (char *)"cgitb", NULL, 1072555, 12094, NUITKA_BYTECODE_FLAG },
    { (char *)"chunk", NULL, 1084649, 5572, NUITKA_BYTECODE_FLAG },
    { (char *)"cmd", NULL, 1090221, 13989, NUITKA_BYTECODE_FLAG },
    { (char *)"code", NULL, 1104210, 10294, NUITKA_BYTECODE_FLAG },
    { (char *)"codeop", NULL, 1114504, 6569, NUITKA_BYTECODE_FLAG },
    { (char *)"collections", NULL, 1121073, 26051, NUITKA_BYTECODE_FLAG },
    { (char *)"colorsys", NULL, 1147124, 3967, NUITKA_BYTECODE_FLAG },
    { (char *)"commands", NULL, 1151091, 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"compileall", NULL, 1153540, 6997, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler", NULL, 1160537, 1287, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"compiler.ast", NULL, 1161824, 71339, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.consts", NULL, 1233163, 727, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.future", NULL, 1233890, 2928, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.misc", NULL, 1236818, 3687, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pyassem", NULL, 1240505, 25783, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pycodegen", NULL, 1266288, 56161, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.symbols", NULL, 1322449, 17557, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.syntax", NULL, 1340006, 1862, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.transformer", NULL, 1341868, 47387, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.visitor", NULL, 1389255, 4159, NUITKA_BYTECODE_FLAG },
    { (char *)"contextlib", NULL, 1393414, 4422, NUITKA_BYTECODE_FLAG },
    { (char *)"cookielib", NULL, 1397836, 54366, NUITKA_BYTECODE_FLAG },
    { (char *)"copy", NULL, 1452202, 12110, NUITKA_BYTECODE_FLAG },
    { (char *)"csv", NULL, 1464312, 13441, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes", NULL, 1477753, 20224, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes._endian", NULL, 1497977, 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.util", NULL, 1500264, 8518, NUITKA_BYTECODE_FLAG },
    { (char *)"curses", NULL, 1508782, 1539, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"curses.ascii", NULL, 1510321, 5085, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.has_key", NULL, 1515406, 5928, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.panel", NULL, 1521334, 267, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.textpad", NULL, 1521601, 7121, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.wrapper", NULL, 1528722, 1206, NUITKA_BYTECODE_FLAG },
    { (char *)"dbhash", NULL, 1529928, 706, NUITKA_BYTECODE_FLAG },
    { (char *)"decimal", NULL, 1530634, 171637, NUITKA_BYTECODE_FLAG },
    { (char *)"difflib", NULL, 1702271, 61782, NUITKA_BYTECODE_FLAG },
    { (char *)"dircache", NULL, 1764053, 1560, NUITKA_BYTECODE_FLAG },
    { (char *)"dis", NULL, 1765613, 6204, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils", NULL, 1771817, 405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.archive_util", NULL, 1772222, 7383, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.bcppcompiler", NULL, 1779605, 7856, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.ccompiler", NULL, 1787461, 36746, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cmd", NULL, 1824207, 16722, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command", NULL, 1840929, 655, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.command.bdist", NULL, 1841584, 5149, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_dumb", NULL, 1846733, 4963, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_msi", NULL, 1851696, 23917, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_rpm", NULL, 1875613, 17580, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_wininst", NULL, 1893193, 10878, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build", NULL, 1904071, 5115, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_clib", NULL, 1909186, 6319, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_ext", NULL, 1915505, 19252, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_py", NULL, 1934757, 11443, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_scripts", NULL, 1946200, 4455, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.check", NULL, 1950655, 6192, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.clean", NULL, 1956847, 3051, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.config", NULL, 1959898, 12637, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install", NULL, 1972535, 17884, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_data", NULL, 1990419, 3096, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_egg_info", NULL, 1993515, 4345, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_headers", NULL, 1997860, 2233, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_lib", NULL, 2000093, 6649, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_scripts", NULL, 2006742, 2906, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.register", NULL, 2009648, 10174, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.sdist", NULL, 2019822, 16652, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.upload", NULL, 2036474, 6288, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.config", NULL, 2042762, 3543, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.core", NULL, 2046305, 7520, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cygwinccompiler", NULL, 2053825, 9793, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.debug", NULL, 2063618, 244, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dep_util", NULL, 2063862, 3164, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dir_util", NULL, 2067026, 6764, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dist", NULL, 2073790, 39034, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.emxccompiler", NULL, 2112824, 7441, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.errors", NULL, 2120265, 6237, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.extension", NULL, 2126502, 7396, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.fancy_getopt", NULL, 2133898, 11908, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.file_util", NULL, 2145806, 6732, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.filelist", NULL, 2152538, 10714, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.log", NULL, 2163252, 2754, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvc9compiler", NULL, 2166006, 21427, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvccompiler", NULL, 2187433, 17465, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.spawn", NULL, 2204898, 6405, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.sysconfig", NULL, 2211303, 15079, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.text_file", NULL, 2226382, 9229, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.unixccompiler", NULL, 2235611, 8087, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.util", NULL, 2243698, 14304, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.version", NULL, 2258002, 7170, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.versionpredicate", NULL, 2265172, 5520, NUITKA_BYTECODE_FLAG },
    { (char *)"doctest", NULL, 2270692, 83440, NUITKA_BYTECODE_FLAG },
    { (char *)"dumbdbm", NULL, 2354132, 6700, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_thread", NULL, 2360832, 5356, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_threading", NULL, 2366188, 1275, NUITKA_BYTECODE_FLAG },
    { (char *)"email", NULL, 2367463, 2852, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email._parseaddr", NULL, 2370315, 13831, NUITKA_BYTECODE_FLAG },
    { (char *)"email.base64mime", NULL, 2384146, 5305, NUITKA_BYTECODE_FLAG },
    { (char *)"email.charset", NULL, 2389451, 13499, NUITKA_BYTECODE_FLAG },
    { (char *)"email.encoders", NULL, 2402950, 2210, NUITKA_BYTECODE_FLAG },
    { (char *)"email.errors", NULL, 2405160, 3491, NUITKA_BYTECODE_FLAG },
    { (char *)"email.feedparser", NULL, 2408651, 11516, NUITKA_BYTECODE_FLAG },
    { (char *)"email.generator", NULL, 2420167, 10334, NUITKA_BYTECODE_FLAG },
    { (char *)"email.header", NULL, 2430501, 13622, NUITKA_BYTECODE_FLAG },
    { (char *)"email.iterators", NULL, 2444123, 2348, NUITKA_BYTECODE_FLAG },
    { (char *)"email.message", NULL, 2446471, 28576, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime", NULL, 2475047, 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email.mime.application", NULL, 2475167, 1570, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.audio", NULL, 2476737, 2893, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.base", NULL, 2479630, 1102, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.image", NULL, 2480732, 2035, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.message", NULL, 2482767, 1434, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.multipart", NULL, 2484201, 1655, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.nonmultipart", NULL, 2485856, 874, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.text", NULL, 2486730, 1294, NUITKA_BYTECODE_FLAG },
    { (char *)"email.parser", NULL, 2488024, 3804, NUITKA_BYTECODE_FLAG },
    { (char *)"email.quoprimime", NULL, 2491828, 8816, NUITKA_BYTECODE_FLAG },
    { (char *)"email.utils", NULL, 2500644, 9284, NUITKA_BYTECODE_FLAG },
    { (char *)"encodings.mbcs", NULL, 2509928, 2019, NUITKA_BYTECODE_FLAG },
    { (char *)"filecmp", NULL, 2511947, 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"fileinput", NULL, 2521521, 14432, NUITKA_BYTECODE_FLAG },
    { (char *)"fnmatch", NULL, 2535953, 3594, NUITKA_BYTECODE_FLAG },
    { (char *)"formatter", NULL, 2539547, 19016, NUITKA_BYTECODE_FLAG },
    { (char *)"fpformat", NULL, 2558563, 4679, NUITKA_BYTECODE_FLAG },
    { (char *)"fractions", NULL, 2563242, 19625, NUITKA_BYTECODE_FLAG },
    { (char *)"ftplib", NULL, 2582867, 34609, NUITKA_BYTECODE_FLAG },
    { (char *)"genericpath", NULL, 2617476, 3487, NUITKA_BYTECODE_FLAG },
    { (char *)"getopt", NULL, 2620963, 6626, NUITKA_BYTECODE_FLAG },
    { (char *)"getpass", NULL, 2627589, 4722, NUITKA_BYTECODE_FLAG },
    { (char *)"gettext", NULL, 2632311, 17993, NUITKA_BYTECODE_FLAG },
    { (char *)"glob", NULL, 2650304, 2919, NUITKA_BYTECODE_FLAG },
    { (char *)"gzip", NULL, 2653223, 15168, NUITKA_BYTECODE_FLAG },
    { (char *)"hashlib", NULL, 2668391, 6883, NUITKA_BYTECODE_FLAG },
    { (char *)"heapq", NULL, 2675274, 14520, NUITKA_BYTECODE_FLAG },
    { (char *)"hmac", NULL, 2689794, 4514, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot", NULL, 2694308, 3454, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"hotshot.log", NULL, 2697762, 5507, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stats", NULL, 2703269, 3374, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stones", NULL, 2706643, 1144, NUITKA_BYTECODE_FLAG },
    { (char *)"htmlentitydefs", NULL, 2707787, 6357, NUITKA_BYTECODE_FLAG },
    { (char *)"htmllib", NULL, 2714144, 20119, NUITKA_BYTECODE_FLAG },
    { (char *)"httplib", NULL, 2734263, 36779, NUITKA_BYTECODE_FLAG },
    { (char *)"ihooks", NULL, 2771042, 21226, NUITKA_BYTECODE_FLAG },
    { (char *)"imaplib", NULL, 2792268, 45161, NUITKA_BYTECODE_FLAG },
    { (char *)"imghdr", NULL, 2837429, 4798, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib", NULL, 2842227, 1488, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"imputil", NULL, 2843715, 15539, NUITKA_BYTECODE_FLAG },
    { (char *)"inspect", NULL, 2859254, 40083, NUITKA_BYTECODE_FLAG },
    { (char *)"io", NULL, 2899337, 3571, NUITKA_BYTECODE_FLAG },
    { (char *)"json", NULL, 2902908, 13908, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"json.decoder", NULL, 2916816, 11928, NUITKA_BYTECODE_FLAG },
    { (char *)"json.encoder", NULL, 2928744, 13679, NUITKA_BYTECODE_FLAG },
    { (char *)"json.scanner", NULL, 2942423, 2215, NUITKA_BYTECODE_FLAG },
    { (char *)"json.tool", NULL, 2944638, 1282, NUITKA_BYTECODE_FLAG },
    { (char *)"keyword", NULL, 2945920, 2093, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3", NULL, 2948013, 117, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.btm_matcher", NULL, 2948130, 5800, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.btm_utils", NULL, 2953930, 7529, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_base", NULL, 2961459, 7146, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_util", NULL, 2968605, 14607, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes", NULL, 2983212, 123, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.fixes.fix_apply", NULL, 2983335, 2070, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_asserts", NULL, 2985405, 1547, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_basestring", NULL, 2986952, 793, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_buffer", NULL, 2987745, 950, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_dict", NULL, 2988695, 3753, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_except", NULL, 2992448, 2993, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exec", NULL, 2995441, 1418, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_execfile", NULL, 2996859, 2059, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exitfunc", NULL, 2998918, 2739, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_filter", NULL, 3001657, 2256, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_funcattrs", NULL, 3003913, 1114, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_future", NULL, 3005027, 919, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_getcwdu", NULL, 3005946, 926, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_has_key", NULL, 3006872, 3184, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_idioms", NULL, 3010056, 4515, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_import", NULL, 3014571, 3233, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports", NULL, 3017804, 5352, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports2", NULL, 3023156, 622, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_input", NULL, 3023778, 1134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_intern", NULL, 3024912, 1783, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_isinstance", NULL, 3026695, 1838, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools", NULL, 3028533, 1791, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools_imports", NULL, 3030324, 2016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_long", NULL, 3032340, 841, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_map", NULL, 3033181, 3040, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_metaclass", NULL, 3036221, 6579, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_methodattrs", NULL, 3042800, 1138, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ne", NULL, 3043938, 985, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_next", NULL, 3044923, 3531, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_nonzero", NULL, 3048454, 1086, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_numliterals", NULL, 3049540, 1249, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_operator", NULL, 3050789, 5112, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_paren", NULL, 3055901, 1543, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_print", NULL, 3057444, 2727, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raise", NULL, 3060171, 2498, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raw_input", NULL, 3062669, 936, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reduce", NULL, 3063605, 1262, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_renames", NULL, 3064867, 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_repr", NULL, 3067316, 1016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_set_literal", NULL, 3068332, 1988, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_standarderror", NULL, 3070320, 853, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_sys_exc", NULL, 3071173, 1705, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_throw", NULL, 3072878, 1996, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_tuple_params", NULL, 3074874, 5430, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_types", NULL, 3080304, 2196, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_unicode", NULL, 3082500, 1716, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_urllib", NULL, 3084216, 7134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ws_comma", NULL, 3091350, 1382, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xrange", NULL, 3092732, 3063, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xreadlines", NULL, 3095795, 1152, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_zip", NULL, 3096947, 1346, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.main", NULL, 3098293, 9803, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.patcomp", NULL, 3108096, 6569, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2", NULL, 3114665, 164, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.pgen2.conv", NULL, 3114829, 8169, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.driver", NULL, 3122998, 6434, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.grammar", NULL, 3129432, 7654, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.literals", NULL, 3137086, 2002, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.parse", NULL, 3139088, 7204, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.pgen", NULL, 3146292, 12166, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.token", NULL, 3158458, 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.tokenize", NULL, 3160745, 16923, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pygram", NULL, 3177668, 1427, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pytree", NULL, 3179095, 30143, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.refactor", NULL, 3209238, 23866, NUITKA_BYTECODE_FLAG },
    { (char *)"linecache", NULL, 3233104, 3252, NUITKA_BYTECODE_FLAG },
    { (char *)"logging", NULL, 3236356, 57332, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"logging.config", NULL, 3293688, 25834, NUITKA_BYTECODE_FLAG },
    { (char *)"logging.handlers", NULL, 3319522, 39488, NUITKA_BYTECODE_FLAG },
    { (char *)"macpath", NULL, 3359010, 7635, NUITKA_BYTECODE_FLAG },
    { (char *)"macurl2path", NULL, 3366645, 2228, NUITKA_BYTECODE_FLAG },
    { (char *)"mailbox", NULL, 3368873, 76271, NUITKA_BYTECODE_FLAG },
    { (char *)"mailcap", NULL, 3445144, 7045, NUITKA_BYTECODE_FLAG },
    { (char *)"markupbase", NULL, 3452189, 9225, NUITKA_BYTECODE_FLAG },
    { (char *)"md5", NULL, 3461414, 368, NUITKA_BYTECODE_FLAG },
    { (char *)"mhlib", NULL, 3461782, 33613, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetools", NULL, 3495395, 8153, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetypes", NULL, 3503548, 18425, NUITKA_BYTECODE_FLAG },
    { (char *)"mimify", NULL, 3521973, 11963, NUITKA_BYTECODE_FLAG },
    { (char *)"modulefinder", NULL, 3533936, 19049, NUITKA_BYTECODE_FLAG },
    { (char *)"multifile", NULL, 3552985, 5382, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing", NULL, 3558367, 8405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.connection", NULL, 3566772, 14310, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.dummy", NULL, 3581082, 5412, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.dummy.connection", NULL, 3586494, 2732, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.forking", NULL, 3589226, 14400, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.heap", NULL, 3603626, 6861, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.managers", NULL, 3610487, 38147, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.pool", NULL, 3648634, 22468, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.process", NULL, 3671102, 9544, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.queues", NULL, 3680646, 11421, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.reduction", NULL, 3692067, 5972, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.sharedctypes", NULL, 3698039, 8572, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.synchronize", NULL, 3706611, 10939, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.util", NULL, 3717550, 10081, NUITKA_BYTECODE_FLAG },
    { (char *)"mutex", NULL, 3727631, 2494, NUITKA_BYTECODE_FLAG },
    { (char *)"netrc", NULL, 3730125, 4688, NUITKA_BYTECODE_FLAG },
    { (char *)"new", NULL, 3734813, 852, NUITKA_BYTECODE_FLAG },
    { (char *)"nntplib", NULL, 3735665, 20948, NUITKA_BYTECODE_FLAG },
    { (char *)"ntpath", NULL, 3756613, 13077, NUITKA_BYTECODE_FLAG },
    { (char *)"nturl2path", NULL, 3769690, 1801, NUITKA_BYTECODE_FLAG },
    { (char *)"numbers", NULL, 3771491, 13880, NUITKA_BYTECODE_FLAG },
    { (char *)"opcode", NULL, 3785371, 6127, NUITKA_BYTECODE_FLAG },
    { (char *)"optparse", NULL, 3791498, 53606, NUITKA_BYTECODE_FLAG },
    { (char *)"os", NULL, 3845104, 25575, NUITKA_BYTECODE_FLAG },
    { (char *)"os2emxpath", NULL, 3870679, 4499, NUITKA_BYTECODE_FLAG },
    { (char *)"pdb", NULL, 3875178, 43433, NUITKA_BYTECODE_FLAG },
    { (char *)"pickle", NULL, 3918611, 38344, NUITKA_BYTECODE_FLAG },
    { (char *)"pickletools", NULL, 3956955, 56960, NUITKA_BYTECODE_FLAG },
    { (char *)"pipes", NULL, 4013915, 9268, NUITKA_BYTECODE_FLAG },
    { (char *)"pkgutil", NULL, 4023183, 18885, NUITKA_BYTECODE_FLAG },
    { (char *)"platform", NULL, 4042068, 38489, NUITKA_BYTECODE_FLAG },
    { (char *)"plistlib", NULL, 4080557, 19115, NUITKA_BYTECODE_FLAG },
    { (char *)"popen2", NULL, 4099672, 8985, NUITKA_BYTECODE_FLAG },
    { (char *)"poplib", NULL, 4108657, 13271, NUITKA_BYTECODE_FLAG },
    { (char *)"posixfile", NULL, 4121928, 7620, NUITKA_BYTECODE_FLAG },
    { (char *)"posixpath", NULL, 4129548, 11377, NUITKA_BYTECODE_FLAG },
    { (char *)"pprint", NULL, 4140925, 10148, NUITKA_BYTECODE_FLAG },
    { (char *)"profile", NULL, 4151073, 16370, NUITKA_BYTECODE_FLAG },
    { (char *)"pstats", NULL, 4167443, 24885, NUITKA_BYTECODE_FLAG },
    { (char *)"pty", NULL, 4192328, 4938, NUITKA_BYTECODE_FLAG },
    { (char *)"py_compile", NULL, 4197266, 6603, NUITKA_BYTECODE_FLAG },
    { (char *)"pyclbr", NULL, 4203869, 9617, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc", NULL, 4213486, 92384, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc_data", NULL, 4305870, 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pydoc_data.topics", NULL, 4305990, 420808, NUITKA_BYTECODE_FLAG },
    { (char *)"random", NULL, 4726798, 25610, NUITKA_BYTECODE_FLAG },
    { (char *)"repr", NULL, 4752408, 5343, NUITKA_BYTECODE_FLAG },
    { (char *)"rexec", NULL, 4757751, 23681, NUITKA_BYTECODE_FLAG },
    { (char *)"rfc822", NULL, 4781432, 31685, NUITKA_BYTECODE_FLAG },
    { (char *)"rlcompleter", NULL, 4813117, 6054, NUITKA_BYTECODE_FLAG },
    { (char *)"robotparser", NULL, 4819171, 7947, NUITKA_BYTECODE_FLAG },
    { (char *)"runpy", NULL, 4827118, 8755, NUITKA_BYTECODE_FLAG },
    { (char *)"sched", NULL, 4835873, 4968, NUITKA_BYTECODE_FLAG },
    { (char *)"sets", NULL, 4840841, 16775, NUITKA_BYTECODE_FLAG },
    { (char *)"sgmllib", NULL, 4857616, 15334, NUITKA_BYTECODE_FLAG },
    { (char *)"sha", NULL, 4872950, 411, NUITKA_BYTECODE_FLAG },
    { (char *)"shelve", NULL, 4873361, 10194, NUITKA_BYTECODE_FLAG },
    { (char *)"shlex", NULL, 4883555, 7524, NUITKA_BYTECODE_FLAG },
    { (char *)"shutil", NULL, 4891079, 18835, NUITKA_BYTECODE_FLAG },
    { (char *)"site", NULL, 4909914, 16582, NUITKA_BYTECODE_FLAG },
    { (char *)"sitecustomize", NULL, 4926496, 224, NUITKA_BYTECODE_FLAG },
    { (char *)"smtpd", NULL, 4926720, 15813, NUITKA_BYTECODE_FLAG },
    { (char *)"smtplib", NULL, 4942533, 30186, NUITKA_BYTECODE_FLAG },
    { (char *)"sndhdr", NULL, 4972719, 7319, NUITKA_BYTECODE_FLAG },
    { (char *)"socket", NULL, 4980038, 16084, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3", NULL, 4996122, 154, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"sqlite3.dbapi2", NULL, 4996276, 2682, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3.dump", NULL, 4998958, 2057, NUITKA_BYTECODE_FLAG },
    { (char *)"sre", NULL, 5001015, 509, NUITKA_BYTECODE_FLAG },
    { (char *)"ssl", NULL, 5001524, 32402, NUITKA_BYTECODE_FLAG },
    { (char *)"stat", NULL, 5033926, 2723, NUITKA_BYTECODE_FLAG },
    { (char *)"statvfs", NULL, 5036649, 610, NUITKA_BYTECODE_FLAG },
    { (char *)"stringold", NULL, 5037259, 12487, NUITKA_BYTECODE_FLAG },
    { (char *)"subprocess", NULL, 5049746, 32282, NUITKA_BYTECODE_FLAG },
    { (char *)"sunau", NULL, 5082028, 18290, NUITKA_BYTECODE_FLAG },
    { (char *)"sunaudio", NULL, 5100318, 1969, NUITKA_BYTECODE_FLAG },
    { (char *)"symbol", NULL, 5102287, 3014, NUITKA_BYTECODE_FLAG },
    { (char *)"symtable", NULL, 5105301, 11678, NUITKA_BYTECODE_FLAG },
    { (char *)"sysconfig", NULL, 5116979, 18805, NUITKA_BYTECODE_FLAG },
    { (char *)"tabnanny", NULL, 5135784, 8199, NUITKA_BYTECODE_FLAG },
    { (char *)"tarfile", NULL, 5143983, 75786, NUITKA_BYTECODE_FLAG },
    { (char *)"telnetlib", NULL, 5219769, 23080, NUITKA_BYTECODE_FLAG },
    { (char *)"tempfile", NULL, 5242849, 20228, NUITKA_BYTECODE_FLAG },
    { (char *)"test", NULL, 5263077, 114, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"test.pystone", NULL, 5263191, 7981, NUITKA_BYTECODE_FLAG },
    { (char *)"textwrap", NULL, 5271172, 11996, NUITKA_BYTECODE_FLAG },
    { (char *)"this", NULL, 5283168, 1210, NUITKA_BYTECODE_FLAG },
    { (char *)"threading", NULL, 5284378, 42422, NUITKA_BYTECODE_FLAG },
    { (char *)"timeit", NULL, 5326800, 12149, NUITKA_BYTECODE_FLAG },
    { (char *)"tkColorChooser", NULL, 5338949, 1402, NUITKA_BYTECODE_FLAG },
    { (char *)"tkCommonDialog", NULL, 5340351, 1496, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFileDialog", NULL, 5341847, 5121, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFont", NULL, 5346968, 7124, NUITKA_BYTECODE_FLAG },
    { (char *)"tkMessageBox", NULL, 5354092, 3864, NUITKA_BYTECODE_FLAG },
    { (char *)"tkSimpleDialog", NULL, 5357956, 9053, NUITKA_BYTECODE_FLAG },
    { (char *)"toaiff", NULL, 5367009, 3090, NUITKA_BYTECODE_FLAG },
    { (char *)"token", NULL, 5370099, 3798, NUITKA_BYTECODE_FLAG },
    { (char *)"tokenize", NULL, 5373897, 14588, NUITKA_BYTECODE_FLAG },
    { (char *)"trace", NULL, 5388485, 22716, NUITKA_BYTECODE_FLAG },
    { (char *)"traceback", NULL, 5411201, 11631, NUITKA_BYTECODE_FLAG },
    { (char *)"ttk", NULL, 5422832, 62344, NUITKA_BYTECODE_FLAG },
    { (char *)"tty", NULL, 5485176, 1303, NUITKA_BYTECODE_FLAG },
    { (char *)"turtle", NULL, 5486479, 139105, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest", NULL, 5625584, 2954, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"unittest.case", NULL, 5628538, 40207, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.loader", NULL, 5668745, 11295, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.main", NULL, 5680040, 7970, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.result", NULL, 5688010, 7869, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.runner", NULL, 5695879, 7581, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.signals", NULL, 5703460, 2753, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.suite", NULL, 5706213, 10460, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.util", NULL, 5716673, 4494, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib", NULL, 5721167, 50958, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib2", NULL, 5772125, 47019, NUITKA_BYTECODE_FLAG },
    { (char *)"urlparse", NULL, 5819144, 15038, NUITKA_BYTECODE_FLAG },
    { (char *)"user", NULL, 5834182, 1714, NUITKA_BYTECODE_FLAG },
    { (char *)"uu", NULL, 5835896, 4294, NUITKA_BYTECODE_FLAG },
    { (char *)"uuid", NULL, 5840190, 23095, NUITKA_BYTECODE_FLAG },
    { (char *)"warnings", NULL, 5863285, 13456, NUITKA_BYTECODE_FLAG },
    { (char *)"wave", NULL, 5876741, 19903, NUITKA_BYTECODE_FLAG },
    { (char *)"weakref", NULL, 5896644, 16327, NUITKA_BYTECODE_FLAG },
    { (char *)"webbrowser", NULL, 5912971, 19652, NUITKA_BYTECODE_FLAG },
    { (char *)"whichdb", NULL, 5932623, 2229, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref", NULL, 5934852, 719, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wsgiref.handlers", NULL, 5935571, 16172, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.headers", NULL, 5951743, 7429, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.simple_server", NULL, 5959172, 6199, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.util", NULL, 5965371, 5961, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.validate", NULL, 5971332, 16767, NUITKA_BYTECODE_FLAG },
    { (char *)"xdrlib", NULL, 5988099, 9810, NUITKA_BYTECODE_FLAG },
    { (char *)"xml", NULL, 5997909, 1068, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom", NULL, 5998977, 6427, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom.NodeFilter", NULL, 6005404, 1112, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.domreg", NULL, 6006516, 3293, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.expatbuilder", NULL, 6009809, 32810, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minicompat", NULL, 6042619, 3394, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minidom", NULL, 6046013, 65141, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.pulldom", NULL, 6111154, 12986, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.xmlbuilder", NULL, 6124140, 15222, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree", NULL, 6139362, 119, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.etree.ElementInclude", NULL, 6139481, 1951, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementPath", NULL, 6141432, 7560, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementTree", NULL, 6148992, 34966, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.cElementTree", NULL, 6183958, 166, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.parsers", NULL, 6184124, 304, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.parsers.expat", NULL, 6184428, 277, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax", NULL, 6184705, 3679, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.sax._exceptions", NULL, 6188384, 6127, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.expatreader", NULL, 6194511, 15081, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.handler", NULL, 6209592, 12970, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.saxutils", NULL, 6222562, 14685, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.xmlreader", NULL, 6237247, 19078, NUITKA_BYTECODE_FLAG },
    { (char *)"xmllib", NULL, 6256325, 26732, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpclib", NULL, 6283057, 43824, NUITKA_BYTECODE_FLAG },
    { (char *)"zipfile", NULL, 6326881, 42007, NUITKA_BYTECODE_FLAG },
    { NULL, NULL, 0, 0, 0 }
};

void setupMetaPathBasedLoader( void )
{
    static bool init_done = false;

    if ( init_done == false )
    {
        registerMetaPathBasedUnfreezer( meta_path_loader_entries );
        init_done = true;
    }
}
