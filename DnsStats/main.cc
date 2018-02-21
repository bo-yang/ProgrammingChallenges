#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include "dns_stats.h"

int main(int argc, char *argv[])
{
	int opt = 0;
    int interval = 5;
    int counts = -1;
    bool debug = false;
    while ((opt = getopt(argc, argv, "i:c:d")) != -1) {
        switch (opt) {
        case 'i':
            interval = atoi(optarg);
            break;
        case 'c':
            counts = atoi(optarg);
            break;
        case 'd':
            debug = true;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-i <interval>] [-c <counts>] [-d]\n", argv[0]);
            fprintf(stderr, "where\n");
            fprintf(stderr, "\t-i <interval>, specifies interval(in seconds) of DNS queries.\n");
            fprintf(stderr, "\t-c <counts>, the total counts for DNS queries per domain, -1 for infinity.\n");
            fprintf(stderr, "\t-d, enable debug.\n");
            exit(EXIT_FAILURE);
        }
    }

	const std::vector<std::string> top_sites {
		"google.com",
		"youtube.com",
		"facebook.com",
		"baidu.com",
		"wikipedia.org",
		"yahoo.com",
		"google.co.in",
		"reddit.com",
		"qq.com",
		"taobao.com" };
    std::vector<SiteDnsStats> site_stats; // in-memory DNS stats
    for (auto &domain : top_sites)
        site_stats.push_back(SiteDnsStats(domain));

    // setup database
    DNSQuerier::DBConfig dbcfg{"dns_stats", "localhost", "dnsstats", "dnsstats", 0};
	DNSQuerier dnsq(dbcfg, interval, debug);
    dnsq.create_table(DNSQuerier::DB_TABLE_STATS);
    dnsq.create_table(DNSQuerier::DB_TABLE_QUERY);

    // periodic DNS query
    int cnt = 0;
    while(true) {
        if (debug)
            std::cout << "\nRound " << cnt << std::endl;

        for (auto &stat: site_stats) {
            if (cnt < 1)
                dnsq.retrieve_stats(stat);
            dnsq.dns_query(stat);
        }

        cnt++;
        if (counts > 0 && cnt >= counts)
            break;

        sleep(interval);
    }

    exit(EXIT_SUCCESS);
}
