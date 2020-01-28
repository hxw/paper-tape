// Reader.ino

#include <Arduino.h>

// stepper phase pins
const int motor_1 = 8;
const int motor_2 = 9;
const int motor_3 = 10;
const int motor_4 = 11;

const int button_start = 22;
const int button_stop = 25;
const int button_step = 27;
const int button_mode = 23;

const int led_0 = 24;
const int led_1 = 28;
const int led_2 = 32;
const int led_3 = 36;
const int led_4 = 40;
const int led_5 = 44;
const int led_6 = 48;
const int led_7 = 52;

// mode setting
typedef enum {
  mode_8_bit_hex,
  mode_7_bit_ascii,
  mode_5_bit_hex,
  mode_ascii,
  mode_elliott,
  mode_h_code,
  mode_calibrate
} mode_t;
mode_t current_mode = mode_8_bit_hex;

// macros
#define SizeOfArray(a) (sizeof(a) / sizeof((a)[0]))

// for calibration tests
int max_bit_analog[8];
int max_sprocket_analog = 0;
int min_bit_analog[8];
int min_sprocket_analog = 0;

// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);
  pinMode(motor_3, OUTPUT);
  pinMode(motor_4, OUTPUT);

  motor_zero();

  pinMode(button_start, INPUT_PULLUP);
  pinMode(button_stop, INPUT_PULLUP);
  pinMode(button_step, INPUT_PULLUP);
  pinMode(button_mode, INPUT_PULLUP);

  pinMode(led_0, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);
  pinMode(led_4, OUTPUT);
  pinMode(led_5, OUTPUT);
  pinMode(led_6, OUTPUT);
  pinMode(led_7, OUTPUT);

  Serial.begin(9600);

  digitalWrite(led_0, LOW);
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, LOW);
  digitalWrite(led_4, LOW);
  digitalWrite(led_5, LOW);
  digitalWrite(led_6, LOW);
  digitalWrite(led_7, LOW);

  delay(100);
  digitalWrite(led_0, HIGH);
  delay(50);
  digitalWrite(led_1, HIGH);
  delay(50);
  digitalWrite(led_2, HIGH);
  delay(50);
  digitalWrite(led_3, HIGH);
  delay(50);
  digitalWrite(led_4, HIGH);
  delay(50);
  digitalWrite(led_5, HIGH);
  delay(50);
  digitalWrite(led_6, HIGH);
  delay(50);
  digitalWrite(led_7, HIGH);

  delay(50);
  digitalWrite(led_0, LOW);
  delay(50);
  digitalWrite(led_1, LOW);
  delay(50);
  digitalWrite(led_2, LOW);
  delay(50);
  digitalWrite(led_3, LOW);
  delay(50);
  digitalWrite(led_4, LOW);
  delay(50);
  digitalWrite(led_5, LOW);
  delay(50);
  digitalWrite(led_6, LOW);
  delay(50);
  digitalWrite(led_7, LOW);

  delay(100);
}

void next_mode(void) {
  int eight = LOW;
  int seven = LOW;
  int five = LOW;
  int ascii = LOW;
  int elliott = LOW;
  int h_code = LOW;

  switch (current_mode) {
  case mode_8_bit_hex:
    current_mode = mode_7_bit_ascii;
    seven = HIGH;
    break;
  case mode_7_bit_ascii:
    current_mode = mode_5_bit_hex;
    five = HIGH;
    break;
  case mode_5_bit_hex:
    current_mode = mode_ascii;
    five = HIGH;
    ascii = HIGH;
    break;
  case mode_ascii:
    current_mode = mode_elliott;
    five = HIGH;
    elliott = HIGH;
    break;
  case mode_elliott:
    current_mode = mode_h_code;
    five = HIGH;
    h_code = HIGH;
    break;
  case mode_h_code:
    current_mode = mode_calibrate;
    eight = HIGH;
    ascii = HIGH;
    break;
  default:
  case mode_calibrate:
    current_mode = mode_8_bit_hex;
    eight = HIGH;
    break;
  }

  for (int i = 0; i < 3; ++i) {
    digitalWrite(led_0, LOW);
    digitalWrite(led_1, LOW);
    digitalWrite(led_2, LOW);
    digitalWrite(led_3, LOW);
    digitalWrite(led_4, LOW);
    digitalWrite(led_5, LOW);
    digitalWrite(led_6, LOW);
    digitalWrite(led_7, LOW);

    delay(10);

    digitalWrite(led_0, ascii);
    digitalWrite(led_1, elliott);
    digitalWrite(led_2, h_code);
    digitalWrite(led_3, LOW);
    digitalWrite(led_4, five);
    digitalWrite(led_5, LOW);
    digitalWrite(led_6, seven);
    digitalWrite(led_7, eight);

    delay(50);
  }

  memset(max_bit_analog, 0, sizeof(max_bit_analog));
  max_sprocket_analog = 0;
  min_bit_analog[0] = 9999;
  min_bit_analog[1] = 9999;
  min_bit_analog[2] = 9999;
  min_bit_analog[3] = 9999;
  min_bit_analog[4] = 9999;
  min_bit_analog[5] = 9999;
  min_bit_analog[6] = 9999;
  min_bit_analog[7] = 9999;
  min_sprocket_analog = 9999;
}

