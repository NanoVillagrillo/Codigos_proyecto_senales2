%Codigo del controlador
%Primero se añade la variable S para usarla en la funcion
s = tf('s');

%Se añade la funcion P(s) para poder verla al usar la herramienta sisotool

C = 1.7564*(s + 0.427)/s;


%El comando disp se usa para poder ver las variables del numerador
%Y denominador para poder usarlas en simulink
disp(C)

% Discretización del controlador
% Tiempo de muestreo
Ts = 0.1;
Cd = c2d(C, Ts, 'zoh');

% Mostrar controlador discreto
disp('Controlador discreto C(z):');
Cd

% Obtener numerador y denominador como vectores (para Simulink)
[numCd, denCd] = tfdata(Cd, 'v');

disp('Numerador de C(z):');
disp(numCd)
disp('Denominador de C(z):');
disp(denCd)
