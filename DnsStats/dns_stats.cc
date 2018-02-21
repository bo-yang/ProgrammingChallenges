#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include "dns_stats.h"

using namespace std;
using namespace mysqlpp;

// Connect to database
bool DNSQuerier::connect_db()
{
    if (_conn.connected())
        return true;

    if (!_conn.connect("", _dbcfg.server.c_str(), _dbcfg.user.c_str(), _dbcfg.password.c_str(), _dbcfg.port)) {
        cerr << "Failed to connect to " << _dbcfg.server << ":" << _dbcfg.port << endl;
        return false;
    }
    try {
        _conn.select_db(_dbcfg.db);
    } catch (const Exception& er) {
        // create database if not existed
        _conn.create_db(_dbcfg.db);
        _conn.select_db(_dbcfg.db);
    }

    return _conn.connected();
}

// Create table from given database
bool DNSQuerier::create_table(table_type_t type)
{
    if (!_conn.connected())
        connect_db();

    mysqlpp::Query query = _conn.query();
    query << "CREATE TABLE " << _tblmap[type];
    if (type == DB_TABLE_STATS) {
         query << "(domain VARCHAR(128) not null,"
               << "num_queries INT not null,"
               << "avg_query_time FLOAT not null,"
               << "sd_query_time FLOAT not null,"
               << "tm_first_query INT not null,"
               << "tm_last_query INT not null,"
               << "PRIMARY KEY(domain), INDEX(domain),  UNIQUE(domain))";
    } else if (type == DB_TABLE_QUERY) {
        query << "(id INT not null auto_increment,"
              << "domain VARCHAR(128) not null,"
              << "querytime INT not null,"
              << "timestamp INT not null,"
              << "PRIMARY KEY(id), INDEX(id),  UNIQUE(id))";
    }

    try {
        cout << "Creating table " << _tblmap[type] << endl;
        query.execute();
    } catch (BadQuery er) {
        cerr << "Error: " << er.what() << endl;
        return false;
    }

    return true;
}

StoreQueryResult DNSQuerier::db_query(SiteDnsStats &site, table_type_t type)
{
    if (!_conn.connected())
        connect_db();
    mysqlpp::Query query = _conn.query();
    query << "SELECT * FROM " << _tblmap[type]
          << " WHERE domain = '" << site.domain << "'";
    StoreQueryResult res = query.store();
    if (!res)
        cerr << "Failed to query DB: " << query.error() << endl;
    return res;
}

// Get stored stats from database
bool DNSQuerier::retrieve_stats(SiteDnsStats &site)
{
    StoreQueryResult res = db_query(site, DB_TABLE_STATS);
    if (res && res.num_rows()) {
        site.total_queries = res[0]["num_queries"];
        site.avg_query_time = res[0]["avg_query_time"];
        site.sd_query_time = res[0]["sd_query_time"];
        site.tm_first_query = res[0]["tm_first_query"];
        site.tm_last_query = res[0]["tm_last_query"];
    } else {
        return false;
    }

    if(_debug)
        cout << "retrieve_stats: " << site.domain << "total_queries = " << site.total_queries  << endl;

    return true;
}

// Insert query into database
bool DNSQuerier::save_query(const string &domain, const ldns_pkt *p)
{
    assert(p != NULL);
    if (!_conn.connected())
        connect_db();
    mysqlpp::Query query = _conn.query();
    query << "INSERT INTO " << _tblmap[DB_TABLE_QUERY]
          << " (domain, querytime, timestamp) "
          << "VALUES ('" << domain << "'," << p->_querytime << ","
          << (time_t)p->timestamp.tv_sec << ")";

    try {
        query.execute();
    } catch (BadQuery er) {
        cerr << "Error: " << er.what() << endl;
        return false;
    }

    return true;
}

