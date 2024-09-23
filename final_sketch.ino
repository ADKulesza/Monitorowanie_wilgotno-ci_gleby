// Deklaracja pinów
#define redPin 9
#define greenPin 10
#define bluePin 11

#define moistSensorPin A0
#define motionSensorPin 2

#define mainLoopRate 200


const int moistArrayLen = 10;
int moistArray[moistArrayLen];

int moistArrayIndex = 0;            // Index of current measurement
int moistArraySum = 0;              // Sum of the moistArray
int moistAverage = 0;          // Moving avarage of the moistArray

int motionState = LOW;           // Initial motion detector status
const int motionArrayLen = 10;
int motionArray[motionArrayLen];

int motionArrayIndex = 0;

bool ledActive = false;       // Flag indicating whether the diode is activated
unsigned long ledActivatedTime = 0;  // LED activation time
const unsigned long ledDuration = 5000;  // LED activity time (1 minute)

int wetThreshold = 300;       // 
int superWetThreshold = 700;       // Próg dla mokrej gleby


void setup() {
  // RGB diode
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // motion sensor
  pinMode(motionSensorPin, INPUT);

  // Initialisation of the array with 0
  for (int i = 0; i < moistArrayLen; i++) {
    moistArray[i] = 0;
  }

  for (int i = 0; i < motionArrayLen; i++) {
    motionArray[i] = 0;
  }

  Serial.begin(9600);
}

void loop() {

  // Updating the moving average
  updateMovingAverage();

  motionArray[motionArrayIndex] = digitalRead(motionSensorPin);
  Serial.println(motionArray[motionArrayIndex]);
  motionArrayIndex = (motionArrayIndex + 1) % motionArrayLen; 

   if (shouldActivateLed()) {
    Serial.println("Mtion detected!");
    ledActive = true;
    ledActivatedTime = millis();
  }


  if (ledActive) {
    // 
    if (moistAverage < wetThreshold) {
      setColor(255, 0, 0);   // Red - dry soil
    } else if (moistAverage < superWetThreshold) {
      setColor(0, 255, 0);   // Green - acceptable level of water in soil
    } else {
      setColor(0, 0, 255);   // Blue - too wet soil
    }

    // Sprawdzenie, czy minął czas świecenia diody
    if (millis() - ledActivatedTime >= ledDuration) {
      ledActive = false;
      setColor(0, 0, 0);     // Wyłącz diodę
    }
  } else {
    // Jeśli dioda nie jest aktywna, upewnij się, że jest wyłączona
    setColor(0, 0, 0);
  }


  delay(mainLoopRate);
}

void updateMovingAverage() {
  moistArraySum = moistArraySum - moistArray[moistArrayIndex];          // Subtract the oldest value from the sum

  moistArray[moistArrayIndex] = analogRead(moistSensorPin);             // Get a new value from sensor

  moistArraySum = moistArraySum + moistArray[moistArrayIndex];          // Add the new value to the total
  moistArrayIndex = (moistArrayIndex + 1) % moistArrayLen;              // Move the index

  moistAverage = moistArraySum / moistArrayLen;

  Serial.print("Average moisture level: ");
  Serial.println(moistAverage);
}



// Motion detector function
bool shouldActivateLed() {

  int highCount = 0;

  // Count how many readings from motionSensor are HIGH
  for (int i = 0; i < motionArrayLen; i++) {
    if (motionArray[i] == HIGH) {

      highCount++;
    }
  }
  Serial.println(highCount);
  
  // If more than half of the readings are HIGH, return true
  return highCount > motionArrayLen / 2;
}


void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);      // PWM for red
  analogWrite(greenPin, greenValue);  // PWM for green
  analogWrite(bluePin, blueValue);    // PWM for blue
}