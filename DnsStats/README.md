## DNS Stats

### 1. Requirement

Write a C++ (not C) program for Linux or BSD (macOS counts) that periodically sends DNS queries to the name servers of the top 10 sites on the web (according to Alexa) and stores the latency values in a MySQL table. The frequency of queries should be specified by the user on command line.

To make sure you're actually measuring the performance of the name server and not your local DNS cache, you should always prepend a random string to each domain. For example, to measure performance to foo.bar, send a query to `<random>.foo.bar` (e.g., `1234xpto.foo.bar`).

Besides the time series values, your program should keep track of some statistics about each domain in the MySQL database:

- Average query time
- Standard deviation of query times
- Number of queries made so far
- Timestamp of first and last query made

If the program is run multiple times, it should continue where it left off. That is, the existing statistics should continue to be updated, rather than being reset.

Use [mysql++](http://tangentsoft.net/mysql++/) to interact with MySQL.
Use [ldns](http://www.nlnetlabs.nl/projects/ldns/) for generating DNS queries.

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

### 2. Implementation

In my implementation, `DNSQuerier` is the main class to handle the DNS query, statistics and database processing, which is defined in files `dns_stats.[h|cc]`.  `main.cc` defines an object of `DNSQuerier` and call the function `DNSQuerier::dns_query()` periodically. 

`DNSQuerier::dns_query()` sends the DNS queries, calculate statistics and update the database tables. For each DNS query packet, the domain name, query time and timestamp are inserted to table dns_queries. The statistics of each domain are saved in struct SiteDnsStats. The per-domain statistics are saved in table dns_stats. The details of tables dns_queries and dns_stats are explained below.

To try out my code, please download this directory and run `make`. The usage of dns_stats is

```
Usage: ./dns_stats [-i <interval>] [-c <counts>] [-d]
where
	-i <interval>, specifies interval(in seconds) of DNS queries.
	-c <counts>, the total counts for DNS queries per domain, -1 for infinity.
	-d, enable debug.
```

If it complains for lacking of `libdns.so`, please `export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH`.

#### MySQL Database

My implementation assumes that the MySQL server is running on localhost. And it also assumes there is database named “dns_stats” created by user "dnsstats". The default password of user "dnsstats" is also "dnsstats".

Table dns_stats stores the per-domain DNS stats:

```sql
MariaDB [dns_stats]> select * from dns_stats;
+---------------+-------------+----------------+---------------+----------------+---------------+
| domain        | num_queries | avg_query_time | sd_query_time | tm_first_query | tm_last_query |
+---------------+-------------+----------------+---------------+----------------+---------------+
| baidu.com     |          40 |        208.075 |       31.9843 |     1518657151 |    1518658835 |
| facebook.com  |          40 |            135 |       791.878 |     1518657152 |    1518658835 |
| google.co.in  |          40 |          38.85 |       2.69425 |     1518657152 |    1518658836 |
| google.com    |          40 |           38.6 |       2.04814 |     1518657152 |    1518658835 |
| qq.com        |          40 |        192.025 |       81.4848 |     1518657151 |    1518658836 |
| reddit.com    |          40 |         66.275 |       63.5134 |     1518657151 |    1518658836 |
| taobao.com    |          40 |        187.625 |       137.658 |     1518657151 |    1518658836 |
| wikipedia.org |          40 |         82.775 |       44.7658 |     1518657151 |    1518658836 |
| yahoo.com     |          40 |           63.7 |       89.7961 |     1518657151 |    1518658836 |
| youtube.com   |          40 |         39.825 |       7.59854 |     1518657152 |    1518658835 |
+---------------+-------------+----------------+---------------+----------------+---------------+
10 rows in set (0.00 sec)
```

Table dns_queries stores all the history DNS queries:

```sql
MariaDB [dns_stats]> select * from dns_queries where domain='facebook.com';
+-----+--------------+-----------+------------+
| id  | domain       | querytime | timestamp  |
+-----+--------------+-----------+------------+
|   7 | facebook.com |        12 | 1518657152 |
|  17 | facebook.com |        12 | 1518657153 |
|  27 | facebook.com |        12 | 1518657191 |
|  37 | facebook.com |        12 | 1518657193 |
|  43 | facebook.com |        15 | 1518657437 |
|  53 | facebook.com |        15 | 1518657439 |
|  63 | facebook.com |         4 | 1518657862 |
|  73 | facebook.com |         5 | 1518657864 |
|  83 | facebook.com |        13 | 1518657866 |
|  93 | facebook.com |         4 | 1518657881 |
```

During the init of program dns_stats, the statistics are retrieved from table dns_stats. For example, after running the following command

```
$ ./dns_stats -c 2
```

Table `dns_stats` became

```sql
MariaDB [dns_stats]> select * from dns_stats;
+---------------+-------------+----------------+---------------+----------------+---------------+
| domain        | num_queries | avg_query_time | sd_query_time | tm_first_query | tm_last_query |
+---------------+-------------+----------------+---------------+----------------+---------------+
| baidu.com     |          42 |        207.452 |       31.5202 |     1518657151 |    1518673044 |
| facebook.com  |          42 |        128.929 |       772.812 |     1518657152 |    1518673044 |
| google.co.in  |          42 |        38.8571 |       2.62791 |     1518657152 |    1518673045 |
| google.com    |          42 |        38.6667 |       2.30235 |     1518657152 |    1518673044 |
| qq.com        |          42 |        191.762 |       79.6507 |     1518657151 |    1518673045 |
| reddit.com    |          42 |        64.9286 |       62.4005 |     1518657151 |    1518673045 |
| taobao.com    |          42 |        203.762 |        152.85 |     1518657151 |    1518673045 |
| wikipedia.org |          42 |         83.619 |        45.392 |     1518657151 |    1518673044 |
| yahoo.com     |          42 |        60.9048 |       88.4878 |     1518657151 |    1518673045 |
| youtube.com   |          42 |        39.7857 |       7.41632 |     1518657152 |    1518673044 |
+---------------+-------------+----------------+---------------+----------------+---------------+
10 rows in set (0.00 sec)
```

Because two new DNS queries were sent.

The standard deviation of DNS query time is calculated based on all history DNS queries saved in table `dns_queries`.

