#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>

typedef struct {
    char* data;
    size_t size;
} IMAPResponse;

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    IMAPResponse* mem = (IMAPResponse*)userp;

    char *ptr = realloc(mem->data, mem->size + total + 1);
    if (!ptr) return 0;

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, total);
    mem->size += total;
    mem->data[mem->size] = 0;

    return total;
}

int main(void)
{
    CURL *curl;
    IMAPResponse chunk = {0};

    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK)
        return (int)result;

    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "info@ikzoekchristelijkehulp.nl");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "3jgnzFDf4DByaQsgyy5H");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);


        /* This fetches message 1 from the user's inbox */
        curl_easy_setopt(curl, CURLOPT_URL,
                "imaps://mail.ikzoekchristelijkehulp.nl/INBOX");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");

        /* Perform the fetch */
        result = curl_easy_perform(curl);

        /* Check for errors */
        if(result != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result));

        printf("RESULT: \n%s\n", chunk.data);
        if (strstr(chunk.data)) {
            // TODO: parse the id's and read them
        }
        /* Always cleanup */
free(chunk.data);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return (int)result;
}

