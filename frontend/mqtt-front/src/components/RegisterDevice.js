import { useState } from "react";
import "./RegisterDevice.css"

export default function RegisterDevice() {
  const [deviceName, setDeviceName] = useState("");
  const [deviceId, setDeviceId] = useState("");
  const [deviceKey, setDeviceKey] = useState("");
  const [message, setMessage] = useState("");

  const handleRegister = async () => {
    setMessage("");
    
    if (!deviceName || !deviceId || !deviceKey) {
      setMessage("Todos os campos são obrigatórios!");
      return;
    }

    try {
      const response = await fetch("http://127.0.0.1:5000/register", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ dnome: deviceName, did: deviceId, dkey: deviceKey }),
      });

      const data = await response.json();
      setMessage(data.message || data.error);
    } catch (error) {
      setMessage("Erro ao registrar o dispositivo.");
    }
  };

  return (
    <div className="max-w-md mx-auto p-4">
      <h1 class="formsTitle">Registro de dispositivos</h1>
      <div className=" space-y-4 forms">
      
        <input
          placeholder="Nome do dispositivo"
          value={deviceName}
          onChange={(e) => setDeviceName(e.target.value)}
        />
        <input
          placeholder="ID do dispositivo"
          value={deviceId}
          onChange={(e) => setDeviceId(e.target.value)}
        />
        <input
          placeholder="Chave do dispositivo"
          type="password"
          value={deviceKey}
          onChange={(e) => setDeviceKey(e.target.value)}
        />
        <button onClick={handleRegister}>Registrar</button>
        {message && <p className="text-center text-sm text-red-500">{message}</p>}
      </div>
    </div>
  );
}
