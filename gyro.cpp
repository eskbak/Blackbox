float read_theta(int chan) {   //reads angle from motion table

  int byte_low = 0;
  int byte_high = 0;
  int raw_angle = 0;
  float deg_angle = 0;

  if (chan == 1) {
    I2Cone.beginTransmission(0x36);
    I2Cone.write(0x0D);
    I2Cone.endTransmission();

    I2Cone.requestFrom(0x36, 1);
    while (I2Cone.available()) {
      Serial.print(" :) ");
      byte_low = I2Cone.read();
    }

    I2Cone.beginTransmission(0x36);
    I2Cone.write(0x0C);
    I2Cone.endTransmission();

    I2Cone.requestFrom(0x36, 1);
    while (I2Cone.available()) {
      byte_high = I2Cone.read();
    }

    byte_high = byte_high & 0b00001111;
    byte_high = byte_high << 8;
    raw_angle = byte_high | byte_low;
    deg_angle = raw_angle * 0.087890625;

    float corrected_angle = (deg_angle - initial_angle_1);

    if (corrected_angle >= 360) {
      corrected_angle -= 360;
    }
    if (corrected_angle < 0) {
      corrected_angle += 360;
    }

    return corrected_angle;
  }

  else if (chan == 2) {
    I2Ctwo.beginTransmission(0x36);
    I2Ctwo.write(0x0D);
    I2Ctwo.endTransmission();

    I2Ctwo.requestFrom(0x36, 1);
    while (I2Ctwo.available()) { // slave may send less than requested
      byte_low = I2Ctwo.read();
    }

    I2Ctwo.beginTransmission(0x36);
    I2Ctwo.write(0x0C);
    I2Ctwo.endTransmission();

    I2Ctwo.requestFrom(0x36, 1);
    while (I2Ctwo.available()) { // slave may send less than requested
      byte_high = I2Ctwo.read();
    }

    byte_high = byte_high & 0b00001111;
    byte_high = byte_high << 8;
    raw_angle = byte_high | byte_low;
    deg_angle = raw_angle * 0.087890625; //12 bits to 360 degrees

    float corrected_angle = (deg_angle - initial_angle_2);

    if (corrected_angle >= 360) {
      corrected_angle -= 360;
    }
    if (corrected_angle < 0) {
      corrected_angle += 360;
    }

    return corrected_angle;
  }
}
