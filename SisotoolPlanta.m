%Primero se añade la variable S para usarla en la funcion
s = tf('s')

%Se añade la funcion P(s) para poder verla al usar la herramienta sisotool

P = 1/((s^2)+3*s+1)

%El comando disp se usa para poder ver las variables del numerador
%Y denominador para poder usarlas en simulink
disp(P)

%Se usa sisotool para poder ver las graficas
sisotool(P)
