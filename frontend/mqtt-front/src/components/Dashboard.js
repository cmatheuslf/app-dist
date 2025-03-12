import React, { useState, useEffect } from "react";
import { useParams } from "react-router-dom"; // Importa useParams para capturar o device_id
import { Line } from "react-chartjs-2";
import "chart.js/auto";
import "./Dashboard.css";

const Dashboard = () => {
  const { device_id } = useParams(); // Captura o device_id da URL
  const [data, setData] = useState({
    co2: { values: [], current: 0 },
    temp: { values: [], current: 0 },
    rh: { values: [], current: 0 },
  });

  useEffect(() => {
    if (!device_id) return; // Se não houver ID na URL, não faz nada

    const fetchData = async () => {
      try {
        const response = await fetch(`http://18.228.228.184:5000/get-measurements/${device_id}`);
        const json = await response.json();

        // Processar os dados recebidos
        const co2Values = json.map((entry) => entry.co2);
        const tempValues = json.map((entry) => entry.temp);
        const rhValues = json.map((entry) => entry.rh);

        setData({
          co2: { values: co2Values, current: co2Values[0] || 0 },
          temp: { values: tempValues, current: tempValues[0] || 0 },
          rh: { values: rhValues, current: rhValues[0] || 0 },
        });
      } catch (error) {
        console.error("Erro ao buscar dados:", error);
      }
    };

    const interval = setInterval(fetchData, 5000);
    fetchData(); // Chamada inicial
    return () => clearInterval(interval);
  }, [device_id]); // Atualiza sempre que o device_id mudar

  const chartOptions = {
    responsive: true,
    plugins: { legend: { display: false } },
  };

  return (
    <div className="bg-gray-100 min-h-screen p-4 container">
      <div className="bg-blue-600 p-4 rounded-lg text-white text-2xl font-bold divlogo flex items-center">
       
        <h3> Monitor de Qualidade do Ar - Dispositivo {device_id} </h3>
      </div>

      <div className="grid grid-cols-2 gap-4 mt-4">
        {["co2", "temp", "rh"].map((type, idx) => (
          <div key={idx} className="flex bg-gray space-x-4 w-1/2 line">
            {/* Gráfico */}
            <div className="p-4 border-2 border-blue-600 bg-black rounded-lg w-1/2 shadow Single-div">
              <h3 className="text-lg font-bold">{type.toUpperCase()} (PPM)</h3>
              <Line
                data={{
                  labels: data[type].values.map((_, i) => i + 1),
                  datasets: [
                    {
                      label: type.toUpperCase(),
                      data: data[type].values.slice().reverse(),
                      borderColor: type === "co2" ? "green" : type === "temp" ? "red" : "blue",
                      backgroundColor: "transparent",
                      pointRadius: 3,
                    },
                  ],
                }}
                options={chartOptions}
              />
            </div>

            {/* Valor Atual */}
            <div className="p-4 border-2 border-blue-600 bg-white rounded-lg shadow current-value">
              <h3 className="text-md font-bold">{type.toUpperCase()} Valor Atual</h3>
              <p className={`text-4xl font-bold ${type === "co2" ? "text-green-600" : type === "temp" ? "text-red-600" : "text-blue-600"}`}>
                {data[type].current}
              </p>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default Dashboard;