// motor start/stop
typedef enum { motor_stop, motor_start, motor_once } motor_state_t;
motor_state_t motor_state = motor_stop;

int debounce_step = 0;
int last_step = 0;
bool step_done = false;

int debounce_mode = 0;
int last_mode = 0;
bool mode_done = false;

// buttons - active low
void scan_buttons(void) {
  if (LOW == digitalRead(button_stop)) {
    motor_state = motor_stop;
  } else if (LOW == digitalRead(button_start)) {
    motor_state = motor_start;
  }

  int m = digitalRead(button_step);
  if (last_step == m) {
    if (200 == debounce_step) {
      if (!step_done && LOW == m && motor_stop == motor_state) {
        motor_state = motor_once;
        step_done = true;
      } else if (HIGH == m) {
        step_done = false;
      }
    } else {
      ++debounce_step;
    }
  } else {
    debounce_step = 0;
  }
  last_step = m;

  m = digitalRead(button_mode);
  if (last_mode == m) {
    if (200 == debounce_mode) {
      if (!mode_done && LOW == m && motor_stop == motor_state) {
        next_mode();
        mode_done = true;
      } else if (HIGH == m) {
        mode_done = false;
      }
    } else {
      ++debounce_mode;
    }
  } else {
    debounce_mode = 0;
  }
  last_mode = m;
}

// zero motor current
void motor_zero() {
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);
  digitalWrite(motor_3, LOW);
  digitalWrite(motor_4, LOW);
}

int stepper_phase = 0;

// stepper state machine
void motor_step() {
  if (motor_stop == motor_state) {
    motor_zero();
    return;
  }

  ++stepper_phase;
  switch (stepper_phase & 0x03) {
  case 0:
    digitalWrite(motor_1, HIGH);
    digitalWrite(motor_2, LOW);
    digitalWrite(motor_3, HIGH);
    digitalWrite(motor_4, LOW);
    break;
  case 1:
    digitalWrite(motor_1, HIGH);
    digitalWrite(motor_2, LOW);
    digitalWrite(motor_3, LOW);
    digitalWrite(motor_4, HIGH);
    break;
  case 2:
    digitalWrite(motor_1, LOW);
    digitalWrite(motor_2, HIGH);
    digitalWrite(motor_3, LOW);
    digitalWrite(motor_4, HIGH);
    break;
  case 3:
    digitalWrite(motor_1, LOW);
    digitalWrite(motor_2, HIGH);
    digitalWrite(motor_3, HIGH);
    digitalWrite(motor_4, LOW);
    break;
  }
}

// to print just ASCII rather than description of control codes
#define ONLY_ELLIOTT 1

#if ONLY_ELLIOTT
const char *letters[32] = {"",  "A", "B", "C", "D", "E",  "F",  "G",
                           "H", "I", "J", "K", "L", "M",  "N",  "O",
                           "P", "Q", "R", "S", "T", "U",  "V",  "W",
                           "X", "Y", "Z", "",  " ", "\r", "\n", ""};

const char *figures_ascii[32] = {"",  "1", "2", "*", "4", "$",  "=",  "7",
                                 "8", "'", ",", "+", ":", "-",  ".",  "%",
                                 "0", "(", ")", "3", "?", "5",  "6",  "/",
                                 "@", "9", "#", "",  " ", "\r", "\n", ""};

const char *figures_elliott[32] = {"",  "1", "2", "*", "4", "$",  "=",  "7",
                                   "8", "'", ",", "+", ":", "-",  ".",  "%",
                                   "0", "(", ")", "3", "?", "5",  "6",  "/",
                                   "@", "9", "£", "",  " ", "\r", "\n", ""};

const char *figures_h_code[32] = {"",  "1", "2", "*", "4", "<",  "=",  "7",
                                  "8", "'", ",", "+", ":", "-",  ".",  ">",
                                  "0", "(", ")", "3", "?", "5",  "6",  "/",
                                  "@", "9", "→", "",  " ", "\r", "\n", ""};

#else
const char *letters[32] = {"Blank Tape", "A",
                           "B",          "C",
                           "D",          "E",
                           "F",          "G",
                           "H",          "I",
                           "J",          "K",
                           "L",          "M",
                           "N",          "O",
                           "P",          "Q",
                           "R",          "S",
                           "T",          "U",
                           "V",          "W",
                           "X",          "Y",
                           "Z",          "Figure shift",
                           "Space",      "Carriage Return",
                           "Line Feed",  "Letter Shift"};

const char *figures[32] = {"Blank Tape", "1",
                           "2",          "*",
                           "4",          "$ or <",
                           "=",          "7",
                           "8",          "'",
                           ",",          "+",
                           ":",          "-",
                           ".",          "% or >",
                           "0",          "(",
                           ")",          "3",
                           "?",          "5",
                           "6",          "/",
                           "@",          "9",
                           "£ or →",     "Figure shift",
                           "Space",      "Carriage Return",
                           "Line Feed",  "Letter Shift"};
