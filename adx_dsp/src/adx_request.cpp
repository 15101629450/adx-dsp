
#include "adx_dsp.h"

int adx_request_init(adx_request_t *r)
{
    /*** post buffer ***/
    r->body_buffer[0] = 0;
    FCGX_Request *fcgi_app = &r->fcgi_app;
    r->content_length = atoi(FCGX_GetParam("CONTENT_LENGTH", fcgi_app->envp));
    if (r->content_length <= 0 || r->content_length >= ADX_POST_SIZE - 1)
        return -1;

    FCGX_GetStr(r->body_buffer, r->content_length, fcgi_app->in);
    r->body_buffer[r->content_length] = 0;
#if 0
    /*** cache ***/
    r->map_list = adx_cache_find(get_cache_root(), ADX_CACHE_STR("map_list"));
    r->policy_list = adx_cache_find(get_cache_root(), ADX_CACHE_STR("policy_list"));
    r->creative_list = adx_cache_find(get_cache_root(), ADX_CACHE_STR("creative_list"));
    r->template_list = adx_cache_find(get_cache_root(), ADX_CACHE_STR("templates"));
    if (!r->map_list || !r->policy_list || !r->creative_list || !r->template_list)
        return -1;

    r->element_root = adx_cache_find(get_cache_root(), ADX_CACHE_STR("element_root"));
    if (!r->element_root)
        return -1;
#endif
    /*** request to struct ***/
    r->adx = 0;
    r->bid = NULL;
    memset(&r->device, 0, sizeof(adx_com_device));
    memset(&r->app, 0, sizeof(adx_com_app));
    memset(&r->geo, 0, sizeof(adx_com_geo));

    adx_list_init(&r->adx_slot_list);
    adx_list_init(&r->device.device_id);
    return 0;
}

int adx_request_parse(adx_request_t *r)
{
    ComRequest com_request;
    if (!com_request.ParseFromArray(r->body_buffer, r->content_length))
        return -1;

    adx_com_device *device = &r->device;
    adx_com_app *app = &r->app;
    adx_com_geo *geo = &r->geo;

    // bid
    r->adx = com_request.adx();
    r->bid = StrDup(r->pool, com_request.bid());

    /*** adx_slot ***/
    for (int i = 0; i < com_request.adx_slot_size(); i++) {
        adx_com_slot *adx_slot = (adx_com_slot *)adx_alloc(r->pool, sizeof(adx_com_slot));
        adx_list_add(&r->adx_slot_list, &adx_slot->next);
        adx_slot->device = device;
        adx_slot->app = app;
        adx_slot->geo = geo;

        // ComRequest_AdxSlot com_adx_slot = com_request.adx_slot(i);
        ComRequest_AdxSlot *com_adx_slot = com_request.mutable_adx_slot(i);

        adx_slot->adx_slot_id = StrDup(r->pool, com_adx_slot->adx_slot_id());
        adx_slot->adx_slot_sequence_id = com_adx_slot->adx_slot_sequence_id();
        adx_slot->adx_slot_type = com_adx_slot->adx_slot_type();
        adx_slot->price_floor = com_adx_slot->price_floor();

        // banner 信息
        adx_slot->banner.width = com_adx_slot->banner().width();
        adx_slot->banner.height = com_adx_slot->banner().height();
        sprintf(adx_slot->banner.key, "%dX%d",
                adx_slot->banner.width, adx_slot->banner.height);

        // native 信息
        adx_slot->native.width = com_adx_slot->native().width();
        adx_slot->native.height = com_adx_slot->native().height();
        sprintf(adx_slot->native.key, "%dX%d",
                adx_slot->native.width, adx_slot->native.height);

        adx_slot->native.icon.width = com_adx_slot->native().icon_width();
        adx_slot->native.icon.height = com_adx_slot->native().icon_height();
        sprintf(adx_slot->native.icon.key, "%dX%d",
                adx_slot->native.icon.width, adx_slot->native.icon.height);

        // video 信息
        adx_slot->video.width = com_adx_slot->video().width();
        adx_slot->video.height = com_adx_slot->video().height();
        adx_slot->video.min_duration = com_adx_slot->video().min_duration();
        adx_slot->video.max_duration = com_adx_slot->video().max_duration();
    }

    /*** device -> id ***/
    // ComRequest_Device com_device = com_request.device();
    ComRequest_Device *com_device = com_request.mutable_device();
    for (int i = 0; i < com_device->device_id_size(); i++) {
        adx_com_device_id *device_id = (adx_com_device_id *)adx_alloc(r->pool, sizeof(adx_com_device_id));
        adx_list_add(&device->device_id, &device_id->next);

        ComRequest_Device_DeviceID com_device_id = com_device->device_id(i);
        device_id->id = StrDup(r->pool, com_device_id.id());
        device_id->type = com_device_id.type();
    }

    /*** device -> info ***/
    device->device_type = com_device->device_type();
    device->os = com_device->os();
    device->os_version = StrDup(r->pool, com_device->os_version());
    device->carrier = com_device->carrier();
    device->connection = com_device->connection();
    device->brand = StrDup(r->pool, com_device->brand());
    device->model = StrDup(r->pool, com_device->model());
    device->user_agent = StrDup(r->pool, com_device->user_agent());
    device->ip = StrDup(r->pool, com_device->ip());
    device->ip_code = getRegionCode(r->IPB, device->ip);

    /*** app ***/
    // ComRequest_App com_app = com_request.app();
    ComRequest_App *com_app = com_request.mutable_app();

    app->app_id = StrDup(r->pool, com_app->app_id());
    app->app_category = com_app->app_category();
    return 0;
}

