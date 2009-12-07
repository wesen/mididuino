#include <SDCard.h>

void setup() {
  if (!sd_raw_init()) {
    LCD.line1();
    LCD.puts_fill("RAW ERROR");
    return;
  } 
  else {
    LCD.line1();
    LCD.puts_fill("RAW OK");
    struct sd_raw_info info;
    uint8_t ret = sd_raw_get_info(&info);
    if (!ret) {
      LCD.line2();
      LCD.puts_fill("INFO ERROR");
    } 
    else {
      LCD.line2();
      LCD.putnumberx16(info.capacity >> 16);
      LCD.puts(" " );
      LCD.putnumber(info.format);
    }
  }

  struct partition_struct *partition;
  struct fat_fs_struct    *fs;

  partition = partition_open(sd_raw_read, sd_raw_read_interval,
  sd_raw_write, sd_raw_write_interval,
  0);

  if (!partition) {
    LCD.line1();
    LCD.puts_fill("PART ERROR");
    return;
  } 
  else {
    LCD.line1();
    LCD.puts_fill("PART OK");
  }

  fs = fat_open(partition);
  if (!fs) {
    LCD.line1();
    LCD.puts_fill("FAT ERROR");
    return;
  } 
  else {
    LCD.line1();
    LCD.puts_fill("FAT OK");
  }

}