#endif

int last_sprocket = 0;
int sprocket_debounce = 0;
bool shifted = false;

int timeout = 0;

int step_delay;

// for analog inputs
const int sprocket_samples = 2;
int bit_analog[8][sprocket_samples];
int sprocket_analog[sprocket_samples];
int sprocket_index = 0;

const int sensor_value = 400 * sprocket_samples;

int opto_to_bit(const int a[]) {
  int v = 0;
  for (size_t i = 0; i < sprocket_samples; ++i) {
    v += a[i];
  }
  if (v > sensor_value) {
    return 1;
  }
  return 0;
}

// the loop function runs over and over again forever
void loop() {

  // check buttons
  scan_buttons();
  delayMicroseconds(300);

  // advance motor
  ++step_delay;
  if (step_delay < 8) {
    return;
  }
  step_delay = 0;
  motor_step();

  ++sprocket_index;
  if (sprocket_index >= SizeOfArray(sprocket_analog)) {
    sprocket_index = 0;
  }
  sprocket_analog[sprocket_index] = analogRead(A8);

  bit_analog[0][sprocket_index] = analogRead(A0);
  bit_analog[1][sprocket_index] = analogRead(A1);
  bit_analog[2][sprocket_index] = analogRead(A2);
  bit_analog[3][sprocket_index] = analogRead(A3);
  bit_analog[4][sprocket_index] = analogRead(A4);
  bit_analog[5][sprocket_index] = analogRead(A5);
  bit_analog[6][sprocket_index] = analogRead(A6);
  bit_analog[7][sprocket_index] = analogRead(A7);

  int sprocket = opto_to_bit(sprocket_analog);
  if (sprocket == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  ++timeout;
  if (motor_stop != motor_state && timeout > 1500) {
    motor_state = motor_stop;
    timeout = 0;
  } else if (motor_stop == motor_state) {
    timeout = 0;
  }

  if (sprocket != last_sprocket) {
    timeout = 0;
    sprocket_debounce = 0;
    last_sprocket = sprocket;
    return;
  }
  ++sprocket_debounce;
  if (sprocket_debounce != 3) {
    return;
  }
  if (mode_calibrate != current_mode && 0 == sprocket) {
    return;
  }

  if (motor_once == motor_state) {
    motor_state = motor_stop;
    timeout = 0;
  }

  int b[8];
  for (size_t i = 0; i < SizeOfArray(b); ++i) {
    b[i] = opto_to_bit(bit_analog[i]);
  }

  digitalWrite(led_0, b[0]);
  digitalWrite(led_1, b[1]);
  digitalWrite(led_2, b[2]);
  digitalWrite(led_3, b[3]);
  digitalWrite(led_4, b[4]);
  digitalWrite(led_5, b[5]);
  digitalWrite(led_6, b[6]);
  digitalWrite(led_7, b[7]);

  int c = (b[7] << 7) | (b[6] << 6) | (b[5] << 5) | (b[4] << 4) | (b[3] << 3) |
          (b[2] << 2) | (b[1] << 1) | (b[0] << 0);

  char buf[50];
  switch (current_mode) {
  default:
  case mode_8_bit_hex:
    c &= 0xff;
    sprintf(buf, "%02x", c);
    Serial.println(buf);
    break;

  case mode_7_bit_ascii:
    c &= 0x7f;
    if (10 == c) {
      Serial.println();
    } else if (c > 0x1f && c < 0x7f) {
      buf[0] = c;
      buf[1] = '\0';
      Serial.print(buf);
    }
    break;

  case mode_5_bit_hex:
    c &= 0x1f;
    sprintf(buf, "%02x", c);
    Serial.println(buf);
    break;

  case mode_ascii:
  case mode_elliott:
  case mode_h_code:
    c &= 0x1f;
    if (31 == c) {
      shifted = false;
    } else if (27 == c) {
      shifted = true;
    }
    if (shifted) {
      if (mode_h_code == current_mode) {
        Serial.print(figures_h_code[c]);
      } else if (mode_elliott == current_mode) {
        Serial.print(figures_elliott[c]);
      } else {
        Serial.print(figures_ascii[c]);
      }
    } else {
      Serial.print(letters[c]);
    }
    break;

  case mode_calibrate:
    // print the results to the Serial Monitor:

    for (int i = 7; i >= 0; --i) {
      int a = bit_analog[i][sprocket_index];
      if (a < min_bit_analog[i]) {
        min_bit_analog[i] = a;
      }
      if (a > max_bit_analog[i]) {
        max_bit_analog[i] = a;
      }
      sprintf(buf, "%4d≤%4d≥%4d", min_bit_analog[i], a, max_bit_analog[i]);
      Serial.print(buf);
      Serial.print("  ");
    }

    int s = sprocket_analog[sprocket_index];
    if (s < min_sprocket_analog) {
      min_sprocket_analog = s;
    }
    if (s > max_sprocket_analog) {
      max_sprocket_analog = s;
    }
    sprintf(buf, "%4d≤%4d≤%4d", min_sprocket_analog, s, max_sprocket_analog);
    Serial.println(buf);

    break;
  }
}
