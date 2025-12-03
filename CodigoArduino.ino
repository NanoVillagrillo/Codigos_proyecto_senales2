// Control discreto con C(z) = (b0*z + b1)/(z - 1)
// u[k] = u[k-1] + b0*e[k] + b1*e[k-1]
// Ts = 100 ms

#include <TaskScheduler.h>

const int pinReferenciaSetPoint = A0;
const int pinSalidaPlanta       = A1;
const int pinActuadorPlanta     = 3;

const int PERIODO_MUESTREO_CONTROLADOR_MS = 100;
const int PERIODO_IMPRESION_MS = 500; // para plotter

// Coeficientes de C(z)
const float b0 = 1.7564f;
const float b1 = -1.6814f;

// Historial (más explícito)

float e_k_1 = 0.0f;   // e[k-1]
float u_k_1 = 0.0f;   // u[k-1]

// variables para graficar
float referencia = 0.0f;
float salida     = 0.0f;
float u_actual   = 0.0f;

// referencia automática (cuadrada entre 20 y 70 cada 6s)
bool usarReferenciaAutomatica = true;
bool estaAlta = false;
unsigned long tUltimoCambio = 0;
const unsigned long DURACION_MEDIA_ONDA = 6000;
float referenciaAlta = 70.0f;
float referenciaBaja = 20.0f;

// mapeo float
float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
  return (x - inMin)*(outMax - outMin)/(inMax - inMin) + outMin;
}

void actualizarReferenciaAutomatica() {
  unsigned long t = millis();
  if (t - tUltimoCambio >= DURACION_MEDIA_ONDA) {
    estaAlta = !estaAlta;
    tUltimoCambio = t;
  }
  referencia = estaAlta ? referenciaAlta : referenciaBaja;
}

float leerSalida() {
  int raw = analogRead(pinSalidaPlanta); // 0..1023
  // Ajusta el mapeo a la escala real de tu planta (0..100 en tu caso)
  return mapFloat((float)raw, 0.0f, 1023.0f, 0.0f, 100.0f);
}

void ejecutarControlador() {
  // referencia
  if (usarReferenciaAutomatica) actualizarReferenciaAutomatica();
  else {
    int rawRef = analogRead(pinReferenciaSetPoint);
    referencia = mapFloat((float)rawRef, 0.0f, 1023.0f, 20.0f, 70.0f);
  }

  salida = leerSalida();
  float e_k = referencia - salida;

  // ecuación en diferencias: u[k] = u[k-1] + b0*e[k] + b1*e[k-1]
  float u_k = u_k_1 + b0*e_k + b1*e_k_1;

  // saturación y anti-windup simple: limitar u y no integrar más allá
  const float U_MIN = 0.0f;
  const float U_MAX = 100.0f;
  if (u_k > U_MAX) u_k = U_MAX;
  if (u_k < U_MIN) u_k = U_MIN;

  // escribir PWM (map 0..100 a 0..255)
  int pwm = (int)round(mapFloat(u_k, 0.0f, 100.0f, 0.0f, 255.0f));
  analogWrite(pinActuadorPlanta, pwm);

  // actualizar históricos
  u_k_1 = u_k;
  e_k_1 = e_k;

  u_actual = u_k;
}

void imprimirDatosPlotter() {
  // Tres columnas: referencia, salida, u_actual
  Serial.print(referencia, 3); Serial.print('\t');
  Serial.print(salida, 3);     Serial.print('\t');
  Serial.println(u_actual, 3);
}

// Tareas
Task tareaControl(PERIODO_MUESTREO_CONTROLADOR_MS, TASK_FOREVER, &ejecutarControlador);
Task tareaPrint(PERIODO_IMPRESION_MS, TASK_FOREVER, &imprimirDatosPlotter);
Scheduler sched;

void setup() {
  pinMode(pinReferenciaSetPoint, INPUT);
  pinMode(pinSalidaPlanta, INPUT);
  pinMode(pinActuadorPlanta, OUTPUT);

  Serial.begin(115200);
  Serial.println("Referencia\tSalida\tControl");

  // init
  u_k_1 = 0.0f;
  e_k_1 = 0.0f;
  referencia = referenciaBaja;
  tUltimoCambio = millis();

  sched.addTask(tareaControl);
  sched.addTask(tareaPrint);
  tareaControl.enable();
  tareaPrint.enable();
}

void loop() {
  sched.execute();
}