// Update the stats saved in database
bool DNSQuerier::update_stats(SiteDnsStats &site, const ldns_pkt *p)
{
    assert(p != NULL);

    // save dns query into database
    save_query(site.domain, p);

    // calc stats
    site.avg_query_time = (double)(site.total_queries * site.avg_query_time + p->_querytime)
                            / (site.total_queries+1);
    site.total_queries++;
    site.tm_last_query = (time_t)p->timestamp.tv_sec;
    if (site.tm_first_query <= 0)
        site.tm_first_query = site.tm_last_query;

    // calc standard deviation
    StoreQueryResult res = db_query(site, DB_TABLE_QUERY);
    if (res) {
        if (res.num_rows() <= 1) {
            site.sd_query_time = 0;
        } else {
            double sum = 0.0;
            for (size_t i = 0; i < res.num_rows(); ++i) {
                sum += pow(res[i]["querytime"] - site.avg_query_time, 2);
            }
            site.sd_query_time = sqrt(sum/(res.num_rows()-1));
        }
    } else {
        cerr << "standard deviation not updated for " << site.domain << endl;
    }

    // update stats if exists, otherwise insert new row
    mysqlpp::Query query = _conn.query();
    query << "INSERT INTO " << _tblmap[DB_TABLE_STATS]
          << " (domain, num_queries, avg_query_time, sd_query_time, tm_first_query, tm_last_query) "
          << "VALUES ('" << site.domain << "'," << site.total_queries << "," << site.avg_query_time
          << "," << site.sd_query_time << "," << site.tm_first_query << "," << site.tm_last_query << ")"
          << " ON DUPLICATE KEY UPDATE num_queries=" << site.total_queries
          << ", avg_query_time=" << site.avg_query_time << ", sd_query_time=" << site.sd_query_time
          << ", tm_first_query=" << site.tm_first_query << ", tm_last_query=" << site.tm_last_query;

    try {
        query.execute();
    } catch (BadQuery er) {
        cerr << "Error: update_stats - " << er.what() << endl;
        return false;
    }

    return true;
}

// Generate random prefix for DNS queries
string DNSQuerier::random_prefix()
{
	string prefix("");
	int size = rand()%6 + 4; // 4-10 bytes
	for (int i = 0; i < size; ++i) {
		if (i%2 != 0)
			prefix += string(1, 'a' + rand()%26);
		else
			prefix += string(1, '0' + rand()%10);
	}
	return prefix;
}

// Send out DNS queries, update statistics and save them into database. 
bool DNSQuerier::dns_query(SiteDnsStats &site)
{
	ldns_resolver *res = NULL;
	ldns_rdf *domain = NULL;
	ldns_pkt *p = NULL;
	ldns_status s;

	/* create a rdf from the command line arg */
	string name = random_prefix() + "." + site.domain;
	domain = ldns_dname_new_frm_str(name.c_str());
	if (!domain) {
		cout << "Error: failed to create domain." << endl;
		return false;
	}
	if (!ldns_dname_str_absolute(name.c_str()) &&
			ldns_dname_absolute(domain)) {

		/* ldns_dname_new_frm_str makes absolute dnames always!
		 * So deabsolutify domain.
		 */
		ldns_rdf_set_size(domain, ldns_rdf_size(domain) - 1);
	}

	/* create a new resolver from /etc/resolv.conf */
	s = ldns_resolver_new_frm_file(&res, NULL);
	if (s != LDNS_STATUS_OK) {
		ldns_rdf_deep_free(domain);
		cout << "Error: failed to create new resolver." << endl;
		return false;
	}

	/* send DNS query */
	p = ldns_resolver_query(res, domain, LDNS_RR_TYPE_NS,
							LDNS_RR_CLASS_IN, LDNS_RD);
	ldns_rdf_deep_free(domain);
	if (p) {
        update_stats(site, p);

        if(_debug) {
            cout << site.domain << " : "
                << "timestamp = " << p->timestamp.tv_sec
                << ", querytime =" << p->_querytime << " msec" << endl;
        }
	} else {
		cerr << "DNS query failed for " << site.domain << endl;
	}

	ldns_pkt_free(p);
    ldns_resolver_deep_free(res);

	return true;
}
