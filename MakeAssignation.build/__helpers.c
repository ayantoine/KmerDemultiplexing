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
    { (char *)"BaseHTTPServer", NULL, 1780, 21674, NUITKA_BYTECODE_FLAG },
    { (char *)"Bastion", NULL, 23454, 6622, NUITKA_BYTECODE_FLAG },
    { (char *)"CGIHTTPServer", NULL, 30076, 10984, NUITKA_BYTECODE_FLAG },
    { (char *)"Canvas", NULL, 41060, 15396, NUITKA_BYTECODE_FLAG },
    { (char *)"ConfigParser", NULL, 56456, 25087, NUITKA_BYTECODE_FLAG },
    { (char *)"Cookie", NULL, 81543, 22574, NUITKA_BYTECODE_FLAG },
    { (char *)"Dialog", NULL, 104117, 1902, NUITKA_BYTECODE_FLAG },
    { (char *)"DocXMLRPCServer", NULL, 106019, 10149, NUITKA_BYTECODE_FLAG },
    { (char *)"FileDialog", NULL, 116168, 9666, NUITKA_BYTECODE_FLAG },
    { (char *)"FixTk", NULL, 125834, 2078, NUITKA_BYTECODE_FLAG },
    { (char *)"HTMLParser", NULL, 127912, 13655, NUITKA_BYTECODE_FLAG },
    { (char *)"MimeWriter", NULL, 141567, 7338, NUITKA_BYTECODE_FLAG },
    { (char *)"Queue", NULL, 148905, 9360, NUITKA_BYTECODE_FLAG },
    { (char *)"ScrolledText", NULL, 158265, 2646, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleDialog", NULL, 160911, 4309, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleHTTPServer", NULL, 165220, 7980, NUITKA_BYTECODE_FLAG },
    { (char *)"SimpleXMLRPCServer", NULL, 173200, 22783, NUITKA_BYTECODE_FLAG },
    { (char *)"SocketServer", NULL, 195983, 23948, NUITKA_BYTECODE_FLAG },
    { (char *)"StringIO", NULL, 219931, 11434, NUITKA_BYTECODE_FLAG },
    { (char *)"Tix", NULL, 231365, 95416, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkconstants", NULL, 326781, 2236, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkdnd", NULL, 329017, 12765, NUITKA_BYTECODE_FLAG },
    { (char *)"Tkinter", NULL, 341782, 199207, NUITKA_BYTECODE_FLAG },
    { (char *)"UserDict", NULL, 540989, 9613, NUITKA_BYTECODE_FLAG },
    { (char *)"UserList", NULL, 550602, 6501, NUITKA_BYTECODE_FLAG },
    { (char *)"UserString", NULL, 557103, 14720, NUITKA_BYTECODE_FLAG },
    { (char *)"_LWPCookieJar", NULL, 571823, 5414, NUITKA_BYTECODE_FLAG },
    { (char *)"_MozillaCookieJar", NULL, 577237, 4445, NUITKA_BYTECODE_FLAG },
    { (char *)"__future__", NULL, 581682, 4203, NUITKA_BYTECODE_FLAG },
    { (char *)"_abcoll", NULL, 585885, 25466, NUITKA_BYTECODE_FLAG },
    { (char *)"_osx_support", NULL, 611351, 11712, NUITKA_BYTECODE_FLAG },
    { (char *)"_pyio", NULL, 623063, 64343, NUITKA_BYTECODE_FLAG },
    { (char *)"_strptime", NULL, 687406, 15124, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata", NULL, 702530, 271, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata_nd", NULL, 702801, 20828, NUITKA_BYTECODE_FLAG },
    { (char *)"_threading_local", NULL, 723629, 6347, NUITKA_BYTECODE_FLAG },
    { (char *)"_weakrefset", NULL, 729976, 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"abc", NULL, 739550, 6113, NUITKA_BYTECODE_FLAG },
    { (char *)"aifc", NULL, 745663, 30305, NUITKA_BYTECODE_FLAG },
    { (char *)"anydbm", NULL, 775968, 2786, NUITKA_BYTECODE_FLAG },
    { (char *)"argparse", NULL, 778754, 64049, NUITKA_BYTECODE_FLAG },
    { (char *)"ast", NULL, 842803, 12882, NUITKA_BYTECODE_FLAG },
    { (char *)"asynchat", NULL, 855685, 8742, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncore", NULL, 864427, 18767, NUITKA_BYTECODE_FLAG },
    { (char *)"atexit", NULL, 883194, 2183, NUITKA_BYTECODE_FLAG },
    { (char *)"audiodev", NULL, 885377, 8407, NUITKA_BYTECODE_FLAG },
    { (char *)"bdb", NULL, 893784, 18971, NUITKA_BYTECODE_FLAG },
    { (char *)"binhex", NULL, 912755, 15350, NUITKA_BYTECODE_FLAG },
    { (char *)"bisect", NULL, 928105, 3053, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb", NULL, 931158, 12368, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"bsddb.db", NULL, 943526, 582, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbobj", NULL, 944108, 18601, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbrecio", NULL, 962709, 5269, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbshelve", NULL, 967978, 12910, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbtables", NULL, 980888, 24402, NUITKA_BYTECODE_FLAG },
    { (char *)"bsddb.dbutils", NULL, 1005290, 1615, NUITKA_BYTECODE_FLAG },
    { (char *)"cProfile", NULL, 1006905, 6263, NUITKA_BYTECODE_FLAG },
    { (char *)"calendar", NULL, 1013168, 27747, NUITKA_BYTECODE_FLAG },
    { (char *)"cgi", NULL, 1040915, 32417, NUITKA_BYTECODE_FLAG },
    { (char *)"cgitb", NULL, 1073332, 12094, NUITKA_BYTECODE_FLAG },
    { (char *)"chunk", NULL, 1085426, 5572, NUITKA_BYTECODE_FLAG },
    { (char *)"cmd", NULL, 1090998, 13989, NUITKA_BYTECODE_FLAG },
    { (char *)"code", NULL, 1104987, 10294, NUITKA_BYTECODE_FLAG },
    { (char *)"codeop", NULL, 1115281, 6569, NUITKA_BYTECODE_FLAG },
    { (char *)"collections", NULL, 1121850, 26051, NUITKA_BYTECODE_FLAG },
    { (char *)"colorsys", NULL, 1147901, 3967, NUITKA_BYTECODE_FLAG },
    { (char *)"commands", NULL, 1151868, 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"compileall", NULL, 1154317, 6997, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler", NULL, 1161314, 1287, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"compiler.ast", NULL, 1162601, 71339, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.consts", NULL, 1233940, 727, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.future", NULL, 1234667, 2928, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.misc", NULL, 1237595, 3687, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pyassem", NULL, 1241282, 25783, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.pycodegen", NULL, 1267065, 56161, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.symbols", NULL, 1323226, 17557, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.syntax", NULL, 1340783, 1862, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.transformer", NULL, 1342645, 47387, NUITKA_BYTECODE_FLAG },
    { (char *)"compiler.visitor", NULL, 1390032, 4159, NUITKA_BYTECODE_FLAG },
    { (char *)"contextlib", NULL, 1394191, 4422, NUITKA_BYTECODE_FLAG },
    { (char *)"cookielib", NULL, 1398613, 54366, NUITKA_BYTECODE_FLAG },
    { (char *)"copy", NULL, 1452979, 12110, NUITKA_BYTECODE_FLAG },
    { (char *)"csv", NULL, 1465089, 13441, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes", NULL, 1478530, 20224, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes._endian", NULL, 1498754, 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.util", NULL, 1501041, 8518, NUITKA_BYTECODE_FLAG },
    { (char *)"curses", NULL, 1509559, 1539, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"curses.ascii", NULL, 1511098, 5085, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.has_key", NULL, 1516183, 5928, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.panel", NULL, 1522111, 267, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.textpad", NULL, 1522378, 7121, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.wrapper", NULL, 1529499, 1206, NUITKA_BYTECODE_FLAG },
    { (char *)"dbhash", NULL, 1530705, 706, NUITKA_BYTECODE_FLAG },
    { (char *)"decimal", NULL, 1531411, 171637, NUITKA_BYTECODE_FLAG },
    { (char *)"difflib", NULL, 1703048, 61782, NUITKA_BYTECODE_FLAG },
    { (char *)"dircache", NULL, 1764830, 1560, NUITKA_BYTECODE_FLAG },
    { (char *)"dis", NULL, 1766390, 6204, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils", NULL, 1772594, 405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.archive_util", NULL, 1772999, 7383, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.bcppcompiler", NULL, 1780382, 7856, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.ccompiler", NULL, 1788238, 36746, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cmd", NULL, 1824984, 16722, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command", NULL, 1841706, 655, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.command.bdist", NULL, 1842361, 5149, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_dumb", NULL, 1847510, 4963, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_msi", NULL, 1852473, 23917, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_rpm", NULL, 1876390, 17580, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_wininst", NULL, 1893970, 10878, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build", NULL, 1904848, 5115, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_clib", NULL, 1909963, 6319, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_ext", NULL, 1916282, 19252, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_py", NULL, 1935534, 11443, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_scripts", NULL, 1946977, 4455, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.check", NULL, 1951432, 6192, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.clean", NULL, 1957624, 3051, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.config", NULL, 1960675, 12637, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install", NULL, 1973312, 17884, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_data", NULL, 1991196, 3096, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_egg_info", NULL, 1994292, 4345, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_headers", NULL, 1998637, 2233, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_lib", NULL, 2000870, 6649, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_scripts", NULL, 2007519, 2906, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.register", NULL, 2010425, 10174, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.sdist", NULL, 2020599, 16652, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.upload", NULL, 2037251, 6288, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.config", NULL, 2043539, 3543, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.core", NULL, 2047082, 7520, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cygwinccompiler", NULL, 2054602, 9793, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.debug", NULL, 2064395, 244, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dep_util", NULL, 2064639, 3164, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dir_util", NULL, 2067803, 6764, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dist", NULL, 2074567, 39034, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.emxccompiler", NULL, 2113601, 7441, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.errors", NULL, 2121042, 6237, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.extension", NULL, 2127279, 7396, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.fancy_getopt", NULL, 2134675, 11908, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.file_util", NULL, 2146583, 6732, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.filelist", NULL, 2153315, 10714, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.log", NULL, 2164029, 2754, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvc9compiler", NULL, 2166783, 21427, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvccompiler", NULL, 2188210, 17465, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.spawn", NULL, 2205675, 6405, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.sysconfig", NULL, 2212080, 15079, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.text_file", NULL, 2227159, 9229, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.unixccompiler", NULL, 2236388, 8087, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.util", NULL, 2244475, 14304, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.version", NULL, 2258779, 7170, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.versionpredicate", NULL, 2265949, 5520, NUITKA_BYTECODE_FLAG },
    { (char *)"doctest", NULL, 2271469, 83440, NUITKA_BYTECODE_FLAG },
    { (char *)"dumbdbm", NULL, 2354909, 6700, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_thread", NULL, 2361609, 5356, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_threading", NULL, 2366965, 1275, NUITKA_BYTECODE_FLAG },
    { (char *)"email", NULL, 2368240, 2852, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email._parseaddr", NULL, 2371092, 13831, NUITKA_BYTECODE_FLAG },
    { (char *)"email.base64mime", NULL, 2384923, 5305, NUITKA_BYTECODE_FLAG },
    { (char *)"email.charset", NULL, 2390228, 13499, NUITKA_BYTECODE_FLAG },
    { (char *)"email.encoders", NULL, 2403727, 2210, NUITKA_BYTECODE_FLAG },
    { (char *)"email.errors", NULL, 2405937, 3491, NUITKA_BYTECODE_FLAG },
    { (char *)"email.feedparser", NULL, 2409428, 11516, NUITKA_BYTECODE_FLAG },
    { (char *)"email.generator", NULL, 2420944, 10334, NUITKA_BYTECODE_FLAG },
    { (char *)"email.header", NULL, 2431278, 13622, NUITKA_BYTECODE_FLAG },
    { (char *)"email.iterators", NULL, 2444900, 2348, NUITKA_BYTECODE_FLAG },
    { (char *)"email.message", NULL, 2447248, 28576, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime", NULL, 2475824, 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email.mime.application", NULL, 2475944, 1570, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.audio", NULL, 2477514, 2893, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.base", NULL, 2480407, 1102, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.image", NULL, 2481509, 2035, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.message", NULL, 2483544, 1434, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.multipart", NULL, 2484978, 1655, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.nonmultipart", NULL, 2486633, 874, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.text", NULL, 2487507, 1294, NUITKA_BYTECODE_FLAG },
    { (char *)"email.parser", NULL, 2488801, 3804, NUITKA_BYTECODE_FLAG },
    { (char *)"email.quoprimime", NULL, 2492605, 8816, NUITKA_BYTECODE_FLAG },
    { (char *)"email.utils", NULL, 2501421, 9284, NUITKA_BYTECODE_FLAG },
    { (char *)"encodings.mbcs", NULL, 2510705, 2019, NUITKA_BYTECODE_FLAG },
    { (char *)"filecmp", NULL, 2512724, 9574, NUITKA_BYTECODE_FLAG },
    { (char *)"fileinput", NULL, 2522298, 14432, NUITKA_BYTECODE_FLAG },
    { (char *)"fnmatch", NULL, 2536730, 3594, NUITKA_BYTECODE_FLAG },
    { (char *)"formatter", NULL, 2540324, 19016, NUITKA_BYTECODE_FLAG },
    { (char *)"fpformat", NULL, 2559340, 4679, NUITKA_BYTECODE_FLAG },
    { (char *)"fractions", NULL, 2564019, 19625, NUITKA_BYTECODE_FLAG },
    { (char *)"ftplib", NULL, 2583644, 34609, NUITKA_BYTECODE_FLAG },
    { (char *)"genericpath", NULL, 2618253, 3487, NUITKA_BYTECODE_FLAG },
    { (char *)"getopt", NULL, 2621740, 6626, NUITKA_BYTECODE_FLAG },
    { (char *)"getpass", NULL, 2628366, 4722, NUITKA_BYTECODE_FLAG },
    { (char *)"gettext", NULL, 2633088, 17993, NUITKA_BYTECODE_FLAG },
    { (char *)"glob", NULL, 2651081, 2919, NUITKA_BYTECODE_FLAG },
    { (char *)"gzip", NULL, 2654000, 15168, NUITKA_BYTECODE_FLAG },
    { (char *)"hashlib", NULL, 2669168, 6883, NUITKA_BYTECODE_FLAG },
    { (char *)"heapq", NULL, 2676051, 14520, NUITKA_BYTECODE_FLAG },
    { (char *)"hmac", NULL, 2690571, 4514, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot", NULL, 2695085, 3454, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"hotshot.log", NULL, 2698539, 5507, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stats", NULL, 2704046, 3374, NUITKA_BYTECODE_FLAG },
    { (char *)"hotshot.stones", NULL, 2707420, 1144, NUITKA_BYTECODE_FLAG },
    { (char *)"htmlentitydefs", NULL, 2708564, 6357, NUITKA_BYTECODE_FLAG },
    { (char *)"htmllib", NULL, 2714921, 20119, NUITKA_BYTECODE_FLAG },
    { (char *)"httplib", NULL, 2735040, 36779, NUITKA_BYTECODE_FLAG },
    { (char *)"ihooks", NULL, 2771819, 21226, NUITKA_BYTECODE_FLAG },
    { (char *)"imaplib", NULL, 2793045, 45161, NUITKA_BYTECODE_FLAG },
    { (char *)"imghdr", NULL, 2838206, 4798, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib", NULL, 2843004, 1488, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"imputil", NULL, 2844492, 15539, NUITKA_BYTECODE_FLAG },
    { (char *)"inspect", NULL, 2860031, 40083, NUITKA_BYTECODE_FLAG },
    { (char *)"io", NULL, 2900114, 3571, NUITKA_BYTECODE_FLAG },
    { (char *)"json", NULL, 2903685, 13908, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"json.decoder", NULL, 2917593, 11928, NUITKA_BYTECODE_FLAG },
    { (char *)"json.encoder", NULL, 2929521, 13679, NUITKA_BYTECODE_FLAG },
    { (char *)"json.scanner", NULL, 2943200, 2215, NUITKA_BYTECODE_FLAG },
    { (char *)"json.tool", NULL, 2945415, 1282, NUITKA_BYTECODE_FLAG },
    { (char *)"keyword", NULL, 2946697, 2093, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3", NULL, 2948790, 117, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.btm_matcher", NULL, 2948907, 5800, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.btm_utils", NULL, 2954707, 7529, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_base", NULL, 2962236, 7146, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_util", NULL, 2969382, 14607, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes", NULL, 2983989, 123, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.fixes.fix_apply", NULL, 2984112, 2070, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_asserts", NULL, 2986182, 1547, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_basestring", NULL, 2987729, 793, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_buffer", NULL, 2988522, 950, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_dict", NULL, 2989472, 3753, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_except", NULL, 2993225, 2993, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exec", NULL, 2996218, 1418, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_execfile", NULL, 2997636, 2059, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exitfunc", NULL, 2999695, 2739, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_filter", NULL, 3002434, 2256, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_funcattrs", NULL, 3004690, 1114, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_future", NULL, 3005804, 919, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_getcwdu", NULL, 3006723, 926, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_has_key", NULL, 3007649, 3184, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_idioms", NULL, 3010833, 4515, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_import", NULL, 3015348, 3233, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports", NULL, 3018581, 5352, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports2", NULL, 3023933, 622, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_input", NULL, 3024555, 1134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_intern", NULL, 3025689, 1783, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_isinstance", NULL, 3027472, 1838, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools", NULL, 3029310, 1791, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools_imports", NULL, 3031101, 2016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_long", NULL, 3033117, 841, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_map", NULL, 3033958, 3040, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_metaclass", NULL, 3036998, 6579, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_methodattrs", NULL, 3043577, 1138, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ne", NULL, 3044715, 985, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_next", NULL, 3045700, 3531, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_nonzero", NULL, 3049231, 1086, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_numliterals", NULL, 3050317, 1249, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_operator", NULL, 3051566, 5112, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_paren", NULL, 3056678, 1543, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_print", NULL, 3058221, 2727, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raise", NULL, 3060948, 2498, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raw_input", NULL, 3063446, 936, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reduce", NULL, 3064382, 1262, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_renames", NULL, 3065644, 2449, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_repr", NULL, 3068093, 1016, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_set_literal", NULL, 3069109, 1988, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_standarderror", NULL, 3071097, 853, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_sys_exc", NULL, 3071950, 1705, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_throw", NULL, 3073655, 1996, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_tuple_params", NULL, 3075651, 5430, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_types", NULL, 3081081, 2196, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_unicode", NULL, 3083277, 1716, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_urllib", NULL, 3084993, 7134, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ws_comma", NULL, 3092127, 1382, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xrange", NULL, 3093509, 3063, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xreadlines", NULL, 3096572, 1152, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_zip", NULL, 3097724, 1346, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.main", NULL, 3099070, 9803, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.patcomp", NULL, 3108873, 6569, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2", NULL, 3115442, 164, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.pgen2.conv", NULL, 3115606, 8169, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.driver", NULL, 3123775, 6434, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.grammar", NULL, 3130209, 7654, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.literals", NULL, 3137863, 2002, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.parse", NULL, 3139865, 7204, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.pgen", NULL, 3147069, 12166, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.token", NULL, 3159235, 2287, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.tokenize", NULL, 3161522, 16923, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pygram", NULL, 3178445, 1427, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pytree", NULL, 3179872, 30143, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.refactor", NULL, 3210015, 23866, NUITKA_BYTECODE_FLAG },
    { (char *)"linecache", NULL, 3233881, 3252, NUITKA_BYTECODE_FLAG },
    { (char *)"logging", NULL, 3237133, 57332, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"logging.config", NULL, 3294465, 25834, NUITKA_BYTECODE_FLAG },
    { (char *)"logging.handlers", NULL, 3320299, 39488, NUITKA_BYTECODE_FLAG },
    { (char *)"macpath", NULL, 3359787, 7635, NUITKA_BYTECODE_FLAG },
    { (char *)"macurl2path", NULL, 3367422, 2228, NUITKA_BYTECODE_FLAG },
    { (char *)"mailbox", NULL, 3369650, 76271, NUITKA_BYTECODE_FLAG },
    { (char *)"mailcap", NULL, 3445921, 7045, NUITKA_BYTECODE_FLAG },
    { (char *)"markupbase", NULL, 3452966, 9225, NUITKA_BYTECODE_FLAG },
    { (char *)"md5", NULL, 3462191, 368, NUITKA_BYTECODE_FLAG },
    { (char *)"mhlib", NULL, 3462559, 33613, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetools", NULL, 3496172, 8153, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetypes", NULL, 3504325, 18425, NUITKA_BYTECODE_FLAG },
    { (char *)"mimify", NULL, 3522750, 11963, NUITKA_BYTECODE_FLAG },
    { (char *)"modulefinder", NULL, 3534713, 19049, NUITKA_BYTECODE_FLAG },
    { (char *)"multifile", NULL, 3553762, 5382, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing", NULL, 3559144, 8405, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.connection", NULL, 3567549, 14310, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.dummy", NULL, 3581859, 5412, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.dummy.connection", NULL, 3587271, 2732, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.forking", NULL, 3590003, 14400, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.heap", NULL, 3604403, 6861, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.managers", NULL, 3611264, 38147, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.pool", NULL, 3649411, 22468, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.process", NULL, 3671879, 9544, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.queues", NULL, 3681423, 11421, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.reduction", NULL, 3692844, 5972, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.sharedctypes", NULL, 3698816, 8572, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.synchronize", NULL, 3707388, 10939, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.util", NULL, 3718327, 10081, NUITKA_BYTECODE_FLAG },
    { (char *)"mutex", NULL, 3728408, 2494, NUITKA_BYTECODE_FLAG },
    { (char *)"netrc", NULL, 3730902, 4688, NUITKA_BYTECODE_FLAG },
    { (char *)"new", NULL, 3735590, 852, NUITKA_BYTECODE_FLAG },
    { (char *)"nntplib", NULL, 3736442, 20948, NUITKA_BYTECODE_FLAG },
    { (char *)"ntpath", NULL, 3757390, 13077, NUITKA_BYTECODE_FLAG },
    { (char *)"nturl2path", NULL, 3770467, 1801, NUITKA_BYTECODE_FLAG },
    { (char *)"numbers", NULL, 3772268, 13880, NUITKA_BYTECODE_FLAG },
    { (char *)"opcode", NULL, 3786148, 6127, NUITKA_BYTECODE_FLAG },
    { (char *)"optparse", NULL, 3792275, 53606, NUITKA_BYTECODE_FLAG },
    { (char *)"os", NULL, 3845881, 25575, NUITKA_BYTECODE_FLAG },
    { (char *)"os2emxpath", NULL, 3871456, 4499, NUITKA_BYTECODE_FLAG },
    { (char *)"pdb", NULL, 3875955, 43433, NUITKA_BYTECODE_FLAG },
    { (char *)"pickle", NULL, 3919388, 38344, NUITKA_BYTECODE_FLAG },
    { (char *)"pickletools", NULL, 3957732, 56960, NUITKA_BYTECODE_FLAG },
    { (char *)"pipes", NULL, 4014692, 9268, NUITKA_BYTECODE_FLAG },
    { (char *)"pkgutil", NULL, 4023960, 18885, NUITKA_BYTECODE_FLAG },
    { (char *)"platform", NULL, 4042845, 38489, NUITKA_BYTECODE_FLAG },
    { (char *)"plistlib", NULL, 4081334, 19115, NUITKA_BYTECODE_FLAG },
    { (char *)"popen2", NULL, 4100449, 8985, NUITKA_BYTECODE_FLAG },
    { (char *)"poplib", NULL, 4109434, 13271, NUITKA_BYTECODE_FLAG },
    { (char *)"posixfile", NULL, 4122705, 7620, NUITKA_BYTECODE_FLAG },
    { (char *)"posixpath", NULL, 4130325, 11377, NUITKA_BYTECODE_FLAG },
    { (char *)"pprint", NULL, 4141702, 10148, NUITKA_BYTECODE_FLAG },
    { (char *)"profile", NULL, 4151850, 16370, NUITKA_BYTECODE_FLAG },
    { (char *)"pstats", NULL, 4168220, 24885, NUITKA_BYTECODE_FLAG },
    { (char *)"pty", NULL, 4193105, 4938, NUITKA_BYTECODE_FLAG },
    { (char *)"py_compile", NULL, 4198043, 6603, NUITKA_BYTECODE_FLAG },
    { (char *)"pyclbr", NULL, 4204646, 9617, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc", NULL, 4214263, 92384, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc_data", NULL, 4306647, 120, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pydoc_data.topics", NULL, 4306767, 420808, NUITKA_BYTECODE_FLAG },
    { (char *)"random", NULL, 4727575, 25610, NUITKA_BYTECODE_FLAG },
    { (char *)"repr", NULL, 4753185, 5343, NUITKA_BYTECODE_FLAG },
    { (char *)"rexec", NULL, 4758528, 23681, NUITKA_BYTECODE_FLAG },
    { (char *)"rfc822", NULL, 4782209, 31685, NUITKA_BYTECODE_FLAG },
    { (char *)"rlcompleter", NULL, 4813894, 6054, NUITKA_BYTECODE_FLAG },
    { (char *)"robotparser", NULL, 4819948, 7947, NUITKA_BYTECODE_FLAG },
    { (char *)"runpy", NULL, 4827895, 8755, NUITKA_BYTECODE_FLAG },
    { (char *)"sched", NULL, 4836650, 4968, NUITKA_BYTECODE_FLAG },
    { (char *)"sets", NULL, 4841618, 16775, NUITKA_BYTECODE_FLAG },
    { (char *)"sgmllib", NULL, 4858393, 15334, NUITKA_BYTECODE_FLAG },
    { (char *)"sha", NULL, 4873727, 411, NUITKA_BYTECODE_FLAG },
    { (char *)"shelve", NULL, 4874138, 10194, NUITKA_BYTECODE_FLAG },
    { (char *)"shlex", NULL, 4884332, 7524, NUITKA_BYTECODE_FLAG },
    { (char *)"shutil", NULL, 4891856, 18835, NUITKA_BYTECODE_FLAG },
    { (char *)"site", NULL, 4910691, 16582, NUITKA_BYTECODE_FLAG },
    { (char *)"sitecustomize", NULL, 4927273, 224, NUITKA_BYTECODE_FLAG },
    { (char *)"smtpd", NULL, 4927497, 15813, NUITKA_BYTECODE_FLAG },
    { (char *)"smtplib", NULL, 4943310, 30186, NUITKA_BYTECODE_FLAG },
    { (char *)"sndhdr", NULL, 4973496, 7319, NUITKA_BYTECODE_FLAG },
    { (char *)"socket", NULL, 4980815, 16084, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3", NULL, 4996899, 154, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"sqlite3.dbapi2", NULL, 4997053, 2682, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3.dump", NULL, 4999735, 2057, NUITKA_BYTECODE_FLAG },
    { (char *)"sre", NULL, 5001792, 509, NUITKA_BYTECODE_FLAG },
    { (char *)"ssl", NULL, 5002301, 32402, NUITKA_BYTECODE_FLAG },
    { (char *)"stat", NULL, 5034703, 2723, NUITKA_BYTECODE_FLAG },
    { (char *)"statvfs", NULL, 5037426, 610, NUITKA_BYTECODE_FLAG },
    { (char *)"stringold", NULL, 5038036, 12487, NUITKA_BYTECODE_FLAG },
    { (char *)"subprocess", NULL, 5050523, 32282, NUITKA_BYTECODE_FLAG },
    { (char *)"sunau", NULL, 5082805, 18290, NUITKA_BYTECODE_FLAG },
    { (char *)"sunaudio", NULL, 5101095, 1969, NUITKA_BYTECODE_FLAG },
    { (char *)"symbol", NULL, 5103064, 3014, NUITKA_BYTECODE_FLAG },
    { (char *)"symtable", NULL, 5106078, 11678, NUITKA_BYTECODE_FLAG },
    { (char *)"sysconfig", NULL, 5117756, 18805, NUITKA_BYTECODE_FLAG },
    { (char *)"tabnanny", NULL, 5136561, 8199, NUITKA_BYTECODE_FLAG },
    { (char *)"tarfile", NULL, 5144760, 75786, NUITKA_BYTECODE_FLAG },
    { (char *)"telnetlib", NULL, 5220546, 23080, NUITKA_BYTECODE_FLAG },
    { (char *)"tempfile", NULL, 5243626, 20228, NUITKA_BYTECODE_FLAG },
    { (char *)"test", NULL, 5263854, 114, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"test.pystone", NULL, 5263968, 7981, NUITKA_BYTECODE_FLAG },
    { (char *)"textwrap", NULL, 5271949, 11996, NUITKA_BYTECODE_FLAG },
    { (char *)"this", NULL, 5283945, 1210, NUITKA_BYTECODE_FLAG },
    { (char *)"threading", NULL, 5285155, 42422, NUITKA_BYTECODE_FLAG },
    { (char *)"timeit", NULL, 5327577, 12149, NUITKA_BYTECODE_FLAG },
    { (char *)"tkColorChooser", NULL, 5339726, 1402, NUITKA_BYTECODE_FLAG },
    { (char *)"tkCommonDialog", NULL, 5341128, 1496, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFileDialog", NULL, 5342624, 5121, NUITKA_BYTECODE_FLAG },
    { (char *)"tkFont", NULL, 5347745, 7124, NUITKA_BYTECODE_FLAG },
    { (char *)"tkMessageBox", NULL, 5354869, 3864, NUITKA_BYTECODE_FLAG },
    { (char *)"tkSimpleDialog", NULL, 5358733, 9053, NUITKA_BYTECODE_FLAG },
    { (char *)"toaiff", NULL, 5367786, 3090, NUITKA_BYTECODE_FLAG },
    { (char *)"token", NULL, 5370876, 3798, NUITKA_BYTECODE_FLAG },
    { (char *)"tokenize", NULL, 5374674, 14588, NUITKA_BYTECODE_FLAG },
    { (char *)"trace", NULL, 5389262, 22716, NUITKA_BYTECODE_FLAG },
    { (char *)"traceback", NULL, 5411978, 11631, NUITKA_BYTECODE_FLAG },
    { (char *)"ttk", NULL, 5423609, 62344, NUITKA_BYTECODE_FLAG },
    { (char *)"tty", NULL, 5485953, 1303, NUITKA_BYTECODE_FLAG },
    { (char *)"turtle", NULL, 5487256, 139105, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest", NULL, 5626361, 2954, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"unittest.case", NULL, 5629315, 40207, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.loader", NULL, 5669522, 11295, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.main", NULL, 5680817, 7970, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.result", NULL, 5688787, 7869, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.runner", NULL, 5696656, 7581, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.signals", NULL, 5704237, 2753, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.suite", NULL, 5706990, 10460, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.util", NULL, 5717450, 4494, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib", NULL, 5721944, 50958, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib2", NULL, 5772902, 47019, NUITKA_BYTECODE_FLAG },
    { (char *)"urlparse", NULL, 5819921, 15038, NUITKA_BYTECODE_FLAG },
    { (char *)"user", NULL, 5834959, 1714, NUITKA_BYTECODE_FLAG },
    { (char *)"uu", NULL, 5836673, 4294, NUITKA_BYTECODE_FLAG },
    { (char *)"uuid", NULL, 5840967, 23095, NUITKA_BYTECODE_FLAG },
    { (char *)"warnings", NULL, 5864062, 13456, NUITKA_BYTECODE_FLAG },
    { (char *)"wave", NULL, 5877518, 19903, NUITKA_BYTECODE_FLAG },
    { (char *)"weakref", NULL, 5897421, 16327, NUITKA_BYTECODE_FLAG },
    { (char *)"webbrowser", NULL, 5913748, 19652, NUITKA_BYTECODE_FLAG },
    { (char *)"whichdb", NULL, 5933400, 2229, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref", NULL, 5935629, 719, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wsgiref.handlers", NULL, 5936348, 16172, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.headers", NULL, 5952520, 7429, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.simple_server", NULL, 5959949, 6199, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.util", NULL, 5966148, 5961, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.validate", NULL, 5972109, 16767, NUITKA_BYTECODE_FLAG },
    { (char *)"xdrlib", NULL, 5988876, 9810, NUITKA_BYTECODE_FLAG },
    { (char *)"xml", NULL, 5998686, 1068, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom", NULL, 5999754, 6427, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom.NodeFilter", NULL, 6006181, 1112, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.domreg", NULL, 6007293, 3293, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.expatbuilder", NULL, 6010586, 32810, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minicompat", NULL, 6043396, 3394, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minidom", NULL, 6046790, 65141, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.pulldom", NULL, 6111931, 12986, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.xmlbuilder", NULL, 6124917, 15222, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree", NULL, 6140139, 119, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.etree.ElementInclude", NULL, 6140258, 1951, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementPath", NULL, 6142209, 7560, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementTree", NULL, 6149769, 34966, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.cElementTree", NULL, 6184735, 166, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.parsers", NULL, 6184901, 304, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.parsers.expat", NULL, 6185205, 277, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax", NULL, 6185482, 3679, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.sax._exceptions", NULL, 6189161, 6127, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.expatreader", NULL, 6195288, 15081, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.handler", NULL, 6210369, 12970, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.saxutils", NULL, 6223339, 14685, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.xmlreader", NULL, 6238024, 19078, NUITKA_BYTECODE_FLAG },
    { (char *)"xmllib", NULL, 6257102, 26732, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpclib", NULL, 6283834, 43824, NUITKA_BYTECODE_FLAG },
    { (char *)"zipfile", NULL, 6327658, 42007, NUITKA_BYTECODE_FLAG },
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
