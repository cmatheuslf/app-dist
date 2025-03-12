from flask import Flask, request, jsonify
from flask_cors import CORS
import psycopg2
import os
import paho.mqtt.client as mqtt
from datetime import datetime

app = Flask(__name__)
CORS(app) 

# Configurações do banco de dados PostgreSQL (AWS RDS)
DB_HOST = os.getenv('DB_HOST', 'database-1.clc0ueuyweg6.sa-east-1.rds.amazonaws.com')
DB_NAME = os.getenv('DB_NAME', 'dbdistribuidos')
DB_USER = os.getenv('DB_USER', 'postgres')
DB_PASSWORD = os.getenv('DB_PASSWORD', 'cartt586674')

# Conectar ao banco de dados
def get_db_connection():
    return psycopg2.connect(host=DB_HOST, dbname=DB_NAME, user=DB_USER, password=DB_PASSWORD)

# Criar tabelas se não existirem
with get_db_connection() as conn:
    with conn.cursor() as cur:
        cur.execute('''CREATE TABLE IF NOT EXISTS devices (
                        dnome CHARACTER VARYING NOT NULL,
                        did CHARACTER VARYING PRIMARY KEY,
                        dkey CHARACTER VARYING NOT NULL)''')
        cur.execute('''CREATE TABLE IF NOT EXISTS dados (
                        id SERIAL PRIMARY KEY,
                        device_id CHARACTER VARYING REFERENCES devices(did),
                        temp REAL,
                        co2 REAL,
                        rh REAL,
                        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP)''')
        conn.commit()

# Configurações do broker MQTT
MQTT_BROKER = "ec2-18-228-228-184.sa-east-1.compute.amazonaws.com"  # Substitua pelo endereço do broker
MQTT_PORT = 1883
MQTT_TOPIC = "teste/data"

# Inicializando o cliente MQTT
mqtt_client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado ao broker com sucesso!")
        client.subscribe(MQTT_TOPIC)
    else:
        print("Falha na conexão com o broker, código:", rc)

def on_message(client, userdata, msg):
    import json
    try:
        data = json.loads(msg.payload.decode())
        device_id = data.get("id")
        device_key = data.get("chave")
        temp = data.get("temp")
        co2 = data.get("co2")
        rh = data.get("rh")

        if not all([device_id, device_key, temp, co2, rh]):
            print("Dados inválidos recebidos")
            return
        print(f"Recebido: {data}")
        
        with get_db_connection() as conn:
            with conn.cursor() as cur:
                cur.execute("SELECT * FROM devices WHERE did = %s AND dkey = %s", (device_id, device_key))
                device = cur.fetchone()
                
                if not device:
                    print("ID ou chave inválidos")
                    return
                
                # Captura o timestamp no próprio aplicativo
                timestamp = datetime.now().replace(microsecond=0).isoformat(sep=' ')


                # Verifica se já existe um registro para o mesmo device_id e timestamp
                cur.execute("""
                    SELECT 1 FROM Dados 
                    WHERE device_id = %s AND timestamp = %s
                """, (device_id, timestamp))

                existing_entry = cur.fetchone()
                
                if existing_entry:
                    print(f"⚠ Registro já existente para {device_id} no timestamp {timestamp}. Ignorando inserção.")
                    return
                
                print(f"🛠 Preparando para inserir: {device_id}, Temp: {temp}, CO2: {co2}, RH: {rh}, Timestamp: {timestamp}")

                cur.execute("""
                    INSERT INTO Dados (device_id, temp, co2, rh, timestamp) 
                    VALUES (%s, %s, %s, %s, %s)
                """, (device_id, temp, co2, rh, timestamp))

                print(f"gerando commit")
                conn.commit()
                print("✅ Dados inseridos com sucesso!")
    except Exception as e:
        print("Erro ao processar mensagem MQTT:", e)

# Configurando callbacks
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

# Conectar ao broker
mqtt_client.connect(MQTT_BROKER, MQTT_PORT)
mqtt_client.loop_start()

@app.route('/register', methods=['POST'])
def register_device():
    data = request.json
    device_name = data.get('dnome')
    device_id = data.get('did')
    device_key = data.get('dkey')
    
    if not device_name or not device_id or not device_key:
        return jsonify({"error": "Nome, ID e chave são obrigatórios"}), 400
    
    with get_db_connection() as conn:
        with conn.cursor() as cur:
            cur.execute("INSERT INTO dispositivos (dnome, did, dkey) VALUES (%s, %s, %s)", (device_name, device_id, device_key))
            conn.commit()
            
    
    return jsonify({"message": "Dispositivo registrado com sucesso!"})

# Endpoint para obter a lista de dispositivos
@app.route("/devices", methods=["GET"])
def get_devices():
    with get_db_connection() as conn:
        with conn.cursor() as cur:
            cur.execute("SELECT dnome, did, dkey FROM dispositivos")
            devices = cur.fetchall()

    # Convertendo os resultados para um formato JSON
    devices_list = [{"dnome": d[0], "did": d[1], "dkey": d[2]} for d in devices]

    return jsonify(devices_list)
@app.route("/get-measurements/<device_id>", methods=["GET"])
def get_measurements(device_id):
    with get_db_connection() as conn:
        with conn.cursor() as cur:
            cur.execute("""
                SELECT temp, co2, rh, timestamp 
                FROM dados 
                WHERE device_id = %s 
                ORDER BY timestamp DESC 
                LIMIT 30
            """, (device_id,))
            measurements = cur.fetchall()

    # Convertendo os resultados para um formato JSON
    measurements_list = [
        {"temp": m[0], "co2": m[1], "rh": m[2], "timestamp": m[3].isoformat()}
        for m in measurements
    ]

    return jsonify(measurements_list)

    
if __name__ == '__main__':
    app.run(debug=True)
