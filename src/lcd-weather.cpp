#include <absl/cleanup/cleanup.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <cstdio>
#include <glaze/glaze.hpp>
#include <print>

#include "weather_format.hpp"

// from https://gist.github.com/alghanmi/c5d7b761b2c9ab199157
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::println("usage: lcd-weather <latitude> <longitude>");
        return -99;
    }
    CURL *curl_easy_ctx = curl_easy_init();
    absl::Cleanup curl_easy_cleaner = [curl_easy_ctx] {
        curl_easy_cleanup(curl_easy_ctx);
    };
    std::string request_string = "http://api.open-meteo.com/v1/forecast?";
    request_string.append("latitude=");
    request_string.append(argv[1]);
    request_string.append("&longitude=");
    request_string.append(argv[2]);
    request_string.append("&current=temperature_2m");
    std::println("{}", request_string);
    int status =
        curl_easy_setopt(curl_easy_ctx, CURLOPT_URL, request_string.c_str());
    if (status != CURLE_OK) {
        std::println("Failed to setopt CURLOPT_URL!");
        return -1;
    }
    status = curl_easy_setopt(curl_easy_ctx, CURLOPT_HTTPGET, 0);
    if (status != CURLE_OK) {
        std::println("Failed to setopt CURLOPT_HTTPGET!");
        return -2;
    }
    std::string readBuffer;
    status =
        curl_easy_setopt(curl_easy_ctx, CURLOPT_WRITEFUNCTION, WriteCallback);
    if (status != CURLE_OK) {
        std::println("Failed to setopt CURLOPT_WRITEFUNCTION");
        return -3;
    }
    status = curl_easy_setopt(curl_easy_ctx, CURLOPT_WRITEDATA, &readBuffer);
    if (status != CURLE_OK) {
        std::println("Failed to setopt CURLOPT_WRITEDATA");
        return -4;
    }
    status = curl_easy_perform(curl_easy_ctx);
    if (status != CURLE_OK) {
        std::println("Failed to make HTTP Request!");
        return -5;
    }
    weather_info_t my_weather_info{};
    auto s = glz::read_json<weather_info_t>(readBuffer);
    if (s) {
        auto s2 = s.value();
        std::println("temperature_2m: {}", s2.current.temperature_2m);
    } else {
        std::println("Failed to parse the json!");
        return -1;
    }
}
