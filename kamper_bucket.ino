// Global Variables
// Pins
#define soil_moist_pin_0       A0
#define relay_control_pin_0    4
#define relay_control_pin_1    5
#define indication_led         13

// test defines
//#define ADC_TEST
//#define RELAY_1_TEST
#define TIMER_SPRAYER

// timer sprayer defines
// CURRENT IMPLEMENTATION: Reset Every 3.8hrs & spray both pumps at reset
// reset time in secs
// for 24 hrs reset time = 86400
#define RESET_TIME              13680
// pump start times, define in mins from start of program
#define PUMP0_START_0           1
#define PUMP0_START_1           0
#define PUMP0_START_2           0
#define PUMP0_START_3           0
#define PUMP0_START_4           0
#define PUMP1_START_0           2 
#define PUMP1_START_1           0
#define PUMP1_START_2           0
#define PUMP1_START_3           0
#define PUMP1_START_4           0
// run times, define in seconds
#define PUMP0_RUN               4
#define PUMP1_RUN               5
// TEST times
#define TEST_ON_TIME            3
#define TEST_OFF_TIME           5

// Global Variables
// adc test variables
long test_total_ctr = 0;
int test_index_ctr;
int percent_adc_array[20];
float avg_percent_adc;
// timersprayer variables - next start and stop time
long second_total_ctr;
long pump0_start_time;
long pump0_stop_time;
long pump1_start_time;
long pump1_stop_time;



void relay1PinTest(void)
{
  delay((TEST_OFF_TIME - 1) * 1000);
  digitalWrite(relay_control_pin_0, LOW);
  digitalWrite(indication_led, HIGH);
  delay(TEST_ON_TIME * 1000);
  digitalWrite(relay_control_pin_0, HIGH);
  digitalWrite(indication_led, LOW);
}

void rawADCTest(void)
{
  // run test ADC test print
  digitalWrite(indication_led, HIGH);
  // re-init index ctr
  if (test_index_ctr == 20)
    test_index_ctr = 0;
  int raw_adc = analogRead(soil_moist_pin_0);
  float ratio = (float)raw_adc / 1023;
  int percent_adc = ratio * 100;
  percent_adc_array[test_index_ctr] = percent_adc;
  if (test_index_ctr == 19)
  {
    // calculate avg
    avg_percent_adc = 0;
    for (int i = 0; i < 20; i++)
    {
      avg_percent_adc += (float)percent_adc_array[i];
    }
    avg_percent_adc /= 20;
    Serial.print("Test T+");
    Serial.println(test_total_ctr);
    Serial.print("Avg ADC Value: ");
    Serial.print(avg_percent_adc);
    Serial.println("%");
  }
  test_total_ctr++;
  test_index_ctr++;
  digitalWrite(indication_led, LOW);
}

void timerSprayerHandler(void)
{
  if (second_total_ctr == RESET_TIME)
  {
    initTimerSprayer();
  }
  // blink light to on
  digitalWrite(indication_led, HIGH);
  // print timing
  Serial.print("Time since test start = ");
  Serial.println(second_total_ctr);
  // check if this is a flagged watering time or stopping time
  if (second_total_ctr == pump0_start_time)
  {
    // handle pump0
    // calculate stop time
    pump0_stop_time = second_total_ctr + PUMP0_RUN;
    // calculate next start time
    if ((PUMP0_START_1 * 60) > second_total_ctr)
    {
      pump0_start_time = PUMP0_START_1 * 60;
    }
    else if ((PUMP0_START_2 * 60) > second_total_ctr)
    {
      pump0_start_time = PUMP0_START_2 * 60;
    }
    else if ((PUMP0_START_3 * 60) > second_total_ctr)
    {
      pump0_start_time = PUMP0_START_3 * 60;
    }
    else if ((PUMP0_START_4 * 60) > second_total_ctr)
    {
      pump0_start_time = PUMP0_START_4 * 60;
    }
    // turn on sprayer
    Serial.print("Start spray 0 @ time ");
    Serial.println(second_total_ctr);
    digitalWrite(relay_control_pin_0, LOW);
  }
  else if (second_total_ctr == pump0_stop_time)
  {
    // turn off sprayer
    Serial.print("Stop spray 0 @ time ");
    Serial.println(second_total_ctr);
    digitalWrite(relay_control_pin_0, HIGH);
  }

  // handle pump 1
  if (second_total_ctr == pump1_start_time)
  {
    // calculate stop time
    pump1_stop_time = second_total_ctr + PUMP1_RUN;
    // calculate next start time
    if ((PUMP1_START_1 * 60) > second_total_ctr)
    {
      pump1_start_time = PUMP1_START_1 * 60;
    }
    else if ((PUMP1_START_2 * 60) > second_total_ctr)
    {
      pump1_start_time = PUMP1_START_2 * 60;
    }
    else if ((PUMP1_START_3 * 60) > second_total_ctr)
    {
      pump1_start_time = PUMP1_START_3 * 60;
    }
    else if ((PUMP1_START_4 * 60) > second_total_ctr)
    {
      pump1_start_time = PUMP1_START_4 * 60;
    }
    // turn on sprayer
    Serial.print("Start spray 1 @ time ");
    Serial.println(second_total_ctr);
    digitalWrite(relay_control_pin_1, LOW);
  }
  else if (second_total_ctr == pump1_stop_time)
  {
    // turn off sprayer
    Serial.print("Stop spray 1 @ time ");
    Serial.println(second_total_ctr);
    digitalWrite(relay_control_pin_1, HIGH);
  }

  // counter and LED
  second_total_ctr++;
  // blink light off
  digitalWrite(indication_led, LOW);
}

// initialize first start times
void initTimerSprayer(void)
{
  // initialize second counter
  second_total_ctr = 0;
#ifdef PUMP0_START_0
  pump0_start_time = PUMP0_START_0 * 60;
#endif // PUMP0_START_0
#ifdef PUMP1_START_0
  pump1_start_time = PUMP1_START_0 * 60;
#endif // PUMP1_START_0
}

void setup() {
  // Setup Serial Port for Debug Output
  Serial.begin(38400);
  Serial.print("Arduino # = ");
  Serial.println(ARDUINO);
  // setup output relay 0 pin on D4, default high
  pinMode(relay_control_pin_0, OUTPUT);
  digitalWrite(relay_control_pin_0, HIGH);
  // setup output relay 1 pin on D5, default high
  pinMode(relay_control_pin_1, OUTPUT);
  digitalWrite(relay_control_pin_1, HIGH);
  // setup led
  pinMode(indication_led, OUTPUT);
  digitalWrite(indication_led, LOW);
#ifdef ADC_TEST
  // initialize test_index_ctr for adc test
  test_index_ctr = 0;
# endif // ADC_TEST
#ifdef TIMER_SPRAYER
  // initialize second counter
  // initialize first start times
  initTimerSprayer();
#endif // TIMER_SPRAYER
}

void loop() {
#ifdef ADC_TEST
  rawADCTest();
#endif //ADC_TEST
#ifdef RELAY_1_TEST
  relay1PinTest();
#endif // RELAY_1_TEST
#ifdef TIMER_SPRAYER
  timerSprayerHandler();
#endif // TIMER_SPRAYER   
  delay(1000);
}
