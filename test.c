#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "approxidate.h"

static int errors = 0;

#define assert_equal(a, b) if (a != b) { \
	fprintf(stderr, "Error at line %d: %ld != %ld\n", __LINE__, (long)a, (long)b); \
	errors++; }

static time_t _start_of_day(time_t sec)
{
	return sec - (sec % 86400);
}

int main()
{
	long usec;
	time_t ts;
	struct tm *tm;
	char buff[128];
	struct timeval tv;

	approxidate("10/Mar/2013:00:00:02.003", &tv);
	assert_equal(tv.tv_sec, 1362891602);
	assert_equal(tv.tv_usec, 3000);

	approxidate("10/Mar/2013:00:00:02", &tv);
	assert_equal(tv.tv_sec, 1362891602);
	assert_equal(tv.tv_usec, 0);

	approxidate("10/Mar/2013:00:00:07", &tv);
	assert_equal(tv.tv_sec, 1362891607);
	assert_equal(tv.tv_usec, 0);

	approxidate("10/Mar/2012:00:00:07", &tv);
	assert_equal(tv.tv_sec, 1331355607);
	assert_equal(tv.tv_usec, 0);

	approxidate("10/Mar/2012:00:00:07 +0500", &tv);
	assert_equal(tv.tv_sec, 1331319607);
	assert_equal(tv.tv_usec, 0);

	approxidate("10/Mar/2012:00:00:07.657891 +0500", &tv);
	assert_equal(tv.tv_sec, 1331319607);
	assert_equal(tv.tv_usec, 657891);

	approxidate("10/Mar/2012:00:00:07.657891 +1400", &tv);
	assert_equal(tv.tv_sec, 1331287207);
	assert_equal(tv.tv_usec, 657891);

	approxidate("10/Mar/2012:00:00:07.657891 -110", &tv);
	assert_equal(tv.tv_sec, 1331355607);
	assert_equal(tv.tv_usec, 657891);

	approxidate("mar 10 2013 00:00:07", &tv);
	assert_equal(tv.tv_sec, 1362891607);
	assert_equal(tv.tv_usec, 0);

	approxidate("mar 10 2013 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("march 10 2013 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("march 10 2013 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("10 march 2013 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("2013 10 march 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("2013 march 10 04:00:07 -0500", &tv);
	assert_equal(tv.tv_sec, 1362906007);
	assert_equal(tv.tv_usec, 0);

	approxidate("00:00:07.657891", &tv);
	assert_equal(tv.tv_usec, 657891);

	approxidate("23:11:07.9876 +1400", &tv);
	assert_equal(tv.tv_usec, 987600);

	approxidate("23:11:07.9876", &tv);
	assert_equal(tv.tv_usec, 987600);

	approxidate("1/1/2014", &tv);
	assert_equal(_start_of_day(tv.tv_sec), 1388534400);

	/*
	 * Git doesn't allow dates more than 10 days in the future. Make sure
	 * approxidate does.
	 *   * if today is 3/15/2015
	 *   * 8/15/2015 should parse as "Aug 15, 2015", not "Mar 8, 2015".
	 */
	ts = time(NULL);
	ts += 86400 * 31 * 5;
	tm = gmtime(&ts);
	strftime(buff, sizeof(buff), "%m/%d/%Y", tm);
	approxidate(buff, &tv);
	assert_equal(_start_of_day(tv.tv_sec), _start_of_day(ts));

	gettimeofday(&tv, NULL);
	usec = tv.tv_usec;
	approxidate("10/Mar/2012", &tv);

	if (!((usec - 10000) < tv.tv_usec && (usec + 10000) > tv.tv_usec)) {
		fprintf(stderr, "Error: usec calculation for anonymous time is off\n"); \
		errors++;
	}

	if (errors > 0) {
		fprintf(stderr, "%d tests failed\n", errors);
		return 1;
	} else {
		fprintf(stderr, "All tests passed!\n");
	}

	return 0;
}
