#include "inspectorlib.h"

int main(void) {
    influxdb_cpp::server_info si("127.0.0.1", 8086, "inspector_data", "", "");
    send_request_fill_db("https://www.sumnotes.net/", si);
    return 0;
}
