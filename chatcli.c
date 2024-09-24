#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to hold the response
struct string {
    char *ptr;
    size_t len;
};

// Initialize the response structure
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(1);
    }
    s->ptr[0] = '\0';
}

// Callback function to write the response data
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(1);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->len = new_len;
    s->ptr[new_len] = '\0';
    return size * nmemb;
}

int main(void) {
    CURL *curl;
    CURLcode res;
    struct string response;

    // Initialize the response structure
    init_string(&response);

    // Your OpenAI API key
    char *api_key = "API-KEY-HERE"; // Replace with your actual API key

    // The user input (for simplicity, hardcoded here)
    char *user_input = "Tell me a joke.";

    // JSON data to send to the API
    char json_data[256];
    snprintf(json_data, sizeof(json_data), 
         "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}", 
         user_input);


    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set the OpenAI API endpoint
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");

        // Set the headers (including the API key)
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
        headers = curl_slist_append(headers, auth_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data (user input in JSON format)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            printf("GPT Response: %s\n", response.ptr);  // Print the response

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    free(response.ptr);
    curl_global_cleanup();

    return 0;
}
//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