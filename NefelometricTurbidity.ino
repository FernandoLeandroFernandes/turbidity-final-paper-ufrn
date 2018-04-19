/*
  UNIVERSIDADE FEDERAL DO RIO GRANDE DO NORTE
  CENTRO DE TECNOLOGIA
  DEPARTAMENTO DE COMPUTAÇÃO E AUTOMAÇÃO
  
  ENGENHARIA MECATÔNICA

*/

#include <ResponsiveAnalogRead.h>

int const D1  = 1;
int const D2  = 2;
int const D3  = 3;
int const D4  = 4;
int const D5  = 5;
int const D6  = 6;
int const D7  = 7;

int L1      = D5;
int L2      = D7;
int RELAY   = D3;

int S1      = A0;
int S2      = A1;

int S1_level  = 0;
int S2_level  = 0;

bool verbose = 2;

int levels[]  = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255};
int mapping[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int S1_direct_mapping[] = { 13, 324, 600, 740, 823, 875, 910, 941, 958, 971, 977, 980, 982, 984, 986, 987, 989, 989, 990, 991, 991, 993, 993, 993, 994, 994, 995, 995, 996, 996, 997, 997, 998, 998, 998, 998, 998, 999, 999, 999, 1000, 1000, 1000, 1000, 1000, 1001, 1001, 1001, 1001, 1001, 1002, 1002 };
int S2_direct_mapping[] = { 285, 545, 693, 781, 839, 881, 913, 937, 954, 963, 968, 972, 975, 977, 979, 981, 982, 983, 984, 989, 990, 987, 991, 992, 990, 993, 991, 994, 994, 995, 995, 996, 996, 996, 997, 997, 997, 998, 998, 998, 998, 998, 999, 999, 999, 999, 1000, 1000, 1000, 1000, 1000 };
int S1_indirect_mapping[] = { 1007, 1010, 1021, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1021, 1021, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1021, 1022, 1022, 1021, 1021, 1021, 1021, 1021, 1021 };
int S2_indirect_mapping[] = { 1023, 1023, 1023, 1023, 1023, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1022, 1022, 1021, 1022, 1021, 1022, 1021, 1022, 1021, 1022, 1022, 1023, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1022, 1021, 1021, 1021, 1021, 1021, 1021, 1021, 1021, 1021, 1021 };


// Brilho dos LEDs para leitura
int const BRIGHTEN_AMOUNT   = 5;
int const BRIGHTNESS_MAX    = 255;
int const BRIGHTNESS_MIN    = 0;
int const BRIGHTNESS_OFF    = 0;
int const BRIGHT_OPERATION  = 255;

// Quantidade de aferições em cada leitura
int const SENSOR_READINGS   = 256;
int const READING_DELAY     = 20;
int const READING_STEADINESS= 5;
int const SETTLING_DELAY    = 10;

int const READING_MAX       = 1024;
int const TURBIDITY_INDEX   = 20;
int const TURBIDITY_READINGS= 5;

int analog_ports[] = { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15 };

int analog_inputs[] = { S1, S2 };

ResponsiveAnalogRead S1_reader(S1, false, .8);
ResponsiveAnalogRead S2_reader(S2, false, .8);

bool isAnalogInput(int port) {

  int length = (sizeof(analog_inputs)/sizeof(int));

  for (int i = 0; i < length; i++) {
    if (port == analog_inputs[i]) {
      return true;
    }
  }
  return false;
}

void setupPorts() {

  Serial.begin(9600);

  analogReference(DEFAULT);

  // Configura pinos dos LEDs como saída (OUTPUT)
  pinMode(L1, OUTPUT);
  analogWrite(L1, BRIGHTNESS_OFF);

  pinMode(L2, OUTPUT);
  analogWrite(L2, BRIGHTNESS_OFF);

  pinMode(RELAY, OUTPUT);
  analogWrite(RELAY, LOW);

  // Configura pinos analógicos como entrada (INPUT) 
  // para os sensores e saída (OUTPUT) para os demais
  int length = (sizeof(analog_ports)/sizeof(int));
  for (int i = 0; i < length; i++) {

    if (isAnalogInput(analog_ports[i])) {
      pinMode(analog_ports[i], INPUT);

    } else {
      pinMode(analog_ports[i], OUTPUT);
      digitalWrite(analog_ports[i], LOW);
    }
  }
}

void calibrate() {

  int reading_index;
  int brightness;

  analogWrite(L2, BRIGHTNESS_OFF);

  reading_index = 0;
  brightness = BRIGHTNESS_MIN;
  
  if (verbose) {
      Serial.println("L1 CALIBRATION SECTION BEGIN ");
  }

  do {

    analogWrite(L1, brightness);

    if (verbose) {
        Serial.print("L1 CALIBRATION /// BRIGHTNESS: ");
        Serial.print(brightness, DEC);
    }

    readSensors();
    
    S1_direct_mapping[reading_index] = S1_level;
    S2_indirect_mapping[reading_index] = S2_level;
    reading_index++;

    if (verbose) {
        // Serial.print("L1 CALIBRATION END /// BRIGHTNESS: ");
        // Serial.print(brightness, DEC);
        Serial.print("\tS1: ");
        Serial.print(S1_level, DEC);
        Serial.print("\tS2: ");
        Serial.print(S2_level, DEC);
        Serial.print("\n");
    }

    brightness += BRIGHTEN_AMOUNT;

  } while (brightness <= BRIGHTNESS_MAX);

  if (verbose) {
    Serial.println("L1 CALIBRATION SECTION END");
  }

  analogWrite(L1, BRIGHTNESS_OFF);

  reading_index = 0;
  brightness = BRIGHTNESS_MIN;
  
  if (verbose) {
      Serial.println("L2 CALIBRATION SECTION BEGIN ");
  }

  do {

    analogWrite(L2, brightness);

    if (verbose) {
        Serial.print("L2 CALIBRATION /// BRIGHTNESS: ");
        Serial.print(brightness, DEC);
    }

    readSensors();
    
    S2_direct_mapping[reading_index] = S2_level;
    S1_indirect_mapping[reading_index] = S1_level;
    reading_index++;

    if (verbose) {
        // Serial.print("L2 CALIBRATION END /// BRIGHTNESS: ");
        // Serial.print(brightness, DEC);
        Serial.print("\tS1: ");
        Serial.print(S1_level, DEC);
        Serial.print("\tS2: ");
        Serial.print(S2_level, DEC);
        Serial.print("\n");
    }

    brightness += BRIGHTEN_AMOUNT;

  } while (brightness <= BRIGHTNESS_MAX);

  if (verbose) {
    Serial.println("L2 CALIBRATION SECTION END");
  }

  analogWrite(L2, BRIGHTNESS_OFF);

  reading_index = 0;
  brightness = BRIGHTNESS_MIN;

  if (verbose) {
      Serial.println("CALIBRATION mapping SECTION BEGIN");
      Serial.print("MAP\t\tS1(D)\tS1(I)\tS2(D)\tS2(I)");
      Serial.print("\n");
  }

  do {

    mapping[reading_index] = ((S1_direct_mapping[reading_index] + S2_direct_mapping[reading_index]) / 2);

    if (verbose) {
        Serial.print(mapping[reading_index], DEC);
        Serial.print("\t");
        Serial.print(S1_direct_mapping[reading_index], DEC);
        Serial.print("\t");
        Serial.print(S1_indirect_mapping[reading_index], DEC);
        Serial.print("\t");
        Serial.print(S2_direct_mapping[reading_index], DEC);
        Serial.print("\t");
        Serial.print(S2_indirect_mapping[reading_index], DEC);
        Serial.print("\n");
    }

    reading_index++;

    brightness += BRIGHTEN_AMOUNT;

  } while (brightness <= BRIGHTNESS_MAX);

  if (verbose) {
      Serial.println("CALIBRATION mapping SECTION END");
  }
}

int readings = 0;
float sensor_reading = 0;

int total_readings = 0;
double total_sensor_reading = 0;

float readingToLuminosityLevel_S1(float input) {

  int idx = 0, start = 0, end = 0;
  int length = (sizeof(mapping)/sizeof(int));
  while (idx < length && mapping[idx] < input) idx++;
  
  if (idx == 0) return mapping[0];
  else if (idx > 255) return mapping[255];
  else if (input == mapping[idx]) return mapping[idx];

  end = idx;
  start = idx - 1;

  float slope = ((float)(levels[end]-levels[start]))/((float)(S1_direct_mapping[end]-S1_direct_mapping[start]));
  return ((input - S1_direct_mapping[start])*slope + levels[start]);
}

float readingToLuminosityLevel_S2(float input) {

  int idx = 0, start = 0, end = 0;
  int length = (sizeof(mapping)/sizeof(int));
  while (idx < length && mapping[idx] < input) idx++;
  
  if (idx == 0) return mapping[0];
  else if (idx > 255) return mapping[255];

  end = idx;
  start = idx - 1;

  float slope = ((float)(levels[end]-levels[start]))/((float)(S2_direct_mapping[end]-S2_direct_mapping[start]));
  return ((input - S2_direct_mapping[start])*slope + levels[start]);
}

float turbidityInputRatio;
float turbidityAdjustedRatio;
float S1_adjusted;
float S2_adjusted;

float readTurbidity_S1(int brightness) {

  if (verbose) {
    Serial.print("TURBIDITY S1 ///// ");
  }

  analogWrite(L2, BRIGHTNESS_OFF);

  analogWrite(L1, brightness);
  delay(SETTLING_DELAY);

  readSensors();

  analogWrite(L1, BRIGHTNESS_OFF);

  S1_adjusted = (readingToLuminosityLevel_S1(S1_level));
  S2_adjusted = (readingToLuminosityLevel_S2(S2_level)); //*(1+(absortion(S1_level, brightness)/2));
  
  turbidityInputRatio = (S2_level < S1_level ? S2_level/S1_level : 1);
  turbidityAdjustedRatio = (S2_adjusted < S1_adjusted ? S2_adjusted/S1_adjusted : 1);

  if (verbose) {
    Serial.print("\tS1(input/adjusted): ");
    Serial.print(S1_level);
    Serial.print("/");
    Serial.print(S1_adjusted);
    Serial.print("\t");

    Serial.print("\tS2(input/adjusted): ");
    Serial.print(S2_level);
    Serial.print("/");
    Serial.print(S2_adjusted);
    Serial.print("\t");

    Serial.print("\tTURBIDITY(input/adjusted) : ");
    Serial.print(turbidityInputRatio, DEC);
    Serial.print("/");
    Serial.println(turbidityAdjustedRatio, DEC);

    // Serial.println("TURBIDITY S1 ///// END");
  }

  return turbidityAdjustedRatio;
}

float readTurbidity_S2(int brightness) {

  if (verbose) {
    Serial.print("TURBIDITY S2 ///// ");
  }
  analogWrite(L1, BRIGHTNESS_OFF);

  analogWrite(L2, brightness);
  delay(SETTLING_DELAY);

  readSensors();

  analogWrite(L2, BRIGHTNESS_OFF);

  S2_adjusted = (readingToLuminosityLevel_S1(S2_level));
  S1_adjusted = (readingToLuminosityLevel_S2(S1_level));
  
  turbidityInputRatio = (S2_level < S1_level ? S2_level/S1_level : 1);
  turbidityAdjustedRatio = (S2_adjusted < S1_adjusted ? S2_adjusted/S1_adjusted : 1);

  if (verbose) {
    Serial.print("\tS1(input/adjusted): ");
    Serial.print(S1_level);
    Serial.print("/");
    Serial.print(S1_adjusted);
    Serial.print("\t");

    Serial.print("\tS2(input/adjusted): ");
    Serial.print(S2_level);
    Serial.print("/");
    Serial.print(S2_adjusted);
    Serial.print("\t");

    Serial.print("\tTURBIDITY(input/adjusted) : ");
    Serial.print(turbidityInputRatio, DEC);
    Serial.print("/");
    Serial.println(turbidityAdjustedRatio, DEC);

    // Serial.println("TURBIDITY S1 ///// END");
  }

  return turbidityAdjustedRatio;
}

void readSensors() {

  if (verbose == 2) {
    Serial.println("READ SENSORS ///// BEGIN");
  }

  read_S1();
  read_S2();

  if (verbose == 2) {
    Serial.println("READ SENSORS ///// END");
  }
}

void read_S1() {

  int steady_count = 0;
  int readings_count = 0;

  do {

    delay(READING_DELAY);

    S1_reader.update();
    if (S1_reader.hasChanged()) {
      S1_level = 1024 - S1_reader.getValue();
      steady_count = 0;
    } else {
      steady_count++;
    }

    if (verbose == 2) {
      Serial.print("\tS1(read): ");
      Serial.println(S1_level);
    }

    readings_count++;

  } while (readings_count < SENSOR_READINGS &&
          (steady_count < READING_STEADINESS));

}

void read_S2() {

  int steady_count = 0;
  int readings_count = 0;

  do {

    delay(READING_DELAY);

    S2_reader.update();
    if (S2_reader.hasChanged()) {
      S2_level = 1024 - S2_reader.getValue();
      steady_count = 0;
    } else {
      steady_count++;
    }

    if (verbose == 2) {
      Serial.print("\tS2(read): ");
      Serial.println(S2_level);
    }

    readings_count++;

  } while (readings_count < SENSOR_READINGS &&
          (steady_count < READING_STEADINESS));
}

float readTurbidity(int brightness) {

  double turbidities = 0.0;
  
  for (int i = 0; i < TURBIDITY_READINGS; i++) {
      turbidities += (readTurbidity_S1(brightness) + readTurbidity_S2(brightness))/2;
  }

  return (turbidities/TURBIDITY_READINGS);
}


float turbidity = 0;

void freezeSensor1() {

  if (verbose) {
    Serial.print("FREEZE [S1] /////\t");
  }

  analogWrite(L2, BRIGHTNESS_OFF);
  analogWrite(L1, BRIGHT_OPERATION);

  readSensors();

  turbidityInputRatio = (S2_level < S1_level ? S2_level/S1_level : 1);

  if (verbose) {
    Serial.print("S1(level): ");
    Serial.print(S1_level);
    Serial.print("\t");

    Serial.print("S2(level): ");
    Serial.print(S2_level);
    Serial.print("\t");

    Serial.print("TURBIDITY(level) : ");
    Serial.println(turbidityInputRatio, DEC);
  }

}

void freezeSensor2() {

  if (verbose) {
    Serial.print("FREEZE [S2] /////\t");
  }

  analogWrite(L1, BRIGHTNESS_OFF);
  analogWrite(L2, BRIGHT_OPERATION);

  readSensors();

  turbidityInputRatio = (S1_level < S2_level ? S1_level/S2_level : 1);

  if (verbose) {
    Serial.print("S1(level): ");
    Serial.print(S1_level);
    Serial.print("\t");

    Serial.print("S2(level): ");
    Serial.print(S2_level);
    Serial.print("\t");

    Serial.print("TURBIDITY(level) : ");
    Serial.println(turbidityInputRatio, DEC);
  }

}

void run() {

  turbidity = readTurbidity(BRIGHT_OPERATION);

  if (verbose) {
    Serial.print("GLOBAL TURBIDITY: ");
    Serial.println(turbidity, DEC);
  }

  digitalWrite(RELAY, (turbidity < TURBIDITY_INDEX ? HIGH : LOW));
}

// Rotina de configuração executada uma vez na inicialização do microcontrolador
void setup() {

  // Configura as portas de entrada/saída e de comunicação
  setupPorts();

  // Calibra os sendores, mapeando a curva de reacao a luminosidade
  calibrate();
}

void loop() {

  // Rotina de monitoramento
  // run();

  freezeSensor1();
  freezeSensor2();

  delay(200);
}