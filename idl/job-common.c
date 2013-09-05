/*
 * This file was generated by orbit-idl-2 - DO NOT EDIT!
 */

#include <string.h>
#define ORBIT2_STUBS_API
#define ORBIT_IDL_C_COMMON
#define job_COMMON
#include "job.h"

static const CORBA_unsigned_long ORBit_zero_int = 0;

#ifndef ORBIT_IDL_C_IMODULE_job
void _ORBIT_skel_small_JobManager__get_FirstName(POA_JobManager             *_o_servant, gpointer            _o_retval,gpointer           *_o_args,CORBA_Context       _o_ctx,CORBA_Environment  *_o_ev,
CORBA_string (*_impl__get_FirstName)(PortableServer_Servant _servant, CORBA_Environment *ev)) {
*(CORBA_string *)_o_retval = _impl__get_FirstName (_o_servant, _o_ev);
}
void _ORBIT_skel_small_JobManager__get_status(POA_JobManager             *_o_servant, gpointer            _o_retval,gpointer           *_o_args,CORBA_Context       _o_ctx,CORBA_Environment  *_o_ev,
CORBA_string (*_impl__get_status)(PortableServer_Servant _servant, CORBA_Environment *ev)) {
*(CORBA_string *)_o_retval = _impl__get_status (_o_servant, _o_ev);
}
void _ORBIT_skel_small_JobManager__set_status(POA_JobManager             *_o_servant, gpointer            _o_retval,gpointer           *_o_args,CORBA_Context       _o_ctx,CORBA_Environment  *_o_ev,
void (*_impl__set_status)(PortableServer_Servant _servant, const CORBA_char * value, CORBA_Environment *ev)) {
_impl__set_status (_o_servant, *(const CORBA_char * *)_o_args[0], _o_ev);
}
void _ORBIT_skel_small_JobManager_QueryJobStatus(POA_JobManager             *_o_servant, gpointer            _o_retval,gpointer           *_o_args,CORBA_Context       _o_ctx,CORBA_Environment  *_o_ev,
CORBA_string (*_impl_QueryJobStatus)(PortableServer_Servant _servant, const CORBA_long number, CORBA_string* property, CORBA_Environment *ev)) {
*(CORBA_string *)_o_retval = _impl_QueryJobStatus (_o_servant, *(const CORBA_long *)_o_args[0], *(CORBA_string* *)_o_args[1], _o_ev);
}

#endif
#if ( (TC_IMPL_TC_JobManager_0 == 'j') \
&& (TC_IMPL_TC_JobManager_1 == 'o') \
&& (TC_IMPL_TC_JobManager_2 == 'b') \
) && !defined(TC_DEF_TC_JobManager)
#define TC_DEF_TC_JobManager 1
#ifdef ORBIT_IDL_C_IMODULE_job
static
#endif
ORBIT2_MAYBE_CONST struct CORBA_TypeCode_struct TC_JobManager_struct = {
{&ORBit_TypeCode_epv, ORBIT_REFCOUNT_STATIC},
CORBA_tk_objref,
0,
0,
ORBIT_ALIGNOF_CORBA_POINTER,
0,
0
,
NULL,
CORBA_OBJECT_NIL,
(char *)"JobManager",
(char *)"IDL:JobManager:1.0",
NULL,
NULL,
-1,
0,
0, 0
};
#endif

#ifndef ORBIT_IDL_C_IMODULE_job
CORBA_unsigned_long JobManager__classid = 0;
#endif

/* Interface type data */

static ORBit_IArg JobManager__set_status__arginfo [] = {
	{ TC_CORBA_string,  ORBit_I_ARG_IN , (char *)"value" }
};
static ORBit_IArg JobManager_QueryJobStatus__arginfo [] = {
	{ TC_CORBA_long,  ORBit_I_ARG_IN | ORBit_I_COMMON_FIXED_SIZE, (char *)"number" },
	{ TC_CORBA_string,  ORBit_I_ARG_OUT , (char *)"property" }
};

#ifdef ORBIT_IDL_C_IMODULE_job
static
#endif
ORBit_IMethod JobManager__imethods [] = {
	{
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
		TC_CORBA_string, (char *)"_get_FirstName", 14,
		0
}
, 	{
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
		TC_CORBA_string, (char *)"_get_status", 11,
		0
}
, 	{
		{ 1, 1, JobManager__set_status__arginfo, FALSE },
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
TC_void, (char *)"_set_status", 11,
		0
}
, 	{
		{ 2, 2, JobManager_QueryJobStatus__arginfo, FALSE },
		{ 0, 0, NULL, FALSE },
		{ 0, 0, NULL, FALSE },
		TC_CORBA_string, (char *)"QueryJobStatus", 14,
		0
}
};

static CORBA_string JobManager__base_itypes[] = {
(char *)"IDL:omg.org/CORBA/Object:1.0"
};
#ifdef ORBIT_IDL_C_IMODULE_job
static
#endif
ORBit_IInterface JobManager__iinterface = {
TC_JobManager,{4, 4, JobManager__imethods, FALSE},
{1, 1, JobManager__base_itypes, FALSE}
};
