
#include "adx_dsp.h"

int adx_response(adx_request_t *r, int status, const char *buffer)
{
    char headers[128];
    int size = sprintf(headers, "Status: %d\r\n\r\n", status);
    FCGX_PutStr(headers, size, r->fcgi_app.out);
    if (buffer) FCGX_PutStr(buffer, strlen(buffer), r->fcgi_app.out);
    FCGX_PutStr("\r\n", 2, r->fcgi_app.out);
    FCGX_Finish_r(&r->fcgi_app);
    return 0;
}

int adx_response_empty(adx_request_t *r)
{
    return adx_response(r, 200, "adx_dsp:empty");
}

int adx_response(adx_request_t *r)
{
    adx_list_t *p = NULL;
    adx_list_for_each(p, &r->adx_slot_list) {
        // adx_com_slot *adx_slot = (adx_com_slot *)p;
    }

    return adx_response(r, 200, "OK");
}



