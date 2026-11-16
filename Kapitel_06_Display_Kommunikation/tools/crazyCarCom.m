% crazyCarCom.m  -  Gegenstelle zum Crazy Car
%
% Liest die Telemetrierahmen des Fahrzeugs und zeichnet sie mit.
% Optional schickt es Kommandorahmen zurueck.
%
% Rahmen vom Auto:   'C' 'S'  v0_hi v0_lo  v1_hi v1_lo ...  'E' 'C'
% Rahmen zum Auto:   'H' 'S'  cmd  val_hi val_lo  'E' 'H'
%
% Das ist absichtlich ein kurzes Skript und kein GUI: Sie sollen die
% Gegenseite Ihres eigenen Protokolls lesen koennen.

PORT      = "COM7";        % <- anpassen. Geraetemanager -> MSP Application UART
BAUD      = 57600;         % 57600 ueber USB, 9600 ueber Bluetooth
NUM_VALS  = 2;             % so viele Werte schickt Ihr driverComSend()
DURATION  = 30;            % Sekunden mitschreiben

TX_LEN = 2 + 2*NUM_VALS + 2;

s = serialport(PORT, BAUD);
configureTerminator(s, "LF");
flush(s);

t0   = tic;
data = [];

fprintf("Lese %d s von %s ...\n", DURATION, PORT);

while toc(t0) < DURATION

    % --- auf 'C','S' synchronisieren -------------------------------------
    if read(s, 1, "uint8") ~= uint8('C'), continue; end
    if read(s, 1, "uint8") ~= uint8('S'), continue; end

    % --- Nutzdaten und Rahmenende ----------------------------------------
    payload = read(s, 2*NUM_VALS, "uint8");
    tail    = read(s, 2, "uint8");

    if ~isequal(tail, uint8(['E' 'C']))
        continue;                       % Rahmenende passt nicht -> verwerfen
    end

    % --- zwei Bytes je Wert, hoeherwertiges zuerst -----------------------
    vals = double(payload(1:2:end)) * 256 + double(payload(2:2:end));
    data(end+1, :) = [toc(t0), vals];   %#ok<SAGROW>

    fprintf("%6.2f s   %s\n", data(end,1), mat2str(vals));
end

clear s;

if isempty(data)
    warning("Kein gueltiger Rahmen empfangen. Port, Baudrate und Rahmenende pruefen.");
    return;
end

figure;
plot(data(:,1), data(:,2:end), '.-');
xlabel('Zeit (s)'); ylabel('Wert');
legend(compose("Wert %d", 0:NUM_VALS-1));
grid on; grid minor;
title('Telemetrie Crazy Car');


%% ---- Kommando an das Auto schicken (einzeln ausfuehren) ----------------
% s = serialport(PORT, BAUD);
% cmd   = uint8(1);          % Ihre Kommandonummer
% value = uint16(50);        % z. B. Lenkwinkel
% frame = uint8(['H' 'S' cmd bitshift(value,-8) bitand(value,255) 'E' 'H']);
% write(s, frame, "uint8");
% clear s;
