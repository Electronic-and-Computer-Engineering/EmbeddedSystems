% crazyCarCom.m  -  Gegenstelle zum Crazy Car
%
% Liest die Telemetrieframes des Fahrzeugs und zeichnet sie mit.
% Optional schickt es Kommandoframes zurueck.
%
% Frame vom Auto:   'C' 'S'  v0_hi v0_lo  v1_hi v1_lo ...  'E' 'C'
% Frame zum Auto:   'H' 'S'  cmd  val_hi val_lo  'E' 'H'
%
% Das ist absichtlich ein kurzes Skript und kein GUI: Sie sollen die
% Gegenseite Ihres eigenen Protokolls lesen koennen.

PORT      = "COM7";        % <- anpassen. Geraetemanager -> MSP Application UART
BAUD      = 57600;         % 57600 ueber USB, 9600 ueber Bluetooth
NUM_VALS  = 4;             % so viele Werte schickt Ihr driverComSend()
SIGNED    = [0 0 0 1];     % 1 = diesen Wert als vorzeichenbehaftet lesen
                           %     (passend zum Testsatz 0 / 4095 / 65535 / -40)
DURATION  = 30;            % Sekunden mitschreiben

TX_LEN = 2 + 2*NUM_VALS + 2;

if numel(SIGNED) ~= NUM_VALS
    error("SIGNED muss genau NUM_VALS Eintraege haben.");
end

s = serialport(PORT, BAUD);
s.Timeout = 2;             % nicht ewig auf Bytes warten
flush(s);

t0   = tic;
data = [];

fprintf("Lese %d s von %s ...\n", DURATION, PORT);

while toc(t0) < DURATION

    % --- auf 'C','S' synchronisieren -------------------------------------
    b = uint8(read(s, 1, "uint8"));
    if isempty(b) || b ~= uint8('C'), continue; end
    b = uint8(read(s, 1, "uint8"));
    if isempty(b) || b ~= uint8('S'), continue; end

    % --- Nutzdaten und Frameende ----------------------------------------
    payload = uint8(read(s, 2*NUM_VALS, "uint8"));
    tail    = uint8(read(s, 2, "uint8"));

    if numel(payload) ~= 2*NUM_VALS || ~isequal(tail, uint8(['E' 'C']))
        continue;                       % unvollstaendig oder falsches Ende -> verwerfen
    end

    % --- zwei Bytes je Wert, hoeherwertiges zuerst -----------------------
    vals = double(payload(1:2:end)) * 256 + double(payload(2:2:end));

    % --- negative Werte: oberhalb von 32767 liegt der negative Bereich ---
    % Uebertragen wird immer nur das 16-Bit-Muster. Ob ein Wert negativ
    % gemeint ist, steht nicht im Frame - das entscheidet erst der Leser.
    neg = (SIGNED == 1) & (vals > 32767);
    vals(neg) = vals(neg) - 65536;
    data(end+1, :) = [toc(t0), vals];   %#ok<SAGROW>

    fprintf("%6.2f s   %s\n", data(end,1), mat2str(vals));
end

clear s;

if isempty(data)
    warning("Kein gueltiger Frame empfangen. Port, Baudrate und Frameende pruefen.");
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
% cmd   = 1;                 % Ihre Kommandonummer
% value = 50;                % z. B. Lenkwinkel  (0 .. 65535)
%
% hi = uint8(floor(value/256));
% lo = uint8(mod(value,256));
% frame = uint8([uint8('H') uint8('S') uint8(cmd) hi lo uint8('E') uint8('H')]);
%
% write(s, frame, "uint8");
% clear s;
%
% Hinweis: alle Elemente muessen uint8 sein. MATLAB bricht ab, sobald in
% einem Array uint8 und uint16 gemischt werden.
