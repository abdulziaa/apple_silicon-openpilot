#include <cstdio>
#include <cstdlib>
#include <string>

#include "cereal/messaging/messaging.h"
#include "common/timing.h"
#include "common/util.h"

ExitHandler do_exit;

int main(int argc, char *argv[]) {
  PubMaster pm({"androidLog"});

  FILE* pipe = popen("log stream --style syslog", "r");
  if (!pipe) {
    fprintf(stderr, "Failed to open log stream\n");
    return 1;
  }

  char buffer[1024];
  while (!do_exit && fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    std::string line(buffer);

    MessageBuilder msg;
    auto androidEntry = msg.initEvent().initAndroidLog();
    androidEntry.setTs(nanos_since_boot());  // fallback timestamp
    androidEntry.setMessage(line);
    androidEntry.setPid(0);      // No PID available
    androidEntry.setPriority(6); // INFO
    androidEntry.setTag("macOS-log");

    pm.send("androidLog", msg);
  }

  pclose(pipe);
  return 0;
}
