
#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
#include "adx_curl.h"

// 请求URL recv 回调
int adx_curl_recv(char *buf, int size, int nmemb, void *dest)
{
    // fwrite(buf, size, nmemb, stdout);
    return (size * nmemb);
}

// GET URL
int adx_curl_open(const char *url)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) return -1;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, adx_curl_recv);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    CURLcode ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return ret;
}

int adx_curl_post(char *url, char *buf)
{

    CURL *curl = curl_easy_init();
    if(curl == NULL)
        return -1; 

    curl_easy_setopt(curl, CURLOPT_POST, 1); 
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // ALL INFO
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);  // HEAD INFO

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "x-openrtb-version: 2.3.1");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recv_post);
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buf);

    CURLcode ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return ret;
}

#if 0
int main()
{

    char buf[4096] = {0};
    FILE *fp = fopen("./buf", "r");
    fread(buf, 1, 4096, fp);
    // fprintf(stdout, "%s\n", buf);

    int ret = adx_curl_post("http://115.182.205.166/o", buf);
    fprintf(stdout, "ret=%d\n", ret);

    return 0;
}
#endif





