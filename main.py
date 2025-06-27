import ast, serial, serial.tools.list_ports, threading, time
from flask import Flask, render_template

def timestamp(today_seconds = False):
    if not today_seconds:
        return time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
    else:
        t = time.localtime()
        return t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec  

try:
    with open('logs.log', 'w') as f:
        can_log = True
except Exception:
    can_log = False

#Определяем, куда подключена Arduino
list_ports = list(serial.tools.list_ports.comports())
ports = []
ser_port = None
print('Список доступных устройств:\n')

for current_port in list_ports:
    print(current_port.device)
    print(current_port.description)
    print(current_port.manufacturer)
    print()
    ports.append((current_port.device, current_port.description, current_port.manufacturer))

for current_port in ports:
    if 'Arduino' in current_port[1]:
        ser_port = current_port[0]

if ser_port is None:
    print('Отсутствуют порты Arduino')
    force_com = input('Введите номер COM-порта вручную или нажмите Enter для выхода: ')
    if not force_com:
        exit()
    ser_port = f'COM{force_com}'

print(f'Выбран порт {ser_port}')
try:
    ser = serial.Serial(ser_port, 38400)
except serial.serialutil.SerialException:
    print('Выбранный порт занят другой программой')
    exit()
else:
    print('Всё хорошо')

#Отправка времени на Arduino при запросе

while True:
    try:
        data = ''#защита от TypeError -> UnboundLocalError
        data = ser.readline().decode('utf-8')
        data = ast.literal_eval(data)
        break
    except (SyntaxError, TypeError):#except (SyntaxError, TypeError, ValueError):
        if data == '<time>\r\n':
            while True:
                if ser.readline().decode('utf-8') == '<ok>\r\n':
                    break
                ser.write(f'<{timestamp(today_seconds = True)}>'.encode('utf-8'))
            break

#Функция для получения данных
def get_data_or_set_time():
    global data
    while True:
        try:
            tmp = ''#защита от TypeError -> UnboundLocalError
            tmp = ser.readline().decode('utf-8')
            data = str(ast.literal_eval(tmp)).replace("'", '"')
            if can_log:
                with open('logs.log', 'a') as f:
                    f.write(f'{data}\n')
        except (SyntaxError, TypeError):#except (SyntaxError, TypeError, ValueError):
            data = '{"temp": "None", "humid": "None", "press": "None", "time": "None"}'
            if tmp == '<time>\r\n':
                while True:
                    if ser.readline().decode('utf-8') == '<ok>\r\n':
                        break
                    ser.write(f'<{timestamp(today_seconds = True)}>'.encode('utf-8'))

get_data_or_set_time_thread = threading.Thread(target = get_data_or_set_time, daemon = True)
get_data_or_set_time_thread.start()

#Сервер
app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def data_read():
    return data

@app.route('/logs')
def logs_read():
    with open('logs.log') as f:
        return f.read()

app.run(port=80)
