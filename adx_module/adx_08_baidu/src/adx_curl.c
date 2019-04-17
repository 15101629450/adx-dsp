
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "adx_curl.h"

int post_recv(void *buf, size_t size, size_t nmemb, void *stream)
{
    return size * nmemb;
}

int post_send(const char *url, const char *buf, int size)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) return -1;

    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "x-openrtb-version: 2.3.1");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_recv);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

    CURLcode ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return ret;
}



