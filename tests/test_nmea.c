#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../main/include/nmea_parser.h"

int main(void) {
    printf("Running NMEA parser unit tests...\n");

    // Test 1: Checksum verification
    const char *known_valid = "$GNRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091218,,,A*42\r\n";
    assert(nmea_verify_checksum(known_valid) == true);

    const char *bad_checksum = "$GNRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091218,,,A*00\r\n";
    assert(nmea_verify_checksum(bad_checksum) == false);

    // Test 2: RMC Parse valid
    nmea_time_t t;
    memset(&t, 0, sizeof(t));
    bool ok = nmea_parse_rmc(known_valid, &t);
    assert(ok == true);
    assert(t.fix_valid == true);
    assert(t.hour == 8);
    assert(t.minute == 35);
    assert(t.second == 59);
    assert(t.day == 9);
    assert(t.month == 12);
    assert(t.year == 2018);

    // Test 3: RMC Parse Void fix ('V')
    const char *void_fix = "$GNRMC,083559.00,V,4717.11437,N,00833.91522,E,0.004,77.52,091218,,,N*5A\r\n";
    assert(nmea_verify_checksum(void_fix) == true);
    assert(nmea_parse_rmc(void_fix, &t) == false);

    // Test 4: ZDA Parse
    // $GNZDA,143025.00,04,11,2026,00,00*7B
    const char *valid_zda = "$GNZDA,143025.00,04,11,2026,00,00*7B\r\n";
    assert(nmea_verify_checksum(valid_zda) == true);
    memset(&t, 0, sizeof(t));
    ok = nmea_parse_zda(valid_zda, &t);
    assert(ok == true);
    assert(t.hour == 14);
    assert(t.minute == 30);
    assert(t.second == 25);
    assert(t.day == 4);
    assert(t.month == 11);
    assert(t.year == 2026);

    printf("All NMEA tests passed successfully!\n");
    return 0;
}
