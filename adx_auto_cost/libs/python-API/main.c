
#include "adx_auto_cost.h"

/*******************/
/* INIT */
/*******************/
static adx_auto_cost_t *auto_cost_handle = NULL;
static char auto_cost_error[128] = "OK";
PyObject *adx_auto_cost_init(PyObject *self, PyObject *args)
{
    if (auto_cost_handle) Py_RETURN_FALSE;

    adx_auto_cost_conf_t *conf = adx_auto_cost_conf_load(auto_cost_error);
    if (!conf) Py_RETURN_FALSE;

    auto_cost_handle = auto_cost_handle_create(conf, auto_cost_error);
    if (!auto_cost_handle) Py_RETURN_FALSE;

    if (adx_auto_cost_wait_init(auto_cost_handle->conn_wait))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

/*******************/
/* CHECK */
/*******************/
PyObject *adx_auto_cost_check(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle) Py_RETURN_FALSE;
    Py_RETURN_TRUE;
}

PyObject *adx_auto_cost_error(PyObject *self, PyObject *args)
{
    return (PyObject *)Py_BuildValue("s", auto_cost_error);
}

/*******************/
/* WAIT */
/*******************/
PyObject *adx_auto_cost_wait(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle) Py_RETURN_FALSE;

    for(;;) {
	PyObject *dict = adx_auto_cost_wait_get(auto_cost_handle);
	if (dict) return dict;
    }
}

/*******************/
/* BID */
/*******************/
PyObject *adx_auto_cost_bid_create(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle) Py_RETURN_FALSE;
    if (adx_auto_cost_bid_create_event(auto_cost_handle))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

PyObject *adx_auto_cost_bid_write(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle) Py_RETURN_FALSE;

    char *mapid, *adxcode, *mediacode, *regioncode, *value;
    if (!PyArg_ParseTuple(args, "sssss", &mapid, &adxcode, &mediacode, &regioncode, &value))
	Py_RETURN_FALSE;

    if (adx_auto_cost_bid_write_event(auto_cost_handle, mapid, adxcode, mediacode, regioncode, value))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

PyObject *adx_auto_cost_bid_flush(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle) Py_RETURN_FALSE;
    if (adx_auto_cost_bid_flush_event(auto_cost_handle))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

PyObject *adx_auto_cost_push(PyObject *self, PyObject *args)
{
    char *mess = NULL;
    if (!PyArg_ParseTuple(args, "s", &mess))
	Py_RETURN_FALSE;

    if (!auto_cost_handle) Py_RETURN_FALSE;
    if (adx_auto_cost_push_event(auto_cost_handle, mess))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

PyObject *adx_auto_cost_forge(PyObject *self, PyObject *args)
{
    char *policyid, *timebucket, *key, *val;
    if (!PyArg_ParseTuple(args, "ssss", &policyid, &timebucket, &key, &val))
	Py_RETURN_FALSE;

    if (!auto_cost_handle) Py_RETURN_FALSE;
    if (adx_auto_cost_forge_event(auto_cost_handle, policyid, timebucket, key, val))
	Py_RETURN_FALSE;

    Py_RETURN_TRUE;
}

PyObject *adx_get_conf(PyObject *self, PyObject *args)
{
    if (!auto_cost_handle || !auto_cost_handle->conf) Py_RETURN_FALSE;
    return (PyObject *)Py_BuildValue("s", auto_cost_handle->conf->auto_cost_python_log_path);
}

PyObject *adx_get_count(PyObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();
    if (!dict) Py_RETURN_FALSE;

    char *key = NULL;
    if (!PyArg_ParseTuple(args, "s", &key))
	return dict;

    if (!auto_cost_handle) return dict;

    adx_auto_cost_get_count(auto_cost_handle->conn_counter, dict, key);
    return dict;
}

PyObject *adx_cmd(PyObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();
    if (!dict) Py_RETURN_FALSE;

    char *key = NULL;
    if (!PyArg_ParseTuple(args, "s", &key))
	return dict;

    if (!auto_cost_handle) return dict;

    adx_auto_cost_cmd(auto_cost_handle->conn_setting, key);
    return dict;
}


PyObject *adx_get_mapid_bid(PyObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();
    if (!dict) Py_RETURN_FALSE;

    char *mapid = NULL;
    if (!PyArg_ParseTuple(args, "s", &mapid))
	return dict;

    if (!auto_cost_handle) return dict;

    char key[1024];
    sprintf(key, "dsp_mapid_bid_%s", mapid);

    adx_auto_cost_get_map(auto_cost_handle->conn_setting, dict, key);
    return dict;
}

PyObject *adx_set_mapid_bid(PyObject *self, PyObject *args)
{
    char *mapid = NULL;
    char *cost = NULL;
    if (!PyArg_ParseTuple(args, "ss", &mapid, &cost))
	Py_RETURN_FALSE;

    if (!auto_cost_handle) Py_RETURN_FALSE;
   
    char key[1024];
    
    sprintf(key, "DEL dsp_mapid_bid_%s", mapid);
    adx_auto_cost_cmd(auto_cost_handle->conn_setting, key);
    
    sprintf(key, "HSET dsp_mapid_bid_%s all_all_all %s", mapid, cost);
    adx_auto_cost_cmd(auto_cost_handle->conn_setting, key);

    Py_RETURN_TRUE;
}

PyMethodDef adx_auto_cost_methods[] = {

    {"init",	adx_auto_cost_init,		METH_VARARGS,	NULL},
    {"check",	adx_auto_cost_check,		METH_VARARGS,	NULL},
    {"error",	adx_auto_cost_error,		METH_VARARGS,	NULL},

    {"wait",	adx_auto_cost_wait,		METH_VARARGS,	NULL},

    {"bid_create",	adx_auto_cost_bid_create,	METH_VARARGS,	NULL},
    {"bid_write",	adx_auto_cost_bid_write,	METH_VARARGS,	NULL},
    {"bid_flush",	adx_auto_cost_bid_flush,	METH_VARARGS,	NULL},

    {"push",		adx_auto_cost_push,		METH_VARARGS,	NULL},
    {"forge_count",	adx_auto_cost_forge,		METH_VARARGS,	NULL},
    
    {"cmd",	adx_cmd,		METH_VARARGS,	NULL},

    {"get_conf",	adx_get_conf,			METH_VARARGS,	NULL},
    {"get_count",	adx_get_count,			METH_VARARGS,	NULL},
    {"get_mapid_bid",	adx_get_mapid_bid,		METH_VARARGS,	NULL},
    {"set_mapid_bid",	adx_set_mapid_bid,		METH_VARARGS,	NULL},

    {NULL,		NULL,				0,		NULL}
};

PyMODINIT_FUNC initadx_auto_cost()
{
    Py_InitModule("adx_auto_cost", adx_auto_cost_methods);
}


