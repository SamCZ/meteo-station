#pragma once

static int getBarsSignal(int rssi){
  // 5. High quality: 90% ~= -55db
  // 4. Good quality: 75% ~= -65db
  // 3. Medium quality: 50% ~= -75db
  // 2. Low quality: 30% ~= -85db
  // 1. Unusable quality: 8% ~= -96db
  // 0. No signal
  
  if(rssi >= 0) {
      return 5;
  } else if(rssi >= -40) {
      return 4;
  } else if(rssi >= -60) {
      return 3;
  } else if(rssi >= -70) {
      return 2;
  } else if(rssi >= -80) {
      return 1;
  } else {
      return 0;
  }
}