int adx_request(adx_request_t *r)
{
    if (adx_request_init(r)) return -1;
    if (adx_request_parse(r)) return -1;
    // return 0;

    adx_com_device *device = &r->device;
    adx_com_app *app = &r->app;
    // adx_com_geo *geo = &r->geo;

    fprintf(stdout, "\n");
    fprintf(stdout, "adx = %d\n", r->adx);
    fprintf(stdout, "bid = %s\n", r->bid);

    int index = 0;
    adx_list_t *p = NULL;
    adx_list_for_each(p, &r->adx_slot_list) {
        adx_com_slot *adx_slot = (adx_com_slot *)p;

        fprintf(stdout, "adx_slot[%d] -> adx_slot_id = %s\n", index, adx_slot->adx_slot_id);
        fprintf(stdout, "adx_slot[%d] -> adx_slot_sequence_id = %d\n", index, adx_slot->adx_slot_sequence_id);
        fprintf(stdout, "adx_slot[%d] -> adx_slot_type = %d\n", index, adx_slot->adx_slot_type);

        fprintf(stdout, "adx_slot[%d] -> banner -> w = %d\n", index, adx_slot->banner.width);
        fprintf(stdout, "adx_slot[%d] -> banner -> h = %d\n", index, adx_slot->banner.height);
        fprintf(stdout, "adx_slot[%d] -> banner -> key = %s\n", index, adx_slot->banner.key);

        fprintf(stdout, "adx_slot[%d] -> native -> w = %d\n", index, adx_slot->native.width);
        fprintf(stdout, "adx_slot[%d] -> native -> h = %d\n", index, adx_slot->native.height);
        fprintf(stdout, "adx_slot[%d] -> native -> key = %s\n", index, adx_slot->native.key);

        fprintf(stdout, "adx_slot[%d] -> native -> icon -> w = %d\n", index, adx_slot->native.icon.width);
        fprintf(stdout, "adx_slot[%d] -> native -> icon -> h = %d\n", index, adx_slot->native.icon.height);
        fprintf(stdout, "adx_slot[%d] -> native -> icon -> key = %s\n", index, adx_slot->native.icon.key);

        fprintf(stdout, "adx_slot[%d] -> video -> w = %d\n", index, adx_slot->video.width);
        fprintf(stdout, "adx_slot[%d] -> video -> h = %d\n", index, adx_slot->video.height);
        fprintf(stdout, "adx_slot[%d] -> video -> key = %s\n", index, adx_slot->video.key);

        fprintf(stdout, "adx_slot[%d] -> price_floor = %d\n", index, adx_slot->price_floor);
        index++;
    }

    index = 0;
    adx_list_for_each(p, &device->device_id) {
        adx_com_device_id *device_id = (adx_com_device_id *)p;
        fprintf(stdout, "device -> device_id[%d] -> id = %s\n", index, device_id->id);
        fprintf(stdout, "device -> device_id[%d] -> type = %d\n", index, device_id->type);
        index++;
    }

    fprintf(stdout, "device -> device_type = %d\n",     device->device_type);
    fprintf(stdout, "device -> os = %d\n",              device->os);
    fprintf(stdout, "device -> os_version = %s\n",      device->os_version);
    fprintf(stdout, "device -> carrier = %d\n",         device->carrier);
    fprintf(stdout, "device -> connection = %d\n",      device->connection);
    fprintf(stdout, "device -> brand = %s\n",           device->brand);
    fprintf(stdout, "device -> model = %s\n",           device->model);
    fprintf(stdout, "device -> user_agent = %s\n",      device->user_agent);
    fprintf(stdout, "device -> ip = %s\n",              device->ip);
    fprintf(stdout, "device -> ip_code = %d\n",         device->ip_code);

    fprintf(stdout, "app -> app_id = %s\n",             app->app_id);
    fprintf(stdout, "app -> app_category = %d\n",       app->app_category);
    return 0;
}






