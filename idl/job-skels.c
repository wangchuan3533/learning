/*
 * This file was generated by orbit-idl-2 - DO NOT EDIT!
 */

#include <string.h>
#define ORBIT2_STUBS_API
#include "job.h"

static ORBitSmallSkeleton get_skel_small_JobManager(POA_JobManager *servant,
const char *opname,gpointer *m_data, gpointer *impl)
{
switch(opname[0]) {
case 'Q':
if(strcmp((opname + 1), "ueryJobStatus")) break;
*impl = (gpointer)servant->vepv->JobManager_epv->QueryJobStatus;
{ORBit_IInterface *volatile _t_=&JobManager__iinterface;*m_data = (gpointer)&_t_->methods._buffer [3];}
return (ORBitSmallSkeleton)_ORBIT_skel_small_JobManager_QueryJobStatus;
break;
case '_':
switch(opname[1]) {
case 'g':
switch(opname[2]) {
case 'e':
switch(opname[3]) {
case 't':
switch(opname[4]) {
case '_':
switch(opname[5]) {
case 'F':
if(strcmp((opname + 6), "irstName")) break;
*impl = (gpointer)servant->vepv->JobManager_epv->_get_FirstName;
{ORBit_IInterface *volatile _t_=&JobManager__iinterface;*m_data = (gpointer)&_t_->methods._buffer [0];}
return (ORBitSmallSkeleton)_ORBIT_skel_small_JobManager__get_FirstName;
break;
case 's':
if(strcmp((opname + 6), "tatus")) break;
*impl = (gpointer)servant->vepv->JobManager_epv->_get_status;
{ORBit_IInterface *volatile _t_=&JobManager__iinterface;*m_data = (gpointer)&_t_->methods._buffer [1];}
return (ORBitSmallSkeleton)_ORBIT_skel_small_JobManager__get_status;
break;
default: break; 
}
break;
default: break; 
}
break;
default: break; 
}
break;
default: break; 
}
break;
case 's':
if(strcmp((opname + 2), "et_status")) break;
*impl = (gpointer)servant->vepv->JobManager_epv->_set_status;
{ORBit_IInterface *volatile _t_=&JobManager__iinterface;*m_data = (gpointer)&_t_->methods._buffer [2];}
return (ORBitSmallSkeleton)_ORBIT_skel_small_JobManager__set_status;
break;
default: break; 
}
break;
default: break; 
}
return NULL;
}

void POA_JobManager__init(PortableServer_Servant servant,
CORBA_Environment *env)
{
  static PortableServer_ClassInfo class_info = {NULL, (ORBit_small_impl_finder)&get_skel_small_JobManager, "IDL:JobManager:1.0", &JobManager__classid, NULL, &JobManager__iinterface};
  PortableServer_ServantBase__init (       ((PortableServer_ServantBase *)servant), env);
   ORBit_skel_class_register (&class_info,
   (PortableServer_ServantBase *)servant, POA_JobManager__fini,
   ORBIT_VEPV_OFFSET (POA_JobManager__vepv, JobManager_epv),
   (CORBA_unsigned_long) 0);}

void POA_JobManager__fini(PortableServer_Servant servant,
CORBA_Environment *env)
{
  PortableServer_ServantBase__fini(servant, env);
}
