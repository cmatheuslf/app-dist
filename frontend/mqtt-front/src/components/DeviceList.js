import { useEffect, useState } from "react";
import { Link } from "react-router-dom";

export default function DeviceList() {
  const [devices, setDevices] = useState([]);
  const [error, setError] = useState("");

  useEffect(() => {
    fetch("http://18.228.228.184:5000/devices")
      .then((response) => response.json())
      .then((data) => {
        if (data.error) {
          setError("Erro ao carregar dispositivos.");
        } else {
          setDevices(data);
        }
      })
      .catch(() => setError("Erro ao conectar com o servidor."));
  }, []);

  return (
    <div style={{ maxWidth: "400px", margin: "auto", textAlign: "center" }}>
      <h2>Lista de Dispositivos</h2>
      {error && <p style={{ color: "red" }}>{error}</p>}
      <ul style={{ listStyle: "none", padding: 0 }}>
        {devices.map((device) => (
            <li key={device.did} style={{ padding: "10px", borderBottom: "1px solid #ddd" }}>
                <Link to={`/dashboard/${device.did}`} style={{ textDecoration: "none", color: "blue" }}>
                    {device.dnome}
                </Link> (ID: {device.did})
            </li>
        ))}
      </ul>
    </div>
  );
}
