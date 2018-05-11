#ifndef _TOP_SITES_H_
#define _TOP_SITES_H_

#include <string>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <ldns/ldns.h>
#include <mysql++.h>

/*
Write a C++ (not C) program for Linux or BSD (macOS counts) that periodically sends DNS queries to the name servers of the top 10 sites on the web (according to Alexa) and stores the latency values in a MySQL table. The frequency of queries should be specified by the user on command line.

To make sure you're actually measuring the performance of the name server and not your local DNS cache, you should always prepend a random string to each domain. For example, to measure performance to foo.bar, send a query to <random>.foo.bar (e.g., 1234xpto.foo.bar).

Besides the time series values, your program should keep track of some statistics about each domain in the MySQL database:

	- Average query time
	- Standard deviation of query times
	- Number of queries made so far
	- Timestamp of first and last query made

If the program is run multiple times, it should continue where it left off. That is, the existing statistics should continue to be updated, rather than being reset.

Use mysql++ (http://tangentsoft.net/mysql++/) to interact with MySQL.
Use ldns (http://www.nlnetlabs.nl/projects/ldns/) for generating DNS queries.

Rank Domain
1 google.com
2 youtube.com
3 facebook.com
4 baidu.com
5 wikipedia.org
6 yahoo.com
7 google.co.in
8 reddit.com
9 qq.com
10 taobao.com
*/

struct SiteDnsStats {
	std::string domain; 	// domain name
	uint32_t total_queries; // Number of queries made so far
	double avg_query_time; 	// Average query time
	double sd_query_time;  // Standard deviation of query times
	time_t tm_first_query; 	// Timestamp of the first query made
	time_t tm_last_query; 	// Timestamp of the last query made

	SiteDnsStats(const std::string &name):domain(name), total_queries(0),
					avg_query_time(0.0), sd_query_time(0.0) {
		tm_first_query = 0;
		tm_last_query = 0;
	}
};

class DNSQuerier {
public:
    struct DBConfig {
        std::string db;
        std::string server;
        std::string user;
        std::string password;
        uint32_t port;
    };

    typedef enum {
        DB_TABLE_STATS,
        DB_TABLE_QUERY,
    } table_type_t;

    DNSQuerier(const DBConfig& dbcfg, uint32_t interval=5, bool debug=false) : _interval(interval),
                 _debug(debug), _dbcfg(dbcfg) {
        _tblmap[DB_TABLE_STATS] = "dns_stats";
        _tblmap[DB_TABLE_QUERY] = "dns_queries";
        srand(time(NULL)); /* initialize random seed */
        connect_db();
    }
    ~DNSQuerier() {
        if (_conn.connected())
            _conn.disconnect();
    }

    bool dns_query(SiteDnsStats &site);
    bool create_table(table_type_t type);
    bool retrieve_stats(SiteDnsStats &site);

protected:
    bool connect_db();
    bool update_stats(SiteDnsStats &site, const ldns_pkt *p);
    bool save_query(const std::string &domain, const ldns_pkt *p);
    mysqlpp::StoreQueryResult db_query(SiteDnsStats &site, table_type_t type);

private:
    uint32_t _interval; // in seconds
    bool _debug;
    DBConfig _dbcfg;
    mysqlpp::Connection _conn;
    std::unordered_map<int, std::string> _tblmap;

    std::string random_prefix();
};

#endif // _TOP_SITES_H_
